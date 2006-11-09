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

static const char* const SdpHandler_cxx_Version =
    "$Id: SdpHandler.cxx,v 1.2 2006/11/09 00:11:43 greear Exp $";

#include "SdpHandler.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Connection.hxx"
#include "Sdp2Media.hxx"
#include "MediaController.hxx"
#include "MediaCapability.hxx"

using namespace Vocal;

using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::SdpRtpMapAttribute;

using namespace Vocal::UA;

void
addMediaAttribute(SdpMedia* sdpMedia, Sptr<CodecAdaptor> cAdp)
{
    //Set the payload type
    assert(cAdp != 0);

    MediaAttributes* mediaAttrib = sdpMedia->getMediaAttributes();
    assert(mediaAttrib);
    //add the rtpmap attribute for the default codec
    SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();

    // So, if the type is not well known, then we'll have to just make
    // up a number > 96.
    // TODO:  Add other odd-ball codecs here.
    if (cAdp->getType() == SPEEX) {
       SdpRtpMapAttribute junk;
       for (int i = rtpPayloadDynMin; i<rtpPayloadDynMax; i++) {
          if (mediaAttrib->getRtpmapAttributeValue(i, junk) < 0) {
             // Ok, found one that doesn't already exist.
             sdpMedia->addFormat(i);
             rtpMapAttrib->setPayloadType(i);
             break;
          }
       }
    }
    else {
       sdpMedia->addFormat(cAdp->getType());
       rtpMapAttrib->setPayloadType(cAdp->getType());
    }

    rtpMapAttrib->setEncodingName(cAdp->getEncodingName().c_str());
    rtpMapAttrib->setClockRate(cAdp->getClockRate());
    // add the value attribute just created to the media attribute object
    mediaAttrib->addmap(rtpMapAttrib);

    const map<string, string>& vMap =  (cAdp->getAttrValueMap());
    for(map<string, string>::const_iterator itr2 = vMap.begin();
        itr2 != vMap.end(); itr2++)
    {
        // create the new value attribute object
        ValueAttribute* attrib = new ValueAttribute();
        attrib->setAttribute((*itr2).first.c_str());
        attrib->setValue((*itr2).second.c_str());
        mediaAttrib->addValueAttribute(attrib);
    }
}


void
setHost(SdpSession& sdpSess, Data hostAddr)
{
    // set IP address and port 
    SdpConnection connection;
    LocalScopeAllocator lo;
    connection.setUnicast(hostAddr.getData(lo));
    if(hostAddr.find(":") != Data::npos)
    {
        connection.setAddressType(SDP::AddressTypeIPV6);
        sdpSess.setAddressType(SDP::AddressTypeIPV6);
    }
    sdpSess.setConnection(connection);
    sdpSess.setAddress(hostAddr);
}


void
setStandardSdp(SdpSession& sdpDesc, Data hostAddr, int port, VSdpMode m)
{
    setHost(sdpDesc, hostAddr);

    // do the media stuff
    sdpDesc.flushMediaList();

    //Set audio formats
    list<Sptr<CodecAdaptor> > cList = 
             MediaController::instance().getMediaCapability().getSupportedAudioCodecs();
    if(cList.size())
    {
        SdpMedia* sdpMedia = new SdpMedia();
        sdpMedia->clearFormatList();
        sdpMedia->setMediaType(Vocal::SDP::MediaTypeAudio);
        sdpMedia->setPort( port );
        sdpDesc.addMedia(sdpMedia);
        // declare the media attribute list pointer
        MediaAttributes* mediaAttrib = new MediaAttributes();

        switch(m)
        {
           case VSDP_SND:
               mediaAttrib->setsendonly();
               break;
           case VSDP_RECV:
               mediaAttrib->setrecvonly();
               break;
           case VSDP_SND_RECV:
               mediaAttrib->setsendrecv();
               break;
        }

        sdpMedia->setMediaAttributes(mediaAttrib);
        for(list<Sptr<CodecAdaptor> >::iterator itr = cList.begin();
                itr != cList.end(); itr++)
        {
            addMediaAttribute(sdpMedia, (*itr));
        }
        switch(m)
        {
           case VSDP_SND:
               mediaAttrib->setsendonly();
               break;
           case VSDP_RECV:
               mediaAttrib->setrecvonly();
               break;
           case VSDP_SND_RECV:
               mediaAttrib->setsendrecv();
               break;
        }

    }

    //Set the Video codecs if any supported
    //TODO

    sdpDesc.setSessionName( "Vocal-Media-Lib");
}


bool 
checkType(const SdpSession& sdp, int& realPayloadType, Data eventName)
{
    list < SdpMedia* > remoteMediaList = sdp.getMediaList();

    if (remoteMediaList.size() < 1)
    {
        cpLog(LOG_ERR, "did not find any media lists");
        return false;
    }

    if (remoteMediaList.size() > 1)
    {
        cpLog(LOG_WARNING, "Found %d media descriptors. Only using first one!",
              remoteMediaList.size());
    }

    MediaAttributes *mediaAttributes 
       = (*remoteMediaList.begin())->getMediaAttributes();
    if(mediaAttributes)
    {
        cpLog( LOG_DEBUG, "Has Media Attribute(s)" );
        vector < SdpRtpMapAttribute* > *rtpMaps = mediaAttributes->getmap();
        vector < SdpRtpMapAttribute* > ::iterator rtpMap;
        rtpMap = rtpMaps->begin();
        while (rtpMap != rtpMaps->end())
        {
            if( realPayloadType != (*rtpMap)->getPayloadType()) 
            {
                ++rtpMap;
                continue;
            }

            Data encodingName = (*rtpMap)->getEncodingName();
            
            if (isEqualNoCase(encodingName, eventName))
            {
                cpLog(LOG_DEBUG, 
                      "Has payload type %s, value %d",
                      encodingName.logData(),
                      realPayloadType);
                return true;
            }
            ++rtpMap;
        }
    }
    else
    {
        cpLog( LOG_DEBUG, "No Media Attribute" );
        return false;
    }
    return false;
}

void negotiateSdp(SdpSession& sdpSess, string localAddr, int localPort, 
                  const SdpSession& remoteSdp) {
    setHost(sdpSess, localAddr);
    //Based on the remote SDP and capability, create a
    //compatible Local SDP and reserve associated Media
    //Resources - TODO, for now just create a lame SDP and
    //reserve a local audio device

    //Handle the audio first
    list < SdpMedia* > mList = remoteSdp.getMediaList();
    list<VCodecType> acList;
    MediaController& mInstance = MediaController::instance();
    for (list < SdpMedia* >::iterator itr = mList.begin();
         itr != mList.end(); itr++) {
       if ((*itr)->getMediaType() == Vocal::SDP::MediaTypeAudio) {
          vector <int>* fmList = (*itr)->getFormatList();
          if (fmList) {
             for(vector<int>::iterator itr = (*fmList).begin();
                 itr != (*fmList).end(); itr++) {
                int supportedType = (*itr);
                int st2 = supportedType;
                
                //In case of dynamic payload type, the remote and local may may differect types of the
                //same codec
                if (mInstance.getMediaCapability().isSupported(st2, remoteSdp, supportedType)) {
                   acList.push_back(static_cast<VCodecType>(supportedType));
                }
             }
          }
       }
    }
    
    if (acList.size() == 0) {
       cpLog(LOG_ERR, "Can not support any of the codecs in the offer.");
    }

    SdpMedia* sdpMedia = new SdpMedia();
    sdpMedia->clearFormatList();
    sdpMedia->setMediaType(Vocal::SDP::MediaTypeAudio);
    sdpMedia->setPort( localPort );
    sdpSess.addMedia(sdpMedia);

    MediaAttributes* mediaAttrib = new MediaAttributes();
    sdpMedia->setMediaAttributes(mediaAttrib);
       
    for (list<VCodecType>::iterator itr = acList.begin(); 
         itr != acList.end(); itr++) {
       Sptr<CodecAdaptor> cAdp = mInstance.getMediaCapability().getCodec(*itr);
       addMediaAttribute(sdpMedia, cAdp);
    }

    sdpSess.setSessionName( "Vocal-Media-Lib");

    //TODO -  Handle Video
}


