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


static const char* const UaServer_cxx_Version =
    "$Id: UaServer.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include "ByeMsg.hxx" 
#include "AckMsg.hxx" 
#include "SipRoute.hxx" 
#include "SystemInfo.hxx" 
#include "UaServer.hxx" 
#include "SipVia.hxx"

using namespace Vocal::UA;

Sptr<SipMsg>
UaServer::sendBye()
{
    cpLog(LOG_DEBUG, "UaServer::sendBye()");
    //Create BYE of the ACK
    Sptr<AckMsg> ackMsg;
    ackMsg.dynamicCast(myAckMsg);
    if(ackMsg == 0)
    {
        cpLog(LOG_INFO, "UaServer:Incomplete transaction while BYE is called from callee");
        return 0;
    }

    Sptr<ByeMsg> byeMsg = new ByeMsg(*ackMsg);

    if(myRouteList.size())
    {
        byeMsg->setRouteList(myRouteList);
        //
        SipRoute siproute = byeMsg->getRoute(0);
        byeMsg->removeRoute(0);

        SipRequestLine& reqLine = byeMsg->getMutableRequestLine();
        reqLine.setUrl( siproute.getUrl() );
    } 

    //Clear VIA and set B2b as first via
    byeMsg->flushViaList();
    SipVia via("", getMyLocalIp());
    via.setHost(getMyLocalIp());
    via.setPort(getMySipPort());
    byeMsg->setVia(via);

    //SP SipCSeq origSeq = getAck()->getCSeq();
    //SP unsigned int cseq = origSeq.getNextCSeq();
    unsigned int cseq = myLocalCSeq.getNextCSeq();
    SipCSeq sipCSeq = byeMsg->getCSeq();
    sipCSeq.setCSeq( cseq );
    byeMsg->setCSeq( sipCSeq );


    myStack->sendAsync(byeMsg);
    return byeMsg;
}
