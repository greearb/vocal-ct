
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

static const char* const State_cpp_Version = 
"$Id: State.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "State.h"
#include "Waiter.h"
#include "Timer.h"

State::State (string Name)
{
  Set (Name);
}

State::~State ()
{
}


/** Generic event handlers */

/** Process DTMF digits */
int
State::ProcessDTMF (pEvent evt)
{
//      m_xSession->dtmf.DigitReceived(evt->p[0]);
  cpLog (LOG_ERR, "State:%s. Unhandled DTMF:%c",
	 m_xName.c_str (), (char) (*evt->IParm ())[0]);
  return StateReturn;
}

/** Process messages from recorder */
int
State::ProcessRecorder (pEvent evt)
{
  cpLog (LOG_ERR, "State:%s. Unhandled recorder message %d",
	 m_xName.c_str (), (char) (*evt->IParm ())[0]);
  return StateStay;
}

/** Process events from player */
int
State::ProcessPlayer (pEvent evt)
{
  cpLog (LOG_ERR, "State:%s. Unhandled player message %d",
	 m_xName.c_str (), (char) (*evt->IParm ())[0]);
  return StateStay;
}

/** Process events from timers */
int
State::ProcessTimer (pEvent evt)
{
  if ((*evt->IParm ())[0] == 0xDEAD)
  {
    cpLog (LOG_ERR, "FATAL!!!:State guard timeout");
    return StatePanic;
  }
  cpLog (LOG_ERR, "State:%s. Unhandled timer with ID :%d",
	 m_xName.c_str (), (*evt->IParm ())[0]);
  return StateReturn;
}

/** Process events from Line */
int
State::ProcessLine (pEvent evt)
{
  if (evt->getType () == eventVmcpClose)
    return StateEndOfSession;
  cpLog (LOG_ERR, "State:%s. Unhandled line message :%d",
	 m_xName.c_str (), evt->getType ());
  return StateStay;
}

/** Process generic events */
int
State::ProcessGeneric (pEvent evt)
{
  cpLog (LOG_ERR, "State:%s. Unhandled generic message :%d",
	 m_xName.c_str (), evt->getType ());
  return StateStay;
}

/** State setup. Should be overriden!!! */
int
State::Process (Session * session)
{
  m_xSession = session;
  m_xEventSources.push_back (m_xSession->getLine ());
  m_xEventSources.push_back (&m_xGuardTimer);
  m_xGuardTimer.set (300000, 0xDEAD);
  m_xGuardTimer.start ();
  cpLog (LOG_DEBUG, "->%s", m_xName.c_str ());
  return ProcessLoop (m_xSession);
}

/** Main message loop  */
int
State::ProcessLoop (Session * session)
{
  Waiter w;			// Waiter
  EventQueue evt;		// EventQueue
  int retCode;			// Return code
  m_xSession = session;		// Save session pointer
  retCode = StateStay;		// Set default behavior
  do
  {
    // Wait for an event on all event sources. Event will arrive in evt
    if (evt.empty ())
      w.Wait (&evt, &m_xEventSources);
    if (!evt.empty ())		// Any new events?
    {
      // get the first event
      Event e = evt.front ();
      evt.pop ();		// remove it from the queue

// !!!!
      e.print ();

      switch (e.getSourceType ())	// check event source type
      {
	  case sourceGeneric:
	    retCode = ProcessGeneric (&e);
	    break;
	  case sourceVmcp:
	    if (e.getType () == eventVmcpDtmf)
	      retCode = ProcessDTMF (&e);
	    else
	      retCode = ProcessLine (&e);
	    break;
	  case sourceTimer:
	    retCode = ProcessTimer (&e);
	    break;
	  case sourcePlayer:
	    retCode = ProcessPlayer (&e);
	    break;
	  case sourceRecorder:
	    retCode = ProcessRecorder (&e);
	    break;
	  default:
	    cpLog (LOG_ERR, "State:%s. Invalid Event Source %d",
		   m_xName.c_str (), e.getSourceType ());
	    retCode = StateStay;
	    break;
      }
    }
  }
  while (retCode == StateStay);
  return retCode;
}

/** set the state name */
void
State::Set (string Name)
{
  m_xName = Name;
}

void
State::setSession(Session *session)
{
  m_xSession=session;
}

Session *
State::getSession()
{
  return m_xSession;
}
