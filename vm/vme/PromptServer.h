#ifndef PROMPTSERVER_H
#define PROMPTSERVER_H

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
           
#include "global.h"
#include <string>
#include <vector>
#include <map>
#include <time.h>


typedef vector<string> Prompt;
typedef Prompt *pPrompt;

/**
 * Not Getting used in this release.
 * This class is for Feture Release for Imap Server Functionality.
 */      

class PromptServer {
public:
    PromptServer();
    PromptServer(const string& promptTableFile);
    PromptServer(const char *promptTableFile);
    
    ~PromptServer();
    
    
public:

    void add(const string& promptName,Prompt prompt);
    pPrompt get(const string& promptName);
    bool find(const string& promptName);
    
    bool conv(const string& promptName,vector<string> *fileNames,const char *path=NULL);
    bool conv(const string& promptName,vector<string> *fileNames,const string& path)
    {
	return conv(promptName,fileNames,path.c_str());
    }
    
    bool parseSegment(const string& segment,vector<string> *fileNames,const char *path);
    bool loadTable();
    bool loadTable(const char *fileName);
    bool loadTable(const string& fileName)
    {
	return loadTable(fileName.c_str());
    }
    
    bool intValue(vector<string> *fileNames,const int val,const char *path);
    bool dateValue(vector<string> *fileNames,const char * val,const char *path);
    bool dateValue(vector<string> *fileNames,const string& val,const char *path);
    bool dateValue(vector<string> *fileNames,const time_t utime,const char *path);
    
    void setCommonVoicePath(const string& path)
    {
	m_xCommonPath=path;
    }
    
public:

    map<string,int>	m_xIntVar;
    map<string,string>	m_xStrVar;
    
private:

    string m_xTableName;
    map<string,Prompt>	m_xTable;
    bool m_bFileNameSet;
    string m_xCommonPath;
};

#endif

