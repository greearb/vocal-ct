
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
static const char* const VmSession_cpp_Version =
"$Id: test.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
 
#include <unistd.h>
#include "global.h"
#include "VmSession.h"
#include "StateSetup.h"
 
extern string g_xCfgPath;         

VmSession::VmSession ()
{
  vector<string> a;
  a.push_back("SS");
}        

VmSession::~VmSession ()
{
 
  cpLog(LOG_DEBUG,"~VmSession");
  /** Remove temporary files created.*/
 
  getMail()->update();

  for (vector<string>::iterator itr = m_xTemporaryFiles.begin();
               itr != m_xTemporaryFiles.end(); itr++)
  {
    cpLog (LOG_DEBUG, "Removing file %s", (*itr).c_str ());
    unlink ((*itr).c_str ());
  }

}            

int
VmSession::Start (Vmcp *vmcp,bool interactive)
{
 
  m_xCfg.parse (g_xCfgPath);
 
  if( !m_xPromptServer.loadTable(getCfg()->m_xPromptTable) )
  {
      cpLog(LOG_ERR,"Can't load prompt table %s",getCfg()->m_xPromptTable.c_str());
      return false;
  }
 
  m_xInteractive=interactive;
  LineVMCP line;
  setLine(&line);
  getLine()->assign(vmcp);
 
  /** start it */
  getLine()->start ();
 
  /** Start session */
  return Run ();
}                         


int
VmSession::Run ()
{
 
  /** Create StateSetup state */
  StateSetup setup ("StateSetup");
 
  /** Run it */
  setup.Process (this);
 
  /** Make shure that the line is closed */
  getLine()->getVmcp ()->sendClose ();
  /** End of session */
  return true;
}
 
/** Add a file to the player queue */
int
VmSession::playFile (const string & fileName)
{
  cpLog (LOG_DEBUG, "Searching %s", fileName.c_str ());
 
  /** Check if the given file is exists. */
  if (access (fileName.c_str (), 0))
    return false;
 
  /** Add it to the queue */
  getLine()->getVmcp ()->playFile (fileName.c_str());
  return true;
}                                                                             


int
VmSession::playStart ()
{
  getLine()->getVmcp ()->startPlay ();
  return true;
}
 
/** Get configuration */
Configuration *
VmSession::getCfg ()
{
  return &m_xCfg;
}
 
/** Add a file from a common directory to the player queue*/
int
VmSession::playFileCommon (const string & fileName)
{
  return playFile (getCfg ()->m_xVoiceRoot + fileName);
}
 
/** Get user configuration */
const UserConfiguration *
VmSession::getUserCfg ()
{
  return &m_xUserCfg;
}                              

/** Add a file from a private user directory to the player queue*/
int
VmSession::playFileUser (const string & fileName)
{
  return playFile (getCfg ()->m_xHomeRoot + m_xUser + "/" + fileName);
}
 
/** Get user name */
string& VmSession::getUser ()
{
  return m_xUser;
}
 
/** Set active user name */
void
VmSession::setUser (const string & user)
{
  m_xUser = user;
}                                                       

string VmSession::CreateTemp (const char *tpl, const char *ext)
{
 
//  /** Allocate temporary storage */
//  char *  buffer = (char *) malloc (strlen (tpl) + 1);
//  /** Copy template string to it */
//  sprintf (buffer, tpl);
 
//  /** Make a temporary file name */
//  char * s;
//  s = mktemp (buffer);
 
//  /** Add an extention to it */
//  string name = (string) s;
//  name += ext;
 
//  string pathName=getCfg()->m_xTempRoot + name;
 
  /** Save it in the temporary files list */
/*
  m_xTemporaryFiles.push_back (pathName);
*/
  m_xTemporaryFiles.push_back("xx");
  //vector<string> a;
  //a.push_back("SS");
 
  /** Free temporary storage */
//  free (buffer);
//  return pathName;
    return "A";
}                                                                 

int VmSession::PlayPrompt(const string& PromptName)
{
/*SP
    if(!m_xPromptServer.conv(PromptName,&m_xPromptFiles,getCfg()->m_xVoiceRoot) )
    {
        m_xPromptFiles.clear();
        return false;
    }
    for(unsigned int i=0;i<m_xPromptFiles.size();i++)
    {
        //getLine()->getVmcp()->playFile(m_xPromptFiles[i].c_str());
    }
    m_xPromptFiles.clear();
*/
    return true;
}
 
int VmSession::PlayPromptUser(const string& PromptName)
{
/*SP
    if(!m_xPromptServer.conv(PromptName,&m_xPromptFiles,getCfg()->m_xHomeRoot + m_xUser + "/") )
    {
        m_xPromptFiles.clear();
        return false;
    }
    for(unsigned int i=0;i<m_xPromptFiles.size();i++)
    {
        //getLine()->getVmcp()->playFile(m_xPromptFiles[i].c_str());
    }
    m_xPromptFiles.clear();
*/
    return true;
}                                                                

int VmSession::PlayPromptPath(const string& PromptName,const string& path)
{
/*SP
    if(!m_xPromptServer.conv(PromptName,&m_xPromptFiles,path) )
    {
        m_xPromptFiles.clear();
        return false;
    }
    for(unsigned int i=0;i<m_xPromptFiles.size();i++)
    {
        //getLine()->getVmcp()->playFile(m_xPromptFiles[i].c_str());
    }
    m_xPromptFiles.clear();
*/
    return true;
} 
                        


void VmSession::setStringVar(const string& varName,const string& var)
{
    m_xPromptServer.m_xStrVar[varName]=var;
}
 
void VmSession::setIntVar(const string& varName,const int var)
{
    m_xPromptServer.m_xIntVar[varName]=var;
}
 
 
bool VmSession::getInteractive()
{
    return m_xInteractive;
}
 
Mail * VmSession::getMail()
{
    return &m_xMail;
}                               

/*
main()
{
//  string a("haaHaa");
//  _vectorString.push_back("haaHaa");
//  _vectorString.push_back(a);
}
*/
