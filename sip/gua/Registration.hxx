#ifndef Registration_HXX
#define Registration_HXX

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

static const char* const Registration_hxx_Version =
    "$Id: Registration.hxx,v 1.3 2004/06/18 07:06:04 greear Exp $";
#include <Data.hxx>
#include <RegisterMsg.hxx>
#include <StatusMsg.hxx>

/**
 * Implements the unit of registration. The class maintains a SIP
 * REGISTER message, processesstration response and maintains a seq. for
 * future registrations.
*/
namespace Vocal
{

class Registration : public BugCatcher {
public:
   Registration(const string& local_ip); // local_ip cannot be ""
   Registration(const RegisterMsg& srcMsg);
   Registration(const Registration& src);

   virtual ~Registration();

   Sptr<RegisterMsg> getNextRegistrationMsg();
   Sptr<RegisterMsg> getNextRegistrationCancel();
   Sptr<RegisterMsg> getRegistrationMsg() {
      return registerMsg;
   };

   int updateRegistrationMsg(const StatusMsg& msg);
   void setRegistrationMsg(Sptr<RegisterMsg> msg);
   bool operator ==( Registration& src ) const;
   int getStatusCode() const {
      return status;
   };
   int getDelay();

   uint64 getNextRegister() const { return nextRegisterMs; }
   void setNextRegister(uint64 r) { nextRegisterMs = r; }

private:

   SipContact findMyContact(const StatusMsg& msg) const;

   int status;
   unsigned int seqNum;
   Sptr<RegisterMsg> registerMsg;
   uint64 nextRegisterMs;

   Registration();
   Registration& operator =(const Registration&);
};
 
}

#endif
