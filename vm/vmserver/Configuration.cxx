
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

static const char* const Configuration_cpp_Version = 
"$Id: Configuration.cxx,v 1.1 2004/05/01 04:15:39 greear Exp $";

#include "Configuration.h"


char *cfgTags[] = {
  "voiceRoot",
  "homeRoot",
  "tempRoot",
  "mailSpool",
  "mailAccessMethod",
  "msgFrom",
  "msgTo",
  "Attendant",
  "ImapServer",
  "ImapServerPort",
  "InputTimeout",
  "RecordOnSelfCall",
  "MaxRecordTime",
  "LogLevel",
  "LogFileName",
  "LogConsole",
  "SendMsgScript",
  "AutoPlay"
};              


/** Constructor: assign default values to member variables <br><br>
 */
Configuration::Configuration ()
{
  m_iLogLevel = LOG_ERR;
  m_iLogConsole = true;
  m_xLogFileName = "";
  m_xVoiceRoot = "./Voice/";
  m_xHomeRoot = "./Home/";
  m_iMailAccessMethod = MAIL_ACCESS_LOCAL;
  m_xMailSpool = "/var/spool/mail/";
  m_xMsgFrom = "FreeBX@vovida.com";
  m_xMsgTo = "vm%s@mail.sdvhome.ru";
  m_xAttendant = "0#";
  m_xImapServer = "127.0.0.1";
  m_iImapServerPort = 143;
  m_iInputTimeout = 30000;
  m_iRecordOnSelfCall = false;
  m_iMaxRecordTime = 60000;
  m_xSendMsgScript = "./sendMsg";
  m_iAutoPlay = false;
  /** For future release */
  //m_xPromptTable = "PromptTable";
}

/** Destructor <br><br>
 */
Configuration::~Configuration ()
{
}

/**
 * Parse config line 
 *
 * @param lineNumber 
 * @param tag
 * @param value, new value to be assigned
 * @return a integer, True or False
 */ 
int
Configuration::parseLine (int lineNumber, string tag, string value) 
{
  switch (tag2int (tag))
  {
      case cfgTagInvalid:
	cpLog (LOG_ERR, "Parser:Invalid tag %s", tag.c_str ());
	return false;

      case cfgVoiceRoot:
	m_xVoiceRoot = value;
	break;

      case cfgHomeRoot:
	m_xHomeRoot = value;
	break;

      case cfgTempRoot:
	m_xTempRoot = value;
	break;

      case cfgMailSpool:
	m_xMailSpool = value;
	break;

      case cfgMailAccessMethod:
	m_iMailAccessMethod = atoi (value.c_str ());
	break;

      case cfgMsgFrom:
	m_xMsgFrom = value;
	break;

      case cfgMsgTo:
	m_xMsgTo = value;
	break;

      case cfgAttendant:
	m_xAttendant = value;
	break;

      case cfgImapServer:
	m_xImapServer = value;
	break;

      case cfgImapServerPort:
	m_iImapServerPort = atoi (value.c_str ());
	break;

      /** For future release 
      case cfgPromptTable:
	m_xPromptTable = value;
	break;
      */

      case cfgInputTimeout:
	m_iInputTimeout = atoi (value.c_str ());
	break;

      case cfgRecordOnSelfCall:
	m_iRecordOnSelfCall = atoi (value.c_str ());
	break;

      case cfgMaxRecordTime:
	m_iMaxRecordTime = atoi (value.c_str ());
	break;

      case cfgLogLevel:
	m_iLogLevel = atoi (value.c_str ());
	break;

      case cfgLogFileName:
	m_xLogFileName = value;
	break;

      case cfgLogConsole:
	m_iLogConsole = atoi (value.c_str ());
	break;

      case cfgSendMsgScript:
	m_xSendMsgScript = value;
	break;
	
      case cfgAutoPlay:
        m_iAutoPlay = atoi(value.c_str());
	break;

      default:
	break;
  }
  return true;
}

/** Map a string type of a tag to a interger value 
 */
int
Configuration::tag2int (string & tag)
{
    for (int i = 0; i < cfgNumberOfTags; i++)
    {
        if (!strcasecmp (tag.c_str (), cfgTags[i]))
            return i;
    }
    return cfgTagInvalid;
}
