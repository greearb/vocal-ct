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


static const char* const Config_cxx_Version = 
    "$Id: Config.cxx,v 1.2 2004/05/04 07:31:15 greear Exp $";


#include "global.h"
#include "Config.hxx"
#include "ParsePair.hxx"
#include "VLog.hxx"
#include <unistd.h>
#include <iostream>


using Vocal::Configuration::Config;
using Vocal::Transport::IPAddress;
using Vocal::Configuration::GetOpt;
using Vocal::Configuration::ParsePair;
using Vocal::Configuration::NameValueMap;
using Vocal::Configuration::NO_VALUE;
using Vocal::Configuration::VALUE_PRESENT;
using Vocal::Configuration::VALUE_OPTIONAL;
using Vocal::Logging::VLog;
using Vocal::ReturnCode;
using Vocal::SUCCESS;
using namespace std;


Config::Config()
    :	cfgStr("config"),
        nodaemonStr("nodaemon"),
        loglevelStr("loglvl"),
        debuggerStr("debugger"),
        xmlcfgStr("xmlcfg"),
        cfgChar('f'),
        nodaemonChar('n'),
        loglevelChar('l'),
        debuggerChar('D'),
        xmlcfgChar('x'),
        myConfigFile(""),
    	myPservers(),
	myPid(getpid()),
	myAsDaemon(true),
        myDaemonRedirectOutput(true),
	myLogLevel(LOG_ERR),
	myUnderDebugger(false),
        myApplicationName(""),
        myXmlCfg(false)
{
}


Config::~Config()
{
}


ReturnCode
Config::load(int argc, char ** argv)
{
    string  appName = argv[0];
    
    size_t pos = appName.rfind('/');
    if ( pos != string::npos )
    {
        myApplicationName = appName.substr(pos+1);
    }
    else
    {
        myApplicationName = appName;
    }

    ReturnCode  rc = SUCCESS;

    if ( ( rc = preParse() ) != SUCCESS )
    {
        return ( rc );
    }

    // Read the command line
    //    
    if ( (rc = parseCmdLine(argc, argv)) != SUCCESS )
    {
        usage();
        return ( rc );
    }

    // Load the config file
    //
    if ( (rc = loadCfgFile()) != SUCCESS )
    {
        loadCfgFileErrorMsg(rc);
        return ( rc );
    }

    rc = postParse();
        
    return ( rc );
}


string	    	    
Config::configFile() const
{
    string rv = myConfigFile;
    return rv;
}


void    
Config::newPid()
{
    myPid = getpid();
}


pid_t	    	    
Config::pid() const
{
    return ( myPid );
}
	
	
bool	    	    
Config::asDaemon() const
{
    return ( myAsDaemon );
}


bool                
Config::daemonRedirectOutput() const
{
    return ( myDaemonRedirectOutput );
}


int 	    	    
Config::logLevel() const
{
    return ( myLogLevel );
}


bool	    	    
Config::underDebugger() const
{
    return ( myUnderDebugger );
}


string
Config::applicationName() const
{
    return ( myApplicationName );
}


bool                
Config::xmlConfig() const
{
    return ( myXmlCfg );
}


const NameValueMap 
Config::options() const
{
    return ( myOptions.options() );
}


ostream & 
Config::writeTo(ostream & out) const
{

    out << myApplicationName << ": " << __DATE__ << ", " << __TIME__ 
        << "\n  pid:                   " << myPid
	<< "\n  log level:             " << VLog::logName(myLogLevel)
    	<< "\n  as daemon:             " << ( myAsDaemon ? "yes" : "no" )
        << "\n  config file:           " << myConfigFile;
    
    if ( myConfigFile.size() && myXmlCfg )
    {
        out << " as XML";
    }
    
    out << "\n  under debugger:        " << ( myUnderDebugger ? "yes" : "no" );
    
    return ( out );
}


void        
Config::usage() const
{
}
        

void        
Config::loadCfgFileErrorMsg(ReturnCode rc) const
{
    cerr << "Could not load config file: " << myConfigFile
         << ", error = " << rc << endl;
}


ReturnCode      
Config::preParse()
{
   ReturnCode rc = SUCCESS;
   
    if  (   ( rc = myOptions.add(cfgStr, cfgChar, VALUE_OPTIONAL) ) 
                != SUCCESS
                
        ||  ( rc = myOptions.add(nodaemonStr, nodaemonChar) ) 
                != SUCCESS
                
        ||  ( rc = myOptions.add(loglevelStr, loglevelChar, VALUE_PRESENT) ) 
                != SUCCESS
                
        ||  ( rc = myOptions.add(debuggerStr, debuggerChar) ) 
                != SUCCESS
                
        ||  ( rc = myOptions.add(xmlcfgStr, xmlcfgChar) ) 
                != SUCCESS
        )
    {
        // Using conditional for early exit.
    }
    return ( rc );
}
        
        
ReturnCode      
Config::postParse()
{
    // Load nodaemon value.
    //
    if ( myOptions.get(nodaemonStr) )
    {
        myAsDaemon = false;
    }
    
    // Load log level value.
    //
    string log_level;
    if ( myOptions.get(loglevelStr, log_level) && log_level.size() )
    {
        myLogLevel = VLog::logValue(log_level);
    }

    // Load underdebugger value.
    //
    if ( myOptions.get(debuggerStr) )
    {
        myUnderDebugger = true;
    }

    return ( SUCCESS );
}
	

ReturnCode  	
Config::parseCmdLine(int argc, char ** argv)
{
    ReturnCode  rc = SUCCESS;

    if ( ( rc = myOptions.parse(argc, argv) ) != SUCCESS )
    {
        return ( rc );
    }

    // Load config file value.
    //
    if  ( myOptions.get(cfgStr, myConfigFile) && myConfigFile.size() == 0 )
    {
        myConfigFile = myApplicationName + ".conf";
    }

    // Load xmlcfg value.
    //
    if ( myOptions.get(xmlcfgStr)  )
    {
        myXmlCfg = true;
    }

    return ( rc );
}


ReturnCode  	
Config::loadCfgFile()
{
    if ( myConfigFile.size() == 0 )
    {
        // No config file to load. That's ok.
        //
        return ( SUCCESS );
    }

    ParsePair   cfgFile;

    cfgFile.tagName(myApplicationName);
    
    ReturnCode  rc = cfgFile.parse(
                        myXmlCfg ? ParsePair::CFG_XML_FILE 
                                 : ParsePair::CFG_FILE,
                        myConfigFile);
    
    if ( rc == SUCCESS )
    {
        const NameValueMap    & pairs = cfgFile.pairs();

        NameValueMap::const_iterator i;

        for ( i = pairs.begin(); i != pairs.end(); ++i )
        {
            myOptions.addValue(i->first, i->second);
        }    
    }    
    
    return ( rc );
}


