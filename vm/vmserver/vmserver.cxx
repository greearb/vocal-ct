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

static const char* const main_cpp_Version = 
"$Id: vmserver.cxx,v 1.1 2004/05/01 04:15:40 greear Exp $";
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <list>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "Timer.h"
#include "Waiter.h"

#include "cpLog.h"
#include "LineVMCP.h"
#include "VmSession.h"

#include <signal.h>
#include <unistd.h>

/** This is the main class for voice mail server (vmserver)
  * VM server uses sockets for communication with clients.
  * It listens on port 8024 for requests. 
  */


/** shutdown flag */
bool g_bShutdown = false;
string g_xCfgPath;


extern "C" {
    void catch_child(int p);
    void catch_term(int p);
};

void
catch_child (int p)
{
    /** Child was terminated, get return code. */
#ifdef WNOHANG
    while (waitpid (-1, NULL, WNOHANG) > 0);
#else
    wait (NULL);
#endif
    signal (SIGCHLD, catch_child);
}

void
catch_term (int p)
{
    /** Termination signal received. Shutown server. 
     */
    g_bShutdown = true;
    exit(1); 
}

int
daemon()
{
    pid_t pid;
    char* envValString = getenv( "NO_DAEMON" );
    if( envValString )
    {
        if( atoi( envValString ) != 0 )
        {
            cpLog(LOG_DEBUG,"NO_DAEMON environment variable set");
            return 0 ;
        }
    }

    if( ( pid = fork() ) < 0 )
    {
        cpLog(LOG_ERR,"Fork failed!");
        return  -1 ;
    }
    else if( pid != 0 )
    {
        exit ( 0 );
    }
    setsid();
    umask(0);
    close(0);
    return 0;
}


int
main (int argc, char *argv[])
{

    int logPrio=LOG_ERR;
    bool daemonflag=false;
    bool inetdflag=false;
    bool interactive=false;
    int vmcpHandle = 0;
    
    g_xCfgPath="./voicemail.config";

    int c = 0;
  
    while( ( c = getopt( argc, argv, "idf:v:h:m") ) != EOF )
    {
	switch(c)
	{
	    /** To run VM Server as the basis for IVR style applications, interactive 
	     * i.e. 'm' flag is used. This is going to be in future release 
	     */
	case 'm':
	{
	    interactive=true;
	}
	break;

	/** With this option, provide a voicemail configuration file */ 
        case 'f':
 	{
	    g_xCfgPath = optarg;
 	}
	break;
	

	case 'h':
	{
	    if( optarg == 0 ) 
		vmcpHandle=0;
	    else
	    {
		vmcpHandle = atoi(optarg);
	    }
	}
	break;
	 
	/** With 'v' option : specify the Log Priority Level */
	case 'v':
	{
	    if( optarg == 0 ) 
		logPrio = 7;
	    else
	    {
		logPrio = cpLogStrToPriority(optarg);
	    }
	}
	break;

	/** Run The VMServer as a daemon */
	case 'd':
	{
	    daemonflag=true;
	}
	break;

	/** Run The VMServer as a Inet daemon */ 
	case 'i':
	{
	    inetdflag=true;
	}
	break;
      
	}
    }
  
    /** Setup Logging priority */
    cpLogSetLabel ("VM");
    cpLogSetPriority (logPrio);

    Configuration cfg;

    /** Parse configuration */
    cfg.parse (g_xCfgPath);
  
    cpLogSetPriority (logPrio);
    cpLogOpen(cfg.m_xLogFileName.c_str());

    if( daemonflag ) daemon();

    /**  Create sockets */
    Vmcp vmcp;
  
    if( inetdflag )
    {
	vmcp.assign(vmcpHandle,vmcpHandle);
	char logLabel[11];
	sprintf (logLabel, "VM%08X", getpid ());
	cpLogSetLabel (logLabel);
	cpLog (LOG_INFO, "Starting session %08X", getpid ());

	/** Close waiting socket. We don't need it in this process */
	/** Start session */
	VmSession session;
	session.Start(&vmcp,interactive);
	cpLog (LOG_INFO, "End of session");
	return 3;

    }
    /** Catch SIGCHLD signal to get rid of zombies */
    signal (SIGCHLD, catch_child);

    /** Catch all termination signals to shutdown gracefully */
    signal (SIGTERM, catch_term);
    signal (SIGHUP, catch_term);
    signal (SIGKILL, catch_term);
    signal (SIGABRT, catch_term);
    signal (SIGSTOP, catch_term);
    signal (SIGTSTP, catch_term);
    signal (SIGINT, catch_term);
    signal (SIGQUIT, catch_term);


    /** Open a socket 
     *  Voice Mail server always listens for the requests on port 8024 
     */
    int sfd;
    if( (sfd=vmcp.wait(8024))==-1 )
    {
	cpLog(LOG_ERR,"Error binding socket %d",sfd );
	exit(3);
    }

    while (!g_bShutdown)
    {
	int fd;
	cpLog(LOG_DEBUG,"Waiting for connection");
	fd=vmcp.accept();
	if( fd ==-1 )
	{
	    cpLog(LOG_ERR,"Error accepting socket" );
	    exit(3);
	}

	if (g_bShutdown)
	{
	    break;
	}
	
	cpLog (LOG_DEBUG, "Connected");

	/** Ok, we have connected client. */
	/** Let's start a new process for it. */
	pid_t cPid;
	cPid = fork();
	switch (cPid)
	{
	case 0:
	{
	    close(vmcp.getListenSocket());
	    /** set log label for a new process */
	    char logLabel[25];
	    sprintf (logLabel, "VM%08X", getpid ());
	    cpLogSetLabel (logLabel);
	    cpLog (LOG_INFO, "Starting session:%d", getpid ());

	    /** Start session */
	    VmSession session;
	    session.Start(&vmcp,interactive);
	    cpLog (LOG_INFO, "End of session:%d", getpid());
	    //return 3;
	    exit(0);
	}
	default:
	    /** Close connected socket. We don't need it in this process. */
	    /** Continue loop */
	    cpLog(LOG_DEBUG, "In parent: closing");
	    close(fd);
	    break;
	}
    }

    cpLog (LOG_INFO, "Server shutdown.");


    return EXIT_SUCCESS;
}

