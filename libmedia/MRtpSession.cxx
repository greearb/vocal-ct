/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2002-2003 Vovida Networks, Inc.  All rights reserved.
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

static const char* const MRtpSession_cxx_Version =
    "$Id: MRtpSession.cxx,v 1.5 2004/10/29 07:22:34 greear Exp $";

#include "global.h"
#include <cassert>
#include "RtpPacket.hxx"

#include "MediaDevice.hxx"
#include "MediaSession.hxx"
#include "MRtpSession.hxx"
#include "MediaController.hxx"
#include "misc.hxx"

using namespace Vocal::UA;
using namespace Vocal::SDP;


MRtpSession::MRtpSession(int sessionId, NetworkRes& local,
                         const string& local_dev_to_bind_to,
                         NetworkRes& remote ,
                         Sptr<CodecAdaptor> cAdp, int rtpPayloadType,
                         u_int32_t ssrc)
    : Adaptor(RTP, NONE), mySessionId(sessionId),
      rtp_rx_packet(1012)
{

    lastRtpRetrieve = 0;
    myCodec = cAdp;
    myLocalAddress = new NetworkRes(local);
    myRemoteAddress = new NetworkRes(remote);
    localDevToBindTo = local_dev_to_bind_to;

    int remotePort = myRemoteAddress->getPort();
    int localPort  = myLocalAddress->getPort();
    int remoteRtcpPort = ( remotePort > 0) ? remotePort + 1 : 0;
    int localRtcpPort = (localPort > 0) ? localPort + 1 : 0;

    //RtpPayloadType pType;
    int pType;  // 96 <-> 127 are dynamic, doesn't map well to an enum.
    switch(cAdp->getType())
    {
        case G711U:
            pType = rtpPayloadPCMU;
            break;
        case G711A:
            pType = rtpPayloadPCMA;
            break;
        case G729:
            pType = rtpPayloadG729;
            break;
        case G726_16:
            pType = rtpPayloadG726_16;
            break;
        case G726_24:
            pType = rtpPayloadG726_24;
            break;
        case G726_32:
            pType = rtpPayloadG726_32;
            break;
        case G726_40:
            pType = rtpPayloadG726_40;
            break;
        case SPEEX:
            pType = rtpPayloadType; // Negotiated
            assert((pType >= rtpPayloadDynMin) && (pType <= rtpPayloadDynMax));
            break;
        default:
            pType = rtpPayloadPCMU;
            break;
    }

    rtpStack =  new RtpSession( myLocalAddress->getIpName().c_str(),
                                localDevToBindTo,
                                myRemoteAddress->getIpName().c_str(), 
                                remotePort,
                                localPort,
                                remoteRtcpPort,
                                localRtcpPort, (RtpPayloadType)(pType),
                                cAdp->getClockRate(), cAdp->getPerSampleSize(),
                                cAdp->getSampleSize());

    myDTMFInterface = new MDTMFInterface(this);
    rtpStack->setDTMFInterface(myDTMFInterface);
    rtpStack->setMarkerOnce();
}

void MRtpSession::readNetwork(fd_set* fds) {
    RtpSessionState sessionState = rtpStack->getSessionState();
    
    if ( sessionState == rtp_session_undefined ) {
        cpLog(LOG_ERR, "Session state undefined, returning");
        return;
    }


    // Generally, we receive 1 pkt every 20ms, so only going to try to read one
    // pkt at a time here.  If there is more to be received, the select will cause
    // us to return here quickly... --Ben

    rtpStack->readNetwork(fds);
}//readNetwork


void MRtpSession::retrieveRtpSample() {
    rtp_rx_packet.clear();

    int rv = rtpStack->retrieve(rtp_rx_packet);

    if (rv > 0) {
        /* Removing check for payload-usage.  It doesn't work for variable
         * encoding schemes. --Ben
         */
        if (rtp_rx_packet.getPayloadType() == (RtpPayloadType)myCodec->getRtpType() ) {
            processRecv(rtp_rx_packet, *myRemoteAddress);
        }
        else {
            cpLog(LOG_ERR, "Received from RTP stack incorrect payload type, rtp_type: %d  codec_type: %d  codec_rtp_type: %d",
                  rtp_rx_packet.getPayloadType(), myCodec->getType(), myCodec->getRtpType());
        }
    }//if
}//retrieveRtpSample


void MRtpSession::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                       uint64 now) {
    readNetwork(input_fds); /** Handles both RTCP and RTP receive logic,
                             * will not block. */

    // Only drain jitter buffer every XXX miliseconds.
    uint64 pref = mySession->getPreferredTimeout(rtpStack->getJitterPktsInQueueCount(),
                                                 rtpStack->getCurMaxPktsInQueue());
    if ((lastRtpRetrieve + pref) <= now) {
        retrieveRtpSample();
        lastRtpRetrieve = now;
    }
}//tick

int MRtpSession::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                        int& maxdesc, uint64& timeout, uint64 now) {

    rtpStack->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);

    if (mySession.getPtr()) {
        uint64 pref = mySession->getPreferredTimeout(rtpStack->getJitterPktsInQueueCount(),
                                                     rtpStack->getCurMaxPktsInQueue());
        if (pref + lastRtpRetrieve > now) {
            timeout = min(timeout, (pref + lastRtpRetrieve) - now);
        }
        else {
            timeout = 0;
        }
    }
    return 0;
}//setFds


void 
MRtpSession::processRecv(RtpPacket& packet, const NetworkRes& sentBy) {
    // Cache the session obj, but have to protect this with a lock to keep
    // it from dis-appearing while we are using it!
    if ((mySession == 0) || (mySessionId != mySession->getSessionId())) {
        mySession = MediaController::instance().getSession(mySessionId);
    }

    if (mySession == 0) {
        cpLog(LOG_ERR, "ERROR:  Could not get session: %d\n", mySessionId);
    }
    else {
        mySession->processRaw((char*) packet.getPayloadLoc(), packet.getPayloadUsage(),
                              myCodec->getType(), myCodec, this,
                              packet.isMissing() || packet.isSilenceFill());
    }
}

///Consume the raw data
void 
MRtpSession::sinkData(char* data, int length, VCodecType type,
                      Sptr<CodecAdaptor> codec, bool silence_pkt) {

    //if the codec type is not same as the network type of the session
    //convert back, say for example if data is PCMU and sent over session codec
    //is G729, convert data first to G729. If G729 data is coming as input, no converssion
    //is needed and send it as is
    //Send RTP packet to the destination
    cpLog(LOG_DEBUG_STACK, "%s sinking data", description().c_str());
    int retVal = 0;
    if(type == DTMF_TONE) {
        //First 8 bits will represent the event
        char event;
        memcpy(&event, data, 1);
        cerr << "Sending digit:" << (int)event << endl;
        retVal = rtpStack->transmitEvent(event);
    }
    else {
        if (type != myCodec->getType()) {
            //First convert the data from type to myType and then feed
            //it to the soundcard
            cpLog(LOG_DEBUG_STACK, "Converting outgoing data, type: %d  myCodec.type: %d, codec: %p",
                  type, myCodec->getType(), codec.getPtr());

            if (codec.getPtr() == 0) {
                codec = MediaController::instance().getMediaCapability().getCodec(type);
            }

            ///Convert from codec type to PCM
            int decLen = 1024;
            char decBuf[1024];
            int decodedSamples = 0;
            int decodedPerSampleSize = 0;

            //string dbg;
            //vhexDump(data, length, dbg);
            //cpLog(LOG_ERR, "Before decode: %s", dbg.c_str());

            codec->decode(data, length, decBuf, decLen, decodedSamples,
                          decodedPerSampleSize);

            //vhexDump(data, decLen, dbg);
            //cpLog(LOG_ERR, "After decode: %s", dbg.c_str());

            int encLen = 1024;
            char encBuf[1024];
            myCodec->encode(decBuf, decodedSamples, decodedPerSampleSize,
                            encBuf, encLen);

            //vhexDump(data, encLen, dbg);
            //cpLog(LOG_ERR, "After encode: %s", dbg.c_str());

            if (encLen > 0) { 
                retVal = rtpStack->transmitRaw(encBuf, encLen);
            } else { 
                // Don't trasmit if there has been an error
                cpLog(LOG_ERR, "Coding error, no data returned from codec");
            }
        } else {
            //string dmp;
            //hexDump(data, length, dmp);
            //cpLog(LOG_DEBUG_STACK, "Transmit raw:%d\n%s", length, dmp.c_str());
            retVal = rtpStack->transmitRaw(data, length);
        }
    }

    if (retVal < 0) {
        cpLog(LOG_ERR, "Failed to transmit RTP packets to %s:%d", 
                       myRemoteAddress->getIpName().c_str(),
                       myRemoteAddress->getPort());
    }
}//sinkData


string MRtpSession::description() {
    char buf[256];
    snprintf(buf, 255, "%s:%d->%s:%d", myLocalAddress->getIpName().c_str(),
             myLocalAddress->getPort(),
             myRemoteAddress->getIpName().c_str(),
             myRemoteAddress->getPort());
    return buf;
}

void MRtpSession::recvDTMF(int event) {
    cpLog(LOG_ERR, "Received DTMF: %d\n", event);
    char eventData = event;
    Sptr<CodecAdaptor> nll;
    MediaController::instance().getSession(mySessionId)->processRaw(&eventData, 1, DTMF_TONE,
                                                                    nll, this, false);
}

void 
MRtpSession::adopt(SdpSession& remoteSdp)
{
    Data rAddress;
    int rPort = -1;
    //Check for address in Media part if found find else use the
    //Global address
    list<SdpMedia*> mediaList = remoteSdp.getMediaList();
    for(list<SdpMedia*>::iterator itr = mediaList.begin(); itr != mediaList.end(); itr++)
    {
        if((*itr)->getMediaType() == Vocal::SDP::MediaTypeAudio)
        {
            if((*itr)->getConnection())
            {
                cpLog(LOG_DEBUG, "Connection address found in media attribute, using it");
                rAddress = (*itr)->getConnection()->getUnicast();
                break;
            }
            rPort = (*itr)->getPort();
        }
    }

    if(rAddress.length() == 0)
    {
        //Check to see if Session-level connection address is present
        SdpConnection* conn = remoteSdp.getConnection();
        if(conn) rAddress = conn->getUnicast();
        if(rAddress.length())
        {
            cpLog(LOG_DEBUG, "Using Top-level connection address");
        }
        else
        {
            cpLog(LOG_ERR, "Bad SDP");
        }
    }
    myRemoteAddress->setHostName(rAddress);
    myRemoteAddress->setPort(rPort);
    /*********Rajarshi************/
    int remotePort  = myRemoteAddress->getPort();
    int remoteRtcpPort = ( remotePort > 0) ? remotePort + 1 : 0;
    rtpStack->setSessionState(rtp_session_sendrecv);

    // TODO:  I'm guessing we should ge the protocol and other info out of
    // the SDP thing too...  What if we're not running PCMU? --Ben
    cpLog(LOG_ERR, "WARNING:  in MRtpSession::adopt, not sure this method works right!");
    rtpStack->setTransmiter(  myLocalAddress->getIpName().c_str(),
                              localDevToBindTo,
                              myRemoteAddress->getIpName().c_str(), remotePort,
                              remoteRtcpPort, rtpPayloadPCMU, 8000, 1, 160);
   /*****************************/

}

void
MRtpSession::setMode(VSdpMode mode)
{
    switch(mode)
    {
        case VSDP_SND:
            rtpStack->setSessionState(rtp_session_sendonly);
        break;
        case VSDP_RECV:
            rtpStack->setSessionState(rtp_session_recvonly);
        break;
        case VSDP_SND_RECV:
            rtpStack->setSessionState(rtp_session_sendrecv);
        break;
    }
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
