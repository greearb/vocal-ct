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


static const char* const MultiLegCallData_cxx_Version =
    "$Id: MultiLegCallData.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "MultiLegCallData.hxx" 
 

void 
MultiLegCallData::copyObj(const MultiLegCallData& src)
{
    myCallLegDataMap.erase(myCallLegDataMap.begin(), myCallLegDataMap.end());
    myTransactionPeerMap.erase(myTransactionPeerMap.begin(), myTransactionPeerMap.end());
#ifndef WIN32
    myCallLegDataMap.insert(src.myCallLegDataMap.begin(), src.myCallLegDataMap.end());
    myTransactionPeerMap.insert(src.myTransactionPeerMap.begin(), src.myTransactionPeerMap.end());
#else
        CallLegDataMap::const_iterator tmpCallLegIter;
        for(tmpCallLegIter = src.myCallLegDataMap.begin(); tmpCallLegIter != src.myCallLegDataMap.end(); ++tmpCallLegIter)
        {
         myCallLegDataMap.insert(*tmpCallLegIter);
        }

        TransactionPeerMap::const_iterator tmpTransPeerIter;
        for(tmpTransPeerIter = src.myTransactionPeerMap.begin(); tmpTransPeerIter != src.myTransactionPeerMap.end(); ++tmpTransPeerIter)
        {
         myTransactionPeerMap.insert(*tmpTransPeerIter);
        }
#endif
    myAccountingData = src.myAccountingData;
}

Sptr<SipCallLegData>  
MultiLegCallData::getCallLeg(const SipCallLeg& callLeg)
{
    CallLegDataMap::iterator itr = myCallLegDataMap.find(callLeg);
    if(itr != myCallLegDataMap.end())
    {
        return(itr->second);
    }
    else
    {
        cpLog(LOG_WARNING, "Data not found for callLeg %s", callLeg.encode().logData());
        assert(0);
        Sptr<SipCallLegData> nll;
        return nll; //quiet compiler warning.
    }
}

void 
MultiLegCallData::addCallLeg(const SipCallLeg& callLeg, Sptr<SipCallLegData> legData)
{
    assert(myCallLegDataMap.count(callLeg) == 0);
    myCallLegDataMap[callLeg] = legData;
}

void 
MultiLegCallData::removeCallLeg(const SipCallLeg& callLeg)
{
    CallLegDataMap::iterator itr = myCallLegDataMap.find(callLeg);
    if(itr != myCallLegDataMap.end())
    {
        myCallLegDataMap.erase(itr);
    }
}


Sptr<SipTransactionPeers> 
MultiLegCallData::findPeer(const SipTransactionId& trId)
{
     Sptr<SipTransactionPeers> retVal = 0;
     TransactionPeerMap::iterator itr = myTransactionPeerMap.find(trId); 
     if(itr != myTransactionPeerMap.end())
     {
         retVal = (*itr).second;
     }
     return retVal;
}


void 
MultiLegCallData::addTransactionPeer(const Sptr<SipTransactionPeers>& peer)
{
    assert(myTransactionPeerMap.count(*(peer->getTrId())) == 0);
    myTransactionPeerMap[*(peer->getTrId())] = peer;
}

void 
MultiLegCallData::removeTransactionPeer(const SipTransactionId& trId)
{
     TransactionPeerMap::iterator itr = myTransactionPeerMap.find(trId); 
     if(itr != myTransactionPeerMap.end())
     {
         myTransactionPeerMap.erase(itr);
     }
     else
     {
         cpLog(LOG_WARNING, "Failed to remove transaction id");
     }
}

MultiLegCallData::~MultiLegCallData()
{
}
