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
#include "global.h"
#include <string>
#include "cpLog.h"
#include "UaFacade.hxx"
#include "UaConfiguration.hxx"
#include "UaCommandLine.hxx"
#include <misc.hxx>
#include "gua.hxx"
#include <sstream>

// To allow us to clean up in a definate manner (helps with memory leak debugging)
#include "controlState/ControlStateFactory.hxx"
#include <NetworkConfig.hxx>
#include <UaStateFactory.hxx>
#include <UaCallControl.hxx>
#include <MediaController.hxx>
#include <CallDB.hxx>

#include <SipUdpConnection.hxx>

#ifdef USE_LANFORGE
#include "LF/LFVoipHelper.hxx"
#include "LF/LFVoipManager.hxx"
#endif

using namespace Vocal;
using namespace Vocal::UA;

int gua_running = 1;

void dumpInstanceCount(const char* msg) {
   ostringstream oss;
   oss << "dumpInstanceCount: " << msg << "  "
       << "  IOBufferv::string_cnt: " << IOBufferv::string_cnt
       << "  Data: " << Data::getInstanceCount()
       << "  BasicAgent: " << BasicAgent::getInstanceCount()
       << "  CallAgent: " << CallAgent::getInstanceCount()
       << "  SipUdpConnection: " << SipUdpConnection::getInstanceCount()
       << "  SipTransceiver: " << SipTransceiver::getInstanceCount()
       << "  MediaSession: " << MediaSession::getInstanceCount()
       << "  BugCatcher: " << BugCatcher::getInstanceCount()
       << "  RtpEvent: " << RtpEvent::getInstanceCount()
       << "  RtpPacket: " << RtpPacket::getInstanceCount()
       << "  SipMsg: " << SipMsg::getInstanceCount()
       << "  SipMsgContainer: " << SipMsgContainer::getInstanceCount()
       << "  SipMsgPair: " << SipMsgPair::getInstanceCount()
       << "  SipCallContainer: " << SipCallContainer::getInstanceCount()
       << endl;

   cpLog(LOG_ERR, oss.str().c_str());
}

int main(const int argc, const char**argv) {
   INIT_DEBUG_MEM_USAGE;
   DEBUG_MEM_USAGE("beginning of main");

   try {
      UaCommandLine::instance( (int)argc, (const char**)argv );
      //Set the default log file size to be 3MB
      DEBUG_MEM_USAGE("after argc");
      //cpLogSetFileSize (3000000);
      cpLogSetFileSize (30000000);
      //Set the num of backup files to be 2
      cpLogSetNumOfBackupFiles (2);

      DEBUG_MEM_USAGE("log has been set up");

      cpLogSetLabel("gua");

      UaConfiguration::instance(UaCommandLine::instance()->getStringOpt("cfgfile"));
   
      DEBUG_MEM_USAGE("UaConfiguraiton has been set up");
      string lFileName = UaConfiguration::instance().getValue(LogFileNameTag).c_str();
      if (lFileName.length()) {
         cpLogOpen(lFileName.c_str());
      }

      cpLogSetPriority(cpLogStrToPriority(UaConfiguration::instance().getValue(LogLevelTag).c_str()));
      cpLogSetPriority(LOG_DEBUG_STACK);

      int localSipPort = 
         atoi(UaConfiguration::instance().getValue(LocalSipPortTag).c_str());

      cpLog(LOG_INFO, "Trying to use port %d for SIP", localSipPort);

      DEBUG_MEM_USAGE("About to init facade");
      cpLog(LOG_ERR, "About to initialize UaFacade...\n");
      UaFacade::initialize("gua", localSipPort, true, true);

#ifdef USE_LANFORGE
      cpLog(LOG_ERR, "About to initialize LANforge thread...\n");
      LFVoipThread* lf_thread = new LFVoipThread(&(UaFacade::instance()), argv, argc);
      UaFacade::instance().setLFThread(lf_thread);
#endif

      DEBUG_MEM_USAGE("About to run facade");

      // Drop into our main loop
      fd_set input_set;
      fd_set output_set;
      fd_set exc_set;

      uint64 sleep_for;
      int maxdesc;
      uint64 now;
      struct timeval timeout_tv;
      uint64 lastInstanceDump = 0;

      while (gua_running) {
         sleep_for = 60 * 1000;

         FD_ZERO(&input_set);
         FD_ZERO(&output_set);
         FD_ZERO(&exc_set);

         maxdesc = 0;

         now = vgetCurMs();

         // Do some debugging.
         if ((lastInstanceDump + (60 * 1000)) < now) {
            lastInstanceDump = now;
            dumpInstanceCount("tick");
         }

         UaFacade::instance().setFds(&input_set, &output_set, &exc_set,
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

         // Either we timed out, or a file descriptor is readable.
         UaFacade::instance().tick(&input_set, &output_set, &exc_set, now);

      }//while

      dumpInstanceCount("end of while");
#ifdef USE_LANFORGE
      VoipHelperMgr::destroy();
      dumpInstanceCount("after VoipHelperMgr::destroy");
#endif
      UaFacade::destroy();

    }
    catch(VException& e) {
       cpLog(LOG_ERR, "Caught exception, termination application. Reason:%s",
             e.getDescription().c_str());
       cerr << "Caught exception, termination application. Reason ";
       cerr << e.getDescription().c_str() << endl;
    }

   // Delete all of the singletons we can find to help valgrind find valid
   // memory leaks.
   SipParserMode::destroy();
   ControlStateFactory::destroy();
   NetworkConfig::destroy();
   UaStateFactory::destroy();
   UaCallControl::destroy();
   MediaController::destroy();
   CallDB::destroy();
   UaConfiguration::destroy();

   dumpInstanceCount("destroyed all singletons");

   cpLog(LOG_ERR, "Exiting gua...\n");
   cerr << "Exiting gua..." << endl;
}//main


void debugMemUsage(const char* msg, const char* file, int line) {
   ostringstream oss;
   oss << file << ":" << line << "  " << msg << endl
       << " SipUdpConnection::_cnt: " << SipUdpConnection::getInstanceCount() << endl
       << " SipTransceiver::_cnt: " << SipTransceiver::getInstanceCount() << endl;

   ::debugMemUsage(oss.str().c_str(),  "gua_mem.txt");
}//debugMemUsage


