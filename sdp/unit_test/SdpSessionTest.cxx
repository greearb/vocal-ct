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

static const char* const SdpAttributeTest_cxx_Version =
    "$Id: SdpSessionTest.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include "Verify.hxx"
#include "SdpSession.hxx"

int main()
{
    {
        SdpSession sdpDesc;
        SdpConnection connection;
        connection.setUnicast( "127.0.0.1"  );
        sdpDesc.setConnection( connection );

        SdpMedia* media = new SdpMedia;
	// Get RTP port number from UaDevice
	int rtpPort = 3456;

        media->setPort( rtpPort );   // which port to use when making a call
        // RTP doesn't have a suggestd number

        sdpDesc.addMedia( media );

	int verStr = 844111165;
	
	int sessionStr = 844111165;
	
	sdpDesc.setVersion(verStr);
	sdpDesc.setSessionId(sessionStr);

	sdpDesc.clearSdpTimeList();
	sdpDesc.addTime(SdpTime(0,0));

	cout << sdpDesc.encode() << endl;

	test_verify(sdpDesc.encode() == "v=0\r\no=- 844111165 844111165 IN IP4 0.0.0.0\r\ns=\r\nc=IN IP4 127.0.0.1\r\nt=2208988800 2208988800\r\nm=audio 3456 RTP/AVP 0\r\n");
	



	{
	    SdpSession copy;
	    copy = sdpDesc;
	}
	SdpSession copy2(sdpDesc);
	{
	    SdpSession copy;
	    copy = sdpDesc;
	}
	{
	    string s("v=0\r\no=CiscoSystemsSIP-IPPhone-UserAgent 2030 7663 IN IP4 10.22.76.105\r\ns=SIP Call\r\nc=IN IP4 0.0.0.0\r\nt=0 0\r\nm=audio 27774 RTP/AVP 0 100\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:100 telephone-event/8000\r\na=fmtp:100 0-15\r\n");
	    SdpSession session;
	    session.decode(s);
	    
	    test_verify(session.isHold());
	}

    }

    return test_return_code(2);
}
