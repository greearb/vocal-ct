
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


#include "MenuState.h"

/** Generic Menu State <br><br>
 */ 
MenuState::MenuState(string Name) : StateVM(Name) {
}

/** Virtual destructor 
 */
MenuState::~MenuState(){
}

/** Process the pressed DTMF digit
 */
int MenuState::ProcessDTMF(pEvent evt){
	int code;
	stateTimer.start();
	getSession()->getLine()->getVmcp()->stopPlay();

	cpLog(LOG_DEBUG,"Got DTMF: %c", (*evt->IParm())[0]);

	if( (code=getSession()->dtmf.DigitReceived((*evt->IParm())[0],&termStrings,&termDigits))!=0 )
	{
		stateCounter=0;
		return ProcessDTMFTermination(code);
	}
	if( !getSession()->dtmf.Validate(&termStrings) )
	{
		if( stateCounter==3 )
		{
			return StateEndOfSession;
		}
		stateCounter++;
		return ProcessDTMFInvalid();
	}
	return StateStay;
}

/** Handle the termination dtmf digit
 */ 
int MenuState::ProcessDTMFTermination(unsigned int code) {
	cpLog(LOG_DEBUG,"State:%s Unhandled DTMF termination:%d",m_xName.c_str(),code);
	cpLog(LOG_DEBUG,"String received:%s",getSession()->dtmf.GetString()->c_str());
	getSession()->dtmf.Clear();
	return StateReturn;
}

/** Process the session   
 */
int MenuState::Process(Session *session) {

	m_xEventSources.push_back(&stateTimer);
	stateTimer.set(getSession()->getCfg()->m_iInputTimeout,0);
	stateCounter=0;
	return State::Process(session);
}

/** Handle the invalid dtmf digit 
 */
int MenuState::ProcessDTMFInvalid(){
	cpLog(LOG_DEBUG,"Invalid code received :%s",getSession()->dtmf.GetString()->c_str());
	getSession()->dtmf.Clear();
	return StateStay;
}

/** Handle the loop
 */
int MenuState::ProcessLoop(Session *session){ 
	setSession(session);
	int code;
	stateCounter=0;
	cpLog(LOG_DEBUG,"StateTimer:%d",stateCounter);
	if( (code=getSession()->dtmf.DigitReceived(0,&termStrings,&termDigits))!=0 )
	{
		return ProcessDTMFTermination(code);
	}
	return State::ProcessLoop(session);
}

/** Add termination code 
*/
void MenuState::AddTermString(string terminator,unsigned int code){
	termStrings[terminator]=code;
}

/** Add termination code
 */ 
void MenuState::AddTermDigit(unsigned int digit,unsigned int code){
	termDigits[digit]=code;
}

/** Handle the audio player event
*/  
int MenuState::ProcessPlayer(pEvent evt){
	if( evt->getType()==eventVmcpPlayCompleted)
	{
		cpLog(LOG_DEBUG,"Play completed");
		stateTimer.start();
	}
	return StateStay;
}

/**  
 */
void MenuState::Help(){
	//PlayPrompt("DefaultHelp");
	getSession()->getLine()->getVmcp()->startPlay();
}

/**  Handle the timer event
 */  
int MenuState::ProcessTimer(pEvent evt){
	if( (*evt->IParm())[0] != 0 ) return State::ProcessTimer(evt);
	cpLog(LOG_DEBUG,"StateTimer:%d",stateCounter);
	if( stateCounter == 3 )
	{
		evt->getSource()->stop();
		// For feture Release
		//PlayPrompt("StateCounterExpired");
		return StateEndOfSession;
	}
	stateCounter++;
	Help();
	return StateStay;
}

/** Play the prompt 
 */
int MenuState::PlayPrompt(const string &PromptName){
	stateTimer.stop();
	return getSession()->PlayPrompt(PromptName);
}

/** Play user prompt     
 */
int MenuState::PlayPromptUser(const string &PromptName){
	stateTimer.stop();
	return getSession()->PlayPromptUser(PromptName);
}

/** Play Prompttable Path 
 */
int MenuState::PlayPromptPath(const string &PromptName,const string& path){
	stateTimer.stop();
	return getSession()->PlayPromptPath(PromptName,path);
}

