
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

static const char* const StateRecordMessage_cpp_Version = 
"$Id: StateRecordMessage.cxx,v 1.1 2004/05/01 04:15:40 greear Exp $";
#include <sys/stat.h>
#include <unistd.h>

#include "StateRecordMessage.h"

StateRecordMessage::StateRecordMessage (string Name):StateVM (Name)
{
}

StateRecordMessage::~StateRecordMessage ()
{
    cpLog(LOG_DEBUG,"~StateRecordMessage");
}

/** State setup */
int
StateRecordMessage::Process (VmSession * session)
{
  setSession(session);		/** Save session pointer */
  m_xEventSources.push_back (&m_xRecordTimer);	/** Add timer event source */

  /** Set maximum message length in msec. */
  m_xRecordTimer.set (session->getCfg ()->m_iMaxRecordTime, 0);

  /** Start recording timer */
  m_xRecordTimer.start ();

  /** Create a temporary filename */
  string tempTpl = "VoiceMessage.XXXXXX";
  m_xRecorderFileName = session->CreateTemp (tempTpl.c_str (), ".wav");

  /** Record to File m_xRecorderFileName */
  session->getLine ()->getVmcp ()->recordFile (m_xRecorderFileName.c_str());
  /** Start Recorder */
  if (!session->getLine ()->getVmcp ()->startRecord ())
  {
    return StateEndOfSession;
  }
  return StateVM::Process (session);	/** Process State */
}

/** Process messages from LineVMCP */
int
StateRecordMessage::ProcessLine (pEvent evt)
{
  switch (evt->getType ())
  {
	/** Connection closed by peer */
      case eventVmcpClose:
	cpLog (LOG_DEBUG, "Recording stopped by close.");
	/** Send a message */
	SendMessage ();
	return StateEndOfSession;
      default:			/** Continue loop */
	return StateVM::ProcessLine (evt);
  }
}

/** Process events from Recorder */
int
StateRecordMessage::ProcessRecorder (pEvent evt)
{
  switch (evt->getType ())
  {
	/** Record Completed. Send a message and exit. */
      case eventVmcpRecordCompleted:
	cpLog (LOG_DEBUG, "Recording stopped by event.");
	SendMessage ();
	return StateReturn;

      default:
	return StateVM::ProcessRecorder (evt);
  }
}

/** Process events from Timers */
int
StateRecordMessage::ProcessTimer (pEvent evt)
{
  cpLog (LOG_DEBUG, "Recording stopped by timer.");
  getSession()->getLine ()->getVmcp ()->stopRecord ();	/** Stop recorder */
  SendMessage ();		/** Send a message and exit */
  return StateReturn;
}

/** Process DTMF */
int
StateRecordMessage::ProcessDTMF (pEvent evt)
{
  // cpLog (LOG_DEBUG, "Recording stopped by DTMF.");
  // getSession()->getLine ()->getVmcp ()->stopRecord ();	/** Stop recorder */
  // SendMessage ();		/** Send a message and exit */
  // return StateReturn;

  // do nothing
  return StateStay;
}

/** Encode an send a message */
void
StateRecordMessage::SendMessage ()
{

  /** Get the size of recorded message */
  struct stat stat_p;
  ::stat (m_xRecorderFileName.c_str (), &stat_p);

  /** More the 3 seconds ? !!! TODO: codecs, user configurable length */
  if (stat_p.st_size < 24000)
  {
    cpLog (LOG_INFO, "Message is less than 3 seconds. Ignored");
    return;
  }

  cpLog (LOG_DEBUG, "Sending message %s to %s from %s",
	 m_xRecorderFileName.c_str (),
	 ((VmSession *)getSession())->getUserCfg ()->m_xSendTo.c_str (),
	 ((VmSession *)getSession())->getLine ()->getVmcp ()->getSessionInfo ().CallerId.c_str ());

  string commandLine = ((VmSession *)getSession())->getCfg()->m_xSendMsgScript + " "  +
    m_xRecorderFileName + " " +
    ((VmSession *)getSession())->getUserCfg ()->m_xSendTo.c_str () + " " +
    ((VmSession *)getSession())->getLine ()->getVmcp ()->getSessionInfo ().CallerId;
  ;
  system (commandLine.c_str ());
}

