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

static const char* const SipTransactionGC_cxx_version =
    "$Id: SipTransactionGC.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipTransceiver.hxx"
#include "SipTransactionGC.hxx"
#include "Fifo.h"

using namespace Vocal;

SipTransactionGC* SipTransactionGC::myInstance = 0;
Mutex SipTransactionGC::myLock;

int SipTransactionGC::msgCleanupDelay = 35;
int SipTransactionGC::invCleanupDelay = 35;
int SipTransactionGC::orphanCleanupDelay = 0;


SipTransactionGC*
SipTransactionGC::instance()
{
    if(SipTransactionGC::myInstance == 0)
    {
	SipTransactionGC::myLock.lock();
	if(!SipTransactionGC::myInstance)
	{
	    SipTransactionGC::myInstance = new SipTransactionGC();
	}
	SipTransactionGC::myLock.unlock();
    }
    return SipTransactionGC::myInstance;
}


void
SipTransactionGC::shutdown()
{
    SipTransactionGC::myLock.lock();
    if(myInstance)
    {
	delete myInstance;
	myInstance = 0;
    }
    SipTransactionGC::myLock.unlock();
}


SipTransactionGC::SipTransactionGC()
    : ThreadIf(0, VTHREAD_PRIORITY_DEFAULT, 64 * 1024 /* VTHREAD_STACK_SIZE_DEFAULT*/),
      doneFlag(false), bins(), binsLock()
{
    run();
}


SipTransactionGC::~SipTransactionGC()
{
    doneFlag = true;
    join();
    BinsNodeType * curr = bins.getFirst();
    while(curr)
    {
	for(int i=0; i<curr->val->myDelay+1;i++)
	{
	    SipTransactionList<SipMsgContainer*>* 
		swap = &(curr->val->bin[i]);

	    SipTransactionList<SipMsgContainer*>::SipTransListNode *garb;
	    garb = swap->getLast();
	    while(garb)
	    {
		trash(garb->val);
		swap->erase(garb);
		garb = swap->getLast();
	    }
	}
	delete[] curr->val->bin;
	delete curr->val;
	curr = bins.getNext(curr);
    }
}


void
SipTransactionGC::collect(SipMsgContainer* item, int delay /*default value*/)
{
    if((item == 0) || item->collected) return;;

    binsLock.ReadLock();
    BinsNodeType * curr = bins.getFirst();
    while(curr)
    {
	if(curr->val->myDelay == delay)
	    break;
	curr = bins.getNext(curr);
    }
    if(!curr)
    {
	binsLock.Unlock();
	GCType * newBin = new GCType(delay);
	binsLock.WriteLock();
	curr = bins.insert(newBin);
    }
    
    curr->val->bin[curr->val->currBin].insert(item);
    item->collected = true;
    binsLock.Unlock();
}


void
SipTransactionGC::thread()
{
    cpLog(DEBUG_NEW_STACK,"Garbage Collector thread running....");
    Fifo <int> x;
    x.addDelay(1, 1000);
    while(!doneFlag)
    {
	//sleep(1);
        x.getNext();
        x.addDelay(1, 1000);
        
	binsLock.ReadLock();
	BinsNodeType * curr = bins.getFirst();
	binsLock.Unlock();
	while(curr)
	{
	    binsLock.ReadLock();
	    int bin2collect = curr->val->currBin+1;
	    if(bin2collect >= curr->val->myDelay+2)
		bin2collect = 0;

	    SipTransactionList<SipMsgContainer*>* 
		swap = &(curr->val->bin[bin2collect]);

	    binsLock.Unlock();
	    SipTransactionList<SipMsgContainer*>::SipTransListNode *garb;
	    garb = swap->getLast();
	    while(garb)
	    {
                garb->val->assertNotDeleted();
		trash(garb->val);
		swap->erase(garb);
		garb = swap->getLast();
	    }
	    
	    binsLock.WriteLock();
	    curr->val->currBin = bin2collect;
	    binsLock.Unlock();
	    curr = bins.getNext(curr);
	}
    }
}


void
SipTransactionGC::trash(SipMsgContainer* item)
{

    SipTransLevel1Node * refTopNode = 0;

    /// check if it has a transaction associated
    /// (otherwise it could have been a filter retrans etc.)
    if(item->level3Ptr)
    {
     	refTopNode = item->level3Ptr->level2Ptr->topNode;
	refTopNode->lock.lock();
	
	if(item->level3Ptr->msgs.request == item)
	    item->level3Ptr->msgs.request = 0;
	else if(item->level3Ptr->msgs.response == item)
	    item->level3Ptr->msgs.response = 0;
	else
	{
	    cpLog(LOG_ERR,"Dangling item:[%s]", item->msg.out.c_str());
	}
	
	if(item->level3Ptr->msgs.response ==
	   item->level3Ptr->msgs.request)
	{
	    SipTransLevel3Node* level3Node = item->level3Ptr;
	    level3Node->level2Ptr->level3.erase(level3Node->myPtr);
	    
	    if(level3Node->level2Ptr->level3.getFirst()==0)
	    {
		SipTransLevel2Node *level2Node = level3Node->level2Ptr;
		level2Node->topNode->level2.erase(level2Node->myPtr);
		
		if(level2Node->topNode->level2.getFirst()==0)
		{
		    SipTransHashTable::SipTransRWLockHelper helper;
		    /// the following three statments flush out any slow
		    /// executing executions that might be working w/
		    /// the concerned nodes
		    level2Node->topNode->lock.unlock();
		    
		    level2Node->topNode->bucketNode->myTable->lock(
			level2Node->topNode->bucketNode, &helper);
		    
		    level2Node->topNode->lock.lock();
		    
		    /// hence, now recheck if REALLY want to delete
		    if(level2Node->topNode->level2.getFirst()==0)
		    {
			level2Node->topNode->bucketNode->myTable->erase(
			    level2Node->topNode->bucketNode);
			level2Node->topNode->lock.unlock();
			delete level2Node->topNode;
			level2Node->topNode = 0;
			refTopNode = 0;
		    }
		}
		cpLog(DEBUG_NEW_STACK,"trashing at level-II:%s",
		      level2Node->myKey.logData());
		delete level2Node;
	    }
	    cpLog(DEBUG_NEW_STACK,"trashing at level-III:%s",
		  level3Node->myKey.logData());
	    delete level3Node;
	}
    }
    cpLog(DEBUG_NEW_STACK,"trashing item:[%s]",item->msg.out.c_str());
    delete item;
    
    if(refTopNode)
    {
	refTopNode->lock.unlock();
    }
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
