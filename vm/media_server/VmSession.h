#ifndef VMSESSION_H
#define VMSESSION_H

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



static const char* const VmSession_h_Version = 
"$Id: VmSession.h,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include "global.h"
#include <vector>
#include <string>

#include "Session.h"
#include "Configuration.h"
#include "UserConfiguration.h"
#include "DTMFCollector.h"
#include "PromptServer.h"
#include "mailx.h"
/**
 * All the information about current session
 */

class VmSession : public Session
{
public:
  VmSession ();
  
	/** Virtual destructor 
   	*/
	virtual ~ VmSession ();

  	/** Run the session */
  	virtual int Run ();

  	/** Vmcp setup complete, start the session setup process */
  	virtual int Start (Vmcp * vmcp,bool interactive);

  	/** add a file to the player queue */
  	virtual int playFile (const string & fileName);

  	/** star player */
  	virtual int playStart ();

  	/** get configuration */
 	virtual Configuration *getCfg ();

 	/** get user name */
  	virtual string& getUser ();

  	/** add a personal user file to the player queue */
  	virtual int playFileUser (const string & fileName);

  	/** get user configuration */
  	virtual UserConfiguration *getUserCfg (); 
	//virtual const UserConfiguration *getUserCfg (); 
	
  	/** create a uniq temporary file name */
  	virtual string CreateTemp (const char *tpl);

  	/** add a file  from the common directory to the player queue */
  	virtual int playFileCommon (const string & fileName);

  	/** set active user name */
  	virtual void setUser (const string & user);
  
  	virtual int PlayPromptUser(const string& PromptName);
  
  	virtual int PlayPrompt(const string& PromptName);
  
  	virtual int PlayPromptPath(const string& PromptName,const string& path);

  	virtual void setIntVar(const string& varName,const int value);
  
  	virtual void setStringVar(const string& varName,const string& value);
  
  	virtual bool getInteractive();

	void  setInteractive(bool val) { m_xInteractive = val; }
  
  	virtual Mail *getMail();
  
  
public:

 	 DTMFCollector dtmf;

/** Private attributes
 */
private:	

  	/** Active user name holder */
  	string m_xUser;

  	/** User configuration  */
  	UserConfiguration m_xUserCfg;

  	/** Server configuration */
  	Configuration m_xCfg;

  	/** List of temporary files */
  	vector<string> m_xTemporaryFiles;

  	bool m_xInteractive;
  
  	PromptServer m_xPromptServer;
  
 	vector<string> m_xPromptFiles;
  
  	Mail	m_xMail;
  
};

#endif
