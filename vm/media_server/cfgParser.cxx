
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

static const char* const cfgParser_cpp_Version = 
"$Id: cfgParser.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include <stdio.h>
#include "cfgParser.h"
#include "cpLog.h"

/*
 * cfgParser parses file fname line by line. Each line is expected
 * to have 3 fields, <tag>-<type>-<value>, separated by blanks or tabs.
 * Lines starting with '#' are comments. Empty lines are ignored.
 * The value field contains a number of characters, even field
 * separators like blanks and tabs, up to the end of the line.
 * User of this utility provides a call back function with 3 arguments
 * (pointers to the tag, type and value strings) to validate and use the
 * data.
 */

/// Constructor
cfgParser::cfgParser ()
{
    cfgStream = NULL;
};

/// Destructor
cfgParser::~cfgParser ()
{
    if (cfgStream)
        fclose (cfgStream);
}

/// This function parses the configuration file, returns boolean
bool
cfgParser::parse (string fname)
{
    char line[CFG_MAX_LINE_LENGTH];
    char tag[CFG_MAX_TAG_LENGTH];
    char type[CFG_MAX_TYPE_LENGTH];

    int lineCnt;
    int argCnt;
    int valuePos = 0;

    fileName = fname;

    /// Open the configuration file
    cfgStream = fopen (fname.c_str (), "r");
 
    if (cfgStream)
    {
        for (lineCnt = 1; fgets (line, 1024, cfgStream); lineCnt++, valuePos = 0)
        {
	    /* remove \n */
            line[strlen (line) - 1] = 0;      
            if ((argCnt = sscanf (line, "%s%*[ \t]%s%*[ \t]%n", tag, type, &valuePos)) == 2)
            {
	        if (line[0] != '#' && line[0] != ';' && strlen (line) > 0)
	        {
	            if (valuePos == 0)
	            {
	                cpLog (LOG_ERR,"VM:CFG:Parser:Line %d has too few arguments: %s", lineCnt, line);
	            }
	            else
	            {
	                parseLine (lineCnt, tag, line + valuePos);
	            }
	        }
            }
            else
            {
	        if (argCnt == 1 && line[0] != '#')
	        {
		    cpLog (LOG_DEBUG,"VM:CFG:Parser:Line %d has too few arguments: %s", lineCnt, line);
	        }
            }
        }
        fclose (cfgStream);
        cfgStream = NULL;
    }
    else
    {
        cpLog (LOG_ERR, "VM:CFG:Parser:Cannot open file:\"%s\"", fname.c_str ());
        return false;
    }
    return true;
}
