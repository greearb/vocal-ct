#ifndef UaCallControl_hxx
#define UaCallControl_hxx

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


static const char* const UaCallControl_hxx_Version =
    "$Id: UaCallControl.hxx,v 1.8 2004/11/09 00:49:51 greear Exp $";

#include "global.h"
#include <list>
#include "SipProxyEvent.hxx"
#include "UaBase.hxx"
#include "GuiEvent.hxx"
#include "CallControl.hxx"
#include "CallAgent.hxx"

namespace Vocal {

namespace UA {

// TODO:  Make this a member of UaFacade and get rid of the
// singleton.  --Ben

/// Main interface to handle requests in UA
class UaCallControl : public CallControl {
public:
   /// Create one with default values
   static UaCallControl& instance();
   
   ///
   string className() { return "UaCallControl"; }
   
   /**
      Frees the memory associated with singelton instance.
      gets register to atexit() function at the time of creation.
   */
   static void destroy();

   ///
   bool processEvent(Sptr<SipProxyEvent> event);

protected:
   
   ///
   void handleGuiEvents(Sptr<GuiEvent> gEvent);
   
private:

   ///
   bool busy(Sptr<SipCommand> sipMsg);
   
   ///
   Sptr<BaseUrl> parseUrl(const string& to);
   
   ///
   Sptr<CallAgent> getActiveCall(Sptr<SipMsg> sipMsg=0);

   ///
   void initiateInvite(const string& to, const char* debug);
   
   ///
   bool handleAlchemEvent(Sptr<SipProxyEvent> event);
   
   ///
   UaCallControl() : CallControl() { };

   ///
   static UaCallControl* myInstance;
};

}

}


#endif
