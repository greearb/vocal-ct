#ifndef SipTransactionPeers_hxx
#define SipTransactionPeers_hxx

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


static const char* const SipTransactionPeers_hxx_Version =
    "$Id: SipTransactionPeers.hxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include "SipTransactionId.hxx" 
#include "SipCallLeg.hxx" 
#include "UaDef.hxx"
#include <BugCatcher.hxx>

using Vocal::SipMsg;
using Vocal::SipCallLeg;
using Vocal::SipTransactionId;

/**  Object SipTransactionPeers
<pre>
 <br> Usage of this Class </br>

   Contains peer information for a given transaction

Example:

 class ClassFoo
 {
    public:

     void addCallLeg(Sptr<UaBase> agent);
 }

 void
 ClassFoo::addCallLeg(Sptr<UaBase> agent)
   {

      Sptr<MultiLegCallData> mData = new MultiLegCallData();
      mData->addCallLeg(cLeg, agent);

      Sptr<SipTransactionId> trId = new SipTransactionId(*(agent->getRequest()));

      Sptr<SipTransactionPeers> tPeer = new SipTransactionPeers(trId);

      tPeer->addPeer(cLeg);

      mData->addTransactionPeer(tPeer);
  }

  For detailed Informantion about extensive use of this object please
  refer to /vocal/sip/libsipua/CallDB.cxx or
  /vocal/sip/libsipua/MultiLegCallData.cxx

</pre>
*/
class SipTransactionPeers : public BugCatcher {
public:
   ///
   typedef vector<SipCallLeg > SipCallLegList;

   ///
   SipTransactionPeers( const Sptr<SipTransactionId>& trId ): myTrId(trId) { };
   
   ///
   void addPeer(const SipCallLeg& callLeg);

   ///
   void removePeer(const SipCallLeg& callLeg);
   
   ///
   const SipCallLegList&  getPeerList()  const { return myCallLegList; };
   
   ///
   const Sptr<SipTransactionId>& getTrId() const { return myTrId; };
   
private:
   /// 
   Sptr<SipTransactionId> myTrId;
   ///
   SipCallLegList myCallLegList; 

   // Not implemented
   SipTransactionPeers( const SipTransactionPeers& src );
   SipTransactionPeers& operator=(const SipTransactionPeers& src );

};


#endif
