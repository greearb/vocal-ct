
/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

static const char* const MediaSession_cxx_Version =
    "$Id: MediaSession.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include <cassert>

#include "MediaDevice.hxx"
#include "MediaSession.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Attributes.hxx"
#include "MediaController.hxx"
#include "SdpHandler.hxx"
#include "CodecSpeex.hxx"
#include "CodecG726_16.hxx"
#include "CodecG726_24.hxx"
#include "CodecG726_32.hxx"
#include "CodecG726_40.hxx"
#include "CodecG729a.hxx"
#include "MRtpSession.hxx"

using namespace Vocal::UA;
using namespace Vocal::SDP;


int MediaSession::_cnt = 0;

MediaSession::MediaSession(int sessionId, 
                           Sptr<NetworkRes> localRes,
                           const string& local_dev_to_bind_to) 
      : mySessionId(sessionId),
        myRtpSession(NULL),
        localDevToBindTo(local_dev_to_bind_to)
{
    _cnt++;
    mySSRC = 0;
    while (mySSRC == 0) {
       mySSRC =  rand();
    }
    localRes->setBusy(true, "MediaSession constructor");
    localRes->setOwner(this);
    myLocalRes = localRes;
    cpLog(LOG_ERR, "Created media session, this: %p, count: %d\n",
          this, _cnt);
}


MediaSession::~MediaSession()
{
    cpLog(LOG_ERR, "MediaSession::~MediaSession, this: %p, count: %d",
          this, _cnt);
    _cnt--;
    tearDown();
    if (myLocalRes.getPtr() != 0) {
       myLocalRes->setOwner(NULL);
       if (myLocalRes->getBusyFlag()) {
          myLocalRes->setBusy(false, "MediaSession destructor");
       }
    }
}
 
void
MediaSession::addToSession( Sptr<MediaDevice> mDevice)
{
   assertNotDeleted();

   if(mDevice != 0)
   {
       mDevice->setBusy(true);
       mDevice->assignedTo(mySessionId);
       myMediaDevice = mDevice;
   }
}


void
MediaSession::addToSession( SdpSession& localSdp, SdpSession& remoteSdp)
{
    assertNotDeleted();
    int fromMedia = 2; // Debugging
    int fmt = 0;
    string fmt_name;

    if (myRtpSession != 0) {
        cpLog(LOG_ERR, "Already has a session, ignoring...");
        return;
    }

    // Set remote host and port
    //   For now only support unicast
    string remAddr;
    string localAddr;
    LocalScopeAllocator lo;
    if (localSdp.getConnection())
       localAddr = localSdp.getConnection()->getUnicast().getData(lo);
    if (remoteSdp.getConnection())
       remAddr = remoteSdp.getConnection()->getUnicast().getData(lo);

    //Get the Negotiated Codec for each media type
    //Get each media description from remoteSdp and map it to the
    //localSdp SDP and create an MRtpSession
    list < SdpMedia* > mList = remoteSdp.getMediaList();
    MediaController& mInstance = MediaController::instance();
    for(list < SdpMedia* >::iterator itr = mList.begin();
                            itr != mList.end(); itr++)
    {
        SdpMedia* rMedia = (*itr);
        string rAddr;
        string lAddr;
        int lPort = -1;
        int rPort;
        Sptr<CodecAdaptor> cAdp;
        rPort = rMedia->getPort();
        list < SdpMedia* > lmList = localSdp.getMediaList(); 
        for(list < SdpMedia* >::iterator itr2 = lmList.begin();
                            itr2 != lmList.end(); itr2++)
        {
            SdpMedia* lMedia = (*itr2);
            if(lMedia->getMediaType() != rMedia->getMediaType())
            {
                continue;
            }
            //Get the very first codec of the media description
            vector < int > * lfmList = lMedia->getFormatList();
            if (!lfmList || (lfmList->size() == 0)) {
               continue;
            }

            vector < int > * rfmList = rMedia->getFormatList();
            if (!rfmList || (rfmList->size() == 0)) {
               continue;
            }

            // So, we put our favorites first.  Assume the remote does
            // the same.  So, find the codec that matches the remote's preference.
            for (unsigned int q = 0; q<rfmList->size(); q++) {
               for (unsigned int r = 0; r<lfmList->size(); r++) {
                  if ((*rfmList)[q] == (*lfmList)[r]) {
                     fmt = (*rfmList)[q];
                     goto found_one;
                  }
               }
            }

            // The goto will jump over this if we actually find a match.
            continue;
               
          found_one:
            char tbuf[50];
            snprintf(tbuf, 49, "%d", fmt);
            fmt_name = tbuf;

            int sample_rate = 8000;

            if (fmt >= rtpPayloadDynMin) {
               SdpRtpMapAttribute attribute;
               int lm_rv = lMedia->getRtpmapAttributeValue(fmt, attribute);
               assert(lm_rv >= 0);

               // Now, we got something like 'speex/8000'
               if (strncasecmp(attribute.getEncodingName().c_str(), "speex", 5) == 0) {
                  fmt_name = "SPEEX";
                  sample_rate = attribute.getClockRate();
               }
               else {
                  cpLog(LOG_ERR, "FATAL:  We do not know about this negotiated protocol: %s",
                        attribute.getEncodingName().c_str());
                  assert("unhandled encoding name for negotiated protocol");
               }
            }

            cAdp = mInstance.getMediaCapability().getCodec(fmt_name);
            cpLog(LOG_ERR, "NOTE:  Negotiated format: %d  name: %s  cAdp->type: %d\n",
                  fmt, fmt_name.c_str(), cAdp->getType());

            if (cAdp->getType() == SPEEX) {
               // Make a copy, we have state in the Speex codec.
               cAdp = new CodecSpeex(*((CodecSpeex*)(cAdp.getPtr())));
               cAdp->setRtpType(fmt);
               cAdp->setClockRate(sample_rate);
            }
#ifdef USE_VOICE_AGE
            else if (cAdp->getType() == G729) {
               // Make a copy, we have state in the g729 codec.
               cAdp = new CodecG729a(*((CodecG729a*)(cAdp.getPtr())));
               cAdp->setRtpType(fmt);
               cAdp->setClockRate(sample_rate);
            }
#endif
            else if (cAdp->getType() == G726_16) {
               // Make a copy, we have state in this codec.
               cAdp = new CodecG726_16(*((CodecG726_16*)(cAdp.getPtr())));
               cAdp->setRtpType(fmt);
               cAdp->setClockRate(sample_rate);
            }
            else if (cAdp->getType() == G726_24) {
               // Make a copy, we have state in this codec.
               cAdp = new CodecG726_24(*((CodecG726_24*)(cAdp.getPtr())));
               cAdp->setRtpType(fmt);
               cAdp->setClockRate(sample_rate);
            }
            else if (cAdp->getType() == G726_32) {
               // Make a copy, we have state in this codec.
               cAdp = new CodecG726_32(*((CodecG726_32*)(cAdp.getPtr())));
               cAdp->setRtpType(fmt);
               cAdp->setClockRate(sample_rate);
            }
            else if (cAdp->getType() == G726_40) {
               // Make a copy, we have state in this codec.
               cAdp = new CodecG726_40(*((CodecG726_40*)(cAdp.getPtr())));
               cAdp->setRtpType(fmt);
               cAdp->setClockRate(sample_rate);
            }

            if(lMedia->getConnection())
            {
                LocalScopeAllocator lo;
                lAddr = lMedia->getConnection()->getUnicast().getData(lo);
                fromMedia = 1;
            }
            else
            {
                lAddr = localAddr;
                fromMedia = 0;
            }
            lPort = lMedia->getPort();
            break;
        }
        if(rMedia->getConnection())
        {
            LocalScopeAllocator lo;
            rAddr = rMedia->getConnection()->getUnicast().getData(lo);
        }
        else
        {
            rAddr = remAddr;
        }

        //Now if we have lAddr, rAddr, lport, rPort and codec adaptor
        if((lAddr.size() ==0) || (rAddr.size() == 0) || (cAdp == 0)) {
           cpLog(LOG_ERR, "Media is not setup correctly, lAddr: %s  rAddr: %s  cAdp: %p, fmt: %d\n",
                 lAddr.c_str(), rAddr.c_str(), cAdp.getPtr(), fmt);
           continue;
        }

        //Create an RTP session
        NetworkRes localRes(lAddr, lPort);
        NetworkRes remoteRes(rAddr, rPort);
        myRtpSession = new MRtpSession(mySessionId, localRes,
                                       localDevToBindTo,
                                       remoteRes,
                                       cAdp, fmt, mySSRC);
    }
}//addToSession


int 
MediaSession::tearDown()
{
    assertNotDeleted();
    cpLog(LOG_DEBUG, "Tearing down the session:%d" , mySessionId);

    // Stop them both...
    if (myRtpSession != 0) {
        myRtpSession->shutdown();
    }

    if (myMediaDevice != 0) {
        myMediaDevice->stop();
    }

    // Now, clear out the references.
    if (myRtpSession != 0) {
        delete myRtpSession;
        myRtpSession = NULL;
    }

    if (myMediaDevice != 0) {
        myMediaDevice->assignedTo(-1);
        myMediaDevice->setBusy(false);
        myMediaDevice = 0;
    }

    myLocalRes->setBusy(false, "tear down");
    assertNotDeleted();
    return 1;
}

void 
MediaSession::processRaw(char *data, int len, VCodecType cType, Sptr<CodecAdaptor> codec,
                         Adaptor* adp, bool silence_pkt)
{
    assertNotDeleted();
    if(adp->getDeviceType() != RTP)
    {
        //Data from hardware, ship it out to the RTP session
        if(myRtpSession != 0)
           myRtpSession->sinkData(data, len, cType, codec, silence_pkt);
    }
    else
    {
         myMediaDevice->sinkData(data, len, cType, codec, silence_pkt);
    }
}


/** Allow the receiver to throttle based on the current size of the
 * jitter buffer.  This timeout will be passed to select.
 */
uint64 MediaSession::getPreferredTimeout(unsigned int jitter_pkts_in_queue,
                                         unsigned int queue_max) {
   uint64 rv = 0;
   if (myMediaDevice.getPtr()) {
      rv += myMediaDevice->getDataRate();
   }

   // Now, deal with some adaptive code to try to keep the jitter buffer half full.
   unsigned int desired = queue_max >> 1; // Half of max
   if (jitter_pkts_in_queue < (desired - desired/2)) {
      rv +=  4;
   }
   else if (jitter_pkts_in_queue > (desired + desired/2)) {
      rv -= 4;
   }
   return rv;
}


void
MediaSession::startSession(VSdpMode mode)
{
    assertNotDeleted();
    if (myRtpSession != 0) {
        myMediaDevice->start(myRtpSession->getCodec()->getType());
        myRtpSession->setMode(mode); 
    }
    else {
        cpLog(LOG_ERR, "WARNING:  myRtpSession is NULL in startSession");
        // Default to use G711 U codec.
        myMediaDevice->start(G711U);
    }
}

void
MediaSession::suspend()
{
    myMediaDevice->suspend(); 
}
void
MediaSession::resume(SdpSession& remoteSdp)
{
    if (myRtpSession != 0)
        myRtpSession->adopt(remoteSdp);
    myMediaDevice->resume(); 
}

SdpSession 
MediaSession::getSdp(VSdpMode mode)
{
    string localAddr;
    int    localPort;
    localAddr = myLocalRes->getIpName().c_str();
    localPort = myLocalRes->getPort();
    SdpSession sdp;
    setStandardSdp(sdp, localAddr, localPort, mode);
    sdp.setSessionId(mySessionId);
    return sdp;
}
 

SdpSession 
MediaSession::getSdp(SdpSession& remoteSdp)
{
    string localAddr;
    int    localPort;
    localAddr = myLocalRes->getIpName().c_str();
    localPort = myLocalRes->getPort();
    SdpSession sdp;
    negotiateSdp(sdp, localAddr, localPort, remoteSdp);
    sdp.setSessionId(mySessionId);
    return sdp;
}
