// copyright notice at end of file.

static const char* const Sdp2Example1_hxx_Version =
    "$Id: Sdp2Example.cxx,v 1.1 2004/05/01 04:15:24 greear Exp $";

#include "Sdp2Session.hxx"

using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::SdpRtpMapAttribute;


int main()
{
    SdpSession sdpDesc;
    
    // v=0
    sdpDesc.setVersion(0);
    
    // this is responsible for c=IN IP4 10.0.0.1
    SdpConnection connection;
    connection.setUnicast("10.0.0.1"); // the 10.0.0.1
    connection.setAddressType(Vocal::SDP::AddressTypeIPV4); // the IP4
    sdpDesc.setConnection(connection);
    
    // the o= line is mostly automatically generated, but these two fields 
    // need to be filled in
    sdpDesc.setAddress("10.0.0.1");
    sdpDesc.setAddressType(Vocal::SDP::AddressTypeIPV4); // the IP4

    // the t= line is also automatically filled in

    // now, set the audio format.  this is responsible for the
    // m=audio 3456 RTP/AVP 0
    SdpMedia* media = new SdpMedia();
    
    media->clearFormatList();
    // responsible for the audio
    media->setMediaType(Vocal::SDP::MediaTypeAudio); 
    // responsible for the 3456
    media->setPort( 3456 );
    // RTP/AVP
    media->setTransportType(Vocal::SDP::TransportTypeRTP);
    
    // here, set the payload type.  this 0 represends G.711 mu-law,
    // and is the 0 at the end of m= line above
    media->addFormat(0);
    
    // now, add a media object
    sdpDesc.addMedia(media);

    // declare the media attribute list pointer.  it creates an line 
    // a=sendrecv
    MediaAttributes* mediaAttrib = new MediaAttributes();
    // mark as  sendrecv
    mediaAttrib->setsendrecv();
    media->setMediaAttributes(mediaAttrib);
    // glue it in

    // this section is responsible for a=rtpmap:0 PCMU/8000
    SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
    rtpMapAttrib->setPayloadType(0); // again, G.711 mu-law
    rtpMapAttrib->setEncodingName("PCMU"); 
    rtpMapAttrib->setClockRate(8000);
    mediaAttrib->addmap(rtpMapAttrib);

    sdpDesc.setSessionName("Vocal-Media-Lib");
    
    // thus, the final result
    cout << sdpDesc.encode() << endl;
    return 0;
}


/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2003 Vovida Networks, Inc.  All rights reserved.
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
