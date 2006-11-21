#ifndef MultiLegCallData_hxx
#define MultiLegCallData_hxx

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


#include "global.h"

#include <map>

#include "SipTransactionPeers.hxx" 
#include "SipCallLegData.hxx" 
#include "AccountingData.hxx" 
#include <BugCatcher.hxx>


/** Object MultiLegCallData
<pre>
 <br> Usage of this Class </br>

   Data container for holding call-leg data for Multiple call-legs in a session.
   A call may involve multiple call-legs, for example if A is connected
   to user B and C in a session, the MultiLegCallData will hold the
   relationships as

   ___________ 
  | A  ---> B |
  |    ---> C |
   -----------
  | B  ---> A |
   -----------
  | C  ---> B |
   -----------

Example:

class ClassFoo
 {
    public:

     typedef map<SipCallLeg , Sptr<MultiLegCallData> > MultiLegCallDataMap;
     void addCallLeg(Sptr<UaBase> agent);

    private:

       MultiLegCallDataMap myMultiLegCallDataMap;
 }

 void
 ClassFoo::addCallLeg(Sptr<UaBase> agent)
   {
      const SipCallLeg& cLeg = agent->getCallLeg();
</br> Sptr<MultiLegCallData> mData = new MultiLegCallData();</br>
      mData->addCallLeg(cLeg, agent);

      Sptr<SipTransactionId> trId = new SipTransactionId(*(agent->getRequest()));
      Sptr<SipTransactionPeers> tPeer = new SipTransactionPeers(trId);
      tPeer->addPeer(cLeg);
      mData->addTransactionPeer(tPeer);
      myMultiLegCallDataMap[cLeg] = mData;
   }

   For detailed Informantion about extensive use of this object please
   refer to /vocal/sip/libsipua/CallDB.cxx

</pre>
*/
class MultiLegCallData : public BugCatcher {
public:
   
   /// Create one with default values
   MultiLegCallData() : myAccountingData(0) { };
   
   ///
   ~MultiLegCallData();
   ///
   MultiLegCallData( const MultiLegCallData& src ) {
      copyObj(src);
   }
   
   string toString();

   /// 
   const MultiLegCallData& operator =( const MultiLegCallData& src ) {
      if (this != &src) {
         copyObj(src);
      }
      return *this;
   }

   ///
   void copyObj(const MultiLegCallData& src);

   ///Get the call-leg data for a given callLeg
   Sptr<SipCallLegData>  getCallLeg(const SipCallLeg& callLeg);

   ///Add call-leg data for a given callLeg
   void addCallLeg(const SipCallLeg& callLeg, Sptr<SipCallLegData> legData);

   ///
   void removeCallLeg(const SipCallLeg& callLeg);

   ///
   Sptr<SipTransactionPeers> findPeer(const SipTransactionId& trId);

   ///
   void addTransactionPeer(const Sptr<SipTransactionPeers>& peer);

   ///
   void removeTransactionPeer(const SipTransactionId& peer);

   ///
   const Sptr<AccountingData> getAccountingData() const { return myAccountingData; };

   ///
   void setAccountingData(const Sptr<AccountingData>& accData) { myAccountingData = accData; };

private:
   /// 
   map<SipCallLeg, Sptr<SipCallLegData> > myCallLegDataMap;
   ///
   map<SipTransactionId, Sptr<SipTransactionPeers> > myTransactionPeerMap;
   //
   Sptr<AccountingData> myAccountingData;
};


#endif
