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
#include "SipTransactionDB.hxx"

using namespace Vocal;


SipTransactionDB::SipTransactionDB(const string& _local_ip) {
   local_ip = _local_ip;
}


SipTransactionDB::~SipTransactionDB() {
   // Nothing to do at this point.
}


string SipTransactionDB::toString() {
   string rv;
   map <SipTransactionId::KeyTypeII, Sptr<SipCallContainer> >::iterator i = table.begin();
   while(i != table.end()) {
      rv += i->first.c_str();
      rv += "\n";
      i++;
   }
   return rv;
}


Sptr<SipCallContainer> SipTransactionDB::getCallContainer(const SipTransactionId& id) {
   string k = id.getLevel1().c_str();
   map <SipTransactionId::KeyTypeI, Sptr<SipCallContainer> >::iterator i = table.find(k);
   if (i != table.end()) {
      return i->second;
   }
   cpLog(LOG_WARNING, "Could not find call container -:%s:-, table:\n%s",
         k.c_str(), toString().c_str());
   return NULL;
}

void SipTransactionDB::addCallContainer(Sptr<SipCallContainer> m) {
   string k(m->getTransactionId().getLevel1().c_str());
   cpLog(LOG_WARNING, "Adding call container -:%s:-", k.c_str());
   table[k] = m;
}

void SipTransactionDB::setPurgeTimer(const SipTransactionId& id) {
   Sptr<SipCallContainer> c = getCallContainer(id);
   if (c != 0) {
      c->setPurgeTimer(vgetCurMs() + (10 * 1000)); // 10 seconds
   }
}


void SipTransactionDB::purgeOldCalls(uint64 now) {
   map <SipTransactionId::KeyTypeII, Sptr<SipCallContainer> >::iterator i = table.begin();
   map <SipTransactionId::KeyTypeII, Sptr<SipCallContainer> >::iterator tmpi;
   while(i != table.end()) {
      uint64 p = i->second->getPurgeTimer();
      string key = i->second->getTransactionId().getLevel1().c_str();
      if (p && (p < now)) {
         tmpi = i;
         i++;
         //TODO:  Make sure this does not invalidate iterator i.
         table.erase(tmpi);
         cpLog(LOG_WARNING, "Purged call: %s from transaction DB, size: %i\n",
               key.c_str(), table.size());
      }
      else {
         i++;
      }
   }
}//purgeOldCalls

