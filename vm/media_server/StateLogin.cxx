
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

static const char* const StateLogin_cpp_Version = 
"$Id: StateLogin.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "StateLogin.h"
#include "StateRecordMessage.h"

/** Play a user greeting <br><br>
 */ 
StateLogin::StateLogin (string Name):MenuState (Name)
{
}

/** Virtual destructor
 */ 
StateLogin::~StateLogin ()
{
    cpLog(LOG_DEBUG,"~StateLogin");
}

/** State setup 
 */
int
StateLogin::Process (VmSession * session)
{
	cpLog(LOG_DEBUG, "StateLogin::Process");
	/** Save session pointer */
  	setSession(session);	
  
  	m_xLoginStage=0;
  	stateCounter=0;
	
	PlayPrompt("Login");
  
  	AddTermDigit('#',DTMF_HASH);
  	AddTermDigit('*',DTMF_STAR);
  
	/** Start player */
  	if (!getSession()->playStart ())	
  	{
	   cpLog(LOG_DEBUG, "StateLogin::Process ending");
 	   return StateEndOfSession;
  	}
	
	/** Process state */
  	return MenuState::Process(session);
}

/** Handle the Dtmf digits
 */
int
StateLogin::ProcessDTMFTermination (unsigned int code)
{
  cpLog(LOG_DEBUG, "StateLogin::ProcessDTMFTermination:%d", m_xLoginStage);
  switch(m_xLoginStage)
  {
      case 0:
      {
          switch(code)
          {
              case DTMF_STAR:
	      {
	          getSession()->dtmf.Clear();
		  
		      PlayPrompt("Login");
		  
		  getSession()->playStart();
		  return StateStay;
	      }
	      case DTMF_HASH:
	      {
                  cpLog(LOG_DEBUG, "#::ProcessDTMFTermination");
	          string userName=*(getSession()->dtmf.GetString());
	          getSession()->setUser (userName);
		  getSession()->dtmf.Clear();
	          m_xLoginStage=1;
	          PlayPrompt("Password"); 
	          getSession()->playStart();
	          return StateStay;
	      }
	      default:
	          return StateStay;
	  }
      }
      case 1:
      {
          switch(code)
	  {
              case DTMF_STAR:
	      {
	          stateCounter=0;
	          getSession()->dtmf.Clear();
		      PlayPrompt("Password");
		  getSession()->playStart();
		  return StateStay;
	      }
	      case DTMF_HASH:
	      {
	          string userPassword=*(getSession()->dtmf.GetString());
	          getSession()->getMail()->setPassword(userPassword.c_str());
		  getSession()->dtmf.Clear();
	          m_xLoginStage=0;

	  	  /** Find and parse user config */
	          if (!(getSession())->
			getUserCfg ()->parse ( getSession()->getCfg ()->m_xHomeRoot +
			           getSession()->getUser () + "/config") )
		  {
		      if( stateCounter < 3 )
		      {
		          cpLog(LOG_DEBUG,"Login incorrect");
		          stateCounter++;
		          m_xLoginStage=0;
		          PlayPrompt("LoginIncorrect");
		          PlayPrompt("Login");
		          getSession()->playStart();
		          return StateStay;
		      }
		      else
		      {
		          return StateReturn;
		      }
		  
		  }				
				  
				   
	          getSession()->getMail()->setUser(getSession()->getUserCfg()->m_xImapId.c_str());
		  string imapName="{"+getSession()->getUserCfg()->m_xImapServer+"}";
		  cpLog(LOG_DEBUG,"Login into Imap");
		  cpLog(LOG_DEBUG, "IMAP Server: [%s]", imapName.c_str());
		  if( getSession()->getMail()->open(imapName) )
		  {
		      cpLog(LOG_DEBUG,"Login OK");
		      return StateLoginOK;
		  }
		  else if( stateCounter < 3 )
		  {
		      cpLog(LOG_DEBUG,"Login incorrect");
		      stateCounter++;
		      m_xLoginStage=0;
		      PlayPrompt("LoginIncorrect");
		      PlayPrompt("Login");
		      getSession()->playStart();
		      return StateStay;
		  }
		  return StateReturn;
	      }
	      default:
	          return StateStay;
	  }
      }
      
  }
  return MenuState::ProcessDTMFTermination(code);
}

/** State setup
 */
int
StateLogin::ProcessDTMF (pEvent evt)
{
  	int code;
  	getSession()->getLine()->getVmcp()->stopPlay();
  	stateTimer.stop();
  	if( (code=getSession()->dtmf.DigitReceived((*evt->IParm())[0],&termStrings,&termDigits))!=0 )
  	{
    		return ProcessDTMFTermination(code);
  	}
  	return StateStay;
}
