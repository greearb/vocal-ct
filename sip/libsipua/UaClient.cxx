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


static const char* const UaClient_cxx_Version =
    "$Id: UaClient.cxx,v 1.3 2004/11/05 07:25:06 greear Exp $";

#include "ByeMsg.hxx" 
#include "StatusMsg.hxx" 
#include "SystemInfo.hxx" 
#include "UaClient.hxx" 
#include "SipVia.hxx"

using namespace Vocal::UA;


Sptr<SipMsg> UaClient::sendBye() {
   cpLog(LOG_DEBUG, "UaClient::sendBye");
   //Create BYE of the 200
   Sptr<StatusMsg> statusMsg;
   statusMsg.dynamicCast(myResponseMsg);
   assert(statusMsg != 0);

   Sptr<ByeMsg> byeMsg = new ByeMsg(*statusMsg, getMyLocalIp());
   if (myRouteList.size()) {
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
   
   cpLog(LOG_DEBUG, "UaClient::sendBye, local_ip: %s, port: %d\n",
         getMyLocalIp().c_str(), getMySipPort());

   //unsigned int cseq = myLocalCSeq.getNextCSeq();
   cpLog(LOG_DEBUG, "MY LOCALCSeq 1st --. [%d]", myLocalCSeq.getCSeq());
   myLocalCSeq.incrCSeq();
   unsigned int cseq = myLocalCSeq.getCSeq();
   SipCSeq sipCSeq = byeMsg->getCSeq();
   sipCSeq.setCSeq( cseq );
   byeMsg->setCSeq( sipCSeq );
   myStack->sendAsync(byeMsg.getPtr());
   cpLog(LOG_DEBUG, "MY LOCALCSeq 1st --. [%d]", myLocalCSeq.getCSeq());
   
   cpLog(LOG_DEBUG, "ByeMsg: %s\n", byeMsg->toString().c_str());
   
   return byeMsg.getPtr();
}
