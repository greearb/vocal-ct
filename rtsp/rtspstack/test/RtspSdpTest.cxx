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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspSdpTest_cxx_version =
    "$Id: RtspSdpTest.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <iostream.h>

#include "RtspSdp.hxx"
#include "RtspRangeHdr.hxx"
#include "cpLog.h"

int main( int argc, char *argv[] )
{
    cpLogSetPriority( LOG_DEBUG_STACK);

    //create SDP on the fly
    RtspSdp mySdp;

    mySdp.setConnAddress("www.vovida.com");
    mySdp.setOwnerAddress("10.0.0.1");
    mySdp.setUserName("wjin");
    mySdp.setSessionName("vovida rtsp session");
    // set the aggregated control
    mySdp.addAttribute("control", "rtsp://www.vovida.org/audio/");

    mySdp.setPort(2345);
    mySdp.setMediaConnAddress("www.vovida.org");
    mySdp.addMediaAttribute("control", "stream=1");
    mySdp.addMediaAttribute("ptime", Data(100));

    RtspRangeHdr range(0.0, 3.9); 
    mySdp.addMediaAttribute("range", range.encodeNPT());

    mySdp.addFormat(0); 
    mySdp.addRtpmap(0, "PCMU", 8000);
    mySdp.addFormat(12);
    mySdp.addRtpmap(12, "QCELP", 8000);

    cout <<"Encoded sdp data:"<<endl;
    cout <<mySdp.encode().getData()<<endl;

    // get a msg contentData, and parse the SDP block
    // TODO warning: a=control:rtsp://... will having problem
    // because of the second ":" after "rtsp". Need SDP fixes
    Data sdpData("v=0\r\no=wjin -1103601275 -1103601275 IN IP4 10.0.0.1\r\ns=vovida rtsp session\r\nc=IN IP4 www.vovida.com\r\nt=3191369266 0\r\na=control:rtsp://www.vovida.org/audio/\r\nm=audio 2345 RTP/AVP 0 12\r\nc=IN IP4 www.vovida.org\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:12 QCELP/8000\r\na=control:stream=1\r\na=ptime:100\r\na=range:npt=0.000-3.900\r\n");
    RtspSdp theSdp(sdpData);

    cout << " ConnAddr: " << theSdp.getConnAddress().getData() << endl;
    cout << " OwnerAddr: " << theSdp.getOwnerAddress().getData() << endl;
    cout << " UserName: " << theSdp.getUserName().getData() << endl;
    cout << " SessName: " << theSdp.getSessionName().getData() << endl;
    cout << " Aggregated control= " << theSdp.getAttributeValue("control").getData() << endl;

    cout << " port: " << theSdp.getPort() << endl;
    cout << " MediaConn: " << theSdp.getMediaConnAddress().getData() << endl;
    cout << " media control= " << theSdp.getMediaAttributeValue("control").getData() << endl;
    cout << " ptime= " << theSdp.getMediaAttributeValue("ptime").getData() << endl;
    cout << " range= " << theSdp.getMediaAttributeValue("range").getData() << endl;

    RtspRangeHdr rghdr;
    rghdr.parseRange(theSdp.getMediaAttributeValue("range").getData());
    cout << "start time: " << rghdr.getStartTime() << endl;
    cout << "end time: " << rghdr.getEndTime() << endl;
    

    cout << "format list 0:" <<  ( *(theSdp.getFormatList()) )[0] << endl;

    cout << "format list 1:" << ( *(theSdp.getFormatList()) )[1] << endl;

    cout << "rtpmap list 0:" << 
      ( *(theSdp.getRtpmapList()) )[0]->getPayloadType() << endl;

    cout << "rtpmap list 0:" << 
      ( *(theSdp.getRtpmapList()) )[0]->getEncodingName() << endl;
    cout << "rtpmap list 0:" << 
      ( *(theSdp.getRtpmapList()) )[0]->getClockRate() << endl;

    cout << "rtpmap list 1:" << 
      ( *(theSdp.getRtpmapList()) )[1]->getPayloadType() << endl;

    return 0;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
