
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

static const char* const SendSIPTest_cxx_Version =
    "$Id: SendSIPTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "global.h"

static const char* version UNUSED_VARIABLE =
    "$";



#include <cassert>
#include <sys/time.h>
#include <cpLog.h>
#include "InviteMsg.hxx"


#include "SipTransceiverFilter.hxx"
#include <VTest.hxx>

using namespace Vocal;

///
class SendSIPTest : protected VTest
{
    public:
        ///
        SendSIPTest(int sipPort = 5065);
        ///
        SendSIPTest(const SendSIPTest&);
        ///
        void send(SipCommand& sipMsg , bool async);
        ///
        void sendReply(StatusMsg& statusMsg);
        ///
        ~SendSIPTest();

    private:
        ///
        SipTransceiverFilter tranceiverFilter;
};


SendSIPTest::SendSIPTest(int sipPort)
        : VTest("SendSIPTest"),
        tranceiverFilter("test", sipPort)
{}


SendSIPTest::SendSIPTest(const SendSIPTest& src)
        : VTest("SendSIPTest")
{
    cpLog(LOG_DEBUG, "Need to implement the copy ctor for SendSIPTest");
    assert(0);
}

SendSIPTest::~SendSIPTest()
{}



void
SendSIPTest::send(SipCommand& sipMsg , bool async)
{
    tranceiverFilter.sendAsync(sipMsg);
}


void
SendSIPTest::sendReply(StatusMsg& statusMsg)
{
    tranceiverFilter.sendReply(statusMsg);
}

int main(int argc, char**argv)
{
    SendSIPTest sendtest;

    //    cpLogSetPriority(LOG_DEBUG_STACK);

    Data url;
    url = "sip:destination@127.0.0.1:5070";
    Sptr <BaseUrl> tourl;
    tourl = BaseUrl::decode(url);

    int listenPort = 5065;
    int rtpPort = 3456;
    cpLog(LOG_DEBUG, "Completed the Url decoding");

    InviteMsg invite(tourl, listenPort, rtpPort);

    int maxMessages = 1;
    if (argc > 1)
    {
        maxMessages = atoi(argv[1]);
    }

    for (int i = 0; i < maxMessages; i++)
    {
        {
            SipCallId id;
            invite.setCallId(id);
            sendtest.send(invite, true);
        }
    }
    //    return 1;
}
