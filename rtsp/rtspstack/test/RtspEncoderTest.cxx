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

static const char* const RtspEncoderTest_cxx_version =
    "$Id: RtspEncoderTest.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <stdio.h>
#include <string.h>

#include "RtspMsg.hxx"
#include "RtspRequest.hxx"
#include "RtspMsgParser.hxx"
#include "RtspResponse.hxx"
#include "cpLog.h"

int main( int argc, char *argv[] )
{
    cpLogSetPriority( LOG_DEBUG_STACK);

    char* msg = "DESCRIBE rtsp://www.vovida.com/vovida/abc.wav RTSP/1.0\r\nContent-Base : rtsp://10.0.0.1:554 user\r\nCseq: 1\r\nTransport: RTP/AVP;multicast;ttl=127;mode=\"PLAY\", RTP/AVP;unicast; client_port=3456-3457;mode=\"PLAY\"; destination=vovida.com ; source=10.0.1.1:3030 \r\nContent-Length: 8\r\nRange: npt=.1-.3;time=\r\nAccept : application/sdp; level=2, application/rtsl \r\nSession:123\r\n\r\n12345678";
    int bytesNeedtoDecode = strlen( msg );
    int bytesDecoded = 0;
    printf( "%s\n(%d)\n", msg, bytesNeedtoDecode );

    Sptr<RtspMsg> rtspMsg = 0;
    rtspMsg = RtspMsgParser::preParse( msg, bytesNeedtoDecode, bytesDecoded, 0);

    Sptr<RtspRequest> request = 0;
    request.dynamicCast(rtspMsg);
    assert (request != 0);
    if (rtspMsg->getCompletionFlag() == RTSP_MSG_COMPLETE)
    {
        RtspResponse response(request, RTSP_410_STATUS);
        response.appendTransportHdr(request, 5678, 5679);
        response.appendNPTRangeHdr(1234.0, 12346.8897);
        response.appendConnectionCloseHdr();
        response.appendEndOfHeaders(); // do this when there is no sdp data
//        response.appendSdpData("sdpsdpsdpsdpsdp", 15);

        printf("The response is: \n%s\n", (response.encode()).getData());
    }

    return 0;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
