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
#include "SipCallContainer.hxx"
#include <sstream>

using namespace Vocal;

unsigned int SipCallContainer::_cnt = 0;

SipCallContainer::SipCallContainer(const SipTransactionId& call_id)
      : id(call_id) {
   setSeq = false;
   curSeqNum = 0;
   _cnt++;
   purgeAt = 0;
}


string SipCallContainer::toString() {
   ostringstream oss;
   oss << "SipCallContainer, id: " << id.toString() << "\n"
       << " msgs.size: " << msgs.size() << endl;
   return oss.str();
}


Sptr<SipMsgPair> SipCallContainer::findMsgPair(const SipTransactionId& id) {
   list<Sptr<SipMsgPair> >::iterator i = msgs.begin();
   while (i != msgs.end()) {
      Sptr<SipMsgPair> mp = *i;
      if ((mp->request != 0) && mp->request->matches(id)) {
         return (*i);
      }
      if ((mp->response != 0) && mp->response->matches(id)) {
         return (*i);
      }
      //cpLog(LOG_ERR, "mp->req: %s did not match id: %s\n",
      //      (mp->request != 0) ? mp->request->toString().c_str() : "NULL",
      //      id.toString().c_str());
      //cpLog(LOG_ERR, "mp->response: %s did not match id: %s\n",
      //      (mp->response != 0) ? mp->response->toString().c_str() : "NULL",
      //      id.toString().c_str());
      i++;
   }
   return NULL;
}//findMsgPair

void SipCallContainer::stopAllRetrans() {
   list<Sptr<SipMsgPair> >::iterator i = msgs.begin();
   while (i != msgs.end()) {
      Sptr<SipMsgPair> mp = *i;
      if ((mp->request != 0) && (mp->request->getRetransmitMax() != 0)) {
         cpLog(LOG_DEBUG, "Stopping retransmit of msg due to stopAllRetrans\n");
         mp->request->setRetransmitMax(0);
      }
      i++;
   }
}//stopAllRetrans

Sptr<SipMsgPair> SipCallContainer::findMsgPair(Method method) {
   list<Sptr<SipMsgPair> >::iterator i = msgs.begin();
   while (i != msgs.end()) {
      Sptr<SipMsgPair> mp = *i;
      if ((mp->request != 0) &&
          (mp->request->getMsgIn() != 0) &&
          (mp->request->getMsgIn()->getType() == method)) {
         return (*i);
      }
      if ((mp->response != 0) &&
          (mp->response->getMsgIn() != 0) &&
          (mp->response->getMsgIn()->getType() == method)) {
         return (*i);
      }
      i++;
   }
   return NULL;
}


void SipCallContainer::addMsgPair(Sptr<SipMsgPair> m) {
   msgs.push_back(m);
   cpLog(LOG_DEBUG, "Added msg-pair: %p to SipCallContainer, this: %p, size: %d\n",
         m.getPtr(), this, msgs.size());
}



void SipCallContainer::clear(const char* debug) {
   cpLog(LOG_DEBUG, "Clearing SipCallContainer: %p, debug: %s\n",
         this, debug);
   msgs.clear();
   setSeq = false;
   curSeqNum = 0;
}
