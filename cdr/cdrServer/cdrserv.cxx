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

static const char* const cdrserv_cxx_Version =
    "$Id: cdrserv.cxx,v 1.1 2004/05/01 04:14:55 greear Exp $";

#include "VCdrException.hxx"
#include "CommandLine.hxx"
#include "CdrManager.hxx"
#include "CdrBilling.hxx"
#include "VFunctor.hxx"
#include "VThreadPool.hxx"
#include "HeartbeatTxThread.hxx"
#include "cpLog.h"


int
main( const int argc, const char** argv )
{
    CdrConfig configData;
    char configFile[512];
    char psHost[512];
    char altHost[512];
    int psPort = 0;
    int altPort = 0;

    Data readSecret, writeSecret;


    int logLevel = LOG_DEBUG;
    bool useProvisioning = false;

    memset(psHost, 0, sizeof(psHost));
    memset(altHost, 0, sizeof(altHost));
    memset(configFile, 0, sizeof(configFile));

    // Read command line or env vars
    //
    char *envptr = getenv("CDRLOCALIP");
    if (envptr)
    {
        configData.m_localIp = envptr;
    }

    envptr = getenv("CDRCONFIGFILE");
    if (envptr)
    {
        strcpy(configFile, envptr);
    }
    else
    {
        CommandLine &parms = *CommandLine::instance(argc, argv);
        logLevel = cpLogStrToPriority(parms.getString("LOGLEVEL").c_str());
        string tip = parms.getString("LOCALIP");
        if (tip.size() && configData.m_localIp.empty()) {
            // No environ-var set, so use cmd-line arg.
            configData.m_localIp = tip;
        }

	{
	    Data psHostPort = CommandLine::instance()->getString( "PSERVER" );
	    char matched;
	    Data myPsHost = psHostPort.matchChar(":", &matched);
	    Data myPsPort = "6005";
	    if(matched != '\0') 
	    { 
		myPsPort = psHostPort;
	    }
	    else 
	    {
		// no match, so the remainder must be the psHost
		myPsHost = psHostPort;
	    }
	    LocalScopeAllocator lo;
	    strncpy(psHost, myPsHost.getData(lo), 254);
	    psPort = myPsPort.convertInt();
	}

	{
	    Data psHostPort = CommandLine::instance()->getString( "PSERVERBACKUP" );
	    char matched;
	    Data myPsHost = psHostPort.matchChar(":", &matched);
	    Data myPsPort = "6005";
	    if(matched != '\0') 
	    { 
		myPsPort = psHostPort;
	    }
	    else 
	    {
		// no match, so the remainder must be the psHost
		myPsHost = psHostPort;
	    }
	    LocalScopeAllocator lo;
	    strncpy(altHost, myPsHost.getData(lo), 254);
	    altPort = myPsPort.convertInt();
	}

        {
            readSecret = CommandLine::instance()->getString("CONFDIR") 
                + "/" + "vocal.secret";
            writeSecret = CommandLine::instance()->getString("CONFDIR") 
                + "/" + "vocal.writesecret";
        }

        useProvisioning = true;
    }

    // Load configuration from provisioning or config file
    //
    try
    {
        if (useProvisioning)
        {
            LocalScopeAllocator lo;
            LocalScopeAllocator lo2;
            configData.getPsData(psHost, 
                                 psPort, 
                                 altHost, 
                                 altPort, 
                                 readSecret.getData(lo), 
                                 writeSecret.getData(lo2));
            configData.m_logLevel = logLevel;
        }
        else if (configFile[0])
        {
            configData.getData(configFile);
        }
        else
        {
            cpLog(LOG_ALERT, "No configuration data.");
            exit(1);
        }
    }
    catch (VException& e)
    {
        cpLog(LOG_ALERT, "Error reading config data. Exiting application. Reason:%s", e.getDescription().c_str());
        exit(1);
    }

    // Start Application
    //
    try
    {
        // set logging level
        cpLogSetPriority(configData.m_logLevel);
        configData.print(LOG_INFO);

        // first call to instance for initialization
        CdrManager::instance(&configData);

        // spawn a new thread to handle the connection to the billing server
        //
        VFunctor func(CdrBilling::run, &configData);
        VThreadPool threadPool(1);
        threadPool.addFunctor(func);

        if (useProvisioning)
        {
            // start the heartbeats, must be done after provisioning is instantiated
            //
            HeartbeatTxThread heartbeat(configData.m_localIp,
                                        "", /* local_dev_to_bind_to */
                                        configData.m_serverPort);
            heartbeat.run();

            CdrManager::instance().run();
            CdrManager::destroy();

            // wait until the threads exit
            heartbeat.join();
        }
        else
        {
            // run the server without heartbeat (if there is no provisioning)
            //
            CdrManager::instance().run();
            CdrManager::destroy();
        }
    }
    catch (VException& e)
    {
        cpLog(LOG_ALERT, "Application exiting, reason:%s", e.getDescription().c_str());
    }
    catch (...)
    {
        cpLog(LOG_ALERT, "Application exiting, reason:Unknown");
    }
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
