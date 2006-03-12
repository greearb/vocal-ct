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

#include <unistd.h> 
#include <errno.h> 
#include "NetworkConfig.hxx"
#include "cpLog.h"
#include "NetworkAddress.h"

// 26/11/03 fpi
// WorkAround Win32
#ifdef WIN32
#include <ws2tcpip.h>
#endif

// 26/11/03 fpi
// WorkAround Win32
// TBR
// All cpLog instances have to be eliminated
// due to static object construction race error
// 16/1/04 fpi
// also in the merge with Ben Greear

NetworkConfig* NetworkConfig::myInstance = 0;

NetworkConfig& NetworkConfig::instance() {
   if (myInstance == 0) {
      myInstance = new NetworkConfig();
   }
   return *myInstance;
}

void NetworkConfig::destroy() {
   if (myInstance) {
      delete myInstance;
      myInstance = NULL;
   }
}

NetworkConfig::NetworkConfig() {
   myAddrFamily = PF_INET;
   init();
}


int NetworkConfig::init() {
   struct addrinfo hints;
   struct addrinfo *res;

   // Setup structures
   memset(&hints, 0, sizeof(hints));
   
   string hostName = NetworkAddress::getLocalHostName().c_str();
   hints.ai_flags = AI_CANONNAME;
   hints.ai_family = PF_UNSPEC;
   hints.ai_socktype = SOCK_DGRAM;
   int error = getaddrinfo(hostName.c_str(), 0, &hints, &res);
   if (error != 0) {
      //cpLog(LOG_ERR, "getaddrinfo(), Failed to get the address info, reason:%s", strerror(errno));
      return -1;
   }
   struct addrinfo* myItr = res;
   int myFlg = 0;
   while (myItr) {
      if (myItr->ai_family == PF_INET6) {
         myFlg |= 0x02;
      }
      else if (myItr->ai_family == PF_INET) {
         myFlg |= 0x01;
      }
      //cpLog(LOG_DEBUG, "Family:%d", myItr->ai_family);
      //cpLog(LOG_DEBUG, "Address Len:%d" , myItr->ai_addrlen);
      //cpLog(LOG_DEBUG, "Protocol:%d" , myItr->ai_protocol);
      //cpLog(LOG_DEBUG, "Host Name:%s" , myItr->ai_canonname);
      myItr = myItr->ai_next;
   }
   dualStack = false;
   if ((myFlg & 0x01) && (myFlg & 0x02)) {
      myAddrFamily = PF_INET6;
      dualStack = true;
      //cpLog(LOG_INFO, "*** Dual-stack supports IPv4 and IPv6 ***" );
   }
   else if ((myFlg & 0x01)) {
      myAddrFamily = PF_INET;
      //cpLog(LOG_INFO, "*** IPv4 support only ***");
   }
   else if (myFlg & 0x02) {
      myAddrFamily = PF_INET6;
      //cpLog(LOG_INFO, "*** IPv6 support only ***" );
   }
   return 0;
}


#endif
