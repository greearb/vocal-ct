
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

static const char* const StateSetup_cpp_Version = 
"$Id: StateSetup.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "StateSetup.h"
#include "StatePlayGreeting.h"
#include "StatePlayOwnerGreeting.h"
#include "StatePlaySync.h"
#include "StateMain.h"
#include "StateLogin.h"

const int setupTimerId = 1;
const int startupTimerId = 2;

StateSetup::StateSetup (string Name):StateVM (Name)
{
}

StateSetup::~StateSetup ()
{
}

  
int 
StateSetup::Process (Session * session) 
{
     /** Save session pointer */
     setSession(session);

     /** Add setup timer event source */
     m_xEventSources.push_back (&m_xSetupTimer);

     /** Add startup timer event source */
     m_xEventSources.push_back (&m_xStartupTimer);

     /** Set setup timer to 60000 ms */
     m_xSetupTimer.set (60000, setupTimerId);

     /** Set startup timer to 1000 ms */
     m_xStartupTimer.set (1000, startupTimerId);

     /** start setup timer */    
     m_xSetupTimer.start ();

     /** request session info from client */
     if (!getSession()->getLine ()->getVmcp ()->reqSessionInfo ())
     {
         return StateEndOfSession;
     }

     return StateVM::Process (session);
}

/**  */
int
StateSetup::ProcessTimer (pEvent evt)
{
    switch ((*evt->IParm ())[0])
    {
	/** No response from client. Cleanup and exit */
	case setupTimerId:
	    cpLog (LOG_DEBUG, "Timeout waiting session info.");
	    return StateReturn;

	/** Startup pause completed, start session. */
      	case startupTimerId:
	{
	    cpLog (LOG_DEBUG, "StartupTimer. Starting session.");

	    /** Get session info from VMCP */
	    VmcpSessionInfo info = getSession()->getLine ()->getVmcp ()->getSessionInfo ();
	    
//	    if( getSession()->getInteractive() )
	    if( info.DialedNumber == getSession()->getCfg ()->m_xAdminNumber )
	    {
                getSession()->setInteractive(true);
	        cpLog (LOG_DEBUG, "State Login.");
	        StateLogin login("StateLogin");
	        if( login.Process(getSession()) != StateLoginOK )
		     return StateReturn;
	        StateMain main("StateMain");
	        return main.Process(getSession());
	    }
            else
            {
                getSession()->setInteractive(false);
            }

	    /** Set user for this session */
	    getSession()->setUser (info.DialedNumber);

	    /** Find and parse user config */
	    if (!(getSession())->getUserCfg ()->parse ( getSession()->getCfg ()->m_xHomeRoot + getSession()->getUser () + "/config")) 

	    {
	         /** User not configured, play error message and exit */
	         StatePlaySync msg ("InvalidUser");
	         msg.Play ((VmSession *)getSession(), "InvalidUser.wav");
	         return StateReturn;
	    }
	    if (!getSession()->getCfg ()->m_iRecordOnSelfCall ||
	       getSession()->getUser () !=
	       getSession()->getLine ()->getVmcp ()->
	       getSessionInfo ().CallerId)
	    {
	         StatePlayGreeting playGreeting ("StatePlayGreeting");
	         return playGreeting.Process (getSession());
	    }
	    else
	    {
	        /** Proceed with CA session */
	        StatePlayOwnerGreeting playGreeting ("StatePlayOwnerGreeting");
	        return playGreeting.Process (getSession());
	    }
	 }
         default:
	 return StateVM::ProcessTimer (evt);
    }
}

/**  */
int
StateSetup::ProcessLine (pEvent evt)
{
  switch (evt->getType ())
  {
      case eventVmcpClose:
	cpLog (LOG_DEBUG, "Received close from client.");
	return StateReturn;
      case eventVmcpEventSessionInfo:
	cpLog (LOG_DEBUG, "Session info received.");
	m_xStartupTimer.start ();
	return StateStay;
      default:
	return StateVM::ProcessLine (evt);
  }
}
