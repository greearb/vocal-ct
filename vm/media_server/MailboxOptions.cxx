
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

static const char* const MailboxOptions_cpp_Version = 
"$Id: MailboxOptions.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "MailboxOptions.h"
#include "StateRecordMessage.h"

MailboxOptions::MailboxOptions (string Name):MenuState (Name)
{
}

MailboxOptions::~MailboxOptions ()
{
    cpLog(LOG_DEBUG,"~MailboxOptions");
}

/** State setup */
int
MailboxOptions::Process (VmSession * session_options)
{
  setSession(session_options);		/** Save session pointer */


  AddTermDigit('#',DTMF_HASH);  
  AddTermDigit('*',DTMF_STAR);  
  AddTermString("1",1);
  AddTermString("2",2);
  AddTermString("3",3);
  AddTermString("9",9);
  
  cpLog(LOG_DEBUG,"Play OptionsMenu Prompt");
  PlayPrompt("OptionsMenu");
    
if (!getSession()->playStart ())	/** Start player */
  {
    return StateEndOfSession;
  }
  return MenuState::Process (session_options);	/** Process state */
}

void 
MailboxOptions::PlayMessage()
{
    if (getSession()->getMail()->envelope()) {
 
    if( memcmp(getSession()->getMail()->env->subject,"VoiceMail",9) )
    {
      cpLog(LOG_DEBUG,"Not a voicemail");
      PlayPrompt("NotAVoiceMail");
    } 
    else 
    {
      cpLog(LOG_DEBUG,"VoiceMail");
      string msgFile=getSession()->CreateTemp("PlayerMessage.XXXXXX");
      getSession()->setStringVar("$CurrentMessage",msgFile);
      getSession()->getMail()->decodeAndSave(msgFile);
      PlayPromptPath("CurrentMessage","");
	}
    PlayPrompt("EndOfMessage");
    getSession()->playStart ();
} else {
	PlayPrompt("EndOfMailbox");
	PlayPrompt("MainMenu");
	getSession()->playStart();
}
}

void 
MailboxOptions::NextMessage()
{
    if( !getSession()->getMail()->nextMessage() )
    {
	PlayPrompt("EndOfMailbox");
	PlayPrompt("MainMenu");
	getSession()->playStart();
    } 
    else 
    {
	PlayHeader();
	}
}

void 
MailboxOptions::FeatureNotAvailable()
{
    PlayPrompt("OptionsMenu");
	PlayPrompt("MainMenu");
	getSession()->playStart();
}

void 
MailboxOptions::PrevMessage()
{
    if( !getSession()->getMail()->prevMessage() )
    {
	PlayPrompt("StartOfMailbox");
	getSession()->playStart();
    } 
    else 
    {
	PlayHeader();
	}
}

void 
MailboxOptions::DeleteRestore()
{
    if( getSession()->getMail()->messageDeleted() )
    {
	getSession()->getMail()->restoreMessage();
	PlayPrompt("MessageRestored");
	getSession()->playStart();
    }
    else
    {
	PlayPrompt("CouldNotRestore");
	getSession()->playStart();
    }
}

void 
MailboxOptions::DeleteMessage()
{
    if( getSession()->getMail()->messageDeleted() )
    {
	getSession()->getMail()->restoreMessage();
	PlayPrompt("MessageAlreadyDeleted");
	getSession()->playStart();
    }
    else
    {
	getSession()->getMail()->deleteMessage();
	PlayPrompt("MessageDeleted");
	getSession()->playStart();
    }
}
void 
MailboxOptions::PlayHeader()
{
    getSession()->setIntVar("%CurMsg",getSession()->getMail()->currentMessage()+1);
    PlayPrompt("MessageHeader");
	getSession()->playStart();
}

int
MailboxOptions::ProcessDTMFTermination(unsigned int code)
{
    getSession()->getCfg()->m_iAutoPlay=false;
    getSession()->getLine()->getVmcp()->stopPlay();
    stateTimer.stop();
    switch(code)
    {
	case DTMF_HASH:
	    stateCounter=0;
	    getSession()->dtmf.Clear();
	    return StateStay;
	    
	case DTMF_STAR:
	    stateCounter=0;
	    getSession()->dtmf.Clear();
	    return StateStay;
	    
	case 1:
	    stateCounter=0;
	    getSession()->dtmf.Clear();
	    PlayMessage();
		return StateStay;

	case 2:
	    stateCounter=0;
	    getSession()->dtmf.Clear();
	    PlayMessage();
		return StateStay;
	
	case 3:
	    stateCounter=0;
	    FeatureNotAvailable();
	    return StateStay;
	    
	case 9:
	    stateCounter=0;
	    FeatureNotAvailable();
	    return StateStay;
    }
    stateCounter++;
    if( stateCounter == 3 )
    {
	return StateReturn;
    }
    return MenuState::ProcessDTMFTermination(code);
}
