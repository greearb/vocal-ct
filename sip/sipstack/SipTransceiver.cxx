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

static const char* const SipTransceiver_cxx_Version =
    "$Id: SipTransceiver.cxx,v 1.8 2004/09/30 23:17:43 greear Exp $";

#include "global.h"
#include <cstdlib>
#include <cassert>
#include <sys/time.h>
#include <string>
#include <Sptr.hxx>
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "InviteMsg.hxx"
#include "CancelMsg.hxx"
#include "AckMsg.hxx"
#include "ByeMsg.hxx"
#include "RegisterMsg.hxx"
#include "OptionsMsg.hxx"
#include "InfoMsg.hxx"
#include "ReferMsg.hxx"
#include "TransferMsg.hxx"
#include "SipVia.hxx"
#include "SipFrom.hxx"
#include "SipContact.hxx"
#include "SipCSeq.hxx"
#include "SipSnmpDetails.hxx"
#include "SipTransactionId.hxx"
#include "SipTransceiver.hxx"
#include "SystemInfo.hxx"
#include "misc.hxx"

using namespace Vocal;

// 15/1/04 fpi
// WorkAround Win32
#ifdef WIN32
#define __PRETTY_FUNCTION__  "SipTransceiver"
#endif


atomic_t SipTransceiver::_cnt;


SipAppContext SipTransceiver::myAppContext = APP_CONTEXT_GENERIC;

SipTransceiver::SipTransceiver(const string& local_ip,
                               const string& local_dev_to_bind_to,
                               Data adata, 
                               int siplistenPort, 
                               bool nat,
                               SipAppContext aContext,
                               bool blocking)
    :
    udpConnection(0),
    tcpConnection(0),
    sentRequestDB(local_ip),
    sentResponseDB(local_ip),
    localIp(local_ip)
{
    //debugMemUsage("Beginning SipTransceiver constructor", "gua_mem.txt");
    cpLog(LOG_INFO, "SipTransceiver:  Construction, local_ip: %s local_dev: %s, port: %d\n",
          local_ip.c_str(), local_dev_to_bind_to.c_str(), siplistenPort);

    myAppContext = aContext;
    cpLog( LOG_DEBUG_STACK, "SipStack listening on port %d ",
           siplistenPort );
    cpLog( LOG_DEBUG_STACK, "SipStack context %s ",
           (myAppContext == APP_CONTEXT_PROXY) ? "Proxy" : "Non-Proxy" );

    udpConnection = new SipUdpConnection(local_ip, local_dev_to_bind_to,
                                         siplistenPort );
    //debugMemUsage("Constructed SipUdpConnection", "gua_mem.txt");

    if ( udpConnection == 0 ) {
        cpLog(LOG_ERR, "SipTransceiver::unable to instantiate UDP connections");
    }
    else {
        cpLog(LOG_INFO, "SipTransceiver:  Created UDP connection...\n");
    }

    tcpConnection = new SipTcpConnection(local_ip, local_dev_to_bind_to,
                                         siplistenPort, blocking);
    //debugMemUsage("Constructed SipTcpConnection", "gua_mem.txt");
    if ( tcpConnection == 0 ) {
        cpLog(LOG_ERR, "SipTransceiver::unable to instantiate TCP connections");
    }
    else {
        cpLog(LOG_INFO, "SipTransceiver:  Created Tcp connection...\n");
    }

    application = adata;
    Data nameport;
    nameport += adata;
    nameport += ":";
    nameport += Data(siplistenPort);
    //sipAgent = new SipAgent(nameport);
    natOn = nat;

    Vocal::theSystem.setSystemPort(siplistenPort);

    char* port;
    if((port = getenv("VOCAL_SIP_REPORTER")))
    {
#if 0
        // I purged the debugging code...maybe put it back someday? --Ben
        //debugMemUsage("Before debugger", "gua_mem.txt");
#endif
    }

    myLocalNamePort = nameport;

    //cpLog(LOG_ERR, "SipTransceiver:  Done with constructor.\n");
    //debugMemUsage("Done with SipTransceiver", "gua_mem.txt");
    atomic_inc(&_cnt);
}


SipTransceiver::~SipTransceiver() {
    atomic_dec(&_cnt);
}


void SipTransceiver::sendAsync(Sptr<SipCommand> sipMessage) {
   if ( !(sipMessage->checkMaxForwards()) ) {
      cpLog(LOG_DEBUG_STACK, "SipTransceiver: sendAsync: %s",
            sipMessage->encode().c_str());
      cpLog(LOG_DEBUG_STACK, "reached MaxForwards limit, not sending");
      return ;
   }

   Sptr<SipMsg> sm;
   sm.dynamicCast(sipMessage);
   Sptr<SipMsgContainer> msgPtr = sentRequestDB.processSend(sm);

   if (msgPtr != 0) {
      //// should this only be for actually sent messages? /////////
      if (sipAgent != 0) {
         updateSnmpData(sm, OUTS);
      }
      //////////////////////////////////////////////////////////////
	
      send(msgPtr);
   }
}//sendAsync


void SipTransceiver::sendAsync(Sptr<SipCommand> sipMessage, const Data& host,
                               const Data& iport) {
   cpLog(LOG_DEBUG_STACK, "Entering: %s, host: %s  Port: %s\n",
         __PRETTY_FUNCTION__, host.c_str(), iport.c_str());

   Data port(iport);
   if ((port == "0") || (port.size() == 0)) {
      port = "5060"; //Default
   }

   if ( !(sipMessage->checkMaxForwards()) ) {
      cpLog(LOG_DEBUG_STACK, "SipTransceiver: sendAsync: %s",
            sipMessage->encode().logData());
      cpLog(LOG_DEBUG_STACK, "reached MaxForwards limit, not sending");
      return ;
   }

   Sptr<SipMsgContainer> msgPtr = sentRequestDB.processSend(sipMessage.getPtr());
    
   if (msgPtr != 0) {
      //// should this only be for actually sent messages? /////////
      if (sipAgent != 0) {
         updateSnmpData(sipMessage.getPtr(), OUTS);
      }
      //////////////////////////////////////////////////////////////

      send(msgPtr, host, port);
   }
}//sendAsync


void SipTransceiver::sendReply(Sptr<StatusMsg> sipMessage) {
   //TODO:  Should be symetric??
   //// should this not be symmetric to what we are doing in sendAsync????
   //// (its missing all the checks and the snmp update)

   cpLog(LOG_DEBUG_STACK, "Entering %s\n", __PRETTY_FUNCTION__);

   Sptr<SipMsg> sm;
   sm.dynamicCast(sipMessage);
    
   Sptr<SipMsgContainer> sipMsg = sentResponseDB.processSend(sm);
   if (sipMsg != 0) {
      send(sipMsg);
   }
}

void SipTransceiver::send(Sptr<SipMsgContainer> sipMsg) {
   send(sipMsg, "", "");
}


void SipTransceiver::send(Sptr<SipMsgContainer> sipMsg, const Data& host,
                          const Data& iport) {

   cpLog(LOG_DEBUG_STACK, "Entering %s, host: %s  port: %s\n",
         __PRETTY_FUNCTION__, host.c_str(), iport.c_str());

   Data port(iport);
   if (port == "0") {
      port = "5060"; //Default
   }

   if ((sipMsg->getTransport() == TCP) ||
       (sipMsg->getTransport() == "tcp")) {
      if (tcpConnection != 0) {
         //send on tcp.
         cpLog(LOG_DEBUG_STACK, "%s Sending on tcp Connection...\n",
               __PRETTY_FUNCTION__);
         tcpConnection->send(sipMsg, host, port);
      }
      else {
         cpLog(LOG_INFO, "%s ERROR: TCP connection not instantiated",
               __PRETTY_FUNCTION__);
      }
   }
   else {
      if (udpConnection != 0) {
         //send on udp.
         cpLog(LOG_DEBUG_STACK, "%s Sending on UDP Connection...\n",
               __PRETTY_FUNCTION__);
         udpConnection->send(sipMsg, host, port);
      }
      else {
         cpLog(LOG_INFO, "%s ERROR: UDP connection not instantiated",
               __PRETTY_FUNCTION__);
      }
   }
}//send


// NOTE:  To receive messages, also call receiveNB
void SipTransceiver::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                          uint64 now) {
   if (udpConnection != 0) {
      udpConnection->tick(input_fds, output_fds, exc_fds, now);
   }
   if (tcpConnection != 0) {
      tcpConnection->tick(input_fds, output_fds, exc_fds, now);
   }
   if (sipAgent != 0) {
      sipAgent->tick(input_fds, output_fds, exc_fds, now);
   }
}

int SipTransceiver::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                           int& maxdesc, uint64& timeout, uint64 now) {
   //cpLog(LOG_DEBUG_STACK, "SipTransceiver::setFds, udpConnection: %p\n",
   //      udpConnection.getPtr());
   if (udpConnection != 0) {
      udpConnection->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   }
   if (tcpConnection != 0) {
      tcpConnection->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   }
   if (sipAgent != 0) {
      sipAgent->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   }
   return 0;
}


Sptr < SipMsgQueue > SipTransceiver::receiveNB() {
   // Check UDP fist, and if nothing there, check the Tcp stack.
   Sptr <SipMsgContainer> msgPtr;

   if (udpConnection != 0) {
      msgPtr = udpConnection->getNextMessage();
   }
   if ( msgPtr.getPtr() == 0) {
      if (tcpConnection != 0) {
         msgPtr = tcpConnection->getNextMessage();
      }
   }

   if (msgPtr.getPtr() == 0) {
      return NULL; // Nothing available
   }

   Sptr<SipMsg> sipPtr = msgPtr->getMsgIn();

   /********************** TO DO ****************************
    * not doing 'coz need to bring some stuff from udp impl
    * to here, so will do after 500 cps
    * msgPtr->msg.in = SipMsg::decode(msgPtr->msg.out);
    *********************************************************/

   /*****************************************************************
    * how does this affect the transactions, i.e. it is only being
    * done on received messages, so should this be before or after
    * going thru the data base (i.e. is it visible to transactionDB)?
    *****************************************************************/

   /*********** decided to remove it from the stack ******************/
   cpLog(LOG_DEBUG, "SipTransceiver: Received: %s, natOn: %d",
         sipPtr->briefString().c_str(), natOn);
   if ( natOn == true) {
      //changes for taking care of the NAT traversals
      SipVia natVia = sipPtr->getVia(0);
      //cpLog (LOG_DEBUG, "natVia = %s", natVia.encode().logData());
      string addr1 = natVia.getHost().c_str();
      string addr2 = sipPtr->getReceivedIPName().c_str();
      
      //addr2 can be empty if stack had generated the message like 408
      if (addr2 != "") {
         NetworkAddress netaddr1(addr1);
         NetworkAddress netaddr2(addr2);
         
         if ( netaddr1.getIpName() != netaddr2.getIpName() || (addr1 == "")) {
            natVia.setReceivedhost(sipPtr->getReceivedIPName());
            natVia.setReceivedport(sipPtr->getReceivedIPPort());
            //remove the first item from the via list
            sipPtr->removeVia(0);
            //insert natvia in the vector via list
            sipPtr->setVia(natVia, 0);
         }
      }
   }
   //---NAT
   /**********************************************************************/
	
      
   cpLog(LOG_DEBUG, "SipTransceiver Received, after NAT handling: %s",
         msgPtr->getMsgIn()->briefString().c_str());
   cpLog(LOG_DEBUG_STACK, "SipTransceiver: msg->out is; %s",
         msgPtr->getEncodedMsg().c_str());
   
   Sptr<SipMsgQueue> msgQ;

   if (msgPtr->getMsgIn()->getType() == SIP_STATUS) {
      msgQ = sentRequestDB.processRecv(msgPtr);
      cpLog(LOG_DEBUG_STACK, "SipTransceiver: was SIP_STATUS..\n");
   }
   else {
      msgQ = sentResponseDB.processRecv(msgPtr);
      cpLog(LOG_DEBUG_STACK, "SipTransceiver: not SIP_STATUS..\n");
   }

   if (msgQ != 0) {
      //need to have snmpDetails for this.
      if (sipAgent != 0) {
         updateSnmpData(sipPtr, INS);
      }
   }

   return msgQ;
}


void SipTransceiver::reTransOff() {
   //TODO:  This should not be a static method on SipUdpConnection!
   cpLog(LOG_DEBUG_STACK, "*** Retransmission is turned off ***");
   SipUdpConnection::reTransOff();
}


void SipTransceiver::reTransOn() {
   cpLog(LOG_DEBUG_STACK, "*** Retransmission is turned on ***");
   SipUdpConnection::reTransOn();
}


void SipTransceiver::setRetransTime(int initial, int max) {
   cpLog(LOG_DEBUG_STACK, "Initial time set to %d ms", initial);
   cpLog(LOG_DEBUG_STACK, "Max time set to %d ms", max);
   SipUdpConnection::setRetransTime(initial, max);
}


void SipTransceiver::setRandomLosePercent(int percent) {
   if (udpConnection != 0) {
      udpConnection->setRandomLosePercent(percent);
   }
}

Sptr<SipCallContainer>
SipTransceiver::getCallContainer(const SipTransactionId& id) {
   return sentRequestDB.getCallContainer(id);
}

void
SipTransceiver::updateSnmpData(Sptr < SipMsg > sipMsg, SnmpType snmpType) {
   //come here only if snmpAgent is valid.

   if (sipAgent == 0)
      return ;

   Method type = sipMsg->getType();
   if (type == SIP_STATUS) {
      if (snmpType == INS) {
         sipAgent->updateSipCounter(sipSummaryInResponses);
      }
      else if (snmpType == OUTS) {
         sipAgent->updateSipCounter(sipSummaryOutResponses);
      }

      //get the status code of this msg.
      assert(sipMsg->isStatusMsg());
      Sptr < StatusMsg > statusMsg((StatusMsg*)(sipMsg.getPtr()));

      int statusCode = statusMsg->getStatusLine().getStatusCode();

      if (snmpType == INS) {
         sipAgent->updateStatusSnmpData(statusCode, INS);
      }
      else if (snmpType == OUTS) {
         sipAgent->updateStatusSnmpData(statusCode, OUTS);
      }
   }
   else {
      //this is a command.
      if (snmpType == INS) {
         sipAgent->updateSipCounter(sipSummaryInRequests);
         sipAgent->updateCommandSnmpData(type, INS);
      }
      else if (snmpType == OUTS) {
         sipAgent->updateSipCounter(sipSummaryOutRequests);
         sipAgent->updateCommandSnmpData(type, OUTS);
      }
   }
}//updateSnmpData

#if 0
void
SipTransceiver::printSize() {
    cpLog(LOG_ERR, "%s:::::\n%s\n%s\n\n%s\n%s\n\n%s\n%s",
	  "Sip Stack Size",
	  "SentResponseDB size...", sentResponseDB.getDetails().logData(),
	  "SentRequestDB size...", sentRequestDB.getDetails().logData(),
          "UDP stack details...", udpConnection->getDetails().logData());
}
#endif

Data
SipTransceiver::getLocalNamePort() const {
    return myLocalNamePort;
}
