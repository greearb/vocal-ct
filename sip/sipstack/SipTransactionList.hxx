#ifndef _Sip_Transaction_List__hxx
#define _Sip_Transaction_List__hxx

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

static const char* const SipTransactionList_hxx_version =
    "$Id: SipTransactionList.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

namespace Vocal
{
    
template <class NodeType> class SipTransactionList
{
    public:
	struct SipTransListNode;

	SipTransactionList()
	    : first(), last()
	{
	    first.next = &last;
	    last.prev  = &first;
	}

	~SipTransactionList()
	{
	    SipTransListNode *curr=first.next;
	    
	    while(curr != &last)
	    {
		SipTransListNode* tmp = curr;
		curr = curr->next;
		delete tmp;
	    }
	}

	SipTransListNode* getFirst()
	{
	    if(first.next == &last)
		return 0;
	    else
		return first.next;
	}

	SipTransListNode* getLast()
	{
	    if(last.prev == &first)
		return 0;
	    else
		return last.prev;
	}

	SipTransListNode* getNext(SipTransListNode* curr)
	{
	    if(!curr || curr == last.prev)
		return 0;
	    else
		return curr->next;
	}

	SipTransListNode* getPrev(SipTransListNode* curr)
	{
	    if(!curr || curr == first.next)
		return 0;
	    else
		return curr->prev;
	}

	void erase(SipTransListNode* curr)
	{
	    if(curr)
	    {
		curr->prev->next = curr->next;
		curr->next->prev = curr->prev;
		delete curr;
	    }
	}
    
	SipTransListNode* insert(const NodeType& val)
	{
	    SipTransListNode * newNode = new SipTransListNode(val);
	    
	    newNode->next = first.next;
	    newNode->prev = newNode->next->prev;
	    first.next = newNode;
	    newNode->next->prev = newNode;
	    return newNode;
	}

	struct SipTransListNode
	{
		NodeType val;
		NodeType& operator()()
		{ return val;}

		SipTransListNode()
		    : val(), prev(0), next(0)
		{}
		SipTransListNode(const NodeType& val)
		    : val(val), prev(0), next(0)
		{}
		~SipTransListNode()
		{}
    
		SipTransListNode& operator=(const SipTransListNode& other);
		SipTransListNode * prev;
		SipTransListNode * next;
	};
    
    private:
	SipTransListNode first;
	SipTransListNode last;

};

 
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
