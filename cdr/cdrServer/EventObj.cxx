
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


static const char* const EventObj_cxx_Version =
    "$Id: EventObj.cxx,v 1.3 2004/06/15 00:30:10 greear Exp $";


#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "EventObj.hxx"


int EventObj::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     int& maxdesc, uint64& timeout, uint64 now) {
   if (m_fileDesc >= 0) {
      FD_SET(m_fileDesc, input_fds);
      FD_SET(m_fileDesc, exc_fds);
      maxdesc = max(m_fileDesc, maxdesc);
   }

   if (timerMs > 0) {
      uint64 nxtx = lastTime + timerMs;
      if (nxtx > now) {
         timeout = min(timeout, nxtx - now);
      }
      else {
         timeout = 0;
      }
   }
   return 0;
}


void EventObj::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                    uint64 now) {

   if (m_fileDesc >= 0) {
      if (FD_ISSET(m_fileDesc, input_fds)) {
         onData();
         if (!m_reoccuring) {
            m_done = true;
         }
      }
   }

   if (timerMs > 0) {
      if (lastTime + timerMs <= now) {
         onTimer();
         if (!m_reoccuring) {
            m_done = true;
         }
      }
   }
}
