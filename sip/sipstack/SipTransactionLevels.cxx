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

static const char* const SipTransactionLevels_cxx_version =
    "$Id: SipTransactionLevels.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipTransactionLevels.hxx"
#include <sstream>

using namespace Vocal;

SipTransactionList<SipTransLevel2Node*>::SipTransListNode *
SipTransLevel1Node::findOrInsert(const SipTransactionId& id)
{
    SipTransactionList<SipTransLevel2Node*>::SipTransListNode * curr =
	level2.getFirst();
    while(curr)
    {
	if(curr->val->myKey == id.getLevel2())
	    break;
	curr = level2.getNext(curr);
    }
    if(!curr)
    {
	cpLog(DEBUG_NEW_STACK,"inserting a New Level2 Node [%s]",
	      id.getLevel2().logData());

	SipTransLevel2Node * newNode = new SipTransLevel2Node;
	newNode->myKey = id.getLevel2();
	curr = level2.insert(newNode);
	newNode->topNode = this;
	newNode->myPtr = curr;
    }
    else
    {
	cpLog(DEBUG_NEW_STACK,"Level2 Node [%s] already exists as %s",
	      id.getLevel2().logData(),
	      curr->val->myKey.logData());
    }
	
    return curr;
}

SipTransactionList<SipTransLevel2Node*>::SipTransListNode *
SipTransLevel1Node::find(const SipTransactionId& id)
{
    SipTransactionList<SipTransLevel2Node*>::SipTransListNode * curr =
	level2.getFirst();
    while(curr)
    {
	if(curr->val->myKey == id.getLevel2())
	    break;
	curr = level2.getNext(curr);
    }
    if(curr)
	return curr;
    else
	return 0;
}

SipTransactionList<SipTransLevel3Node *>::SipTransListNode *
SipTransLevel1Node::findLevel3AckInvite( const SipTransactionId& id )
{
    SipTransactionList<SipTransLevel2Node *>::SipTransListNode * level2Node =
	level2.getLast();
    SipTransactionList<SipTransLevel3Node *>::SipTransListNode * level3Node =
	0;
    while( level2Node )
    {
	if( level2Node->val->myKey.matchChar("V") == id.getLevel2().matchChar("V") )
        {
	    cpLog( LOG_DEBUG_STACK, "CSeq number matched" );
            level3Node = level2Node->val->findInvite();
	    if( level3Node )
	        break;
	}
	level2Node = level2.getPrev( level2Node );
    }
    return level3Node;
}

SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
SipTransLevel2Node::findOrInsert(const SipTransactionId& id)
{
    SipTransactionList<SipTransLevel3Node*>::SipTransListNode * curr =
	level3.getFirst();
    while(curr)
    {
	if(curr->val->myKey == id.getLevel3())
	    break;
	curr = level3.getNext(curr);
    }
    if(!curr)
    {
	cpLog(DEBUG_NEW_STACK,"inserting a New Level3 Node [%s]",
	      id.getLevel3().logData());
	SipTransLevel3Node * newNode = new SipTransLevel3Node;
	newNode->myKey = id.getLevel3();
	curr = level3.insert(newNode);
	newNode->level2Ptr = this;
	newNode->myPtr = curr;
    }
    return curr;
}

SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
SipTransLevel2Node::find(const SipTransactionId& id)
{
    SipTransactionList<SipTransLevel3Node*>::SipTransListNode * curr =
	level3.getFirst();
    while(curr)
    {
	if(curr->val->myKey == id.getLevel3())
	    break;
	curr = level3.getNext(curr);
    }
    if(curr)
	return curr;
    else
	return 0;
}

SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
SipTransLevel2Node::findInvite()
{
    SipTransactionList<SipTransLevel3Node*>::SipTransListNode * curr =
	level3.getFirst();
    while(curr)
    {
	if(curr->val->myKey == INVITE_METHOD)
	{
	    cpLog( LOG_DEBUG_STACK, "Found INVITE" );
	    break;
	}
	curr = level3.getNext(curr);
    }
    if(curr)
	return curr;
    else
	return 0;
}


string SipMsgContainer::toString() const {
    ostringstream oss;
    if (msg.in.getPtr()) {
        oss << " in: " << msg.in->toString() << endl;
    }
    else {
        oss << " in: NULL\n";
    }

    
    oss << "out: " << msg.out.c_str()
        << "\nnetAddr: ";

    if (msg.netAddr != 0) {
        oss << msg.netAddr->toString();
    }
    else {
        oss << "NULL";
    }

    oss << "\ntype: " << msg.type << " transport: " << msg.transport.c_str()
        << "\nretransCount: " << retransCount << " collected: " << collected
        << endl;

    return oss.str();
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
