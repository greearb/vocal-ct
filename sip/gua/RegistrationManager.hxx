#ifndef RegistrationManager_HXX
#define RegistrationManager_HXX

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

static const char* const RegistrationManager_hxx_Version =
    "$Id: RegistrationManager.hxx,v 1.2 2004/06/17 06:56:51 greear Exp $";
#include <vector>
#include <map>

#include <StatusMsg.hxx>
#include "SipTransceiver.hxx"
#include "Registration.hxx"

namespace Vocal
{

namespace UA
{

#define DEFAULT_DELAY  60000  /* 60 sec. */


/**
 * RegistrationManager implements the registration of User agent.
*/
class RegistrationManager : public BugCatcher {
public:
   RegistrationManager( Sptr < SipTransceiver > sipstack );
   virtual ~RegistrationManager();

   ///
   void addRegistration(Sptr<Registration> item);

   //this function return false if the input StatusMsg is not
   //a response to a register message; otherwise, true is returned
   bool handleRegistrationResponse(const StatusMsg& msg);

   int doRegistration(Sptr<Registration> registration, uint64 now);

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);

private:
   typedef vector < Sptr<Registration> > RegistrationList;
   RegistrationList registrationList;
   
   Sptr<Registration> findRegistration(const StatusMsg& msg);
   void flushRegistrationList();

   void registrationMain();

   Sptr < SipTransceiver > sipStack;
};
 
}

}

#endif
