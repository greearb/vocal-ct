#ifndef GuiEvent_hxx
#define GuiEvent_hxx

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


static const char* const GuiEvent_hxx_Version =
    "$Id: GuiEvent.hxx,v 1.2 2004/06/20 07:09:38 greear Exp $";

#include "global.h"
#include <string>
#include "SipProxyEvent.hxx"
#include "BasicAgent.hxx"
#include "misc.hxx"

namespace Vocal {


#define UA_INVITE_STR            "INVITE"
#define UA_RINGING_STR           "RINGING"
#define UA_INFO_STR              "INFO"
#define UA_PREF_STR              "PREFERENCES"
#define UA_REDIRECT_STR          "REDIRECT"
#define UA_REGISTER_STR          "REGISTER"
#define UA_HOLD_STR              "HOLD"
#define UA_RESUME_STR            "RESUME"
#define UA_DOSUBSCRIBE_STR       "DOSUBSCRIBE"
#define UA_REGISTRATIONEXPIRED_STR        "REGISTRATIONEXPIRED"
#define UA_STOP_STR              "STOP"

 /* Stops call and media.  This is a more
  * brutal stop, but is needed when the
  * peer phone is not behaving correctly.
  */
#define UA_PURGE_STR             "PURGE"

#define UA_ACCEPT_STR            "ACCEPT"
#define UA_SHUTDOWN_STR          "SHUTDOWN"
#define UA_L_HANG_UP_STR         "L_HANG_UP"
#define UA_R_HANG_UP_STR         "R_HANG_UP"

typedef enum {
    G_NONE=0,
    G_INVITE,
    G_ACCEPT,
    G_REDIRECT,
    G_HOLD,
    G_RESUME,
    G_DOSUBSCRIBE,
    G_REGISTRATIONEXPIRED,
    G_PREF,
    G_STOP,
    G_PURGE,
    G_SHUTDOWN,
    G_MAX
} GuiEventType;

///GUI event for call control
class GuiEvent : public SipProxyEvent {
public:
   GuiEvent(string eName, uint64 runAfter = 0);

   ///
   virtual ~GuiEvent() { };
   ///
   virtual string className() { return "GuiEvent"; }

   ///
   const char* const name() const { return "GuiEvent"; };

   ///
   GuiEventType getType() { return myType; }

   ///
   const string& getValue() const { return myValue; };
   ///
   const string& getKey() const { return myKey; };
private:
   GuiEvent();
   ///
   GuiEventType myType;
   ///
   string myKey;
   ///
   string myValue;
};



}

#endif
