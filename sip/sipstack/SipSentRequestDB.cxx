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

static const char* const SipSentRequestDB_cxx_version =
    "$Id: SipSentRequestDB.cxx,v 1.4 2004/05/29 01:10:33 greear Exp $";

#include "global.h"
#include "SipSentRequestDB.hxx"
#include "SipMsg.hxx"
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "SipTransceiver.hxx"

using namespace Vocal;

SipSentRequestDB::SipSentRequestDB(const string& _local_ip)
    : SipTransactionDB(local_ip)
{
}


SipSentRequestDB::~SipSentRequestDB()
{
    // the SipTransactionDB::~ will do the clean up
}


Sptr<SipMsgContainer>
SipSentRequestDB::processSend(const Sptr<SipMsg>& msg) {
    // the only send in THIS db can be of the requests
    assert(dynamic_cast<SipCommand*>(msg.getPtr()));

    SipTransactionId id(*msg);
    Sptr<SipMsgContainer> retVal = new SipMsgContainer(id);
    retVal->msg.in = msg;
    retVal->msg.type = msg->getType();

    Sptr<SipCommand> command((SipCommand*)(msg.getPtr()));

    if ( command != 0 ) {
        Sptr<SipUrl> dest((SipUrl*)(command->getRequestLine().getUrl().getPtr()));
        if (dest != 0) {
            cpLog(LOG_DEBUG_STACK, "Setting transport %s", dest->getTransportParam().logData());
            retVal->msg.transport = dest->getTransportParam();
        }
    } 

    if (msg->getType() != SIP_ACK) {
        retVal->setRetransCount(MAX_RETRANS_COUNT);
    }

    Sptr<SipCallContainer> call = getCallContainer(id);
    Sptr<SipMsg> mold;
    if ((call != 0) && ((mold = call->findMsg(id)) != 0)) {
	cpLog(LOG_ERR,"two identical requests from application...");
	cpLog(DEBUG_NEW_STACK,"Old...\n%s\n\nNew...\n%s",
	      mold.toString().c_str(), msg.toString().c_str());
    }
    else {
        if (call == 0) {
            // Create us a new call container
            call = new SipCallContainer(id);
            addCallContainer(call);
        }

	// if this is a CANCEL then cancel all the active retranses...
	if (msg->getType() == SIP_CANCEL) {
            call->clear();
	}

	// insert the request into data base
        call->addMsg(retval);
    }
    return retVal;
}//processSend


Sptr<SipMsgQueue>
SipSentRequestDB::processRecv(Sptr<SipMsgContainer> msgContainer, uint64& now) {
    // the only receive in THIS db can be of the responses
    Sptr<StatusMsg > response;
    response.dynamic_cast(msgContainer->msg.in);
    assert(response != 0);

    Sptr<SipMsgQueue> retVal;

    SipTransactionId id(*(msgContainer->msg.in));

    Sptr<SipCallContainer> call = getCallContainer(id);
    if (call == 0) {
        // there's no transaction for this response message
        cpLog(LOG_DEBUG_STACK,"No transaction for %s",
              msgContainer->msg.in->encode().logData());
    }
    else {
        Sptr<SipMsgPair> msgPair = call->findMsgPair(response);
        if (msgPair == 0) {
            // Response to something we didn't send??
            cpLog(LOG_ERR, "ERROR:  Could not find request for response: %s\n",
                  response->toString().c_str());
        }
        else {
            if (msgPair->request != 0) {
                msgPair->request->setRetransCount(0); // Cancel it's retransmission
            }

            // If appContext is a proxy and a 200 or any provisional
            // response of INVITE do not filter it and give it to proxy
            int statusCode = response->getStatusLine().getStatusCode();
            if ((statusCode < 200)  ||
                ((SipTransceiver::myAppContext == APP_CONTEXT_PROXY) && 
                 (statusCode == 200) &&
                 (response->getCSeq().getMethod() == INVITE_METHOD))) {
                // simply forward this response up to application
                retVal = new SipMsgQueue();
                retVal->push_back(msgContainer->msg.in);
            }
            else if (response->getStatusLine().getStatusCode() >= 200) {
                // if it is a final response, then process the transaction
                // there's a transaction, hence check for filtering
                if (msgPair->response != 0) {
                    // Already had a response!
                    cpLog(LOG_ERR, "WARNING:  Received duplicate response, initial: %s\nnew: %s\n",
                          msgPair->response->toString().c_str, response->toString().c_str());
                    // Ignore this later response
                }
                else {
                    msgPair->response = response;
                    
                    retVal = new SipMsgQueue();
                    retval->push_back(msgPair->request->msg.in); //TODO:  Why?
                    retVal->push_back(msgContainer->msg.in);
                    
                    //Clean up Object leave the raw data
                    if (!msgContainer->msg.out.length()) {
                        msgContainer->msg.out = msgContainer->msg.in->encode(); 
                    }
                }
            }
        }
    }
    return retVal;
}
