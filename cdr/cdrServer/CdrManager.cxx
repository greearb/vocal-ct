
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


static const char* const CdrManager_cxx_Version =
    "$Id: CdrManager.cxx,v 1.5 2004/08/18 22:39:14 greear Exp $";


#include <time.h>

#include "CdrManager.hxx"
#include "CdrServer.hxx"
#include "CdrCache.hxx"
#include "EventObj.hxx"
#include "CommandLine.hxx"
#include "VCdrException.hxx"
#include "cpLog.h"


// Global constants
//
const unsigned long int MIN_SLEEP_TIME = 75000;
const int CYCLE_COUNT = 1000;

CdrManager::CdrManager( const CdrConfig &cdata )
      : m_configData(cdata) {
   cdrServer = new CdrServer(cdata);
   cdrCache = new CdrCache(cdata);

   // register events
   registerEvent(cdrServer.getPtr());
   registerEvent(cdrCache.getPtr());
}


CdrManager::~CdrManager() {
   // Nothing to do
}


int CdrManager::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                       int& maxdesc, uint64& timeout, uint64 now) {
   list < Sptr<EventObj> > ::iterator itr;
   itr = m_eventList.begin();
   while (itr != m_eventList.end()) {
      Sptr<EventObj> obj = *itr;
      obj->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
      itr++;
   }
   billing.setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   return 0;
}
   
void CdrManager::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      uint64 now) {
   billing.tick(input_fds, output_fds, exc_fds, now);
   list < Sptr<EventObj> > ::iterator itr;
   itr = m_eventList.begin();
   while (itr != m_eventList.end()) {
      Sptr<EventObj> obj = *itr;
      obj->tick(input_fds, output_fds, exc_fds, now);
      if (obj->eventDone()) {
         itr = m_eventList.erase(itr);
      }
      else {
         itr++;
      }
   }
}//tick


void
CdrManager::registerEvent( Sptr<EventObj> obj ) {
   m_eventList.push_back(obj);
}

void
CdrManager::unregister( Sptr<EventObj> obj ) {
   for (list < Sptr<EventObj> > ::iterator itr = m_eventList.begin();
        itr != m_eventList.end(); itr++) {
      if ((*itr).getPtr() == obj.getPtr()) {
         m_eventList.erase(itr);
         break;
      }
   }
}

void
CdrManager::addCache( const CdrClient &msg ) {
   cdrCache->add(msg);
}
