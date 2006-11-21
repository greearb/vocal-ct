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


#include "UaDef.hxx" 
#include "CallDB.hxx" 
#include <sstream>

using namespace Vocal::UA;
using namespace std;

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

void CallDB::addCallLeg(Sptr<UaBase> agent) {
   cpLog(LOG_INFO, "CallDB:  Adding call leg for agent: %p", agent.getPtr());
   SipCallLeg cLeg = agent->getCallLeg();
   assert(myMultiLegCallDataMap.count(cLeg) == 0);
   Sptr<MultiLegCallData> mData = new MultiLegCallData();
   mData->addCallLeg(cLeg, agent.getPtr());

   Sptr<SipTransactionId> trId = new SipTransactionId(*(agent->getRequest()));
   Sptr<SipTransactionPeers> tPeer = new SipTransactionPeers(trId);
   tPeer->addPeer(cLeg);
   mData->addTransactionPeer(tPeer);

   myMultiLegCallDataMap[cLeg] = mData;
}

Sptr<MultiLegCallData> CallDB::getMultiLegCallData(const SipCallLeg& callLeg) {
   //cerr << "CallDB::getMultiLegCallData:" << myMultiLegCallDataMap.size() << endl; 
   Sptr<MultiLegCallData> retVal = 0;
   MultiLegCallDataMap::iterator itr = myMultiLegCallDataMap.find(callLeg);
   if (itr != myMultiLegCallDataMap.end()) {
        retVal = (*itr).second;
   }
   else {
      cpLog(LOG_DEBUG, "CallDB Call Leg match not found %s, sz: %d",
            callLeg.encode().logData(), myMultiLegCallDataMap.size());
      for (itr = myMultiLegCallDataMap.begin(); itr != myMultiLegCallDataMap.end(); itr++) {
         cpLog(LOG_DEBUG, "Possible match: %s", (*itr).first.encode().c_str());         
      }
   }
   return retVal;
}

string CallDB::toString() {
   ostringstream oss;
   oss << "  CallDB: \n";
   MultiLegCallDataMap::iterator itr = myMultiLegCallDataMap.begin();
   while (itr != myMultiLegCallDataMap.end()) {
      oss << "  cleg: " <<  (*itr).first.toString() << endl;
      oss << "   call-data:  " << (*itr).second->toString() << endl;
      itr++;
   }
   return oss.str();
}

void CallDB::removeCallData(const UaBase& agent) {
   cpLog(LOG_INFO, "CallDB::removeCallData, agent: %p  cleg: %s", &agent, agent.getCallLeg().toString().c_str());
   MultiLegCallDataMap::iterator itr = myMultiLegCallDataMap.find(agent.getCallLeg()); 
   if (itr != myMultiLegCallDataMap.end()) {
      SipTransactionId trId(*(agent.getRequest()));
      Sptr<MultiLegCallData> mData = (*itr).second;
      Sptr<SipTransactionPeers> tPeer = mData->findPeer(trId);
      if (tPeer == 0) {
         cpLog(LOG_INFO, "No call data found for peer, removing agent mapping.");
         myMultiLegCallDataMap.erase(itr);
      }
      else {
         if (tPeer->getPeerList().size() == 1) {
            //Peer is the only one remaining, remove its ref from the CallDB
            mData->removeTransactionPeer(trId);
            mData->removeCallLeg(agent.getCallLeg());
            myMultiLegCallDataMap.erase(itr);
         }
      }
   }
   else {
      cpLog(LOG_INFO, "Couldn't find agent: %p by call-leg", &agent);
   }
   cpLog(LOG_DEBUG_STACK, "  %s", toString().c_str());
}


