
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

static const char* const ReceiveSIPTest_cxx_Version =
    "$Id: ReceiveSIPTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "global.h"

static const char* version UNUSED_VARIABLE =
    "$";



#include <cassert>
#include <sys/time.h>
#include <cpLog.h>
#include "SipTransceiver.hxx"
#include <VTest.hxx>

using namespace Vocal;


///
class ReceiveSIPTest : protected VTest
{
    public:
        ///
        ReceiveSIPTest(int sipPort = 5070);
        ///
        ReceiveSIPTest(const ReceiveSIPTest&);
        ///
        ~ReceiveSIPTest()
        {
            receiveThread.join();
        }

    private:
        ///
        SipTransceiver tranceiverFilter;
        ///
        VThread receiveThread;
        ///
        static void* receiveThreadWrapper(void* session);
        ///
        void receiveMain();
};



ReceiveSIPTest::ReceiveSIPTest(int sipPort)
        : VTest("ReceiveSIPTest"),
        tranceiverFilter("test", sipPort)
{
    //start the receive thread
    receiveThread.spawn(receiveThreadWrapper, this);
    cpLog( LOG_DEBUG, "Spawning the receive thread");
}

ReceiveSIPTest::ReceiveSIPTest(const ReceiveSIPTest& src)
        : VTest("ReceiveSIPTest")
{
    cpLog(LOG_DEBUG, "Need to implement the copy ctor for ReceiveSIPTest");
    assert(0);

}


void*
ReceiveSIPTest::receiveThreadWrapper(void* trans)
{
    assert( trans );

    ReceiveSIPTest* s = static_cast < ReceiveSIPTest* > (trans);
    s->receiveMain();

    return 0;
}


void
ReceiveSIPTest::receiveMain()
{
    cpLog(LOG_DEBUG, "starting receiveMain");

    int count = 0;

    while (1)
    {
        Sptr < SipMsgQueue > sipReceive;
        sipReceive = tranceiverFilter.receive();
        if (sipReceive != 0)
        {
            count++;
            if ((count % 100) == 0)
            {
                cout << "count: " << count << endl;
            }
            //sipMsg = tranceiverFilter.receiveAsync();
#if 0
            SipMsgQueue::iterator i;
            cpLog( LOG_DEBUG, "RECEIVED msG");
            for (i = sipReceive->begin(); i != sipReceive->end(); ++i)
            {
                Sptr < SipMsg > sipMsg = *i;
                if (sipMsg != 0)
                {
                    cpLog( LOG_DEBUG, "RECEIVED NEW SIP MSG: %s", sipMsg->encode().logData());
                }
            }
#endif
        }
    }//while 1
}



int main()
{
    cpLogSetPriority(LOG_DEBUG);
    ReceiveSIPTest receivetest;
}
