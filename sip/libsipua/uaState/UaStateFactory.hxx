#ifndef UaStateFactory_hxx
#define UaStateFactory_hxx

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



static const char* const UaStateFactory_hxx_Version = 
"$Id: UaStateFactory.hxx,v 1.4 2004/11/19 01:54:38 greear Exp $";

#include <map>
#include "CInvalidStateException.hxx"
#include "UaState.hxx"
#include "cpLog.h"
#include <errno.h>

namespace Vocal {

namespace UA {

typedef enum {
    U_STATE_IDLE=0,
    U_STATE_S_TRYING, //Server Trying state
    U_STATE_C_TRYING, //Client trying state
    U_STATE_RINGING,
    U_STATE_FAILURE,
    U_STATE_INCALL,
    U_STATE_END,
    U_STATE_REDIRECT,
    U_STATE_HOLD,
    U_STATE_MAX
} UStateType;

/** Object UaStateFactory
<pre>
<br> Usage of this Class </br>

    UaStateFactory represent the factory class to hold all the UA state objects. 

    To add new state one can do this

    1. add an entry to enum UStateType which is defined above

        example :

               U_STATE_MESSAGE

    2. Update the Method UaState* UaStateFactory::getState(UStateType stateType)

        example :

        case  U_STATE_MESSAGE:
        retVal = new UaStateInMessage();
        break;

    3. Create the State class derived form UaState

    example:

          class UaStateInMessage : public UaState
          {

          }

    4. In /vocal/sip/libsipua/UaBase.hxx add the above class as freind.

    example:

         friend class UaStateInMessage;
</pre>
*/


class UaStateFactory {
public:
   ///
   typedef map<UStateType, Sptr<UaState> > UaStateMap;
   
   ///
   static UaStateFactory& instance();
   
   ///
   virtual string className() { return "UaStateFactory"; }
   
   
   ///
   Sptr<UaState> getState(UStateType);
   
   ///
   static void destroy();
   
protected:
   ///
   UaStateFactory() { };
   
   ///Destructor
   virtual ~UaStateFactory();

   UaStateMap stateMap;
   
   ///
   static UaStateFactory* myInstance;
};

}

}

#endif
