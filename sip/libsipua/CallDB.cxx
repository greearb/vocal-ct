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


static const char* const CallDB_cxx_Version =
    "$Id: CallDB.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "UaDef.hxx" 
#include "CallDB.hxx" 
#include "Lock.hxx"

using namespace Vocal::UA;
using Vocal::Threads::Lock;

CallDB* CallDB::myInstance = 0;

CallDB& 
CallDB::instance()
{
    if(myInstance == 0)
    {
        myInstance = new CallDB();
    }
    return *myInstance;
}

void
CallDB::destroy(void)
{
    cpLog(LOG_DEBUG, "CallDB::destroy");
    delete CallDB::myInstance;
    CallDB::myInstance = 0;
}


CallDB::~CallDB()
{
}

CallDB::CallDB()
{
}

void
CallDB::addCallLeg(Sptr<UaBase> agent)
{
    Lock lock(myMutex);
    const SipCallLeg& cLeg = agent->getCallLeg();
    assert(myMultiLegCallDataMap.count(cLeg) == 0);
    Sptr<MultiLegCallData> mData = new MultiLegCallData();
    mData->addCallLeg(cLeg, agent);

    Sptr<SipTransactionId> trId = new SipTransactionId(*(agent->getRequest()));
    Sptr<SipTransactionPeers> tPeer = new SipTransactionPeers(trId);
    tPeer->addPeer(cLeg);
    mData->addTransactionPeer(tPeer);

    myMultiLegCallDataMap[cLeg] = mData;
}

void
CallDB::addPeer(Sptr<UaBase> userAgent, Sptr<UaBase> peerAgent)
{
    const SipCallLeg& cLeg = userAgent->getCallLeg();
    const SipCallLeg& peerLeg = peerAgent->getCallLeg();

    assert(myMultiLegCallDataMap.count(cLeg) != 0);

    //First update userAget data, with peer init
    Sptr<MultiLegCallData> umData = myMultiLegCallDataMap[cLeg];

    SipTransactionId trId(*(userAgent->getRequest()));
    Sptr<SipTransactionPeers> tPeer = umData->findPeer(trId);
    assert(tPeer != 0);
    tPeer->addPeer(peerLeg);


    //Now add peer data with userAgent init 
    myMultiLegCallDataMap[peerLeg] = umData;
    umData->addCallLeg(peerLeg, peerAgent);

    Sptr<SipTransactionId> peertrId = new SipTransactionId(*(peerAgent->getRequest()));
    Sptr<SipTransactionPeers> peertPeer = new SipTransactionPeers(peertrId);
    peertPeer->addPeer(peerLeg);
    peertPeer->addPeer(cLeg);
    umData->addTransactionPeer(peertPeer);

    //Add the new contact
    Sptr<ContactData> contactData = new ContactData(peerAgent);
    userAgent->pushContact(contactData);

    Sptr<ContactData> contactData2 = new ContactData(userAgent);
    peerAgent->pushContact(contactData2);
}

Sptr<MultiLegCallData> 
CallDB::getMultiLegCallData(const SipCallLeg& callLeg)
{
    Lock lock(myMutex);
    //cerr << "CallDB::getMultiLegCallData:" << myMultiLegCallDataMap.size() << endl; 
    Sptr<MultiLegCallData> retVal = 0;
    MultiLegCallDataMap::iterator itr = myMultiLegCallDataMap.find(callLeg);
    if(itr != myMultiLegCallDataMap.end())
    {
        retVal = (*itr).second;
    }
	 else {
		 cpLog(LOG_DEBUG, "CallDB Call Leg not found %s", callLeg.encode().logData());
	 }
    return retVal;
}

void 
CallDB::setAccountingData(const SipCallLeg& callLeg, int sessionId, Sptr<AccountingData> aData)
{
    //cerr << "Accounting Data map size:" << myAccountingDataMap.size() << endl;
    Sptr<MultiLegCallData> mData = getMultiLegCallData(callLeg);
    assert(mData != 0);
    mData->setAccountingData(aData);
    myAccountingDataMap[sessionId] = mData; 
}

Sptr<AccountingData> 
CallDB::getAccountingData( int sessionId)
{
    Sptr<AccountingData> retVal = 0;
    AccountingDataMap::iterator itr = myAccountingDataMap.find(sessionId);
    if(itr != myAccountingDataMap.end())
    {
        Sptr<MultiLegCallData> mData = (*itr).second;
        retVal = mData->getAccountingData();
    }
    return retVal;
}


Sptr<UserAgentPeerList>
CallDB::findAllPeers(const UaBase& userAgent)
{
    Sptr<UserAgentPeerList> retVal = new UserAgentPeerList;
    const SipCallLeg& cLeg = userAgent.getCallLeg();
    Sptr<MultiLegCallData> mData = getMultiLegCallData(cLeg);
    if(mData == 0)
    {
        cpLog(LOG_DEBUG, "No peer found for leg %s", cLeg.encode().logData());
        return retVal;
    };

    SipTransactionId trId( *(userAgent.getRequest())); 
    Sptr<SipTransactionPeers> tPeer = mData->findPeer(trId);
    if(tPeer == 0)
    {
        cpLog(LOG_DEBUG, "No peer found for leg %s", cLeg.encode().logData());
        return retVal;
    }
    const SipTransactionPeers::SipCallLegList& clegList = tPeer->getPeerList();    
    for(SipTransactionPeers::SipCallLegList::const_iterator itr = clegList.begin();
                                       itr != clegList.end(); itr++)
    {
        Sptr<SipCallLegData> cLegData = mData->getCallLeg(*itr);
        assert(cLegData != 0);
        Sptr<UaBase> uBase;
        uBase.dynamicCast(cLegData);
        assert(uBase != 0);
        if(uBase->getCallLeg() != userAgent.getCallLeg())
        {
            retVal->push_back(uBase);
        }
    }
    return retVal;
}

int 
CallDB::removePeer(const UaBase& srcAgent, const UaBase& peerAgent)
{
	 cpLog(LOG_INFO, "RemovePeer Agent-Peer");

    Lock lock(myMutex);
    int retVal =0;
    Sptr<MultiLegCallData> mData = 0;
    MultiLegCallDataMap::iterator itr = myMultiLegCallDataMap.find(peerAgent.getCallLeg());
    if(itr != myMultiLegCallDataMap.end())
    {
        return (cleanupTransactionPeer(srcAgent, peerAgent, (*itr).second));
    }
    else
    {
        cpLog(LOG_WARNING, "Failed to get the CallLeg entry");
    }
    return (retVal);
}
///
void 
CallDB::removeAccountingData(int sessionId)
{
    AccountingDataMap::iterator itr = myAccountingDataMap.find(sessionId);
    if(itr != myAccountingDataMap.end())
    {
        myAccountingDataMap.erase(itr);
    }
}

///Simply remove agent and all its peers from the CallDB
void 
CallDB::removePeer(const UaBase& agent)
{
	cpLog(LOG_INFO, "RemovePeer Agent and all Peers");

   Sptr<UserAgentPeerList> pList = findAllPeers(agent); 
   for(UserAgentPeerList::iterator itr = pList->begin();
                                    itr != pList->end(); itr++)
   {
        const SipCallLeg& cLeg = (*itr)->getCallLeg();
        Lock lock(myMutex);
        MultiLegCallDataMap::iterator itr2 = myMultiLegCallDataMap.find(cLeg); 
        if(itr2 != myMultiLegCallDataMap.end())
        {
            cleanupTransactionPeer(agent, *(*itr), (*itr2).second);
            myMultiLegCallDataMap.erase(itr2);
        }
   }

   removeCallData(agent);
}

int
CallDB::cleanupTransactionPeer(const UaBase &srcAgent, const UaBase& peerAgent,
                               Sptr<MultiLegCallData> mData)
{
    SipTransactionId srcTrId(*(srcAgent.getRequest()));
    SipTransactionId trId(*(peerAgent.getRequest()));
    //First remove its own reference
    Sptr<SipTransactionPeers> tPeer = mData->findPeer(trId);
    if(tPeer == 0)  return 0;
    tPeer->removePeer(srcAgent.getCallLeg());
 
    //Now remove its reference from srcAgent
    Sptr<SipTransactionPeers> tPeer2 = mData->findPeer(srcTrId);
    if(tPeer2 != 0)
    {
        tPeer2->removePeer(peerAgent.getCallLeg());
        return( tPeer2->getPeerList().size() - 1);
    }
    else
    {
        //No peer
        return 0;
    }
    return 0;
}

void
CallDB::removeCallData(const UaBase& agent)
{
   Lock lock(myMutex);
   MultiLegCallDataMap::iterator itr = myMultiLegCallDataMap.find(agent.getCallLeg()); 
   if(itr != myMultiLegCallDataMap.end())
   {
        SipTransactionId trId(*(agent.getRequest()));
        Sptr<MultiLegCallData> mData = (*itr).second;
        Sptr<SipTransactionPeers> tPeer = mData->findPeer(trId);
        if(tPeer == 0)
        {
            cpLog(LOG_DEBUG, "No call data found for peer, ignore");
            return;
        }
        if(tPeer->getPeerList().size()== 1)
        {
            //Peer is the only one remaining, remove its ref from the CallDB
            mData->removeTransactionPeer(trId);
            mData->removeCallLeg(agent.getCallLeg());
            myMultiLegCallDataMap.erase(itr);
        }
   }
}


