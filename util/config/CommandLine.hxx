#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2003 Vovida Networks, Inc.  All rights reserved.
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

static const char* const CommandLineVersion 
    = "$Id: CommandLine.hxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include <unistd.h>
#include <cstdio>

#if defined(__svr4__)
#include <stdio.h>
#endif

#include <cassert>
#include <map>
#include <string>
#include "global.h"
#include "Sptr.hxx"
#include "Data.hxx"

/**Singelton class, implements the prtocessing of command line arguments 
in Vocal applications. Options available wth default values and mappings 
to query the values for the options
<pre>
  -Option  Query-String     What it means 
 ----------------------------------------------------------------
     'd'  "NO_DAEMON"       "do not fork into background" 
     'r'  "NO_RETRANSMIT"   "disable SIP retransmission"  
     'f'  "LOGFILE"         "specify log file" 
     'v'  "LOGLEVEL"        "specify log level" 
     'R'  "RETRANS_INITIAL" "initial retransmission time in ms" 
     'M'  "RETRANS_MAX"     "maximum retransmission time in ms" 
     'h'  "HEARTBEAT"       "disable multicast heartbeat" 
     's'  "USE_SYSLOG"      "use syslog for logging" 
     'n'  "NO_CONFFILE"     "do not read options from any configuration file 
     'P'  "PSERVER"         "default provisioning server" 
     'S'  "PSERVERBACKUP"   "backup provisioning server" 
     'V'  "VERSION"         "print version and exit" 

For example to check if -d option is given, call interface

    // you must first instantiate the CommandLine object by passing in
    // the argc and argv.

    CommandLine::instance(argc, argv, 
        "[-rd] [-f log-filename] [-v DEBUG_LEVEL] pserver:port redundantpserver:port sip-port");


    if(CommandLine::instance().getInt("NO_DEAMON") == 1)
    {
        //Not running in daemon mode
    }
</pre>
*/
class CommandLine : public RCObject
{
    public:

        ///
        static Sptr < CommandLine > instance (
            const int argc,
            const char** argv,
            const char* applicationUsage =
                " [-rd] [-f log-filename] [-v DEBUG_LEVEL] pserver:port redundantpserver:port sip-port"
        );


        ///
        static Sptr < CommandLine > instance();
        /**Returns the string value of a command line option
          * For example, to get value for option  -v LOG_DEBUG_STACK
          * Call CommandLine::instance().getString("LOGLEVEL")
          * to get value as "LOG_DEBUG_STACK"
          */
        string getString(const string& cmdLineOption);

        /**Returns the int value of a command line option
          * For example, to get value for option  -p 5060
          * Call CommandLine::instance().getInt("PORT")
          * to get value as 5060
          */
        int getInt(const string& cmdLineOption);
        ///
#ifdef WIN32_DEBUGGING
	void print(const int loglevel, const char* file = NULL, int line = 0);
#else
        void print(const int loglevel);
#endif
        ~CommandLine() {};
    private:

        CommandLine();

        /// print out usage information
        void usage(const char* cmd_name) const;

        //Set the default values for all the command line  options supported
        void setDefaultValues ( map <Data, Data>& map);
        ///
        void parseCommandLine( const int argc, 
                               const char**argv, 
                               map <Data, Data>& map );
        ///
        static Sptr < CommandLine > commandLine;
        static const pair < const char*, const char* > cmdLineOptionString[];
        map < Data, Data > myMap;
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
