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

static const char* const CommandLine_cxx_Version =
    "$Id: CommandLine.cxx,v 1.3 2006/03/12 07:41:28 greear Exp $";

#include "global.h"
#include <iostream>
#include "CommandLine.hxx"
#include "Daemon.hxx"
#include "cpLog.h"
#include "ParsePair.hxx"
#include "ConfFile.hxx"
#include "vocalconfig.h"

#if defined(WIN32) || defined(linux)
#if defined(WIN32)
#define __GNU_LIBRARY__
#ifndef __MINGW32__
#define __STDC__ 1
#endif
#endif // defined(WIN32)
#include <getopt.h>

#else
#include "../../contrib/libcext_bsd/getopt_long.h"
#endif

#include "vocal_version.h"

using Vocal::Configuration::NameValueMap;
using Vocal::Configuration::ParsePair;
using Vocal::ConfFile;


Sptr < CommandLine > CommandLine::commandLine = 0;


const pair < const char*, const char* > CommandLine::cmdLineOptionString[] =
{
    pair < const char*, const char* > ("PSERVER", "localhost:6005"),
    pair < const char*, const char* > ("LOCAL_IP", "" ),
    pair < const char*, const char* > ("SIP_PORT", "5060" ),
    pair < const char*, const char* > ("NO_RETRANSMIT", "0" ),
    pair < const char*, const char* > ("NO_DAEMON", "0" ),
    pair < const char*, const char* > ("LOGLEVEL", "LOG_INFO" ),
    pair < const char*, const char* > ("LOGFILE", "" ),
    
    pair < const char*, const char* > ("PSERVERBACKUP", "localhost:6005"),
    pair < const char*, const char* > ("RETRANS_INITIAL", "500" ),
    pair < const char*, const char* > ("RETRANS_MAX", "4000" ),
    pair < const char*, const char* > ("HEARTBEAT", "1" ),
    pair < const char*, const char* > ("USE_SYSLOG", "0"),
    pair < const char*, const char* > ("CONFFILE", VOCAL_INSTALL_PATH "/etc/vocal.conf"),
    pair < const char*, const char* > ("CONFDIR", VOCAL_INSTALL_PATH "/etc"),
    pair < const char*, const char* > ("USE_TLS", "0"),
};


struct optionMap
{
        char key;
        const char* value;
        const char* boolValue;
        const char* description;
};

const optionMap myOptionMap[] =
{
    { 'c', "CONFFILE",        0,   "specify configuration file" }, 
    { 'C', "CONFDIR",         0,   "specify configuration directory" }, 
    { 'd', "NO_DAEMON",       "1", "do not fork into background" }, 
    { 'r', "NO_RETRANSMIT",   "1", "disable SIP retransmission"  },
    { 'f', "LOGFILE",         0,   "specify log file" },
    { 'I', "LOCAL_IP",        0,   "specify local IP to which we'll bind" },
    { 'v', "LOGLEVEL",        0,   "specify log level" },
    { 'R', "RETRANS_INITIAL", 0,   "initial retransmission time in ms" },
    { 'M', "RETRANS_MAX",     0,   "maximum retransmission time in ms" },
    { 'h', "HEARTBEAT",       "0", "disable multicast heartbeat" },
    { 's', "USE_SYSLOG",      "1", "use syslog for logging" },
    { 'n', "NO_CONFFILE",     "1", "do not read options from any configuration file" },
    //{ 'p', "SIP_PORT",        0,   "SIP Port" }, /* Not needed, is last arg on cmd-line */
    { 'P', "PSERVER",         0,   "default provisioning server" },
    { 'S', "PSERVERBACKUP",   0,   "backup provisioning server" },
    { 't', "USE_TLS",         "1", "backup provisioning server" },
    { 'V', "VERSION",         "1", "print version and exit" },
    { 0, 0, 0, 0 }
};


CommandLine::CommandLine()
{
}


Sptr < CommandLine >
CommandLine::instance(
    const int argc,
    const char** argv,
    const char* applicationUsage
)
{
    if (commandLine == 0 )
    {
        map < Data, Data > defaultMap;
        map < Data, Data > cmdLineMap;
        ConfFile configFileMap;

        commandLine = new CommandLine();
        assert( commandLine != 0 );

        commandLine->setDefaultValues(defaultMap);
        commandLine->parseCommandLine( argc, argv, cmdLineMap );
        Data configPath;

        if(cmdLineMap.find("CONFFILE") != cmdLineMap.end())
        {
            // read the config file specified on the command line
            configPath = cmdLineMap["CONFFILE"];
        }
        else
        {
            // read the default config file
            configPath = defaultMap["CONFFILE"];
        }

        commandLine->myMap = defaultMap;

        // read and merge conffile if the NO_CONFFILE flag hasn't been
        // set somewhere
        map<Data,Data>::iterator i;

        if(cmdLineMap.find("NO_CONFFILE") == cmdLineMap.end())
        {
            cpLog(LOG_DEBUG, "reading from config file %s",
                  configPath.logData());
            bool result = configFileMap.parse( configPath );

            if(result)
            {
                for(i = configFileMap.begin(); i != configFileMap.end() ; ++i )
                {
                    cpLog(LOG_DEBUG, "from config file: %s: %s",
                          i->first.logData(), i->second.logData());
                    commandLine->myMap[i->first] = i->second;
                }
            }
            else
            {
                // no config file for some reason
            }
        }

        for(i = cmdLineMap.begin(); i != cmdLineMap.end() ; ++i )
        {
            cpLog(LOG_DEBUG, "from command line: %s: %s",
                  i->first.logData(), i->second.logData());
            commandLine->myMap[i->first] = i->second;
        }

        // Run as a daemon
        if (commandLine->getInt( "NO_DAEMON" ) == 0)
        {
            if (Daemon() != 0 )
            {
                cerr << "Cannot start as daemon!" << endl;
                exit( -1);
            }
        }

        // Set the debug level
        string debugLevel = CommandLine::instance()->getString( "LOGLEVEL" );

        int priority = cpLogStrToPriority(debugLevel.c_str());
        if (priority >= 0 && priority <= 10)
        {
            cpLog( LOG_ALERT, "set level to %s", cpLogPriorityToStr(priority));
#ifdef WIN32_DEBUGGING
	    cpLogSetPriority(priority, 0, 0);
#else
            cpLogSetPriority(priority);
#endif
        }
        else
        {
            cpLog( LOG_ALERT, "set level to LOG_DEBUG");
            cpLogSetPriority( LOG_DEBUG );
        }

        string syslog = commandLine->getString("USE_SYSLOG");

        if(syslog != "" && syslog != "0")
        {
            cpLog(LOG_ALERT, "setting logging to syslog");
            cpLogOpenSyslog();
        }

        string logfile = commandLine->getString("LOGFILE");

        if (logfile != "")
        {
            cpLog( LOG_ALERT, "setting log file to %s", logfile.c_str());
            cpLogOpen(logfile.c_str());
        }

        commandLine->print(LOG_INFO);
        for(i = configFileMap.begin(); i != configFileMap.end() ; ++i )
        {
            cpLog(LOG_DEBUG, "from config file: %s: %s",
                  i->first.logData(), i->second.logData());
        }
    }
    return commandLine;
}


void
CommandLine::setDefaultValues(map < Data, Data>& cmap)
{
    for (unsigned int i = 0;
            i < sizeof(cmdLineOptionString) / sizeof(cmdLineOptionString[0]);
            i++)
    {
        cmap[CommandLine::cmdLineOptionString[i].first] 
            = CommandLine::cmdLineOptionString[i].second;
    }
}


void
CommandLine::usage( const char* cmd_name ) const
{
    cerr << "Usage: " << cmd_name << " [options] sip-port\n\n";
    cerr << "Options:\n";
    for(int i = 0; myOptionMap[i].key != 0; ++i)
    {
        cerr << "    -" << myOptionMap[i].key;
        if(myOptionMap[i].boolValue == 0)
        {
            cerr << " arg";
        }
        else
        {
            cerr << "    ";
        }
        cerr << "    ";
        cerr << myOptionMap[i].description << "\n";
    }
    cerr << endl;
}


void
CommandLine::parseCommandLine( const int argc, 
                               const char**argv,
                               map <Data, Data>& omap)
{
    bool printUsage = false;
    int arg_count = 0;
    
    int c = 0;
    
#if defined(__svr4__)|| defined (__SUNPRO_CC)
    
    struct option
    {
# if defined __STDC__ && __STDC__
            const char *name;
# else
            const char *name;
# endif 
            /* has_arg can't be an enum because some compilers complain about
               type mismatches in all the code that assumes it is an int.  */
            int has_arg;
            int *flag;
            int val;
    };

#endif
    char optlist[256];
    int j = 0;
    for(int i = 0; myOptionMap[i].key != 0; ++i)
    {
        if(j < 250)
        {
            optlist[j++] = myOptionMap[i].key;
            if(myOptionMap[i].boolValue == 0)
            {
                optlist[j++] = ':';
            }
        }
    }
    optlist[j] = '\0';

    while ( (c = getopt ((argc),
                         const_cast < char** > (argv),
                         optlist) ) != EOF)

    {
        // yes, this is inefficient.
        int i;
        for(i = 0; myOptionMap[i].key != 0; ++i)
        {
            if(myOptionMap[i].key == c)
            {
                if(myOptionMap[i].boolValue != 0)
                {
                    // this is a flag, not an optarg
                    omap[myOptionMap[i].value] = myOptionMap[i].boolValue;
                }
                else
                {
                    // this is an optarg
                    omap[myOptionMap[i].value] = optarg;
                }
                break;
            }
        }

        if(c == 'V')
        {
            printf("VOCAL version %s\n", VOCAL_VERSION);
//            printf("Compiled on %s\n",   vocal_build_date);
            exit(-1);
        }

        if(c == '?')
        {
            // this is an unknown option -- print usage
            printUsage = true;
        } 
        else if(myOptionMap[i].key == 0)
        {
            // didn't find it
            printf("could not find flag -%c -- this should be impossible\n", c);
            assert(0);
        }
    }

    if ((argc - optind) < 1)
    {
        printUsage = true;
    }

    if ((argc - optind) > 1)
    {
        printUsage = true;
    }

    if (optind < argc)
    {
        // printf ("non-option ARGV-elements...\n");
        while (optind < argc)
        {
            // the required argument must be the sip port.
            omap["SIP_PORT"] = argv[optind];

            arg_count++;
            optind++;
        }
    }
    if(printUsage)
    {
        usage(argv[0]);
        exit(-1);
    }
}


Sptr < CommandLine >
CommandLine::instance()
{
    if (commandLine == 0 )
    {
        assert( commandLine != 0 );
        return 0;
    }
    else
    {
        return commandLine;
    }
}


string
CommandLine::getString(const string& cmdOption)
{
    map<Data,Data>::iterator i = myMap.find(cmdOption);

    if ( i != myMap.end() )
        return i->second.convertString();

    return "";
}


int
CommandLine::getInt(const string& cmdOption)
{
    map<Data,Data>::iterator i = myMap.find(cmdOption);

    int ret = 0;

    if ( i != myMap.end() )
        ret = i->second.convertInt();

    return ret;
}


#ifdef WIN32_DEBUGGING
void
CommandLine::print(const int loglevel, const char* file, int line)
{
    cpLog(loglevel, file, line, "************************************");

    for(map <Data, Data>::iterator i = myMap.begin();
        i != myMap.end();
        ++i)
    {
        cpLog(loglevel, file, line, "%s: %s",
              i->first.logData(),
              i->second.logData());
    }

    cpLog(loglevel, file, line, "compiled at: %s", __DATE__ " " __TIME__);

    cpLog(loglevel, file, line, "************************************");
}
#else
void
CommandLine::print(const int loglevel)
{
    cpLog(loglevel, "************************************");

    for(map <Data, Data>::iterator i = myMap.begin();
        i != myMap.end();
        ++i)
    {
        cpLog(loglevel, "%s: %s",
              i->first.logData(),
              i->second.logData());
    }

    cpLog(loglevel, "compiled at: %s", __DATE__ " " __TIME__);

    cpLog(loglevel, "************************************");
}

#endif // WIN32_DEBUGGING

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
