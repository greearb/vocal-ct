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

static const char* const ByeMsgTest_cxx_Version =
    "$Id: ByeMsgTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "global.h"

#include <cassert>
#include <sys/time.h>
#include <cpLog.h>
#include "InviteMsg.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "ByeMsg.hxx"
#include "Verify.hxx"
#include "SipContact.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;

void testEncodeDecode()
{
    string host_addr = theSystem.gethostAddress();
    Data url;
    url = "sip:destination@192.168.5.12";
    Sptr <BaseUrl> tourl;
    tourl = BaseUrl::decode(url, host_addr);
    int listenPort = 5060;
    int rtpPort = 3456;
    cpLog(LOG_DEBUG, "Completed the Url decoding");

    InviteMsg invite(tourl, host_addr, listenPort, rtpPort);
    StatusMsg status(invite, 200);
    SipContact myContact(url, host_addr);
    status.setContact(myContact);

    AckMsg ack(status, host_addr);

    //construct bye on status.
    ByeMsg bye(status, host_addr);
    if(status.getNumRecordRoute() == 0)
    {
        SipContact cnt = status.getContact();
        bye.getMutableRequestLine().setUrl(cnt.getUrl());
    }

    Data data = bye.encode();


    ByeMsg newBye(data, host_addr);
    Data newdata = newBye.encode();


    //construct bye on Ack.
    ByeMsg byeAck(ack);
    Data byeAckdata = byeAck.encode();

    ByeMsg newbyeAck(byeAckdata, host_addr);
    Data newbyeAckdata = newbyeAck.encode();

    
    LocalScopeAllocator lo;
    test_verify (data == newdata);
    test_verify (byeAckdata ==  newbyeAckdata);
}

int main()
{

//    cpLogSetPriority(LOG_DEBUG);
#if 0
    Data url;
    url = "sip:destination@192.168.5.12";
    SipUrl tourl(url);
#endif


    testEncodeDecode();

    return test_return_code(2);

}
