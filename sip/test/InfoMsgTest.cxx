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

static const char* const InfoMsgTest_cxx_Version =
    "$Id: InfoMsgTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipAuthorization.hxx"
#include <cassert>
#include "cpLog.h"
#include "InfoMsg.hxx"
#include "Verify.hxx"
#include "Sptr.hxx"
#include "BaseUrl.hxx"
#include "SipUser.hxx"

using namespace Vocal;
    
//message sent by telecaster
char* msgtext1[] = { 
"INFO sip:192.168.2.208:5061 SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.2.205:7777\r\n\
From: <sip:000013@192.168.2.206:5060;user=phone>\r\n\
To: <sip:000013@192.168.2.206:5060;user=phone>\r\n\
Call-ID: 000013@192.168.2.206:5060\r\n\
CSeq: 2 INFO\r\n\
Route: <sip:5001@172.19.174.64:5060;maddr=172.19.174.64>,<sip:5001@172.19.174.166:5060>\r\n\
User-Agent: Cisco ATA182 v.1.44\r\n\
Content-Length: 37\r\n\
\r\n\
You have (30) seconds for the call\r\n\
",

};    
    
void testEncodeDecode()
{
    {
	InfoMsg im1(msgtext1[0]);
        InfoMsg im2;
        im2 = im1;
	test_verify(im1 == im2);
	test_verify(im1.encode() == im2.encode());
    }
}


int main()
{
    testEncodeDecode();

    return test_return_code(2);
}
