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
    "$Id: cdrserv.cxx,v 1.4 2004/08/18 22:39:14 greear Exp $";

#include "VCdrException.hxx"
#include "CommandLine.hxx"
#include "CdrManager.hxx"
#include "CdrBilling.hxx"
#include "HeartbeatThread.hxx"
#include "cpLog.h"
#include <ProvisionInterface.hxx>


bool cdr_running = true;

Sptr<CdrManager> cdrManager;

int main( const int argc, const char** argv ) {
    CdrConfig configData;
    string configFile;
    NetworkAddress psHost("127.0.0.1", 5060);
    NetworkAddress altHost("127.0.0.1", 5060);

    string readSecret, writeSecret;

    int logLevel = LOG_DEBUG;

    // Read command line or env vars
    //
    char *envptr = getenv("CDRLOCALIP");
    if (envptr) {
        configData.m_localIp = envptr;
    }

    CommandLine &parms = *CommandLine::instance(argc, argv);
    logLevel = cpLogStrToPriority(parms.getString("LOGLEVEL").c_str());
    string tip = parms.getString("LOCALIP");
    if (tip.size() && configData.m_localIp.empty()) {
       // No environ-var set, so use cmd-line arg.
       configData.m_localIp = tip;
    }
    
    NetworkAddress na1(CommandLine::instance()->getString( "PSERVER" ));
    psHost = na1;
    
    NetworkAddress na2(CommandLine::instance()->getString( "PSERVERBACKUP" ));
    altHost = na2;
    
    readSecret = CommandLine::instance()->getString("CONFDIR") 
       + "/" + "vocal.secret";
    writeSecret = CommandLine::instance()->getString("CONFDIR") 
       + "/" + "vocal.writesecret";

    cdrManager = new CdrManager(configData);

    ProvisionInterface::initialize(psHost.getHostName().c_str(),
                                   psHost.getPort(),
                                   altHost.getHostName().c_str(),
                                   altHost.getPort(),
                                   readSecret.c_str(),
                                   writeSecret.c_str(),
                                   true);

    // TODO:  Do we need server callbacks??
    HeartbeatThread::initialize(configData.m_localIp, "", /*localDevToBindTo*/
                                SERVER_CDR,
                                HB_RX|HB_HOUSEKEEPING, NULL);

    // Listen for heartbeats from various server types, and
    // provisioning information too.
    HeartbeatThread::instance().addServerContainer(SERVER_RS);
    HeartbeatThread::instance().addServerContainer(SERVER_POS);
    HeartbeatThread::instance().addServerContainer(SERVER_JS);
    HeartbeatThread::instance().addServerContainer(SERVER_FS);
    HeartbeatThread::instance().addServerContainer(SERVER_MS);
    
    ProvisionInterface::instance().setHeartbeatThread(HeartbeatThread::instance());

    ProvisionInterface::instance().registerHandler(cdrManager.getPtr());

    string all;
    ProvisionInterface::instance().registerSubscriberInterest(all);
    ProvisionInterface::instance().registerServerInterest();

    ProvisionInterface::instance().requestAllServers();
    if (ProvisionInterface::instance().waitFor(ALL_SERVERS, 60 * 1000)) {
       cpLog(LOG_ERR, "ERROR:  Did not get the listing of ALL_SERVERS in time.\n");
       exit(8);
    }

    ProvisionInterface::instance().requestAllSubscribers();
    // Wait untill we have a response (or 60 seconds has elapsed)
    if (ProvisionInterface::instance().waitFor(ALL_SUBSCRIBERS, 60 * 1000) < 0) {
       cpLog(LOG_ERR, "ERROR:  Did not get the listing of ALL_SUBSCRIBERS in time.\n");
       exit(7);
    }

    // Start Application
    //
    try {
        // set logging level
        cpLogSetPriority(configData.m_logLevel);
        configData.print(LOG_INFO);

        // Drop into our main loop.
        fd_set input_set;
        fd_set output_set;
        fd_set exc_set;

        uint64 sleep_for;
        int maxdesc;
        uint64 now;
        struct timeval timeout_tv;

        while (cdr_running) {
            sleep_for = 60 * 1000;

            FD_ZERO(&input_set);
            FD_ZERO(&output_set);
            FD_ZERO(&exc_set);

            maxdesc = 0;

            now = vgetCurMs();
            
            cdrManager->setFds(&input_set, &output_set, &exc_set, maxdesc,
                               sleep_for, now);
            HeartbeatThread::instance().setFds(&input_set, &output_set, &exc_set,
                                               maxdesc, sleep_for, now);

            timeout_tv = vms_to_tv(sleep_for);

            int fds = select(maxdesc + 1, &input_set, &output_set, &exc_set, &timeout_tv);
            if (fds < 0) {
                if (errno == EBADF) {
                    cpLog(LOG_ERR, "ERROR:  bad file desc. given in a set to select.\n");
                    break;
                }//if
                else if (errno == EINTR) {
                    cpLog(LOG_WARNING, "ERROR:  A non blocked signal was caught (EINTR).\n");
                    // Clear all the FD-sets
                    FD_ZERO(&input_set);
                    FD_ZERO(&output_set);
                    FD_ZERO(&exc_set);
                }//if
            }

            now = vgetCurMs();

            cdrManager->tick(&input_set, &output_set, &exc_set, now);
            HeartbeatThread::instance().tick(&input_set, &output_set, &exc_set, now);
        }//while
    }
    catch (VException& e) {
        cpLog(LOG_ALERT, "Application exiting, reason:%s", e.getDescription().c_str());
    }
}//main
