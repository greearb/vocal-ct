
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

static const char* const StateMain_cpp_Version = 
"$Id: StateMain.cxx,v 1.1 2004/05/01 04:15:39 greear Exp $";
#include "StateMain.h"
#include "StateRecordMessage.h"

StateMain::StateMain (string Name):MenuState (Name)
{
}

StateMain::~StateMain ()
{
    cpLog(LOG_DEBUG,"~StateMain");
}

/** State setup */
int
StateMain::Process (VmSession * session)
{
  setSession(session);		/** Save session pointer */


  AddTermDigit('#',DTMF_HASH);  
  AddTermDigit('*',DTMF_STAR);  
  AddTermString("2",2);
  AddTermString("4",4);
  AddTermString("6",6);
  AddTermString("76",76);
  
  
  getSession()->setIntVar((string)"%MsgNum",getSession()->getMail()->Messages());

  if( getSession()->getMail()->Messages() > 0 )
  {
      PlayPrompt("Header");
  }
  else
  {
      PlayPrompt("MailboxEmpty");
  }
  
  cpLog(LOG_DEBUG,"Check for AutoPlay");
  if( getSession()->getMail()->Messages() > 0 && getSession()->getCfg()->m_iAutoPlay )
  {
      cpLog(LOG_DEBUG,"AutoPlay");
      for(int i=0;i<getSession()->getMail()->Messages();i++)
      {
          cpLog(LOG_DEBUG,"Message %d",i);
          getSession()->getMail()->gotoMessage(i);
	  PlayHeader();
	  PlayMessage();
      }
      PlayPrompt("EndOfMailbox");
  } 
  else if( getSession()->getMail()->Messages() > 0 )
  {
    getSession()->getMail()->gotoMessage(0);
    PlayHeader();
  }
  if (!getSession()->playStart ())	/** Start player */
  {
    return StateEndOfSession;
  }
  return MenuState::Process (session);	/** Process state */
}

void 
StateMain::PlayMessage()
{
    getSession()->getMail()->envelope();
 
    if( memcmp(getSession()->getMail()->env->subject,"VoiceMail",9) )
    {
      cpLog(LOG_DEBUG,"Not a voicemail");
      PlayPrompt("NotAVoiceMail");
    } 
    else 
    {
      cpLog(LOG_DEBUG,"VoiceMail");
      string msgFile=getSession()->CreateTemp("PlayerMessage.XXXXXX",".wav");
      getSession()->setStringVar("$CurrentMessage",msgFile);
      getSession()->getMail()->decodeAndSave(msgFile);
      PlayPromptPath("CurrentMessage","");
    }
    PlayPrompt("EndOfMessage");
    getSession()->playStart ();
}

void 
StateMain::NextMessage()
{
    if( !getSession()->getMail()->nextMessage() )
    {
	PlayPrompt("EndOfMailbox");
    } 
    else 
    {
	PlayHeader();
    }
    getSession()->playStart();
}

void 
StateMain::PrevMessage()
{
    if( !getSession()->getMail()->prevMessage() )
    {
	PlayPrompt("StartOfMailbox");
    } 
    else 
    {
	PlayHeader();
    }
    getSession()->playStart();
}

void 
StateMain::DeleteRestore()
{
    if( getSession()->getMail()->messageDeleted() )
    {
	getSession()->getMail()->restoreMessage();
	PlayPrompt("MessageRestored");
    }
    else
    {
	getSession()->getMail()->deleteMessage();
	PlayPrompt("MessageDeleted");
    }
    getSession()->playStart();
}

void 
StateMain::PlayHeader()
{
    getSession()->setIntVar("%CurMsg",getSession()->getMail()->currentMessage()+1);
    PlayPrompt("MessageHeader");
}

int
StateMain::ProcessDTMFTermination(unsigned int code)
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
	    
	case 2:
	    stateCounter=0;
	    getSession()->dtmf.Clear();
	    PlayMessage();
	    return StateStay;
	    
	case 4:
	    stateCounter=0;
	    getSession()->dtmf.Clear();
	    PrevMessage();
	    return StateStay;

	case 6:
	    stateCounter=0;
	    getSession()->dtmf.Clear();
	    NextMessage();
	    return StateStay;
	    
	case 76:
	    stateCounter=0;
	    getSession()->dtmf.Clear();
	    DeleteRestore();
	    NextMessage();
	    return StateStay;
    }
    stateCounter++;
    if( stateCounter == 3 )
    {
	return StateReturn;
    }
    return MenuState::ProcessDTMFTermination(code);
}
