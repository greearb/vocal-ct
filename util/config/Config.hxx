#if !defined(VOCAL_CONFIG_HXX)
#define VOCAL_CONFIG_HXX

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
 * IN EXCESS OF ,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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


static const char* const Config_hxx_Version = 
  "$Id: Config.hxx,v 1.2 2004/05/04 07:31:15 greear Exp $";


#include "IPAddress.hxx"
#include "GetOpt.hxx"
#include "NameValue.hxx"
#include "Writer.hxx"
#include <string>
#include <list>


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Vocal 
{


/** 
 */
namespace Configuration
{


using Vocal::IO::Writer;
using Vocal::Transport::IPAddress;
using Vocal::ReturnCode;
//using Vocal::Configuration::GetOpt;
//using Vocal::Configuration::NameValueMap;


/** 
 */
class Config : public Vocal::IO::Writer
{
    public:


    	/** Creates the configuration information. The defaults are
	 *  set here.
	 */
    	Config();


    	/** Destroys the configuration information.
    	 */
    	virtual ~Config();


    	/** Loads the configuration information from the command line.
	 *  The command line may in turn load information from 
	 *  a configuration and/or a provisioning server.
	 */
	ReturnCode	    load(int, char **);

    
    	/** If set, it is the configuration file used to configure the
	 *  application.
	 */
	string	    	    configFile() const;
	
	
        /** Get the new pid. Useful if you have been Daemon-ed.
         */
        virtual void        newPid();
        
        
    	/** Accessor for process id. A good thing to know.
	 */
	pid_t	    	    pid() const;
	
	
	/** Flag indicates whether we are running as a daemon or not.
	 */
	bool	    	    asDaemon() const;


        /** Set to true if when the application runs as a daemon, it
         *  is to redirect stdout and stderr to a file.
         */
        bool                daemonRedirectOutput() const;


    	/** Flag indicates at which log level to run.
	 */
	int 	    	    logLevel() const;
	

	/** Flag indicates if we are being run under the debugger.
	 */
	bool	    	    underDebugger() const;
	

        /** Provides the name of the application.
         */
        string              applicationName() const;
        
        
        /** The config file loads is xml.
         */
        bool                xmlConfig() const;


        /** Returns a copy of the map of all options set.
         */
        const NameValueMap  options() const;
        
        
    	/** If we need to dump it to a log.
	 */
        virtual ostream &   writeTo(ostream &) const;


    protected:


        /** Provide the usage to the user. Mutex is locked on entry.
         */
        virtual void            usage() const;
        

        /** Provide the error message when the config file parsing fails.
         *  Mutex is locked on entry.
         */
        virtual void            loadCfgFileErrorMsg(ReturnCode) const;
        
        
        /** The user has the ability to modify the options before
         *  it is used to parse the command line. The mutex is write locked on
         *  entry.
         */
        virtual ReturnCode      preParse();
        
        
        /** The user has the ability to query the options after
         *  it has been used to parse the command line. The mutex is write
         *  locked on entry.
         */
        virtual ReturnCode      postParse();
        

    	/** Parse the command line. The mutex is write locked on entry.
	 */
    	virtual ReturnCode  	parseCmdLine(int argc, char ** argv);
	

	/** Load configuration from a file. The mutex is write locked on entry.
	 */
	virtual ReturnCode  	loadCfgFile();
	
	
        string  cfgStr;
        string  nodaemonStr;
        string  loglevelStr;
        string  debuggerStr;
        string  xmlcfgStr;

        char    cfgChar;
        char    nodaemonChar;
        char    loglevelChar;
        char    debuggerChar;
        char    xmlcfgChar;


    	/** If set, it is the configuration file used to configure the
	 *  application.
	 */
	string	    	    myConfigFile;
        	
	
	/** If set, it is the ip addresses of the provisioning servers 
	 *  used to configure the application.
	 */
	list<IPAddress>     myPservers;
	

    	/** It's a good thing to know.
	 */
	pid_t	    	    myPid;
	
	
	/** Flag indicates whether we are running as a daemon or not.
	 */
	bool	    	    myAsDaemon;


        /** Set to true when the application runs as a daemon, it
         *  is to redirect stdout and stderr to a file.        
         */
        bool                myDaemonRedirectOutput;
        
        
    	/** Flag indicates at which log level to run.
	 */
	int 	    	    myLogLevel;
	
	
	/** Flag indicates if we are being run under the debugger.
	 */
	bool	    	    myUnderDebugger;
	

        /** Application name
         */
        string              myApplicationName;


        /** Should we load an xml cfg file?
         */
        bool                myXmlCfg;

        /** My options
         */
        GetOpt              myOptions;
                
        
};


} // namespace Configuration
} // namespace Vocal


#endif // !defined(VOCAL_CONFIG_HXX)
