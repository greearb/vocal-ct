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

static const char* const SipSentResponseDB_cxx_version =
    "$Id: SipSentResponseDB.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipTransceiver.hxx"
#include "SipSentResponseDB.hxx"
#include "SipMsg.hxx"
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "SipVia.hxx"

using namespace Vocal;

/*
SipSentResponseDB::SipSentResponseDB(const string& _local_ip)
    : SipTransactionDB(_local_ip)
{
}
*/

SipSentResponseDB::SipSentResponseDB(int size, const string& _local_ip)
    : SipTransactionDB(size, _local_ip)
{}

SipSentResponseDB::~SipSentResponseDB()
{
    /// the SipTransactionDB::~ will do the clean up
}

SipMsgContainer*
SipSentResponseDB::processSend(const Sptr<SipMsg>& msg)
{
    /// the only send in THIS db can be of the responses
    StatusMsg * response = dynamic_cast<StatusMsg*>(msg.getPtr());
    assert(response);

    SipMsgContainer * retVal = new SipMsgContainer();
    retVal->msg.in = msg;
    retVal->msg.type = msg->getType();
    retVal->msg.transport = msg->getVia(0).getTransport();

    /// if its a final response then update the transactionDB
    if(response->getStatusLine().getStatusCode() >= 200)
    {
	SipTransactionId id(*msg);
	SipTransactionList<SipTransLevel3Node *>::SipTransListNode*
	    level3Node = 0;
	SipTransLevel1Node * topNode = getTopNode(id,msg);
	if(topNode)
	{
	    SipTransactionList<SipTransLevel2Node *>::SipTransListNode*
		level2Node = 0;

	    level2Node = topNode->find(id);
	    if(level2Node)
	    {
		level3Node = level2Node->val->find(id);
	    }
	    else
	    {
		cpLog(DEBUG_NEW_STACK,
		      "ERROR, no Level2[%s] entry for outgoing response...",
		      id.getLevel2().logData());
	    }
	}
	if(level3Node)
	{
	    /// there's a transaction for this response message
	    /// hence add it to the transaction
	    if(level3Node->val->msgs.response)
	    {
		cpLog(DEBUG_NEW_STACK,"Collision\nOLD:%s\n\nNEW:%s",
		      level3Node->val->msgs.response->msg.out.logData(),
		      retVal->msg.out.logData());
	    }
	    else
	    {
		level3Node->val->msgs.response = retVal;
		retVal->level3Ptr = level3Node->val;

		/// make the transport retrans it repeatedly
		if((level3Node->val->myKey == INVITE_METHOD) &&
                   (SipTransceiver::myAppContext != APP_CONTEXT_PROXY))
                {
                    cpLog( LOG_DEBUG_STACK, "Set UA INVITE final response retransmission" );
		    retVal->retransCount = MAX_RETRANS_COUNT;
                }
                cpLog( LOG_DEBUG_STACK, "Retransmission count %d", retVal->retransCount );

                if(level3Node->val->myKey == INVITE_METHOD)
                {
                    SipTransactionGC::instance()->
                        collect(level3Node->val->msgs.request,
                                INVITE_CLEANUP_DELAY);
                }
	    }
	}

	/// release the lock acquired on top level node
	if(topNode)
        {
	    topNode->lock.unlock();
        }
    }

    return retVal;
}

SipMsgQueue*
SipSentResponseDB::processRecv(SipMsgContainer* msgContainer)
{
    /// the only receive in THIS db can be of the requests
    assert(dynamic_cast<SipCommand*>(msgContainer->msg.in.getPtr()));

    SipTransactionId id(*(msgContainer->msg.in));
    SipMsgQueue * retVal = 0;

    SipTransLevel1Node * topNode = getTopNode(id,msgContainer->msg.in);
    if(!topNode)
    {
	/// there was no transaction found/created for this message,
	/// so just proxy it up and hope for the best!!!
	retVal = new SipMsgQueue;
	retVal->push_back(msgContainer->msg.in);
        msgContainer->msg.in = 0;
	cpLog(LOG_DEBUG,"No trasaction for %s",
	      msgContainer->msg.out.logData());

	///// leak fixed 04/11 @ 1230
	SipTransactionGC::instance()->
	    collect(msgContainer,
		    ORPHAN_CLEANUP_DELAY);
    }
    else
    {
        SipTransLevel2Node * level2Node = topNode->findOrInsert(id)->val;

	SipTransactionList<SipTransLevel3Node *>::SipTransListNode *nodePtr =
	    level2Node->findOrInsert(id);

	if(nodePtr->val->msgs.request)
	{
	    /// if there is a coresponding response then retrans it
	    if(nodePtr->val->msgs.response)
	    {
		cpLog(LOG_INFO,"duplicate message: %s",
		      msgContainer->msg.out.logData());
		msgContainer->msg.in = nodePtr->val->msgs.response->msg.in;
		msgContainer->msg.netAddr = nodePtr->val->msgs.response->msg.netAddr;
                if(nodePtr->val->msgs.response->msg.out.length())
                {
		    msgContainer->msg.out 
                        = nodePtr->val->msgs.response->msg.out;
                }
                else
                { 
		    msgContainer->msg.out 
                        = nodePtr->val->msgs.response->msg.in->encode();
                }
		msgContainer->msg.type 
                    = nodePtr->val->msgs.response->msg.type;
		msgContainer->msg.transport 
                    = nodePtr->val->msgs.response->msg.transport;
		msgContainer->msg.netAddr 
                    = nodePtr->val->msgs.response->msg.netAddr;
		msgContainer->retransCount = FILTER_RETRANS_COUNT;
	    }
	    else
	    {
		/// we didn't find a matching response but since the request
		/// has been received already so assume that application will
		/// be sending a response (if it's not an ACK)
		/// and hence just drop this duplicate
		msgContainer->msg.in = 0;
		msgContainer->msg.out = "";
		msgContainer->retransCount = 0;
	    }
	}
	else
	{
	    /// this is a first time receive of this message
            int msgSeqNumber 
                  = msgContainer->msg.in->getCSeq().getCSeqData().convertInt();
#if 1
            if (SipTransceiver::myAppContext != APP_CONTEXT_PROXY)
            {
                if(!( (msgContainer->msg.in->getType() == SIP_ACK) ||
                      (msgContainer->msg.in->getType() == SIP_CANCEL) ))
                {
		    // Not ACK or CANCEL
                    if(level2Node->seqSet)
                    {
                      // Check the seq number of the message for the same call-leg.
                
                      // 1.  if the seq number is higher than previous one Msg is OK
                      // 2.  If sequence number is lower, it is an error
                      // 3.  If sequence number is equal and message is not an
                      //     ACK or CANCEL it is an error.  ACK and CANCELs w/
                      //     the same seqence number are OK.

                        int storedSeqNum = level2Node->seqNumber;
                        if( msgSeqNumber > storedSeqNum )
                        {
                            cpLog(LOG_DEBUG, "***** Bumping seq to %d", msgSeqNumber);
                            //Bump up the next sequenece only if not ACK or CANCEL      
                            level2Node->seqNumber = msgSeqNumber;
                        }
                        else if( (msgSeqNumber < storedSeqNum) ||
                                 (msgSeqNumber == storedSeqNum) )
                        {             
                             // Error condition
                             Sptr<SipCommand> sipCmd;
                             sipCmd.dynamicCast(msgContainer->msg.in);
                             assert(sipCmd != 0);
                             Sptr<StatusMsg> statusMsg = new StatusMsg(*sipCmd, 400); 
                             Data reason = "Sequence number out of order";
                             statusMsg->setReasonPhrase(reason);
		             msgContainer->msg.in = statusMsg;
                             msgContainer->msg.type = statusMsg->getType();
                             msgContainer->msg.transport = statusMsg->getVia(0).getTransport();

		             msgContainer->msg.out = "";
	                     msgContainer->retransCount = 1;
                     
                             if(topNode)
                               topNode->lock.unlock();
                             processSend(statusMsg);
                             return 0;
                        }
                    }
                 }
                 else
                 {
                     level2Node->seqNumber = msgSeqNumber;
                     level2Node->seqSet = true;
                     cpLog(LOG_DEBUG_STACK, "***** Setting seq to %d for callId %s", msgSeqNumber, msgContainer->msg.in->getCallId().encode().logData());
                 }
            }
#endif

	    nodePtr->val->msgs.request = msgContainer;
	    nodePtr->val->msgs.request->level3Ptr = nodePtr->val;

	    /// construct the msg queue to be sent up to application
	    retVal = new SipMsgQueue;
	    if(msgContainer->msg.in->getType() == SIP_ACK)
	    {
		SipTransactionList<SipTransLevel3Node *>::SipTransListNode
		    *curr = level2Node->level3.getLast();
		while(curr)
		{
		    /// look for the INVITE message
		    if(curr->val->myKey == INVITE_METHOD)
			break;
		    curr = level2Node->level3.getPrev(curr);
		}
		if( !curr )
                {
		    cpLog(LOG_DEBUG_STACK,"No INVITE, try without Via branch");
		    curr = topNode->findLevel3AckInvite( id );
                }
		if(curr)
		{
		    cpLog(LOG_DEBUG_STACK,"Found INVITE");
                    // add the other items of this transaction, and
                    // reduce memory usage by clearing the parsed
                    // message in the in pointer

		    if(curr->val->msgs.request)
                    {
			retVal->push_back(curr->val->msgs.request->msg.in);
                        curr->val->msgs.request->msg.in = 0;
                    }
		    if(curr->val->msgs.response)
                    {
			retVal->push_back(curr->val->msgs.response->msg.in);
                        curr->val->msgs.response->msg.in = 0;
                    }

		    /// also cancel the retrans of response
		    cancel(curr->val->msgs.response);
		    cpLog(DEBUG_NEW_STACK,"Stopping retrans of response[%s]",
			  curr->val->myKey.logData());
		}
		else
		{
		    cpLog(LOG_DEBUG_STACK,"INVITE not Found");
		    /////////// ACK w/o INVITE !!!!
		    ////////// (may have been gc'd)
		}
	    }
	    retVal->push_back(msgContainer->msg.in);
	    /// also inform the cleanup
            if(msgContainer->msg.in->getType() != SIP_INVITE)
            {
                SipTransactionGC::instance()->
                    collect(nodePtr->val->msgs.request,
                            MESSAGE_CLEANUP_DELAY);
            }
            msgContainer->msg.in = 0;
	}
    }
    /// release the lock acquired on top level node
    if(topNode)
    {
        topNode->lock.unlock();
    }
    return retVal;
}

SipTransLevel1Node*
SipSentResponseDB::getTopNode(const SipTransactionId& id, 
                              const Sptr<SipMsg>& msg)
{
    SipTransHashTable::SipTransRWLockHelper helper;
    SipTransHashTable::Node * bktNode;

    if(msg->getType()==SIP_STATUS)
    {
	/// this finds if there's a transaction, and locks for READ
	bktNode = table.find(id.getLevel1(), &helper);
	if(bktNode)
	  if(bktNode->myNode->toTag == NOVAL)
	  {
              // this is the first response going out for this call
              // leg, hence / update the state info with To tag

              bktNode->myNode->toTag = msg->getTo().getTag();
	  }
	  else
	  {
	    /// make sure that response has the To tag same as previous
	    /// responses for this call leg
	    if(bktNode->myNode->toTag != msg->getTo().getTag())
	      {
		cpLog(LOG_DEBUG_STACK,
		      "To tag of outgoing %d[%s] %s [%s] is incorrect",
                      msg->getType(),
		      msg->getTo().getTag().logData(),
		      "in existing transaction",
		      bktNode->myNode->toTag.logData());
                if(SipTransceiver::myAppContext != APP_CONTEXT_PROXY)
                {
                    SipTo to = msg->getTo();
		    cpLog(LOG_DEBUG, "Setting to-tag of outgoing response from=%s to=%s", to.getTag().logData(), bktNode->myNode->toTag.logData() );
                    to.setTag(bktNode->myNode->toTag);
                    msg->setTo(to);
                }
	      }
	  }
    }
    else
    {
	// this creates a new transaction, if none exists, and locks
	// for write otherwise just returns the existing transaction
	// with read lock

	bktNode = table.findOrInsert(id.getLevel1(), &helper);
	if(bktNode->myNode == 0)
	{
	    bktNode->myNode = new SipTransLevel1Node();
	    bktNode->myNode->bucketNode = bktNode;
	}
	/// if there's a transaction, then check the to tag
	else if((bktNode->myNode->toTag!= NOVAL) &&
		(bktNode->myNode->toTag != msg->getTo().getTag()))
	{
	    cpLog(LOG_DEBUG_STACK,"To tag of incoming %d[%s] %s [%s] is incorrect",
		  msg->getType(),
		  msg->getTo().getTag().logData(),
		  "in existing transaction",
		  bktNode->myNode->toTag.logData());
	}
    }

    if(bktNode)
    {
	// acquire a lock on the top level node, before releasing the
	// bucket node: due to race w/ cleanup thread!!!
	bktNode->myNode->lock.lock();

	return bktNode->myNode;
    }
    else
	return 0;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
