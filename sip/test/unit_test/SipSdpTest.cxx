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

static const char* const SipSdpTest_cxx_version =
    "$Id: SipSdpTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "SipSdp.hxx"
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::SdpTime;

void test()
{

    {
	Data correct = "v=0\r\no=user 4817875 183425 IN IP4 127.0.0.1\r\ns=session\r\nc=IN IP4 127.0.0.1\r\nt=2208988800 2208988800\r\nm=audio 5000 RTP/AVP 0\r\n";

	SipSdp sdp("", host_addr);
	//Change this so that we call functions in SDP, and SDP abstracts this
	//from us.
	// set connection details.
	SdpSession sdpDesc;

	Data connAddr = "127.0.0.1";
	SdpConnection connection;
        LocalScopeAllocator lo;
	connection.setUnicast(connAddr.getData(lo));
	sdpDesc.setConnection(connection);

	SdpMedia* media = new SdpMedia;
	media->setPort(5000);

	sdpDesc.addMedia(media);

	int verStr = 183425;

	int sessionStr = 4817875;

	sdpDesc.setVersion(verStr);
	sdpDesc.setSessionId(sessionStr);

	sdpDesc.clearSdpTimeList();
	sdpDesc.addTime(SdpTime(0,0));

	//To further perform any other operation on that particular media
	//item, use loc.
	sdp.setSdpDescriptor(sdpDesc);
	sdp.setUserName("user");

	sdp.setSessionName("session");
	sdp.setAddress("127.0.0.1");

	int spare;
	Data result = sdp.encodeBody(spare);
//	cout << result << endl;
	test_verify(result == correct);
    }    
}

int main()
{
    test();
    return test_return_code(1);
}
