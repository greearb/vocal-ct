#ifndef MENUSTATE_H
#define MENUSTATE_H
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
             

#include "StateVM.h"
#include "Timer.h"
#include "VmSession.h"

/** Generic Menu State <br><br>
  * By pressing the digit one can traverse the menu.
  */

class MenuState : public StateVM  {
public: 
	MenuState(string Name);
	
  	/** Virtual destructor
   	*/   
	virtual ~MenuState();
  	
	/** Process the pressed DTMF digit 
	*/
  	virtual int ProcessDTMF(pEvent evt);

/** Public attributes
 */
public: 
	/** Handle the termination dtmf digit 
	 */
  	virtual int ProcessDTMFTermination(unsigned int code);
  	
	/** Process the session 
	 */
  	virtual int Process(Session *session);
  	
	/** Handle the invalid dtmf digit
	 */
  	virtual int ProcessDTMFInvalid();
  	
	/** Handle the loop 
	 */
  	virtual int ProcessLoop(Session *session); 

	/** Add termination code 
	 */
  	virtual void AddTermString(string terminator,unsigned int code);
  	
	/** Add termination code
         */  
  	virtual void AddTermDigit(unsigned int digit,unsigned int code);
  	
	/** Handle the audio player event
	 */
  	virtual int ProcessPlayer(pEvent evt);
  
	/**  Handle the timer event 
	 */
  	virtual int ProcessTimer(pEvent evt);

  	/**  */
  	virtual void Help();

  	/** Play the prompt
	 */
  	virtual int PlayPrompt(const string &PromptName);

  	/**  Play user prompt 
	 */
  	virtual int PlayPromptUser(const string &PromptName);

  	/**  Play Prompttable  Path
	 */
  	virtual int PlayPromptPath(const string &PromptName,const string& path);
  
/**  protected attributes
 */     
protected:
  	/** Timer type variable
	 */
  	Timer stateTimer;

  	/** Integer counter
	 */
  	int stateCounter;

	/** Map the strings to integer
	*/
  	map<string,unsigned int> termStrings;
  	map<unsigned int,unsigned int> termDigits;
};

#endif
