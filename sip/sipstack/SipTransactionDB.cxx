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

static const char* const SipTransactionDB_cxx_version =
    "$Id: SipTransactionDB.cxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include "global.h"
#include "SipTransactionDB.hxx"

using namespace Vocal;

/*
SipTransactionDB::SipTransactionDB(const string& _local_ip)
    : table()
{
    local_ip = _local_ip;
}
*/

SipTransactionDB::SipTransactionDB(int size, const string& _local_ip)
    : table(size)
{
    local_ip = _local_ip;
}


SipTransactionDB::~SipTransactionDB()
{
    /// clean up the table...

    /// TODO: GC all the items...
}


Data
SipTransactionDB::getDetails()
{
    return table.giveDetails();
}


void
SipTransactionDB::cancel(SipMsgContainer *msg)
{
    if(!msg) 
    {
        return;
    }

    /// just set the retrans count to 0, and the transport stack will
    /// feed it to cleanup thread
    msg->retransCount = 0;
}


SipTransactionDB::CallLegVector
SipTransactionDB::getCallLegMsgs(Sptr<SipMsg>& sipMsg)
{
    CallLegVector retVal;
    SipTransactionId id(*sipMsg);

    SipTransLevel1Node * topNode = getTopNode(id,sipMsg);
    if(topNode)
    {
	SipTransactionList<SipTransLevel2Node*>::SipTransListNode*
            level2Node = topNode->level2.getFirst();
	while(level2Node)
	{
	    SipTransactionList<SipTransLevel3Node*>::SipTransListNode 
		*level3Node = level2Node->val->level3.getFirst();
	    while(level3Node)
	    {
		if(level3Node->val->msgs.request)
                {
                    Data toBeEatenData 
                        = level3Node->val->msgs.request->msg.out;
                    SipMsg* sipMsg = SipMsg::decode(toBeEatenData, local_ip);
                    sipMsg->setSendAddress(*(level3Node->val->msgs.request->msg.netAddr));
                    retVal.push_back(sipMsg);
		    //retVal.push_back(SipMsg::decode(toBeEatenData));
                }
		if(level3Node->val->msgs.response)
                {
                    Data toBeEatenData 
                        = level3Node->val->msgs.response->msg.out;
		    retVal.push_back(SipMsg::decode(toBeEatenData, local_ip));
                }
		level3Node = level2Node->val->level3.getNext(level3Node);
	    }
	    level2Node = topNode->level2.getNext(level2Node);
	}
    }

    return retVal;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
