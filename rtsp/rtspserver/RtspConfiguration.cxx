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

static const char* const RtspConfiguration_cxx_version =
    "$Id: RtspConfiguration.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspConfiguration.hxx"

#include <parse3tuple.h>
#include <stdio.h>
#include "cpLog.h"

typedef enum
{
    ServerNameTag,
    RtspPortTag,
    AudioDirectoryTag,
    MaxSessionsTag,
    RtpPortRangeMinTag,
    RtpPortRangeMaxTag,
    DefaultFileExtensionTag,
    DefaultCodecTag,
    RtpMapRequiredTag,
    LogRtpTrafficTag,
    MaxRecordingFileBytesTag,
    LockRecordingFilesTag,
    StatisticsLogFileTag,
    TagMax
} RtspConfigTags;


static const char* TagString[] =
{
    "ServerName",
    "RtspPort",
    "AudioDirectory",
    "MaxSessions",
    "RtpPortRangeMin",
    "RtpPortRangeMax",
    "DefaultFileExtension",
    "DefaultCodec",
    "RtpMapRequired",
    "LogRtpTraffic",
    "MaxRecordingFileBytes",
    "LockRecordingFiles",
    "StatisticsLogFile",
    "Unknown"
};


// singelton object definitionn
RtspConfiguration* RtspConfiguration::myInstance = 0;


RtspConfiguration&
RtspConfiguration::instance( const string cfgFile )
{
    assert( myInstance == 0 );
    myInstance = new RtspConfiguration();
    assert( myInstance != 0 );
    assert( parse3tuple( cfgFile.c_str(), parseCfgFileCallBack ) );
    return *myInstance;
}


RtspConfiguration&
RtspConfiguration::instance()
{
    assert( myInstance != 0 );
    return *myInstance;
}


RtspConfiguration::RtspConfiguration()
    : serverName( "Vovida RTSP Server" ),
      rtspPort( 544 ),
      audioDirectory( "" ),
      maxSessions( 200 ),
      rtpPortRangeMin( 8000 ),
      rtpPortRangeMax( 8001 ),
      defaultFileExtension( 1 ),
      defaultCodec( 1 ),
      rtpMapRequired( true ),
      logRtpTraffic( false ),
      maxRecordingFileBytes( 10240 ),
      lockRecordingFiles( true ),
      statisticsLogFile( "" ),
      logStatistics( false )
{
#if defined (_linux_)    
    // register destroy function to delete singelton
    if( atexit( RtspConfiguration::destroy ) < 0 )
    {
        cpLog( LOG_ALERT, "Failed to register with atexit()" );
    };
#endif
}


RtspConfiguration::~RtspConfiguration()
{
}


void
RtspConfiguration::destroy()
{
    delete RtspConfiguration::myInstance;
    RtspConfiguration::myInstance = 0;
}


void
RtspConfiguration::parseCfgFileCallBack( char* tag, char* type, char* value )
{
    RtspConfiguration& cfg = RtspConfiguration::instance();

    switch( strConst2i( tag, TagString, TagMax ) )
    {
        case ServerNameTag:
        {
            cfg.serverName = value;
            break;
        }
        case RtspPortTag:
        {
            int i;
            sscanf( value, "%d", &i );
            cfg.rtspPort = i;
            break;
        }
        case AudioDirectoryTag:
        {
            cfg.audioDirectory = value;
            break;
        }
        case MaxSessionsTag:
        {
            int i;
            sscanf( value, "%d", &i );
            cfg.maxSessions = i;
            break;
        }
        case RtpPortRangeMinTag:
        {
            int i;
            sscanf( value, "%d", &i );
            cfg.rtpPortRangeMin = i;
            break;
        }
        case RtpPortRangeMaxTag:
        {
            int i;
            sscanf( value, "%d", &i );
            cfg.rtpPortRangeMax= i;
            break;
        }
        case DefaultFileExtensionTag:
        {
            int i;
            sscanf( value, "%d", &i );
            cfg.defaultFileExtension = i;
            break;
        }
        case DefaultCodecTag:
        {
            int i;
            sscanf( value, "%d", &i );
            cfg.defaultCodec = i;
            break;
        }
        case RtpMapRequiredTag:
        {
            string str = value;
            cfg.rtpMapRequired = ( str == "True" ) ||
                                 ( str == "true" ) ||
                                 ( str == "TRUE" );
            break;
        }
        case LogRtpTrafficTag:
        {
            string str = value;
            cfg.logRtpTraffic = ( str == "True" ) ||
                                ( str == "true" ) ||
                                ( str == "TRUE" );
            break;
        }
        case MaxRecordingFileBytesTag:
        {
            int i;
            sscanf( value, "%d", &i );
            cfg.maxRecordingFileBytes = i;
            break;
        }
        case LockRecordingFilesTag:
        {
            string str = value;
            cfg.lockRecordingFiles = ( str == "True" ) ||
                                     ( str == "true" ) ||
                                     ( str == "TRUE" );
            break;
        }
        case StatisticsLogFileTag:
        {
            cfg.statisticsLogFile = value;
            cfg.logStatistics = true;
            break;
        }
        default:
            cpLog( LOG_WARNING, "Unknown Tag: %s", tag );
    }
}


void
RtspConfiguration::show()
{
    printf( "********** Server Configuration ***********\n" );
    printf( "  serverName:            %s\n", serverName.c_str() );
    printf( "  rtspPort:              %d\n", rtspPort );
    printf( "  audioDirectory:        %s\n", audioDirectory.c_str() );
    printf( "  maxSessions:           %d\n", maxSessions );
    printf( "  rtpPortRange:          %d-%d\n", rtpPortRangeMin, rtpPortRangeMax );
    printf( "  defaultFileExtension:  %d\n", defaultFileExtension );
    printf( "  defaultCodec:          %d\n", defaultCodec );
    printf( "  rtpMapRequried:        %d\n", rtpMapRequired );
    printf( "  logRtpTraffic:         %d\n", logRtpTraffic );
    printf( "  maxRecordingFileBytes: %d\n", maxRecordingFileBytes );
    printf( "  lockRecordingFiles:    %d\n", lockRecordingFiles );
    printf( "  statisticsLogFile:     %s\n", statisticsLogFile.c_str() );
    printf( "  logStatistics:         %d\n", logStatistics );
    printf( "*******************************************\n" );
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
