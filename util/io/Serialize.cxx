/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2002 Vovida Networks, Inc.  All rights reserved.
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

static const char* const Serialize_cxx_Version = "$Id: Serialize.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include <iostream.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cassert>

#include "Serialize.hxx"
#include "cpLog.h"

using namespace Vocal::IO;


char* Serialize::myScriptFileName = 0;
char* Serialize::myReadWriteMode = 0;
FILE* Serialize::myFilePtr = 0;
Serialize* Serialize::self = 0;


Serialize::Serialize()
{
    myScriptFileName = 0;
    myReadWriteMode = 0;
    myFilePtr = 0;
    self = 0;
}


Serialize& 
Serialize::singleton(const char* scriptFileName, const char* readWriteMode)
{
    Serialize::doCpLog();

    if (self != 0)          // singleton already created
    {
        if (scriptFileName == 0)  // reusing the same singleton
        {
             return *self; 
        }
        else
        {
            delSingleton();     // releasing the existing singleton
        }
    }

    // databasefilename and readwritemode must be present
    
    assert ( scriptFileName != 0);
    assert ( ( strncmp(readWriteMode, "w", 1) == 0  ) || ( strncmp( readWriteMode, "r", 1) == 0) );

    self = new Serialize();
    
    myScriptFileName = new char[strlen(scriptFileName)+1];
    strncpy( myScriptFileName, scriptFileName, strlen(scriptFileName)+1);

    myReadWriteMode = new char[strlen(readWriteMode)+1];
    strncpy( myReadWriteMode, readWriteMode, strlen(readWriteMode)+1);
	 
	 // 26/11/03 fpi
	 // WorkAround Win32
	 #ifdef WIN32
	 myFilePtr = _popen(scriptFileName, readWriteMode);
	 #else
	 myFilePtr = popen(scriptFileName, readWriteMode);
	 #endif
    
    if (myFilePtr == 0)
        cpLog(LOG_ERR, "popen failed on File: %s .  Mode: %s\n", scriptFileName, readWriteMode);
    else
        cpLog(LOG_ERR, "popen successful on File: %s .  Mode: %s\n", scriptFileName, readWriteMode);

    assert(myFilePtr);


    Serialize::doCpLog();

    return *self;

}

    
void
Serialize::doCpLog() 
{
    cpLog(LOG_DEBUG, "MyScript: %s . MyReadWriteMode: %s . MyFilePtr: %XX .\n",
			 (myScriptFileName ? myScriptFileName : "InvalidFileName"), 
			   (myReadWriteMode ? myReadWriteMode : "InvalidMode"), myFilePtr);

}
    

void
Serialize::delSingleton()
{

    Serialize::doCpLog();

    if (self->validFile())
    {
		 // 26/11/03 fpi
		 // WorkAround Win32
		#ifdef WIN32
		 int retClose =  _pclose(myFilePtr);
		#else
		 int retClose =  pclose(myFilePtr);
		#endif
    	cpLog(LOG_DEBUG, "Closing File: %s .  pclose() ret: %d\n", myScriptFileName, retClose);
    }

    delete [] myScriptFileName;
    delete [] myReadWriteMode;
    delete self;

    myScriptFileName = 0;
    myReadWriteMode = 0;
    myFilePtr = 0;
    self = 0;

    Serialize::doCpLog();

}

bool
Serialize::validFile() const
{
    Serialize::doCpLog();

    if (myScriptFileName == 0)
    {
        cpLog(LOG_DEBUG, "Invalid File Name");
        return false;
    }

    if (myFilePtr <= 0)
    {
        cpLog(LOG_DEBUG, "File: %s Not opened or error in opening\n", (strlen(myScriptFileName) ? myScriptFileName : "InvalidFileName") );
        return false;
    }

    if (feof(myFilePtr))
    {
        cpLog(LOG_DEBUG, "File: %s EOF\n", myScriptFileName);
        return false;
    }

    return true;
}

void 
Serialize::addWrite(const Data& key, const Data& value)
{

    if (!validFile()) return;

    cpLog(LOG_DEBUG, "FileName: %s .  Adding... Key: %s . Value: %s .\n", myScriptFileName, key.logData(), value.logData());
  
 
    int valueLen = value.length();
    int cmdLen = strlen("ADD");
    int keyLen = key.length();
  
    char* temp = new char[valueLen+cmdLen+keyLen+3];

    memset (temp, 0, (valueLen+keyLen+cmdLen+3)); 
    strncat (temp, value.logData(), valueLen);
    strncat (temp, "\n",1);
    strncat (temp, "ADD" , cmdLen);
    strncat (temp, " " , 1);
    strncat (temp, key.logData(), keyLen);

    fprintf (myFilePtr, "%s\n", temp);
    fflush(myFilePtr);

    cpLog(LOG_DEBUG, "Wrote(add): %s\n", temp);
    delete [] temp;
}

void
Serialize::delWrite(const Data& key)
{
   if (!validFile()) return ;

    cpLog(LOG_DEBUG, "FileName: %s .  Deleting... Key: %s .\n", myScriptFileName, key.logData());

    int keyLen = key.length();
    int cmdLen = strlen("DEL");

    char* temp = new char[keyLen+cmdLen+2];
    memset (temp, 0, (keyLen+cmdLen+2)); 

    strncat (temp, "DEL", cmdLen);
    strncat (temp, " ", 1);
    strncat (temp, key.logData(), keyLen);

    fprintf (myFilePtr, "%s\n",temp);
    fflush(myFilePtr);

    cpLog(LOG_DEBUG, "Wrote(del): %s .\n", temp);
    delete [] temp;
}


char*
Serialize::read(char* readBuffer, int numBytesToRead)
{
    if (!validFile()) return 0;

    assert (readBuffer);
    assert (numBytesToRead);
    
    
    char* readChar = fgets(readBuffer,numBytesToRead, myFilePtr);

    cpLog(LOG_DEBUG, "Read: %s .\n", readBuffer);

    return readChar;
}

//End of File
