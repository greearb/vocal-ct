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


static const char* const BasicProxy_cxx_Version =
    "$Id: BasicProxy.cxx,v 1.6 2004/06/09 07:19:35 greear Exp $";


#include "global.h"
#include "BasicProxy.hxx"
#include "CommandLine.hxx"
#include "SipThread.hxx"
#include "Builder.hxx"
#include "HeartbeatParms.hxx"


using namespace Vocal;

BasicProxy::BasicProxy(const Sptr < Builder >  builder,
                       const string&           local_ip,
                       const string&           local_dev_to_bind_to,
                       ServerType              myType,
                       unsigned short          defaultSipPort,
                       Data                    applName,
                       bool                    filteron, 
                       bool                    nat ,
                       SipAppContext           aContext
   )
      :   HeartLessProxy(builder, local_ip,
                         local_dev_to_bind_to, defaultSipPort,
                         applName, filteron, nat, aContext)
{

    if (CommandLine::instance()->getInt("HEARTBEAT")) {
        cpLog(LOG_INFO, "Initializing heartbeat mechanism");
        myHeartbeatThread = new HeartbeatThread(local_ip, local_dev_to_bind_to,
                                                myType, defaultSipPort,
                                                HB_RX|HB_TX|HB_HOUSEKEEPING);
        myHeartbeatThread->addServerContainer(SERVER_RS); //Listen for Register Servers
        myHeartbeatThread->addServerContainer(SERVER_POS); //Listen for Provision servers.
    }
}


void BasicProxy::startTxHeartbeat() {
   assert(myHeartbeatThread);
   myHeartbeatThread->startTxHeartbeat();
}


void BasicProxy::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      uint64 now) {
   if (myHeartbeatThread) {
      myHeartbeatThread->tick(input_fds, output_fds, exc_fds, now);
   }

   // Let the base class have it's say.
   HeartLessProxy::tick(input_fds, output_fds, exc_fds, now);
}


int BasicProxy::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now) {
   if (myHeartbeatThread) {
      myHeartbeatThread->setFds(input_fds, output_fds, exc_fds,
                                maxdesc, timeout, now);
   }

   // Let the base class have it's say.
   HeartLessProxy::setFds(input_fds, output_fds, exc_fds,
                                   maxdesc, timeout, now);
   return 0;
}

BasicProxy::~BasicProxy()
{
   if (myHeartbeatThread) {
      delete myHeartbeatThread;
      myHeartbeatThread = NULL;
   }
}
