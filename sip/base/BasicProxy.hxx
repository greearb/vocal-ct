#ifndef BASIC_PROXY_HXX
#define BASIC_PROXY_HXX

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


static const char* const BasicProxy_hxx_Version =
    "$Id: BasicProxy.hxx,v 1.3 2004/05/05 06:37:33 greear Exp $";


#include "HeartbeatThread.hxx"
#include "HeartLessProxy.hxx"

namespace Vocal
{

/** Object BasicProxy
<pre>
<br> Usage of this Class </br>

     BasicProxy is a derived class from HeartLessProxy.

     A BasicProxy object should be used in a system which supports
     the heartbeating mechanism. This class adds three heartbeating threads.

     Create the proxy given the builder, the sip port (which defaults
     to 5060), the application name (which defaults to unknown).
     The boolean filteron indicates whether to createg a filtering sip
     transceiver or not (defaults to true). The boolean nat indicates
     whether the sip transceiver should be created to handle network
     address translation (defaults to false).The application context
     is used by the sip-stack to alter retransmission behavior. When 
     context is APP_CONTEXT_PROXY, sipstack will not retransmit 200
     responses for INVITE and BYE.

     BasicProxy(const Sptr < Builder >   builder,
		   unsigned short           defaultSipPort = 5060,
		   Data                     applName = "unknown",
		   bool                     filteron = true,
                   bool                     nat = false,
                   SipAppContext            aContext = APP_CONTEXT_GENERIC);
</pre>
*/
class BasicProxy: public HeartLessProxy
{
public:


   /** Explained in the Usage of the Class
    * hashTableSize is initial bucket count for underlying hash table(s).
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   BasicProxy(const Sptr < Builder >   builder, 
              int hashTableSize,
              const string&            local_ip,
              const string&            local_dev_to_bind_to,
              unsigned short           defaultSipPort = 5060,
              Data                     applName = "unknown",
              bool                     filteron = true, 
              bool                     nat = false,
              SipAppContext            aContext = APP_CONTEXT_GENERIC);
   
   
   /** Virtual destructor
    */
   virtual ~BasicProxy();
   
   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

protected:


   /** Pointer to heartbeat tx thread object
    */
   HeartbeatThread*  myHeartbeatThread;
   
   
private:
   
   
   /** Suppress copying
    */
   BasicProxy( const BasicProxy &);
   BasicProxy();
   
   /** Suppress copying
    */
   const BasicProxy & operator=(const BasicProxy &);
};
 
}

#endif // BASIC_PROXY_HXX
