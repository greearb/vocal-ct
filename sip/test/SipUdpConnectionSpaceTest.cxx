
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

static const char* const SipUdpConnectionSpaceTest_cxx_Version =
    "$Id: SipUdpConnectionSpaceTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
// simple test case

#include "SipUdpConnection.hxx"
#include "Fifo.h"
#include "SipMsg.hxx"
#include <iostream>
#include "InviteMsg.hxx"
#include "VThread.hxx"

VMutex mutex;
VCondition condition;

void* thread_0(void*)
{
    int count = 0;
    Fifo < Sptr < SipMsg > > fifo1;
    SipUdpConnection conn1(&fifo1);

    Sptr <BaseUrl> baseUrl;
    baseUrl = BaseUrl::decode(Data("sip:destination@127.0.0.1:5070"));
    
    Sptr < InviteMsg > send1
    = new InviteMsg(baseUrl, 5060, 2000);
    SipTransactionId id(*send1);
    while (1)
    {
        conn1.send(send1);
        Sptr < SipMsg > get1
        = fifo1.getNext();
        count++;
        if ((count % 1000) == 0)
        {
            cout << "sent: " << count << endl;
            usleep(1);
            //	    condition.wait(&mutex, -1);
        }
        //	conn1.cancel(id);
    }

    return 0;
}

void* thread_1(void*)
{
    int count = 0;
    Fifo < Sptr < SipMsg > > fifo2;
    SipUdpConnection conn2(&fifo2, 5070);

    SipTransactionId* id = 0;

    Sptr <BaseUrl> baseUrl;
    baseUrl = BaseUrl::decode(Data("sip:destination@127.0.0.1:5060") );
    
    Sptr < InviteMsg > send1 = new InviteMsg(baseUrl, 5070, 2000);

    while (1)
    {
        Sptr < SipMsg > get1
        = fifo2.getNext();
        conn2.send(send1);
        count++;

        //
        if (id == 0)
        {
            id = new SipTransactionId(*get1);
        }

        if ((count % 1000) == 0)
        {
            cout << "rec:  " << count << endl;
            //	    condition.signal();
        }
        //	conn2.cancel(*id);
    }

    return 0;
}


// unfortunately, this won't quite be automatic... ?

int main()
{
    //    cpLogSetPriority(LOG_DEBUG_STACK);
    SipUdpConnection::reTransOff();
    VThread thread0;
    VThread thread1;

    thread0.spawn(&thread_0, 0);
    thread1.spawn(&thread_1, 0);
    thread0.join();
    thread1.join();

    return 0;
}
