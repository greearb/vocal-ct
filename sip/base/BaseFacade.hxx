#ifndef __VOCAL__BaseFacade_hxx
#define __VOCAL__BaseFacade_hxx

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


static const char* const BaseFacade_hxx_Version = 
    "$Id: BaseFacade.hxx,v 1.2 2004/06/17 06:56:51 greear Exp $";


#include <string>
#include "NetworkAddress.h"
#include <BaseProxy.hxx>


namespace Vocal
{

/** Facade is a wrapper, that wraps all the components needed
    for UA.
*/
class BaseFacade : public BaseProxy {
public:
   
   virtual string className() = 0;
   
   /** Virtual destructor
    */
   virtual ~BaseFacade() {
      delete proxyAddr;
      proxyAddr = NULL;
   }
   
   BaseFacade( const string& _localIp, unsigned short _localSipPort,
               const string& _natIp, int _transport,
               const NetworkAddress& _proxyAddr, bool fix_sdp_for_nat) :
         localIp(_localIp),
         localSipPort(_localSipPort),
         natIp(_natIp),
         transport(_transport),
         fixSdpForNat(fix_sdp_for_nat) {
      proxyAddr = new NetworkAddress(_proxyAddr);
      // NOTE:  It's ok for proxyAddr to be 0.0.0.0, just means we won't
      // try to register!
   }
   
   
   // Store our 'global' info in this class, instead of UaBase.
   // Globals suck!
   virtual const string& getLocalIp() const { return localIp; }
   virtual int getLocalSipPort() const { return localSipPort; }
   virtual void setLocalSipPort(unsigned short sp) { localSipPort = sp; }
   virtual int getTransport() const { return transport; }
   virtual const string& getNatHost() const { return natIp; }
   virtual NetworkAddress* getProxyAddr() const { return proxyAddr; }

   virtual bool shouldFixSdpForNat() { return fixSdpForNat; }

   virtual void setNatHost(const string& nh) { natIp = nh; }

protected:
   
   string localIp;
   unsigned short localSipPort;
   string natIp;
   int transport;
   NetworkAddress* proxyAddr;
   bool fixSdpForNat;

};

}//namespace VOCAL

#endif
