/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2001 Vovida Networks, Inc.  All rights reserved.
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

static const char* const ReferMsgTest_cxx_Version =
    "$Id: SipDataTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <cstdio>
#include <iostream>
#include "Sptr.hxx"
#include "Fifo.h"
#include "InviteMsg.hxx"
#include "SystemInfo.hxx"
#include "SipVia.hxx"
using namespace Vocal;

//#define  cpLog(pri, fmt, args)  cerr << (fmt) << (args) << endl
const char* inv_msg = "INVITE sip:6500@128.107.140.141:5065;user=phone SIP/2.0\r\n\
Via: SIP/2.0/UDP 128.107.140.141:5064\r\n\
From: 6400<sip:6400@128.107.140.141:5064>\r\n\
To: 6500<sip:6500@128.107.140.141:5065;user=phone>\r\n\
Call-ID: 247515495@128.107.140.140\r\n\
CSeq: 1 INVITE\r\n\
Subject: VovidaINVITE\r\n\
Session-Expires: 60;refresher=uac\r\n\
Min-SE: 30\r\n\
Contact: <sip:6400@128.107.140.141:5064>\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 168\r\n\
\r\n\
v=0\r\n\
o=- 99513809 99513809 IN IP4 128.107.140.140\r\n\
s=VOVIDA Session\r\n\
c=IN IP4 128.107.140.140\r\n\
t=962892393 0\r\n\
m=audio 3456 RTP/AVP 0\r\n\
a=rtpmap:0 PCMU/8000\r\n\
a=ptime:20\r\n\
\r\n";

Fifo<Sptr<InviteMsg> >  wFifo;
Fifo<Sptr<InviteMsg> >  sFifo;
SipVia gVia;
void* sendThreadWrapper(void* args)
{
    cerr << "sendThreadWrapper" << endl;
    while(1)
    {
        Sptr<InviteMsg> iMsg = sFifo.getNext();
	cpLog(LOG_ERR, "*****sendThreadWrapper, To:%s", iMsg->getTo().encode().logData());
	cpLog(LOG_ERR, "*****sendThreadWrapper, From:%s", iMsg->getFrom().encode().logData());
	cpLog(LOG_ERR, "*****sendThreadWrapper, Via:%s", iMsg->getVia(0).encode().logData());
    }
    return ((void*)0);
}

void* workerThreadWrapper(void* args)
{
    cerr << "workerThreadWrapper" << endl;
    long i = 0;
    while(1)
    {
        i++;
        Sptr<InviteMsg> iMsg = wFifo.getNext();
        Data a((int)i);
        gVia.setBranch(iMsg->computeBranch()+"."+a);
        Data t = "TCP";
        gVia.setTransport(t); 
        iMsg->setVia(gVia,0);
        //mVia.setPortData("5070");
	//iMsg->setVia(mVia, 0);
        sFifo.add(iMsg);
        SipTo to = iMsg->getTo();
        SipFrom from = iMsg->getFrom();
	SipVia via = iMsg->getVia(0);
	cpLog(LOG_ERR, "+++++workerThreadWrapper, To:%s", to.encode().logData());
	cpLog(LOG_ERR, "+++++workerThreadWrapper, From:%s", from.encode().logData());
	cpLog(LOG_ERR, "+++++workerThreadWrapper, Via:%s", via.encode().logData());
    }
    return ((void*)0);
}



int main()
{
    gVia.setprotoName("SIP");
    gVia.setHost(theSystem.gethostAddress());
    gVia.setPortData("5070");

    VThread t1, t2;
    t1.spawn(workerThreadWrapper, 0);
    t2.spawn(sendThreadWrapper, 0);
    int i =0;
    while(1)
//    for(int i =0; i < 10000; i++)
    {
       Sptr<InviteMsg> iMsg = new InviteMsg(inv_msg);
       wFifo.add(iMsg);
       cpLog(LOG_ERR, "Main thread:%s" , iMsg->encode().logData() );
       usleep(10000);
    }
    t1.join();
    t2.join();
    return 0;
}
