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

static const char* const NegotiateSdp_cxx_Version =
    "$Id: NegotiateSdp.cxx,v 1.1 2004/05/01 04:15:16 greear Exp $";

#include "MediaController.hxx"
#include "Sdp2Session.hxx"
#include "Verify.hxx"

using namespace Vocal;
using Vocal::SDP::SdpSession;
using namespace Vocal::UA;


void test()
{
    {
	// test a simple SipUrl
        MediaController::instance(10000, 10005);
        {
            //Create a Remote SDP object
            Data s("v=0\r\no=- 1025036129 1025036129 IN IP4 192.168.123.10\r\ns=SIP Call\r\nt=0 0\r\nm=audio 4136 RTP/AVP 0 100\r\nc=IN IP4 192.168.123.10\r\na=rtpmap:0 pcmu/8000\r\na=rtpmap:100 telephone-event/8000\r\na=fmtp:100 0-15\r\n");
            SdpSession session;
            session.decode(s);
            SdpSession localSdp = MediaController::instance().createSession(session);
            NetworkAddress na;
	    test_verify(localSdp.getConnection()->getUnicast() == Data(na.getIpName().c_str()));
        }
        {
            //Create a Remote SDP object
            Data s("v=0\r\no=- 1025036129 1025036129 IN IP4 192.168.123.10\r\ns=SIP Call\r\nt=0 0\r\nm=audio 4136 RTP/AVP 0 \r\nc=IN IP4 192.168.123.10\r\na=rtpmap:0 pcmu/8000\r\n");
            SdpSession session;
            session.decode(s);
            SdpSession localSdp = MediaController::instance().createSession(session);
            NetworkAddress na;
	    test_verify(localSdp.getConnection()->getUnicast() == Data(na.getIpName().c_str()));
        }
        {
            Data s("v=0\r\no=- 1025036129 1025036129 IN IP4 192.168.123.10\r\ns=SIP Call\r\nt=0 0\r\nm=audio 4136 RTP/AVP 18 \r\nc=IN IP4 192.168.123.10\r\na=rtpmap:18 G729/8000\r\n");
            SdpSession session;
            session.decode(s);

            try
            {
                SdpSession localSdp = MediaController::instance().createSession(session);
	        test_verify(0);
            }
            catch (MediaException& e)
            {
	        test_verify(1);
            }
        }
    }
}

int main()
{
    test();
    return test_return_code(3);
}
