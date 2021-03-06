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

#include "global.h"
#include "SipUdpConnection.hxx"
#include "SystemInfo.hxx"
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "SipVia.hxx"
#include <errno.h>
#ifndef WIN32
//android needs this.
#include "netinet/in.h"
#endif


using namespace Vocal;

/// this is a function to print messages
string printMsgBrief(Sptr<SipMsg> msg); 

 /*  retransmitOff from TransceiverSymbols.hxx */
//bool SipUdpConnection::Udpretransmitoff = retransmitOff;

 /* retransmitTimeInitial from TransceiverSymbols.hxx */  
int SipUdpConnection::Udpretransmitimeinitial = retransmitTimeInitial; 

 /* retransmitTimeMax from TrasnceiverSymbols.hxx */
int SipUdpConnection::Udpretransmittimemax = retransmitTimeMax; 

atomic_t SipUdpConnection::_cnt;

SipUdpConnection::SipUdpConnection(const char* debug_msg, uint16 tos, uint32 priority,
                                   const string& local_ip,
                                   const string& local_dev_to_bind_to,
                                   int port)
    : randomLosePercent(0),
      udpStack(debug_msg, tos, priority, false, local_ip, local_dev_to_bind_to, NULL, port ) {

    atomic_inc(&_cnt);
}

SipUdpConnection::~SipUdpConnection() {
    atomic_dec(&_cnt);
}

const string SipUdpConnection::getLocalIp() const {
    return udpStack.getSpecifiedLocalIp();
}

// May pull from incomming fifo
Sptr<SipMsgContainer> SipUdpConnection::getNextMessage() {
   // First, see if we can pull something off of the fifo
   if (rcvFifo.size()) {
      Sptr<SipMsgContainer> rv = rcvFifo.front();
      rcvFifo.pop_front();
      return rv;
   }
   return NULL;
}


void SipUdpConnection::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                            uint64 now) {

   if (udpStack.checkIfSet(input_fds)) {
      cpLog(LOG_DEBUG_STACK, "input FD is set!\n");
      Sptr<SipMsgContainer> m = receiveMessage();
      if (m != 0) {
         // The SipThread class will slurp this with receiveNB later...
         cpLog(LOG_DEBUG_STACK, "received Message, pushing onto rcvFifo\n");
         rcvFifo.push_back(m);
      }
   }

   int sofar = 0;
   while (sendQ.size()) {
      int ret_status = 0;
      uint64 ntx = sendQ.top()->getNextTx();
      if (ntx > now) {
         // Wait a while, not ready to send any more at this time
         cpLog(LOG_DEBUG_STACK, "Waiting to send later: nextTx: %llu  now: %llu\n",
               ntx, now);
         break;
      }

      Sptr<SipMsgContainer> sipMsg = sendQ.top();
      sendQ.pop(); // We may re-add it later

      //cpLog(LOG_ERR, "popped msg from sendQ: %p  max-retrans: %i  sofar: %i\n",
      //      sipMsg.getPtr(), sipMsg->getRetransmitMax(), sipMsg->getRetransSoFar());
      //cpLog(LOG_DEBUG_STACK, "popped msg from sendQ: %s\n", sipMsg->toString().c_str());

      if (sipMsg->getRetransmitMax() > sipMsg->getRetransSoFar()) {

         //get host, and port, and send it off.
         if ((!randomLosePercent) || 
             ((random() % 100) >= randomLosePercent)) {
            //
            ret_status = udpSend(sipMsg);
            if (ret_status < 0) {
               switch(ret_status) {
               case -ECONNREFUSED:
                  cpLog(LOG_INFO, "ECONNREFUSED! ");
                  ret_status = 403;
                  break;
               case -EHOSTDOWN:
                  cpLog(LOG_INFO, "EHOSTDOWN! ");
                  ret_status = 408;
                  break;
               case -EHOSTUNREACH:
                  cpLog(LOG_INFO, "EHOSTUNREACH! ");
                  ret_status = 404;
                  break;
               case -EAGAIN:
                  //case -EWOULDBLOCK: duplicate of EAGAIN
               case -EINTR:
                  // Try again, do not decrement the retransCount as these errors
                  // are local.
                  // TODO:  We *MAY* want to set these aside to not block other
                  // messages if we are failing this because of waiting on ARP,
                  // for instance.
                  sendQ.push(sipMsg);
                  return;
               default:
                  cpLog(LOG_INFO, "UDP send ERROR: %s", strerror(-ret_status));
                  ret_status = 400;
               }
            }
            else {
               sofar += ret_status;
               if (ret_status == 0) {
                  // Try again later
                  sendQ.push(sipMsg);
                  return;
               }
               // Rest of the code wants 0 == success, doesn't care how much
               // written.
               ret_status = 0;
            }
               
            if (ret_status) {
               if  ((sipMsg != 0) &&
                    (sipMsg->getMsgIn() != 0) &&
                    (sipMsg->getMsgIn()->getType() != SIP_STATUS)) {
                  // Strange, looks like we build an error response message
                  // here and put it in the stack as if it was received by
                  // the network!
                  Sptr<SipMsg> mmsg = SipMsg::decode(sipMsg->getEncodedMsg().c_str(),
                                                     getLocalIp());
                  Sptr<SipCommand> commandMsg;
                  commandMsg.dynamicCast(mmsg);
                  SipTransactionId id(*(commandMsg));
                  Sptr<SipMsgContainer> retVal = new SipMsgContainer(id);
                     
                  retVal->setMsgIn(new StatusMsg((*commandMsg),
                                                 ret_status));
                  rcvFifo.push_back(retVal);
               }
            }
         }
         else {
            cpLog(LOG_INFO, "XXX random lose: message not sent!");
         }
            
         sipMsg->incRetransSoFar();
         
         //
         //cpLog(LOG_DEBUG_STACK, "Sent msg, ret_status: %i  retrans-max: %i  sofar: %i\n",
         //      ret_status, sipMsg->getRetransmitMax(),
         //      sipMsg->getRetransSoFar());

         //increment the time to send out.               
         if (sipMsg->getRetransmitMax() > sipMsg->getRetransSoFar()) {
            int wait_period = sipMsg->getLastWaitPeriod();
            if (wait_period == 0) {
               sipMsg->setWaitPeriod(retransmitTimeInitial);
            }
            else if (wait_period < retransmitTimeMax) {
               sipMsg->setWaitPeriod(wait_period * 2);
            }
            sipMsg->setNextTx(now + sipMsg->getLastWaitPeriod());

            //add into Fifo.
            sendQ.push(sipMsg);
         }
         else {
            if (ret_status != 0) {
               // No more retransmits, and status was a failure
               // send 408 if a command, other than an ACK.
               if ((sipMsg->getMsgIn()->getType() != SIP_ACK) &&  
                  (sipMsg->getMsgIn()->getType() != SIP_STATUS)) {
                  cpLog(LOG_DEBUG_STACK, "The SipMeaasge is \n%s\n",
                        sipMsg->getEncodedMsg().c_str());
                  Sptr<SipMsg> mmsg = SipMsg::decode(sipMsg->getEncodedMsg(),
                                                     getLocalIp());
                  Sptr<SipCommand> commandMsg;
                  commandMsg.dynamicCast(mmsg);
                  SipTransactionId id(*(commandMsg));
                  Sptr<SipMsgContainer> statusMsg = new SipMsgContainer(id);
                  statusMsg->setMsgIn(new StatusMsg((*commandMsg), ret_status));
                  rcvFifo.push_back(statusMsg);
               }
            }
         }//else no more retrans
      }//if retransmit count > 0
      else {
         // Ignore the message, it seems we decided not to send this guy for
         // some reason.
         cpLog(LOG_DEBUG_STACK, "NOT sending msg because retransmit is zero:\n%s\n",
               sipMsg->getEncodedMsg().c_str());
      }
   }//while there is something in the send queue
}//tick


int SipUdpConnection::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                             int& maxdesc, uint64& timeout, uint64 now) {
   //cpLog(LOG_DEBUG_STACK, "adding FD to input set.\n");
   udpStack.addToFdSet(input_fds);
   if (sendQ.size()) {
      uint64 ntx = sendQ.top()->getNextTx();
      if (ntx > now) {
         timeout = min(timeout, ntx - now);
      }
      else {
         udpStack.addToFdSet(output_fds); //Need to write
      }
   }
   return 0;
}//setFds


Sptr<SipMsgContainer>
SipUdpConnection::receiveMessage() {
   Sptr<SipMsgContainer> sipMsg;
   struct sockaddr_in sender;

   int len = udpStack.receiveFrom( rcvBuf, MAX_UDP_RCV_BUF, &sender, 0 /* flags */);
            
   if ( len == 0 ) {
      // Nothing to read
   }
   else if ( len < 0 ) {
      cpLog(LOG_DEBUG_STACK, "receiveTimeout error");
      // TODO:  Propagate error up the stacks??
   }
   else {
      rcvBuf[len] = '\0';
        
      /******************************************************
       * will need to move this stuff into the stack later
       */
        
      Sptr<SipMsg> sm = SipMsg::decode(rcvBuf, getLocalIp());
      if (sm != 0) {
         SipTransactionId id(*sm);
         sipMsg = new SipMsgContainer(id);
         
         string senderip(vtoStringIp(ntohl(sender.sin_addr.s_addr)));
         short senderport = ntohs(sender.sin_port);

         sm->setReceivedIPName(senderip);
         sm->setReceivedIPPort(senderport);
         cpLog(LOG_INFO, "Received UDP Message:\n\n<- HOST[%s] PORT[%d]\n\n%s",
               senderip.c_str(), senderport, rcvBuf);
         
         sipMsg->setMsgIn(sm);
      }
      else {
         // this message failed to decode
         cpLog(LOG_DEBUG, 
               "SipUdp_impl::receiveMain, ERROR: received message did not decode\n%s",
               rcvBuf);
      }
   }
   return sipMsg;
}//receiveMessage


// Resolves IP address and enqueues the message for later delivery.
int
SipUdpConnection::send(Sptr<SipMsgContainer> msg, const Data& host,
                       const Data& port) {

    cpLog(LOG_DEBUG_STACK, "Destination (%s:%s), out.length: %i\n",
                            host.logData(), port.logData(),
          msg->getEncodedMsg().size());

    if (!msg->getEncodedMsg().size()) {
        Data nhost;
        int nport;

        assert(msg->getMsgIn() != 0);
	if (host.length()) {
            nhost = host;
            nport = port.convertInt();
	}
	else {
            getHostPort(msg->getMsgIn(), nhost, nport);
        }

        msg->cacheEncode();
        if (msg->getNetworkAddr() == 0 ) {
           Sptr<NetworkAddress> na = new NetworkAddress(nhost.convertString(), nport);
           if (na->getIp4Address() == 0) {
              cpLog(LOG_ERR, "Destination (%s) is not reachable.",
                    nhost.logData());
              return -1;
           }
           msg->setNetworkAddr(na);
        }
    }

    //if this has to be retransmitted.
    msg->setRetransSoFar(0);

    cpLog(LOG_DEBUG_STACK,
          "*** Msg is for transmission:\n%s\n dest host: %s  port: %s",
          msg->toString().c_str(), host.c_str(), port.c_str());

    assert(msg->getNetworkAddr()->getPort() >= 0);

    sendQ.push(msg);
    return 0;
}


// Returns number of bytes written.  On error, returns -errno.
// Helper method called by 'tick'
int SipUdpConnection::udpSend(Sptr<SipMsgContainer> sipMsg) {
    assert(sipMsg != 0);
    sipMsg->assertNotDeleted();

    int lngth = sipMsg->getEncodedMsg().size();
    assert(lngth);

    Sptr<NetworkAddress> nAddr = sipMsg->getNetworkAddr();
    int ret_status = udpStack.doTransmitTo(sipMsg->getEncodedMsg().c_str(),
                                           lngth, nAddr.getPtr());
    cpLog(LOG_INFO, "Sent UDP Message: HOST[%s] PORT[%d] sz: %d  rv: %d\n%s",
          nAddr->getIpName().c_str(), nAddr->getPort(), lngth, ret_status,
          sipMsg->getEncodedMsg().c_str());
    
    return ret_status;
}//udpSend


void SipUdpConnection::getHostPort(Sptr<SipMsg> sipMessage, Data& host, int& port) {
   if ( sipMessage->getType() == SIP_STATUS  ) {
      assert( sipMessage->getNumVia() > 0 );
	
      if ( sipMessage->getNumVia() > 0 ) {
         const SipVia& via = sipMessage->getVia( 0 );
         // check for viaReceived parameter in the via header
         if ( via.isViaReceived() == true) {
            host = via.getReceivedhost();
            port = via.getReceivedport().convertInt();
         } 
         else {
            host = via.getMaddr();
            if( host.length() == 0 ) {
               host = via.getHost();
            }
            port = via.getPort();
            cpLog( LOG_DEBUG_STACK,"got host from Via of %s", host.logData());
            cpLog( LOG_DEBUG_STACK,"got port from Via of %d", port);
         }
      }
   }
   else {
      assert(sipMessage->isSipCommand());
      Sptr<SipCommand> command((SipCommand*)(sipMessage.getPtr()));
      if ( command != 0) {
         Sptr<SipUrl> dest = command->postProcessRouteAndGetNextHop();
         
         if (dest != 0) {
            cpLog(LOG_DEBUG_STACK, "Found dest by processRouteAndGetNextHop: %s\n",
                  dest->encode().c_str());
            Data maddr = dest->getMaddrParam();
            if (maddr.length() > 0) {
               host = maddr;
            }
            else {
               host = dest->getHost();
            }
            port = dest->getPort().convertInt();
            
            cpLog( LOG_DEBUG_STACK, "got host from start line: %s:%d",
                   host.logData(), port);
         }
         else {
            cpLog(LOG_ERR, "attempting to send message to a non-sip URL, discarding");
            assert(0);
         }
      }
      else {
         assert(0);
      }
   }//if sipCommand

    //If port is zero set the default port
   if (port == 0) {
      port = SIP_PORT;
   }
}//getHostPort

#if 0
void SipUdpConnection::reTransOn() {
    Udpretransmitoff = false;
}

void SipUdpConnection::reTransOff() {
    Udpretransmitoff = true;
}
#endif

void SipUdpConnection::setRetransTime(int initial, int max) {
    Udpretransmitimeinitial = initial;
    Udpretransmittimemax = max;
}

void SipUdpConnection::setRandomLosePercent(int percent) {
    if((percent >=0) && (percent <= 100)) {
	randomLosePercent = percent;
    }
}


void SipUdpConnection::printSize() const {
    cpLog(LOG_INFO, "rcvFifo: %d, sendFifo: %d",
	  rcvFifo.size(), sendQ.size());
}


Data SipUdpConnection::getDetails() const
{
    char buf[256];
    snprintf(buf, 255, "receive fifo: %d    send fifo: %d"
             "\npkts received: %d   pkts sent: %d",
             (int)(rcvFifo.size()), (int)(sendQ.size()),
             (int)(udpStack.getPacketsReceived()),
             (int)(udpStack.getPacketsTransmitted()));
    return buf;
}
