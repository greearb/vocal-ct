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

static const char* const mediaServer_cxx_Version =
    "$Id: mediaServer.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <signal.h>

#include "Daemon.hxx"
#include "VException.hxx"
#include "CommandLine.hxx"
#include "RtspCommandLine.hxx"
#include "RtspConfiguration.hxx"

#include "RtspServer.hxx"
#include "RtspSessionManager.hxx"
#include "RtspFileManager.hxx"
#include "RtspRtpProcessor.hxx"
#include "TickCount.hxx"

#include "cpLog.h"
#include <string>
#include <stdio.h>

using namespace std;
using Vocal::Statistics::TickCount;
using Vocal::Statistics::Duration;


/**
 * Handler for signal SIGABRT
 * @param signo the SIGABRT number
 * @return void
 */
void
signalHandler( int signo )
{
    if ( signo == SIGABRT )
    {
        cpLog( LOG_DEBUG, "Signal received" );
    }
}


/**
 * MediaServer entry function; receives command line parameters,
 * calls parser, sets provisioning up, creates and runs rtsp
 * media server instance
 * @param argc the number of command line arguments
 * @param argv pointer to the list of command line arguments
 * @return 1 on abort of program
 */
int
main( const int argc, const char* argv[] )
{
    // setup SIGABRT handler, for the tcp connection
    if( signal( SIGABRT, signalHandler ) == SIG_ERR )
    {
        return 1;
    }

    // parse command line parameters
    RtspCommandLine cIn( (const int)argc, (const char**) argv );

    // run as a daemon
    if( cIn.getBool( "Daemon" ) )
    {
        cpLog( LOG_ALERT, "Running as daemon" );
        Daemon();
    }

    // setup cpLog
    cpLogSetLabel( "MediaServer" );
    cpLogSetPriority( cIn.getInt( "LogLevel" ) );

    // read configuration file
    const string cfgStr = cIn.getString( "CfgFile" );
    FILE *cfgFile = fopen( cfgStr.c_str(), "r" );
    if( !cfgFile )
    {
        cpLog( LOG_ERR, "Fail to open configuration file: %s",
               cfgStr.c_str() );
        exit( 1 );
    }
    fclose( cfgFile );
    RtspConfiguration& cfgIn = RtspConfiguration::instance( cfgStr );
    if( cpLogGetPriority() >= LOG_ALERT )
    {
        cfgIn.show();
    }

    // read rtsp port
    int rtspPort = cfgIn.rtspPort;
    if( !rtspPort )
    {
        cpLog( LOG_ERR, "Fail to read rtsp port from cfg file" );
        return 1;
    }

    string filename = RtspConfiguration::instance().statisticsLogFile;
    if (filename.length() == 0)
        RtspConfiguration::instance().logStatistics = false;

    if (RtspConfiguration::instance().logStatistics)
    {
        TickCount::calibrate();
        assert(TickCount::ticksPerSec != 0);
    
        // open statistics file
        string filename = RtspConfiguration::instance().statisticsLogFile;
        FILE* statsFile = fopen( filename.c_str(), "w" );
        if ( !statsFile )
        {
            cpLog( LOG_ERR, "Fail to open rtsp statistic log file to write" );
        }
    
        fprintf( statsFile, "RTSP server statistics\n" );
        fprintf( statsFile, "----------------------\n" );
    
        fflush( statsFile );
        fclose( statsFile );
    }

    // create and run rtsp server instance, run() will block until shutdown
    RtspServer::instance( rtspPort );
    RtspServer::instance().run();

    // deallocate all singeltons
    RtspServer::instance().destroy();
    RtspSessionManager::instance().destroy();
    RtspRtpProcessor::instance().destroy();
    RtspFileManager::instance().destroy();
    RtspConfiguration::instance().destroy();
    RtspConfiguration::instance().destroy();
    StateMachine::instance().destroy();

    // print completion message
    char datebuf[32];
    struct timeval tv;
    assert( gettimeofday( &tv, 0 ) == 0 );
    const time_t timeInSeconds = (time_t)tv.tv_sec;
    strftime( datebuf, 32, "%H:%M:%S - %m/%d/%Y", localtime(&timeInSeconds) ); 

    cpLog( LOG_ALERT, "Server shutdown at %s", datebuf );

    return 0;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
