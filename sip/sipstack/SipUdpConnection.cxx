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

static const char* const SipUdpConnection_cxx_Version =
    "$Id: SipUdpConnection.cxx,v 1.3 2004/05/27 04:32:18 greear Exp $";

#include "global.h"
#include "SipUdpConnection.hxx"

using namespace Vocal;

/// this is a function to print messages
string printMsgBrief(Sptr<SipMsg> msg); 

 /*  retransmitOff from TransceiverSymbols.hxx */
bool SipUdpConnection::Udpretransmitoff = retransmitOff;

 /* retransmitTimeInitial from TransceiverSymbols.hxx */  
int SipUdpConnection::Udpretransmitimeinitial = retransmitTimeInitial; 

 /* retransmitTimeMax from TrasnceiverSymbols.hxx */
int SipUdpConnection::Udpretransmittimemax = retransmitTimeMax; 

atomic_t SipUdpConnection::_cnt;

SipUdpConnection::SipUdpConnection(list < Sptr <SipMsgContainer> > *fifo,
                                   const string& local_ip,
                                   const string& local_dev_to_bind_to,
                                   int port)
    : randomLosePercent(0),
      udpStack(local_ip, local_interface_to_bind_to, NULL, port ),
      sendQ(retransContentsComparitor),
      rcvFifo(fifo) {

    udpStack.setModeBlocking(false);

    atomic_inc(&_cnt);
}

SipUdpConnection::~SipUdpConnection() {
    atomic_dec(&_cnt);
}

const string SipUdpConnection::getLocalIp() const {
    string rv = udpStack.getSpecifiedLocalIp();
    if (rv.size()) {
        return rv;
    }
    else {
        return theSystem.gethostAddress(); //OK
    }
}

Sptr<SipMsgContainer>
SipUdpConnection::getNextMessage() {
    // First, see if we can pull something off of the fifo
    if (rcvFifo.size()) {
        return rcvFifo.pop_front();
    }
    else {
        // Maybe can read from the Udp Stack
        return receiveMessage();
    }
}

Sptr<SipMsgContainer>
SipUdpConnection::receiveMessage() {
    NetworkAddress sender("0.0.0.0");

    int len = udpStack.receiveTimeout( rcvBuf, MAX_UDP_RCV_BUF, &sender, 1, 0 );
            
    if ( len == 0 ) {
        // Nothing to read
    } 
    else if ( len < 0 ) {
        cpLog(LOG_DEBUG_STACK, "receiveTimeout error");
        // TODO:  Propagate error up the stacks??
    }
    else {
        buf[len] = '\0';
        
        Sptr<SipMsgContainer> sipMsg = new SipMsgContainer();
        
        sipMsg->msg.out = Data(buf, len);
        
        /******************************************************
         * will need to move this stuff into the stack later
         */
        
        cpLog( LOG_DEBUG_STACK, "Received UDP Message \n%s" , 
               toBeEaten.logData());
        
        // NOTE:  decode makes a copy, so it is fine to pass msg.out here.
        sipMsg->msg.in = SipMsg::decode(sipMsg->msg.out, getLocalIp());
        if (sipMsg->msg.in != 0) {
            sipMsg->msg.in->setReceivedIPName( sender.getIpName() );
            sipMsg->msg.in->setReceivedIPPort( Data(sender.getPort()) );
            cpLog(LOG_INFO, "Received UDP Message:\n\n<- HOST[%s] PORT[%d]\n\n%s",
                  sender.getIpName().c_str(), sender.getPort(), toBeEaten.logData());
            
            return sipMsg;
        }
        else {
            // this message failed to decode
            cpLog(LOG_DEBUG, 
                  "SipUdp_impl::receiveMain, ERROR: received message did not decode\n%s" , 
                  sipMsg->msg.out.logData());
        }
    }
    return 0;
}//receiveMessage


int
SipUdpConnection::send(Sptr<SipMsgContainer> msg, const Data& host,
                       const Data& port) {
    Sptr<RetransmitContents> retransDetails = new RetransmitContents(msg, 0);

    cpLog(LOG_DEBUG_STACK, "Destination (%s:%s), out.length: %i\n",
                            host.logData(), port.logData(), msg->msg.out.length());

    if (!msg->msg.out.length()) {
        Data nhost;
        int nport;

        assert(msg->msg.in != 0);
	if (host.length()) {
            nhost = host;
            nport = port.convertInt();
	}
	else {
            getHostPort(msg->msg.in, &nhost, &nport);
        }

        msg->msg.out = msg->msg.in->encode();
        try {
            if (msg->msg.netAddr == 0 ) {
                msg->msg.netAddr = new NetworkAddress(nhost.convertString(), nport);
            }
        }
        catch(NetworkAddress::UnresolvedException& e) {
            //cpLog(LOG_ERR, "host: %s\n nhost: %s", host, nhost);
            cpLog(LOG_ERR, "Destination (%s) is not reachable, reason:%s.",
                  nhost.logData(), e.getDescription().c_str());
            return -1;
        }
    }

    // free the msg.in ptr if this is a status msg for a non-INVITE msg
    // or if it is an ACK message
    if ((msg->msg.in != 0) && 
        (((msg->msg.in->getType() == SIP_STATUS) &&
          (msg->msg.in->getCSeq().getMethod() != "INVITE")) || 
         (msg->msg.in->getType() == SIP_ACK))) {
        // clear msg.in
        msg->msg.in = 0;
    }

    //if this has to be retransmitted.
    if (msg->retransCount > 1) {
        cpLog(LOG_DEBUG_STACK, 
	      "Msg is for retransmission:\n %s", 
	      msg->msg.out.logData());

	//////// will have to go after moving retrans count etc to stack!!!
	msg->retransCount = sipmaxRetrans;
	///////////////////////////////////////////////////////////////////
    }

    cpLog(LOG_DEBUG_STACK,
          "*** Msg is for transmission:\n%s\n dest host: %s  port: %s",
          retransDetails->toString().c_str(), host.c_str(), port.c_str());

    if (msg->msg.netAddr.getPtr()) {
        assert(msg->msg.netAddr->getPort() >= 0);
    }

    sendQ.push(retransDetails);
    return 0;
}


// Returns number of bytes written.  On error, returns -errno.
int SipUdpConnection::udpSend(Sptr<SipMsgContainer> sipMsg) {
    assert(sipMsg);
    sipMsg->assertNotDeleted();

    // send it 
    if (sipMsg->msg.netAddr == 0){
        cpLog(LOG_WARNING,"UDP SEND IS FAILED NULL MESSAGE :( To Send");
        return -EINVAL;
    }

    int lngth = sipMsg->msg.out.length(); 
    assert(lngth);
        
    cpLog(LOG_INFO, "Sending UDP Message :\n\n-> HOST[%s] PORT[%d]\n\n%s",
          nAddr->getIpName().c_str(), nAddr->getPort(),
          sipMsg->msg.out.logData());
    
    int ret_status = udpStack.transmitTo(sipMsg->msg.out.c_str(), lngth, nAddr);
    return ret_status;
}


int
SipUdpConnection::sendMain(uint64& now) {
    int sofar = 0;
    while (sendQ.size()) {
	int ret_status = 0;

        if (sendQ.top().getNextTx() > now) {
            // Wait a while, not ready to send any more at this time
            break;
        }

    	Sptr<RetransmitContents> retransmit = sendQ.top();
        sendQ.pop(); // We may re-add it later

        if (retransmit != 0) {
    	    Sptr<SipMsgContainer> sipMsg = retransmit->getMsg();
    	    if (sipMsg != 0) {
		int count = sipMsg->retransCount;
		if (count) {
		    cpLog(LOG_DEBUG_STACK, "retransmission count = %d", count);

		    //get host, and port, and send it off.
		    if (!randomLosePercent || 
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
                            case -EWOULDBLOCK:
                                sendQ.push(sipMsg);
                                return sofar;
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
                                return sofar;
                            }
                            // Rest of the code wants 0 == success, doesn't care how much
                            // written.
                            ret_status = 0;
                        }

			if (ret_status) {
			    if  ((sipMsg != 0) && 
                                 (sipMsg->msg.type != SIP_STATUS)) {
                                // Strange, looks like we build an error response message
                                // here and put it in the stack as if it was received by
                                // the network!
                                Sptr<SipMsg> mmsg = SipMsg::decode(sipMsg->msg.out,
                                                                   getLocalIp());
			        Sptr<SipCommand> commandMsg;
			        commandMsg.dynamicCast(mmsg);
				Sptr<SipMsgContainer> retVal = new SipMsgContainer();
				
				retVal->msg.in = new StatusMsg((*commandMsg),
                                                               ret_status);
				retVal->msg.out = retVal->msg.in->encode();
				recFifo.push_back(retVal);
                            }
                        }
                    }
		    else {
			cpLog(LOG_INFO, "XXX random lose: message not sent!");
                    }
		    
                    //update retransDetails
		    count--;

		    if (Udpretransmitoff) {
                        count = 0;
                    }
		    //// required or else will send 408 up for retransoff!
		    else {
			if ( ret_status != 0) {
			    count = 0;
                        }
			else if ((!count) && (ret_status != 0)) {
			    /// flag the message got All its chances
			    retransmit->setCount(1);
                        }
		    }

		    /// check if it was canceled already
		    if(sipMsg->retransCount) {
			sipMsg->retransCount = count;
                    }
		    else {
                        count = 0;
                    }

		    //increment the time to send out.

		    //// we don't want to hold on to messages that don't
		    //// need to be retransmitted
		    if (count) {
			int wait_period = retransmit->getWaitPeriod();
			if (wait_period == 0) {
                            retransmit->setWaitPeriod(retransmitTimeInitial);
			}
			else if (wait_period < retransmitTimeMax) {
			    retransmit->setWaitPeriod(wait_period * 2);
			}
		    }
		    else {
			retransmit->setWaitPeriod(0);
                    }
                    retransmit->setNextTx(now + retransmit->getWaitPeriod());

		    //add into Fifo.
		    sendQ.push(retransmit);
                }
		else { //i.e. count == 0
		    /// check if this message got all it's chances!
                    if (retransmit->getCount()) {
			// send 408 if a command, other than an ACK.
			if ((sipMsg->msg.type != SIP_ACK) && 
                            (sipMsg->msg.type != SIP_STATUS)) {
	                    cpLog(LOG_DEBUG_STACK, "THE COUNT IS [ %d ]\n",
                                  retransmit->getCount());
	                    cpLog(LOG_DEBUG_STACK, "The SipMeaasge is \n%s\n",
                                  sipMsg->msg.out.logData());
                            Sptr<SipMsg> mmsg = SipMsg::decode(sipMsg->msg.out, getLocalIp());
                            Sptr<SipCommand> commandMsg;
                            commandMsg.dynamicCast(mmsg);
                            Sptr<SipMsgContainer> statusMsg = new SipMsgContainer();
                            statusMsg->msg.in = new StatusMsg((*commandMsg), 408);
                            rcvFifo.push_back(statusMsg);
			}
		    }

		    /// feed it to the GC...
                    if (sipMsg->level3Ptr == 0) {
                        SipTransactionGC::instance()->collect(sipMsg, ORPHAN_CLEANUP_DELAY);
                    }
                    else if (sipMsg->msg.type != SIP_INVITE) {
                        SipTransactionGC::instance()->collect(sipMsg, MESSAGE_CLEANUP_DELAY);
                    }
                }
            }//if sipMsg
	}//if retransmit.
    }//end while
    return sofar;
}


void SipUdpConnection::getHostPort(Sptr<SipMsg> sipMessage, Data& host, int& port)
{
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
}


void SipUdpConnection::reTransOn() {
    Udpretransmitoff = false;
}

void SipUdpConnection::reTransOff() {
    Udpretransmitoff = true;
}

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
    cpLog(LOG_INFO, "recFifo: %d, sendFifo: %d",
	  recFifo->size(), sendFifo.size());
}


Data SipUdpConnection::getDetails() const
{
    char buf[256];
    snprintf(buf, 255, "receive fifo: %d    send fifo: %d"
             "\npkts received: %d   pkts sent: %d",
             recFifo->size(), sendFifo.size(),
             udpStack.getPacketsReceived(),
             udpStack.getPacketsTransmitted());
    return buf;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
