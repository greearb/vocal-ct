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

static const char* const AckMsgTest_cxx_Version =
    "$Id: AckMsgTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "global.h"


#include <cassert>
#include <sys/time.h>

#include "cpLog.h"

#include "InviteMsg.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"

#include "Verify.hxx"
#include "SipVia.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;

char* msgtext[] = {
    "SIP/2.0 302 Moved Temporarily\r
Via: SIP/2.0/UDP 192.168.66.103:5060;branch=0258542943543c0dd9bfd645ef51e3a9.1\r
Via: SIP/2.0/UDP 192.168.66.2:5060\r
To: 5221<sip:5221@192.168.66.103:5060;user=phone>\r
From: UserAgent<sip:5220@192.168.66.2:5060;user=phone>\r
Call-ID: 209747329338013@192.168.66.2\r
CSeq: 1 INVITE\r
Record-Route: <sip:5221@192.168.66.103:5060>\r
Content-Length: 0\r
\r
"
};


void
testEncodeDecode()
{
    Data url;
    url = "sip:destination@192.168.5.12";
    Sptr <BaseUrl> tourl;
    tourl = BaseUrl::decode(url, host_addr);
    int listenPort = 5060;
    int rtpPort = 3456;

    InviteMsg invite(tourl, host_addr, listenPort, rtpPort);
    StatusMsg status(invite, 200);

    SipCSeq oldCSeq = status.getCSeq();

    test_verify(status.getCSeq() == oldCSeq);


    test_verify(status.getCSeq() == oldCSeq);

    AckMsg ack(status, host_addr);
    ack.getMutableRequestLine().setUrl(tourl);

    Data data = ack.encode();
    AckMsg newAck(data, host_addr);

    Data newdata = newAck.encode();

    test_verify(data == newdata);

    {
            StatusMsg status(msgtext[0]);
            AckMsg ack(status, host_addr);

            cout << "Status: " << status.getVia(0).encode() << endl;
            cout << "Ack:  : " << ack.getVia(0).encode() << endl;
            test_verify(ack.getVia(0).encode() == "Via: SIP/2.0/UDP 192.168.66.103:5060;branch=0258542943543c0dd9bfd645ef51e3a9.1\r\n");
    }
}



int main()
{
//    cpLogSetPriority(LOG_DEBUG);

    testEncodeDecode();

    return test_return_code(4);
}


// Local Variables:
// mode:c++
// c-file-style:"bsd"
// c-basic-offset:4
// c-file-offsets:((inclass . ++))
// indent-tabs-mode:nil
// End:
