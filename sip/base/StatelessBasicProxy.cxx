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


static const char* const StatelessBasicProxy_cxx_Version =
    "$Id: StatelessBasicProxy.cxx,v 1.10 2005/03/03 19:59:49 greear Exp $";


#include "CommandLine.hxx"
#include "SipThread.hxx"
#include "SipTransceiverFilter.hxx"
#include "StatelessBasicProxy.hxx"

using namespace Vocal;


StatelessBasicProxy::StatelessBasicProxy(uint16 tos, uint32 priority,
                                         const string& local_ip,
                                         const string& local_dev_to_bind_to,
                                         ServerType              myType,
                                         unsigned short defaultSipPort,
                                         Data applName,
                                         bool filterOn, 
                                         bool nat,
                                         SipAppContext aContext) {

   mySipStack = new SipTransceiver(tos, priority, local_ip,
                                   local_dev_to_bind_to,
                                   applName, defaultSipPort, nat, aContext,
                                   false);
   
   mySipThread = new SipThread(mySipStack, this, false);    

   cpLog(LOG_INFO, "Initializing heartbeat mechanism");
   HeartbeatThread::initialize(local_ip, local_dev_to_bind_to,
                               myType,
                               HB_RX|HB_TX|HB_HOUSEKEEPING, NULL);
   HeartbeatThread::instance().addServerContainer(SERVER_RS);  // Listen for Register Servers
   HeartbeatThread::instance().addServerContainer(SERVER_POS); // Listen for Provision servers
}//constructor


/** Calls the heartbeat tx run method.
 */
void StatelessBasicProxy::startHeartbeat() {
   HeartbeatThread::instance().startTxHeartbeat();
}

void StatelessBasicProxy::stopHeartbeat() {
   HeartbeatThread::instance().stopTxHeartbeat();
}


StatelessBasicProxy::~StatelessBasicProxy() {
   // Nothing to do
}

int StatelessBasicProxy::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                                int& maxdesc, uint64& timeout, uint64 now) {
   mySipThread->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);

   HeartbeatThread::instance().setFds(input_fds, output_fds, exc_fds,
                                      maxdesc, timeout, now);
   return 0;
}

// Call this before trying to delete this guy, or circular
// smart-pointer references will cause memory leaks.
void StatelessBasicProxy::clear() {
   myOperators.clear();
   mySipStack = NULL;
   mySipThread = NULL;
}


void StatelessBasicProxy::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                               uint64 now) {
   HeartbeatThread::instance().tick(input_fds, output_fds, exc_fds, now);
   mySipThread->tick(input_fds, output_fds, exc_fds, now);
}


void
StatelessBasicProxy::process(Sptr < SipProxyEvent > event) {
   assert ( event != 0 );

   cpLog( LOG_DEBUG, "StatelessBasicProxy::process event...\n");

   for (   vector<Sptr <Operator> >::const_iterator iter = myOperators.begin(); 
           iter != myOperators.end(); 
           iter++) {
      Sptr < State > newState = (*iter)->process(event);

      if ( newState == PROXY_DONE_WITH_EVENT ) {
         break;
      }
   }
}

