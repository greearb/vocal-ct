#ifndef NetworkRes_hxx
#define NetworkRes_hxx

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


static const char* const NetworkRes_hxx_Version = 
    "$Id: NetworkRes.hxx,v 1.1 2004/05/01 04:15:16 greear Exp $";

#include "Sptr.hxx"
#include "NetworkAddress.h"

namespace Vocal
{

namespace UA
{

   class MediaSession;

/** Derived from NetworkAddress, NetworkRes encapsulates the usage of 
  * an (IP,PORT) pair.Offers interfaces to reserve and release an (IP,PORT) pair.
 */
class NetworkRes : public NetworkAddress
{
public:

   NetworkRes(const string host, int port) 
         :NetworkAddress(host, port), busy(false), ms_owner(NULL)
      { };

   NetworkRes(const NetworkRes &src)
         : NetworkAddress(src), busy(src.busy), ms_owner(src.ms_owner) { }

   /**Makes the resource busy/free.
    * @param val - True ( busy), false( free)
    * @param reason - Used for debugging.
    */
   void setBusy(bool val, const char* reason);

   /* For debugging purposes. */
   void setOwner(MediaSession* o);
   MediaSession* getOwner() { return ms_owner; }

   ///
   bool isBusy(const string& local_ip, string local_if = "") const { return busy || isPortTaken(local_ip, local_if); };

   bool getBusyFlag() { return busy; }

   /** Actually takes the OS into account, as we are definately not the only thing
    * running on the machine at any given time!!!
    * Specify local_ip if you want to bind to that in particular...
    */
   bool isPortTaken(const string& local_ip, string local_if = "") const;

   ///
   string className() { return "NetworkRes"; }

   /** Virtual destructor
    */
   virtual ~NetworkRes() { };

private:

   ///  Don't use this operator.
   const NetworkRes & operator=(const NetworkRes &src);
   

   ///
   bool busy;
   MediaSession* ms_owner;

};

}
}

#endif
