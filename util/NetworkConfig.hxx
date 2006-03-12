#ifndef NETWORKCONFIG_HXX_
#define NETWORKCONFIG_HXX_

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2002 Vovida Networks, Inc.  All rights reserved.
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

#warning "Dead code"

#if 0

static const char* const NetworkConfig_hxx_Version =
"$Id: NetworkConfig.hxx,v 1.3 2006/03/12 07:41:28 greear Exp $";

#ifndef __MINGW32__
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <iostream>
#include <string>
#include <stdio.h>

#include "global.h"
#include "vtypes.h"

/** Singleton class, detects the IPv4 and IPv6 protocol support
 *  on the running host. 
 *  Relies on DNS query to decide what IP ptotocol is supporetd.
 *  In case of dual stack, chooses IPv6 as default address family, 
 *  that can be  over-witten by calling setAddrFamily()
 */
class NetworkConfig {
public:
   ///Sets the address family to be PF_INET or PF_INET6
   void setAddrFamily(int addrFamily) { myAddrFamily = addrFamily; };            
   ///Returns the IP address family supported
   const int getAddrFamily() const { return myAddrFamily; };
   
   ///
   static NetworkConfig& instance();
   static void destroy();
   
   ///Returns true, if the host supports both IPv4 and IPv6
   bool isDualStack() const { return dualStack; };
   
private:
   ///
   NetworkConfig();
   ///
   int init();
   ///
   int myAddrFamily;
   ///
   bool dualStack;
   ///
   static NetworkConfig* myInstance;
};

// NETWORKCONFIG_HXX_
#endif
#endif
