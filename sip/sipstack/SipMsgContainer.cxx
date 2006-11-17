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
#include "SipMsgContainer.hxx"
#include <sstream>

using namespace Vocal;

unsigned int SipMsgContainer::_cnt = 0;
unsigned int SipMsgPair::_cnt = 0;

SipMsgContainer::SipMsgContainer(const SipTransactionId& id)
      : trans_id(id) {
   clear();
   trans_id = id;
   _cnt++;
}

void SipMsgContainer::setMsgIn(Sptr<SipMsg> inm) {
   in = inm;
}

void SipMsgContainer::clear() {
   in = NULL;
   in_encode = "";
   netAddr = NULL;
   transport = "";
   shouldGcAt = 0;
   retransCount = 1;
   collected = false;
   prepareCount = 0;
   trans_id.clear();

   nextTx = 0;
   wait_period = 0;
}//clear

bool SipMsgContainer::matches(const SipTransactionId& id) {
   bool v = (id == trans_id);
   cpLog(LOG_DEBUG, "matches: %d, id: %s  trans_id: %s\n",
         (int)(v), id.toString().c_str(), trans_id.toString().c_str());
   return v;
}//matches

void SipMsgContainer::cleanup() {
    in = NULL;
    netAddr = NULL;
    in_encode = "";
    trans_id.clear();
}

string SipMsgContainer::toString() const {
    ostringstream oss;
    if (in.getPtr()) {
        oss << " in: " << in->toString() << endl;
    }
    else {
        oss << " in: NULL\n";
    }

    
    oss << "out: " << in_encode.c_str()
        << "\nnetAddr: ";

    if (netAddr != 0) {
        oss << netAddr->toString();
    }
    else {
        oss << "NULL";
    }

    oss << "\ntransport: " << transport.c_str()
        << " retransCount: " << retransCount << " collected: " << collected
        << endl;

    return oss.str();
}


