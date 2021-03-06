
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

static const char* const StateRecordGreeting_cpp_Version = 
"$Id: StateRecordGreeting.cxx,v 1.1 2004/05/01 04:15:40 greear Exp $";
#include "StateRecordGreeting.h"
#include <sys/stat.h>
#include <unistd.h>

///
StateRecordGreeting::StateRecordGreeting (string Name):StateVM (Name)
{
}

///
StateRecordGreeting::~StateRecordGreeting ()
{
}

/// Records a greeting from user
int
StateRecordGreeting::Process (VmSession * session)
{
  setSession(session);
  m_xEventSources.push_back (session->getLine ());
  m_xEventSources.push_back (&m_xRecordTimer);

  m_xRecordTimer.set (60000, 0);

  m_xRecordTimer.start ();

  string tempTpl = session->getCfg ()->m_xTempRoot + "VoiceMessage.XXXXXX";
  m_xRecorderFileName = session->CreateTemp (tempTpl.c_str (), ".wav");
  session->getLine ()->getVmcp ()->recordFile (m_xRecorderFileName.c_str());
  session->getLine ()->getVmcp ()->startRecord ();
  return StateVM::Process (session);
}

/// Process the event
int
StateRecordGreeting::ProcessLine (pEvent evt)
{
  switch (evt->getType ())
  {
      case eventVmcpClose:
	cpLog (LOG_DEBUG, "Recording stopped by close.");
	SaveGreeting ();
	return StateReturn;
      default:
	return StateVM::ProcessLine (evt);
  }
}

/// Process a Recorder event
int
StateRecordGreeting::ProcessRecorder (pEvent evt)
{
  switch (evt->getType ())
  {
      case eventVmcpRecordCompleted:
	cpLog (LOG_DEBUG, "Recording stopped by event.");
	SaveGreeting ();
	return StateReturn;
      default:
	return StateVM::ProcessRecorder (evt);
  }
}

/// Process a Timer Event
int
StateRecordGreeting::ProcessTimer (pEvent evt)
{
  cpLog (LOG_DEBUG, "Recording stopped by timer.");
  getSession()->getLine()->getVmcp()->stopRecord();
/**              SendMessage();   */
  return StateReturn;
}

/// Saves a greeting
void
StateRecordGreeting::SaveGreeting ()
{
  struct stat stat_p;
  ::stat (m_xRecorderFileName.c_str (), &stat_p);
  if (stat_p.st_size < 24000)
  {
    cpLog (LOG_DEBUG, "Greeting is less than 3 seconds. Ignored");
    return;
  }
  string commandLine =
    "cp " + m_xRecorderFileName + " " + ((VmSession *)getSession())->getCfg ()->m_xHomeRoot +
    ((VmSession *)getSession())->getUser () + "/greeting.wav";
  system (commandLine.c_str ());
}
