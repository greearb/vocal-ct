
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

#include "PromptServer.h"
#include <assert.h>
#include <fstream>
#include "Lineparser.h"
#include <stdio.h>
#include "cpLog.h"

/**
 * Not Getting used in this release.
 * This class is for Feture Release for Imap Server Functionality.
 */

PromptServer::PromptServer()
{
    m_bFileNameSet=false;
}

PromptServer::PromptServer(const string& promptTableName) 
{
    m_xTableName=promptTableName;
}

PromptServer::PromptServer(const char *promptTableName)
{
    m_xTableName=(string)promptTableName;
}

PromptServer::~PromptServer()
{
    m_bFileNameSet=false;
}

void PromptServer::add(const string& promptName,Prompt prompt)
{
    m_xTable[promptName]=prompt;
}

pPrompt PromptServer::get(const string& promptName)
{
    if( find(promptName) ) return &m_xTable[promptName];
    else return NULL;
}

bool PromptServer::find(const string& promptName)
{
    if( m_xTable.find(promptName)!= m_xTable.end()) return true;
    return false;
}

bool PromptServer::conv(const string& promptName,vector<string> *fileNames,const char *path)
{
    if( !find(promptName) )
    {
	cpLog(LOG_DEBUG,"Can't open find prompt :%s",promptName.c_str());
        return false;
    }
    const char *vpath;
    if( path!=NULL ) vpath=path;
    else vpath=m_xCommonPath.c_str();
    pPrompt prompt=get(promptName);
    int retval;
    retval=0;
    for(unsigned int iter=0;iter<prompt->size();iter++)
    {
	retval+=parseSegment((*prompt)[iter],fileNames,vpath);
    }
    return retval;
}

bool PromptServer::parseSegment(const string& segment,vector<string> *fileNames,const char *path)
{
    switch(segment[0])
    {
	case '%': // Integer variable
	{
	    return intValue(fileNames,m_xIntVar[segment],path);
	}
	
	
	case '#': // string of integers
	{
	    int retval;
	    retval=0;
	    string var=m_xStrVar[segment];
	    for(unsigned int iter=0;iter<var.size();iter++)
	    {
		string str="";
		str+=var[iter];
		int val=atoi(str.c_str());
		retval+=intValue(fileNames,val,path);
	    }
	    return retval;
	}
	
	case '$': // filename
	{
	    string file=path+m_xStrVar[segment];
	    fileNames->push_back(file);
	    return true;
	}
	
	case '@': // date
	{
	    return dateValue(fileNames,m_xIntVar[segment],path);
	}
	
	default:
	{
	    string file=path+segment;
	    fileNames->push_back(file);
	    return true;
	}
    }
}

bool PromptServer::loadTable()
{
    if( !m_bFileNameSet ) return false;
    return loadTable(m_xTableName.c_str());
}

bool PromptServer::loadTable(const char *fileName)
{
    m_xTableName=fileName;
    ifstream file(m_xTableName.c_str());
    if( !file.is_open() ) return false;
    while(1)
    {
	Prompt prompt;
	string key;
	file >> key;
	if( file.eof() ) break;
	char istring[1024];
	file.getline(istring,1024,'\n');
	char *s;
	s=istring;
	while((*s == ' ' || *s== '\t') && *s!=0  ) s++;
	Lineparser parser;
	parser << s;
	while(parser.pcount()>0)
	{
	    string str;
	    parser >> str;
	    if( str.size() == 0 ) break;
	    prompt.push_back(str);
	}
	m_xTable[key]=prompt;
	parser.freeze(0);
    }
    return true;
}

bool PromptServer::intValue(vector<string> *fileNames,const int vl,const char *path)
{
    
    int val;
    val=vl;
    char buff[128];
    if( val >= 20 )
    {
	int val1=(val/10)*10;
	sprintf(buff,"%d",val1);
	string str=buff;
	conv(str,fileNames,path);
	if( (val%10)==0 ) return true;
	val=val%10;
    }
    if( val < 20 )
    {
	sprintf(buff,"%d",val);
	string str=buff;
	conv(str,fileNames,path);
    }
    return true;
}

bool PromptServer::dateValue(vector<string> *fileNames,const time_t val,const char *path)
{
    char s[128];
    struct tm t;
    memcpy(&t,localtime(&val),sizeof(struct tm));
    strftime(s,128,"%A",&t);
    conv(s,fileNames,path);
    strftime(s,128,"%B",&t);
    conv(s,fileNames,path);
    intValue(fileNames,t.tm_mday,path);
    intValue(fileNames,t.tm_hour,path);
    intValue(fileNames,t.tm_min,path);
    return true;
}

bool PromptServer::dateValue(vector<string> *fileNames,const string& val,const char *path)
{
    return true;
}

bool PromptServer::dateValue(vector<string> *fileNames,const char *val,const char *path)
{
    return true;
}
