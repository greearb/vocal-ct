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

static const char* const UaCommandLine_cxx_Version =
    "$Id: UaCommandLine.cxx,v 1.3 2005/08/18 21:52:03 bmartel Exp $";

#include "global.h"
#if defined(WIN32) || defined(linux)
#if defined(WIN32)
#define __GNU_LIBRARY__
#define __STDC__ 1
#endif // defined(WIN32)
#include "getopt.h"

#else
#include "getopt_long.h"
#endif

#include <cstdio>
#include <iostream>
#include "cpLog.h"
#include "UaCommandLine.hxx"

using namespace Vocal::UA;
using namespace std;

Sptr<UaCommandLine> UaCommandLine::commandLine = 0;


UaCommandLine::UaCommandLine()
{
}


const pair<const char*,const char*> UaCommandLine::cmdLineOptionString[] =
{
    pair < const char*, const char* > ( "cfgfile", "gua.cfg" ),
    pair < const char*, const char* > ( "local_ip", "" ),
    pair < const char*, const char* > ( "quicknet", "0" ),
    pair < const char*, const char* > ( "soundcard", "0" ),
    pair < const char*, const char* > ( "cmdline", "1" ),
    pair < const char*, const char* > ( "annon", "0" ),
    pair < const char*, const char* > ( "speech", "0" ),
    pair < const char*, const char* > ( "voicemail", "0" ),
};


void
UaCommandLine::instance( const int argc,
                         const char** argv,
                         const char* applicationUsage )
{
    assert (commandLine == 0 );

    commandLine = new UaCommandLine();
    assert( commandLine != 0 );

    commandLine->setDefaultValues();
    commandLine->parseCommandLine( argc, argv, applicationUsage );
}


void
UaCommandLine::setDefaultValues()
{
    for ( unsigned int i = 0;
          i < sizeof(cmdLineOptionString) / sizeof(cmdLineOptionString[0]);
          i++ )
    {
        cmdLineOptionTable.insert(pair < string, string > (
            UaCommandLine::cmdLineOptionString[i].first,
            UaCommandLine::cmdLineOptionString[i].second ) );
    }
}


void
UaCommandLine::parseCommandLine( const int argc,
                                 const char** argv,
                                 const char* applicationUsage )
{

    // Need to set this up to ignore all --FOO arguments, as they
    // will be handled by LANforge.  Ugly as sin, but will work so
    // long as we are careful.
    const char * myargv[argc+1];
    int idx = 0;
    int q = 0;
    for (q = 0; q<argc; q++) {
        if (strncmp(argv[q], "--", 2) == 0) {
            q++; //Skip next one too, as it's the argument
        }
        else {
            myargv[idx] = argv[q];
            idx++;
        }
    }
    for (q = idx; q < argc; q++) {
        myargv[q] = NULL;
    }

    int c = 0;
    int cfgFlag = 0, cmdLineFlg = 1;
    int annonFlg =0, vmFlg = 0, helpFlg =0;
    while( ( c = getopt( idx, const_cast<char**>( myargv ), "f:gamMhsI:" ) ) 
	   != EOF )
    {
        switch( c )
        {
        case 'f':  // cfgfile
            cfgFlag = 1;
            cmdLineOptionTable["cfgfile"] = optarg;
            break;
        case 'g':  // GUI mode
            cmdLineFlg = 0;
            cmdLineOptionTable["cmdline"] = "0";
            break;
        case 'I':  // local-IP
            cfgFlag = 1;
            cmdLineOptionTable["local_ip"] = optarg;
            break;
        case 'a':  // Announce mode
            cmdLineFlg = 0;
            annonFlg = 1;
            cmdLineOptionTable["annon"] = "1";
            cmdLineOptionTable["cmdline"] = "0";
            break;
        case 's':  // Speech recognition mode
            cmdLineFlg = 0;
            annonFlg = 1;
            cmdLineOptionTable["speech"] = "1";
            cmdLineOptionTable["cmdline"] = "0";
            break;
        case 'm':  // VoiceMail mode
            cmdLineFlg = 0;
            vmFlg = 1;
            cmdLineOptionTable["voicemail"] = "1";
            cmdLineOptionTable["cmdline"] = "0";
            break;
        case 'M':  // VmAdmin mode
            cmdLineFlg = 0;
            vmFlg = 1;
            cmdLineOptionTable["vmadmin"] = "1";
			cmdLineOptionTable["voicemail"] = "1";
            cmdLineOptionTable["cmdline"] = "0";
            break;
        case 'h': //Help mode
            helpFlg = 1;
        break;
        case '?':  // getopt_long already printed an error message
            break;

        default:
            cpLog( LOG_ERR, "getopt() returned 0%o", c );
        }
    }

    if ( ( idx - optind ) > 0 )
    {
        cpLog( LOG_ERR, "Unknown option: %s", myargv[ optind ] );
        exit( -1 );
    }

    if (helpFlg) {
        cout << "Usage:" << myargv[0] ;
        cout << " [ -a | -m | -M ] [ -h ] -f <config_file>  " << endl;
        cout << " -a : run gua in announcement mode" << endl;
        cout << " -m : run gua as VoiceMail agent" << endl;
		cout << " -M : run gua as VmAdmin agent" << endl;
        cout << " -h : display usage" << endl;
        cout << " -f : specify configuration file"<< endl;
        cout << " -I : specify local IP address to which we'll bind to"<< endl;
        exit(0);
    }

    if(!cfgFlag && cmdLineFlg)
    {
        cout << "Usage:" << myargv[0] ;
        cout << " [ -a | -m | -M ] [ -h ] -f <config_file>  " << endl;
        cout << " -a : run gua in announcement mode" << endl;
        cout << " -m : run gua as VoiceMail agent" << endl;
		cout << " -M : run gua as VmAdmin agent" << endl;
        cout << " -h : display usage" << endl;
        cout << " -f : specify configuration file"<< endl;
        cout << " -I : specify local IP address to which we'll bind to"<< endl;
        exit(0);
    }
    if(vmFlg && annonFlg)
    {
        cpLog(LOG_ERR, "Usage:%s [ -a | -m | -M ] [ -h ] -I <local_ip> -f <config_file>  " , myargv[0]) ;
        exit(0);
    }
}


Sptr < UaCommandLine >
UaCommandLine::instance()
{
    assert( commandLine != 0 );

    return commandLine;
}


const string&
UaCommandLine::getStringOpt(const string& cmdOption)
{
    static string nullstring = "";

    TableIter i = cmdLineOptionTable.find(cmdOption);

    if ( i != cmdLineOptionTable.end() )
        return i->second;

    return nullstring;
}


int
UaCommandLine::getIntOpt(const string& cmdOption)
{
    TableIter i = cmdLineOptionTable.find(cmdOption);

    int ret = 0;

    if ( i != cmdLineOptionTable.end() )
        ret = atoi(i->second.c_str());

    return ret;
}


bool
UaCommandLine::getBoolOpt(const string& cmdOption)
{
    TableIter i = cmdLineOptionTable.find(cmdOption);

    bool ret = false;

    if ( i != cmdLineOptionTable.end() )
        ret = ( i->second == "1" );

    return ret;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
