
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

static const char* const UserConfiguration_cpp_Version = 
"$Id: UserConfiguration.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "UserConfiguration.h"
#include "cpLog.h"


static char *usrcfgTags[] = {
  "Name",
  "SendTo",
  "Greeting",
  "ImapServer",
  "ImapId"
};

UserConfiguration::UserConfiguration ()
{
  m_xName = "Unknown User";
  m_xSendTo = "root@localhost";
  m_xGreeting = "greeting.wav";
  m_xImapServer = "127.0.0.1";
  m_xImapId = "invalid";
}

UserConfiguration::~UserConfiguration ()
{
}

/** partse config line */
int
UserConfiguration::parseLine (int lineNumber, string tag, string value)
{ 
  /** This is for IVR style application : future release */
  switch (tag2int (tag))
  {
      case usrcfgTagInvalid:
	cpLog (LOG_ERR, "Parser:Invalid tag %s", tag.c_str ());
	return false;
      case usrcfgName:
	m_xName = value;
	break;
      case usrcfgSendTo:
	m_xSendTo = value;
	break;
      case usrcfgGreeting:
	m_xGreeting = value;
	break; 
	 case usrcfgImapServer:
	m_xImapServer = value;  
	break;
      case usrcfgImapId:
	m_xImapId = value;
	break;
      default:
	return false;
  }
  return true;
}

/** convert tag to config value */
int
UserConfiguration::tag2int (string & tag)
{
    for (int i = 0; i < usrcfgNumberOfTags; i++)
    {
        if (!strcasecmp (tag.c_str (), usrcfgTags[i]))
            return i;
    }
    return usrcfgTagInvalid;
}
