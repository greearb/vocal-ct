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

static const char* const GuiEventThread_cxx_Version =
    "$Id: GuiEventThread.cxx,v 1.2 2004/06/18 07:06:04 greear Exp $";


#include "Sptr.hxx"
#include "SipEvent.hxx"
#include "UaBase.hxx"
#include "GuiEventThread.hxx"
#include "GuiEvent.hxx"
#include "UaFacade.hxx"


using namespace Vocal;
using namespace Vocal::UA;



GuiEventThread::GuiEventThread(int readFd)
      : myReadFd(readFd) {
   // Nothing to do
};

void GuiEventThread::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                          uint64 now) {

   // This could be much more elegant, like making sure we get a newline, etc.
   if (FD_ISSET(myReadFd, input_fds)) {
      char buf[256];
      int readCnt;
      cpLog(LOG_DEBUG, "Reading on %d", myReadFd);
      if ((readCnt = read(myReadFd, buf, 256)) > 0) {
         cpLog(LOG_DEBUG, "Read from GUI: [%s]", buf);
         buf[readCnt] = 0;
         //Translate the messages into a SipProxyEvent
         Sptr<GuiEvent> gEvent = new GuiEvent(buf);
         UaFacade::instance().queueEvent(gEvent.getPtr());
         cpLog(LOG_DEBUG, "%s, %s",gEvent->getKey().c_str() , gEvent->getValue().c_str());
      }
      else {
         cpLog(LOG_DEBUG, "Read %d bytes", readCnt);
      }
   }
}

int GuiEventThread::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                           int& maxdesc, uint64& timeout, uint64 now) {
   FD_SET(myReadFd, input_fds);
   maxdesc = max(maxdesc, myReadFd);
   return 0;
}

