#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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



static const char* const Configuration_h_Version = 
"$Id: Configuration.h,v 1.1 2004/05/01 04:15:39 greear Exp $";

#include "cpLog.h"
#include "cfgParser.h"

/** For future release these tags will get used 
 */
enum MailAccessType
{
    MAIL_ACCESS_LOCAL,
    MAIL_ACCESS_IMAP
};

/** Array of Configurarable Tags <br><br>
 *  Needs to add 'PromptTable' tag for IVR Style application
 */
extern char *cfgTags[];

/** VmServer configuration records and config file parser  
 */
class Configuration:public cfgParser
{
private:

  /** Needs to add 'cfgPromptTable' tag for future relase (Imap server)
   */
  enum
  {
    cfgVoiceRoot = 0,
    cfgHomeRoot,
    cfgTempRoot,
    cfgMailSpool,
    cfgMailAccessMethod,
    cfgMsgFrom,
    cfgMsgTo,
    cfgAttendant,
    cfgImapServer,
    cfgImapServerPort,
    cfgInputTimeout,
    cfgRecordOnSelfCall,
    cfgMaxRecordTime,
    cfgLogLevel,
    cfgLogFileName,
    cfgLogConsole,
    cfgSendMsgScript,
    cfgAutoPlay,
    cfgNumberOfTags,
    cfgTagInvalid
  };

public:
   
    Configuration ();
   
    /** Virtual destructor 
    */
    virtual ~Configuration ();

    /** Override cfgParser methods */
    virtual int parseLine (int lineNumber, string tag,string value);
    virtual int tag2int (string & tag);

public:

    /** path to common voice prompt files*/
    string m_xVoiceRoot;		
	
    /** path to user home directories
    */ 
    string m_xHomeRoot;	

    /** path to temporary directory*/
    string m_xTempRoot;		

    /** mail boxes directory for local access method
    */
    string m_xMailSpool;		

    /** obsolete
    */ 
    string m_xMsgFrom;		

    /** obsoleteSend message to 
    */ 
    string m_xMsgTo;

    /** attendant phone number (reserved for future use) 
    */
    string m_xAttendant;

    /** IMAP server address 
    */
    string m_xImapServer;	

    /** IMAP server port number
    */
    int m_iImapServerPort;

    /** mail server access method (Local,IMAP,etc) 
    */
    int m_iMailAccessMethod;

    /** user input timeout (msec) 
    */
    int m_iInputTimeout;		

    /** max message time (msec) 
    */
    int m_iMaxRecordTime;		

    /** reserved 
    */
    int m_iRecordOnSelfCall;

    string m_xSendMsgScript;

    int m_iLogLevel;
    int m_iLogConsole;
    string m_xLogFileName;
  
    int m_iAutoPlay;
};

#endif
