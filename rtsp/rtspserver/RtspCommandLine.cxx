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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspCommandLine_cxx_version =
    "$Id: RtspCommandLine.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#if defined(__FreeBSD__) || defined (__svr4__)
#include "getopt.h"
#else
#include <getopt.h>
#endif
#include <cstdio>

#include "parse3tuple.h"
#include "cpLog.h"

#include "RtspCommandLine.hxx"


const pair<const char*,const char*>
RtspCommandLine::cmdLineOptionString[] =
{
    pair<const char*, const char*>( "CfgFile",      "mediaServer.cfg" ),
    pair<const char*, const char*>( "LogLevel",     "4"),
    pair<const char*, const char*>( "Daemon",       "0" )
};


RtspCommandLine::RtspCommandLine( const int argc,
                                  const char** argv,
                                  const char* applicationUsage )
{
    setDefaultValues();
    parseCommandLine( argc, argv, applicationUsage );
}


RtspCommandLine::~RtspCommandLine()
{
}


void
RtspCommandLine::setDefaultValues()
{
    for( unsigned int i = 0;
         i < ( sizeof(cmdLineOptionString) / sizeof(cmdLineOptionString[0]) );
         i++ )
    {
        cmdLineOptionTable.insert( pair<string, string> (
                                   RtspCommandLine::cmdLineOptionString[i].first,
                                   RtspCommandLine::cmdLineOptionString[i].second ) );
    }
}


void
RtspCommandLine::parseCommandLine( const int argc,
                                   const char** argv,
                                   const char* applicationUsage )
{
    static struct option long_options[] =
    {
        { RtspCommandLine::cmdLineOptionString[0].first, 1, 0, 0 },
        { RtspCommandLine::cmdLineOptionString[1].first, 1, 0, 0 },
        { RtspCommandLine::cmdLineOptionString[2].first, 1, 0, 0 },
    };

    int c = 0;
    while( ( c = getopt( argc, const_cast<char**>( argv ),
                         "hdf:v:" ) ) != EOF )
    {
        switch( c )
        {
            case 'f':  // cfgfile
            {
                cmdLineOptionTable[long_options[0].name] = optarg;
                break;
            }
            case 'v':  // log level
            {
                int priority = cpLogStrToPriority( optarg );
                if( priority )
                {
                    char buffer[1];
                    sprintf( buffer,  "%d", priority );
                    cmdLineOptionTable[long_options[1].name] = buffer;
                }
                else
                {
                    cmdLineOptionTable[long_options[1].name] = "7";
                }
                break;
            }
            case 'd':  // daemon flag
            {
                cmdLineOptionTable[long_options[2].name] = "1";
                break;
            }
            case 'h':  // help
            {
                printf( "Usage: %s %s\n", argv[0], applicationUsage );
                exit( 0 );
                break;
            }
            default:
                break;
        }
    }

    if( ( argc - optind ) > 0 )
    {
        cpLog( LOG_ALERT, "Unknown option: %s", argv[ optind ] );
        exit( -1 );
    }
}


const string&
RtspCommandLine::getString(const string& cmdOption)
{
    static string nullstring = "";

    Table::iterator i = cmdLineOptionTable.find(cmdOption);

    if ( i != cmdLineOptionTable.end() )
        return i->second;

    return nullstring;
}


const int
RtspCommandLine::getInt(const string& cmdOption)
{
    Table::iterator i = cmdLineOptionTable.find(cmdOption);

    int ret = 0;

    if ( i != cmdLineOptionTable.end() )
        ret = atoi(i->second.c_str());

    return ret;
}


const bool
RtspCommandLine::getBool(const string& cmdOption)
{
    Table::iterator i = cmdLineOptionTable.find(cmdOption);

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
