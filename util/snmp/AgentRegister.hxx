
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




#ifndef AgentRegister_H
#define AgentRegister_H

static const char* const AgentRegisterHeaderVersion =
    "$Id: AgentRegister.hxx,v 1.3 2004/05/06 05:41:05 greear Exp $";

#ifdef __cplusplus

#include <unistd.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "Sptr.hxx"
#include "SnmpCommon.h"
#include "UdpStack.hxx"
#include "cpLog.h"
#include <misc.hxx>

#endif /* __cplusplus */

/* common between c and c++ code: */
#define agentTrapPort 33602
#define agentCommPort 33604
#define  registerMulticastPort 33616
//#define  registerMulticastPort 12616
#define registerMulticastIP "230.1.2.3"
#ifdef __cplusplus

class AgentRegister: public BugCatcher {
public:
   AgentRegister(void *msg, int msgLEN);
   virtual ~AgentRegister();

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);
   
private:
   //This stack is used to receive multicast messages 
   Sptr < UdpStack > regUdpStack;

   // This stack is used to send unicast messages to the SNMPD
   // Modified by nismail@cisco.com
   Sptr <UdpStack > regTrUdpStack;
   int SockNum, MaxSockNum;
   NetworkAddress dest;
   char *txMessage;
   int txMsgLen;
   char rxMessage[2048];  /* for lack of a better size */
};

#endif /* __cplusplus */

#endif
