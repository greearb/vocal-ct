#ifndef RtspConfiguration_hxx
#define RtspConfiguration_hxx
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

static const char* const RtspConfiguration_hxx_version =
    "$Id: RtspConfiguration.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include <string>
#include "VMutex.h"


/** Parses and syntax checks the rtsp configuration file and allows other
    classes to access those values.<p>

    Valid access values and their default values:
    <ul>
    <li>ServerName ("Vovida RTSP Server")
    <li>RtspPort (554)
    <li>AudioDirectory ("")
    <li>MaxSessions (200)
    <li>RtpPortRangeMin (8000)
    <li>RtpPortRangeMax (8001)
    <li>DefaultFileExtension* (1)
    <li>DefaultCodec* (1)
    <li>RtpMapRequired (true)
    <li>LogRtpTraffic (false)
    <li>MaxRecordingFileBytes (10240)
    <li>LockRecordingFiles (true)
    <li>StatisticsLogFile ("")
    </ul>

    <i>DefaultFileExtensions</i> (see RtspFileHandler class):
    <ul>
    <li>1 - AU_EXT
    <li>2 - WAV_EXT
    <li>3 - RAW_EXT
    <li>4 - RTP_EXT
    </ul>

    <i>DefaultCodec</i> (see RtpFileTypes structure).  Default is g711.

 */
class RtspConfiguration
{
   public:
        /** create a singelton object based on string */
        static RtspConfiguration& instance( const string cfgFile );
        /** create a singelton object */
        static RtspConfiguration& instance();
        /** destructor */
        ~RtspConfiguration();
        /** frees the memory associated with singelton instance */
        static void destroy( void );

        /** parse configuration file arguments */
        static void parseCfgFileCallBack( char* tag, char* type, char* value );

        /** print all configuration values - used for debugging */
        void show();

    public:
        /** */
        string serverName;
        /** */
        int rtspPort;
        /** */
        string audioDirectory;
        /** */
        int maxSessions;
        /** */
        int rtpPortRangeMin;
        /** */
        int rtpPortRangeMax;
        /** */
        int defaultFileExtension;
        /** */
        int defaultCodec;
        /** */
        bool rtpMapRequired;
        /** not used */
        bool logRtpTraffic;
        /** */
        int maxRecordingFileBytes;
        /** */
        bool lockRecordingFiles;

        /** */
        string statisticsLogFile;
        /** */
        bool logStatistics;
        /** maintain locking statistics log file */
        VMutex logFileMutex;

    private:
        /** constructor */
        RtspConfiguration();

        /** singelton object */
        static RtspConfiguration* myInstance;

    protected:
        /** suppress copy constructor */
        RtspConfiguration( const RtspConfiguration& );
        /** suppress assignment operator */
        RtspConfiguration& operator=( const RtspConfiguration& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
