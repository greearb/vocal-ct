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


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#ifdef __FreeBSD__
#include <sys/ioctl.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>


#include <fcntl.h>
#include <iostream.h>
#include <unistd.h>
#include "UaFacade.hxx"
#include "SipThread.hxx"
#include "SipTransceiver.hxx"
#include "RegistrationManager.hxx"
#include "UaConfiguration.hxx"
#include "UaCommandLine.hxx"
#include "UaCallControl.hxx"
#include "UaCli.hxx"
#include <misc.hxx>
#include "gua.hxx"
#include <SipEvent.hxx>

//Libmedia
#include "MediaController.hxx"

//Devices
#include "LinAudioDevice.hxx"
#include "NullDevice.hxx"
#include "FileMediaDevice.hxx"

#ifdef VOCAL_USE_SPHINX
#include "SpeechDevice.hxx"
#endif

#ifdef USE_LANFORGE
#include "LF/LANforgeCustomDevice.hxx"
#include "LF/LFVoipHelper.hxx"
#endif

#ifdef USE_VM
#include "VmcpDevice.hxx"
#endif

#ifdef USE_MPEGLIB
#include "VideoDevice.hxx"
#endif

#define NamedDir      "/tmp/sipset."
#define NamedReadFile   "GuiToUa."
#define NamedWriteFile   "UaToGui."

using namespace Vocal;
using namespace Vocal::UA;


Sptr<UaFacade> UaFacade::myInstance;
char* UaFacade::VOIP_ID_STRING = "LANforge-SIP-SoftPhone/4.4.11 (www.candelatech.com)";

void UaFacade::destroy() {
   if (myInstance.getPtr()) {
      UaCallControl::destroy();
      cpLog(LOG_DEBUG, "UaFacade::destroy()");

      // There exist circular links...this should break them gracefully.
      myInstance->mySipThread = NULL;
      myInstance->mySipStack = NULL;
      myInstance->myRegistrationManager = NULL;

      myInstance = NULL;
   }
}

UaFacade& UaFacade::instance() {
   return *(myInstance.getPtr());
}

void
UaFacade::initialize(const Data& applName,
                     unsigned short  defaultSipPort, bool filteron, bool nat)
{
    assert(myInstance == 0);
    
    string pip = UaConfiguration::instance().getValue(ProxyServerTag);
    if (pip.empty()) {
       cpLog(LOG_ERR, "WARNING:  Proxy-IP was not set, must use peer-to-peer dialing..\n");
       pip = "0.0.0.0";
    }

    NetworkAddress proxyAddr(pip);
    if ((pip != "0.0.0.0") && (proxyAddr.getPort() < 0)) {
       cout << "WARNING:  Setting proxy port to 5060, it was not specified...\n";
       cpLog(LOG_ERR, "WARNING:  Setting proxy port to 5060, it was not specified...\n");
       proxyAddr.setPort(5060);
    }

    string NAT_HOST = UaConfiguration::instance().getValue(NATAddressIPTag).c_str();
    int transport = 1;
    string tp = UaConfiguration::instance().getValue(SipTransportTag).c_str();
    if (strcasecmp(tp.c_str(), "TCP") == 0) {
       transport = 2;
    }

    // Command-line option -I [ip] over-rides anything read from cfg file.
    string local_ip = UaCommandLine::instance()->getStringOpt("local_ip");
    if (local_ip.size() != 0) {
       UaConfiguration::instance().setValue(RegisterFromTag, local_ip);
    }

    local_ip = UaConfiguration::instance().getMyLocalIp();

    string tmp = UaConfiguration::instance().getValue(IP_TOS_TAG).c_str();
    uint16 tos = 0;
    if (tmp.size()) {
       tos = atoi(tmp.c_str());
    }

    tmp = UaConfiguration::instance().getValue(PKT_PRIORITY_TAG).c_str();
    uint32 priority = 0;
    if (tmp.size()) {
       priority = atoi(tmp.c_str());
    }

    tmp = UaConfiguration::instance().getValue(VAD_ON_TAG).c_str();
    uint32 vad_on = 0;
    if (tmp.size()) {
       vad_on = atoi(tmp.c_str());
    }

    tmp = UaConfiguration::instance().getValue(VADMsBeforeSuppressionTag).c_str();
    uint32 vadmsbeforesuppression = 0;
    if (tmp.size()) {
       vadmsbeforesuppression = atoi(tmp.c_str());
    }

    tmp = UaConfiguration::instance().getValue(VADForceSendAfterMsTag).c_str();
    uint32 vad_fs = 0;
    if (tmp.size()) {
       vad_fs = atoi(tmp.c_str());
    }

    VADOptions va(vad_on, vadmsbeforesuppression, vad_fs);

    if (!NAT_HOST.length()){
       NAT_HOST = local_ip;
    }
    cpLog(LOG_ERR, "About to create UaFacade, tos: %i  priority: %i vad_on: %d vadmsbeforesuppression: %d local_ip: %s\n",
          tos, priority, vad_on, vadmsbeforesuppression, local_ip.c_str());
    myInstance = new UaFacade(applName, tos, priority, local_ip, defaultSipPort,
                              NAT_HOST, transport, proxyAddr, filteron, nat,
                              va);
}


// _localIp is the real local IP to use, ie pick whatever hostname resolves
//  to even if you are not trying to bind to a specific local interface.
UaFacade::UaFacade(const Data& applName, uint16 tos, uint32 priority,
                   const string& _localIp,
                   unsigned short _localSipPort, const string& _natIp,
                   int _transport, const NetworkAddress& proxyAddr,
                   bool filteron, bool nat, VADOptions& vadOptions__) :
      BaseFacade(_localIp, _localSipPort, _natIp, _transport, proxyAddr, nat)
#ifdef USE_LANFORGE
      , myLFThread(NULL)
#endif
      , vad_options(vadOptions__)
{
   DEBUG_MEM_USAGE("Beginning Facade constructor");
   try {
      myMode = CALL_MODE_UA;
      if (UaCommandLine::instance()->getIntOpt("speech")) {
#ifdef VOCAL_USE_SPHINX
         cpLog(LOG_INFO, "Agent running in speech rec mode");
         myMode = CALL_MODE_SPEECH;
#else
         cpLog(LOG_INFO, "Speech mode not supported in this version");
#endif
      }
      else if (UaCommandLine::instance()->getIntOpt("annon")) {
         cpLog(LOG_INFO, "Agent running in announcement mode");
         myMode = CALL_MODE_ANNON;
      }
      else if (UaCommandLine::instance()->getIntOpt("voicemail")) {
         cpLog(LOG_INFO, "Agent running in Voice Mail mode");
         myMode = CALL_MODE_VMAIL;
         UaConfiguration::instance().setValue(RegisterOnTag,  "false");
      }

      if (UaConfiguration::instance().getValue(UseLANforgeDeviceTag) != "0") {
         cpLog(LOG_INFO, "Agent running in LANforge mode");
         myMode = CALL_MODE_LANFORGE;
      }
      
      string cfg_local_ip = UaConfiguration::instance().getConfiguredLocalIp();
      string cfg_local_sip_dev = UaConfiguration::instance().getConfiguredLocalSipDev();
      string cfg_local_rtp_dev = UaConfiguration::instance().getConfiguredLocalRtpDev();


      string cfg_force_ipv6 = UaConfiguration::instance().getValue(ForceIPv6Tag);

      if (cfg_force_ipv6 == "true") {
         cerr << "ERROR:  ipv6 not supported.\n";
         cpLog(LOG_ERR, "ERROR:  ipv6 not supported.\n");
         exit(5);
      }


      //The object will try to bind to port in range sipPort ->
      //sipPort+10 as the local sipPort, if it fails, it will send error
      //application.
      
      cpLog(LOG_ERR, "UaFacade::UaFacade, cfg_local_ip: %s, localIp: %s",
            cfg_local_ip.c_str(), _localIp.c_str());
      int defaultSipPort = getLocalSipPort();
      int startPort = defaultSipPort;
      bool guessSipPort = false;
      
      if (defaultSipPort == 0) {
         //Guess the SIP port starting from 5060 till 5069
         defaultSipPort = 5060;
         startPort = defaultSipPort;
         guessSipPort = true;
      }
      cpLog(LOG_DEBUG, "UaFacade::UaFacade using local port:%d", defaultSipPort);
      //if (atexit(UaFacade_destroy) < 0) {
      //   cpLog(LOG_ALERT, "Failed to register with atexit()");
      //};
      
      DEBUG_MEM_USAGE("Before setting up GUI thread.");

      DEBUG_MEM_USAGE("After GUI thread.");
      while (true) {
         try {
            cpLog(LOG_ERR, "Creating SipTransceiver on local_ip: %s:%d, dev: %s\n",
                  cfg_local_ip.c_str(), defaultSipPort, cfg_local_sip_dev.c_str());
            DEBUG_MEM_USAGE("Creating SipTransceiver.");
            mySipStack = new SipTransceiver(tos, priority, cfg_local_ip, cfg_local_sip_dev,
                                            applName, defaultSipPort, nat,
                                            APP_CONTEXT_GENERIC, false);
            cpLog(LOG_ERR, "Created SipTransceiver...\n");
            DEBUG_MEM_USAGE("Created SipTransceiver.");
            UaConfiguration::instance().setValue(LocalSipPortTag, Data(defaultSipPort).logData());
            setLocalSipPort(defaultSipPort); //NOTE:  DefaultSipPort has been modified.

            cpLog(LOG_ERR, "Listening on port %d for SIP", defaultSipPort);
            char buf[256];
            snprintf(buf, 255, "INFO Listening on port %d for SIP", defaultSipPort);
            postMsg(buf, "listening on port");
            SipTransceiver::reTransOn();
         
   
            DEBUG_MEM_USAGE("Creating sipThread.");
            cpLog(LOG_ERR, "UaFacade:  About to create SIP thread.\n");
            mySipThread = new SipThread(mySipStack, this, false);
            
            //Start the registration manager to handle the registration 
            DEBUG_MEM_USAGE("Creating RegistrationManager.");
            myRegistrationManager = new RegistrationManager(mySipStack);
            
            break;
         }
         catch( ... ) {
            //Port is not free, try next
            cpLog(LOG_ERR, "UaFacade:  Port was not free: %d, trying next one..\n",
                  defaultSipPort);
            if ((defaultSipPort > startPort+9) || !guessSipPort) {
               char buf[256];
               snprintf(buf, 255, "ERROR Port in range %d->%d are not available",
                        startPort, defaultSipPort);
               postMsg(buf, "port range invalid");
               exit(-1);
            }
            defaultSipPort++;
            //Leave port 5061 as it is reserved for TLS
            if (defaultSipPort == 5061)
               defaultSipPort++;
         }
      }

      if (UaCommandLine::instance()->getIntOpt("cmdline")) {
         DEBUG_MEM_USAGE("Creating CLI.");
         myUaCli = new UaCli(this);
      }
      
      //Initialize the MediaController library
      cpLog(LOG_ERR, "UaFacade:  About to create Media Controller..\n");
      int minRtpPort = atoi(UaConfiguration::instance().getValue(MinRtpPortTag).c_str());
      int maxRtpPort = atoi(UaConfiguration::instance().getValue(MaxRtpPortTag).c_str());


      // Enable the codecs we care about...
      map<VCodecType, int> priority_map;
      int pr = atoi(UaConfiguration::instance().getValue(G711uPrioTag).c_str());
      if (pr != -1) {
         priority_map[G711U] = pr;
      }
      pr = atoi(UaConfiguration::instance().getValue(SpeexPrioTag).c_str());
      if (pr != -1) {
         priority_map[SPEEX] = pr;
      }
      pr = atoi(UaConfiguration::instance().getValue(G726_16PrioTag).c_str());
      if (pr != -1) {
         priority_map[G726_16] = pr;
      }
      pr = atoi(UaConfiguration::instance().getValue(G726_24PrioTag).c_str());
      if (pr != -1) {
         priority_map[G726_24] = pr;
      }
      pr  = atoi(UaConfiguration::instance().getValue(G726_32PrioTag).c_str());
      if (pr != -1 ) {
         priority_map[G726_32] = pr;
      }
      pr  = atoi(UaConfiguration::instance().getValue(G726_40PrioTag).c_str());
      if (pr != -1) {
         priority_map[G726_40] = pr;
      }
      
      /* Restrict this to use with LANforge only, to better protect the
       * licensed codec.  LANforge has it's own licensing scheme, if someone
       * comes up with an alternative scheme, this code can be changed to
       * use that instead. --Ben
       */
      if (UaConfiguration::instance().getValue(UseLANforgeDeviceTag) != "0") {
         pr = atoi(UaConfiguration::instance().getValue(G729aPrioTag).c_str());
         if (pr != -1) {
            priority_map[G729] = pr;
         }
      }

      priority_map[DTMF_TONE] = 0;

      int jitter_buffer_sz = atoi(UaConfiguration::instance().getValue(JitterBufferSzTag).c_str());

      DEBUG_MEM_USAGE("Initializing media controller");
      MediaController::initialize(tos, priority, cfg_local_ip, cfg_local_rtp_dev,
                                  minRtpPort, maxRtpPort, priority_map,
                                  &vad_options, jitter_buffer_sz);

      cpLog(LOG_DEBUG, "Initialized UaFacade");
   }
   catch(VException& e) {
      cpLog(LOG_ERR, "Caught exception, terminating application. Reason:%s",
            e.getDescription().c_str());
      cerr << "Caught exception, termination application. Reason ";
      cerr << e.getDescription().c_str() << endl;
   }
   
   DEBUG_MEM_USAGE("Done creating UaFacade.");
   cpLog(LOG_ERR, "UaFacade:  Done with constructor...\n");
}


Sptr<SipFrom> UaFacade::generateSipFrom() {
   string sipPort = UaConfiguration::instance().getValue(LocalSipPortTag);
   string un(UaConfiguration::instance().getValue(UserNameTag));
   Sptr<SipFrom> fromUrl;

   int pos = un.find_first_of("@");
   if (pos != (int)(string::npos)) {
      Sptr<BaseUrl> su = UaCallControl::parseUrl(un);
      if (su == 0) {
         string mFrom("sip:");
         mFrom += un;
         su = UaCallControl::parseUrl(mFrom);
         if (su == 0) {
            cpLog(LOG_ERR, "WARNING:  Could not parse 'from' -:%s:-, will treat as just a phone number.\n",
                  un.c_str());
         }
      }
      if (su != 0) {
         fromUrl = new SipFrom(su, UaConfiguration::instance().getMyLocalIp());
      }
   }
   if (fromUrl == 0) {
      fromUrl = new SipFrom("", UaConfiguration::instance().getMyLocalIp());
      fromUrl->setHost(Data(UaConfiguration::instance().getMyLocalIp()));
      fromUrl->setUser(un);
      //if(sipPort != "5060") {
      fromUrl->setPort(sipPort.c_str());
      //}
   }
   return fromUrl;
}//generateSipFrom


string UaFacade::getBareUserName() {
   Data user = UaConfiguration::instance().getValue(UserNameTag);
   int q = user.find_first_of("@");
   if (q != Data::npos) {
      Data du = user.substring(0, q);
      user = du;
   }
   return user.c_str();
}



#ifdef USE_LANFORGE
void UaFacade::setLFThread(LFVoipThread* lft) {
   myLFThread = lft;
}

LFVoipThread* UaFacade::getLFThread() {
   return myLFThread;
}

#endif

UaFacade::~UaFacade() {
#ifdef USE_LANFORGE
   if (myLFThread) {
      delete myLFThread;
      myLFThread = NULL;
   }
#endif
}

// Received from the stack
void UaFacade::process(Sptr < SipProxyEvent > event) {
   queueEvent(event);
}//process


/* If !sending, assume we are receiving this */
void UaFacade::postMsg(Sptr<SipMsg> sMsg, bool sending, const char* debug) {
    assert(sMsg != 0);
    strstream s;

    if (sending) {
       s << "SENDING ";
    }
    else {
       s << "RECEIVING ";
    }

    cpLog(LOG_INFO, "postMsg<ptr> :%s sending: %d  debug: %s",
          sMsg->encode().logData(), (int)(sending), debug);
    if (sMsg->getType() == SIP_STATUS) { 
        Sptr<StatusMsg> statusMsg;
        statusMsg.dynamicCast(sMsg); 
        assert(statusMsg != 0);
        int statusCode = statusMsg->getStatusLine().getStatusCode();
        if ((statusMsg->getCSeq().getMethod() != INVITE_METHOD) &&
            (statusMsg->getCSeq().getMethod() != CANCEL_METHOD)) {
           s << "INFO ";
        }
        else {
           switch(statusCode) {
           case 100: {
              s << "TRYING ";
              break;
           }
           case 180:
           case 183: {
              //cerr << "RINGING:" << endl;
              s << "RINGING ";
              break;
           }
           case 200: {
              s << "INCALL ";
              break;
           }
           case 302: {
              cerr << "In REDIRECT:" << endl;
              s << "REDIRECT ";
              break;
           }
           case 480:
           case 486: {
              s << "BUSY ";
              break;
           }
           case 404: {
              s << "INFO ";
              break;
           }
           case 403: {
              s << "INFO ";
              break;
           }
           case 408: {
              s << "INFO ";
              break;
           }
           case 407:
           case 487: {
              s << "INFO ";
              break;
           }
           case 401: {
              s << "UNAUTHORIZED ";
              break;
           }
           case 603: {
              s << "INFO ";
              break;
           }
           default: {
              s << "ERROR ";
              break;
           }
           }//switch
        }
        s << sMsg->encode().logData() << endl << ends;
    }
    else {
       if ((sMsg->getType() == SIP_BYE) ||
           (sMsg->getType() == SIP_CANCEL)) {
          s << "HANGUP " << sMsg->encode().logData() << endl << ends;
       }
       else {
          s << "INFO " << sMsg->encode().logData() << endl << ends;
       }
    }
    postMsg(s.str(), debug);
    s.freeze(false);
}


void UaFacade::notifyCallEnded() {
   cpLog(LOG_DEBUG, "UaFacade::notifyCallEnded...\n");
   postMsg("CALL_ENDED", "notify call ended");
}

void UaFacade::postMsg(const string& msg, const char* dbg) {
   cpLog(LOG_INFO, "postMsg<string>, debug: %s  msg: -:%s:-\n", dbg, msg.c_str());
   if ((myMode == CALL_MODE_UA) || (myMode == CALL_MODE_LANFORGE)) {
      // TODO:  Post to GUI if we support that again in the future.

      if (myUaCli != 0) {
         myUaCli->parseInput(msg);
      }
   }

   if (myMode == CALL_MODE_LANFORGE) {
#ifdef USE_LANFORGE
      if (myLFThread) {
         myLFThread->postMsg(msg);
      }
#endif
   }
}


// Return existing, or create new media device with specified id.
Sptr<MediaDevice> 
UaFacade::getMediaDevice(int id) { 
   
   cpLog(LOG_DEBUG, "getMediaDevice, id: %d  myMode: %d\n", id, myMode);

   if ((myMode == CALL_MODE_ANNON) ||
       (myMode == CALL_MODE_VMAIL) ||
       (myMode == CALL_MODE_LANFORGE) ||
       (myMode == CALL_MODE_SPEECH)) {
      //Devices are created/destroyed per call basis
      Sptr<MediaDevice> retDev = 0;
      MediaDeviceMap::iterator itr = myMediaDeviceMap.find(id);
      if (itr != myMediaDeviceMap.end()) {
         return (itr->second);
      }

      if (myMode == CALL_MODE_ANNON) {
         retDev = new FileMediaDevice(id);
      }
#ifdef USE_LANFORGE
      else if (myMode == CALL_MODE_LANFORGE) {
         retDev = new LANforgeCustomDevice(id);
      }
#endif
      else if (myMode == CALL_MODE_SPEECH) {
#ifdef VOCAL_USE_SPHINX
         retDev = new SpeechDevice(id);
#else
         cerr << "Speech is not supported in this application" << endl;
         exit(0);
#endif
      }
#ifdef USE_VM
      else if (myMode == CALL_MODE_VMAIL) {
         cpLog(LOG_DEBUG, "Creating new VoiceMail device");
         retDev = new VmcpDevice(id);
      }
#endif
      assert(retDev != 0);
      myMediaDeviceMap[id] = retDev;
      myMediaDevice = retDev; // Cache last created.  Will we ever have more than one???
      return retDev;
   }

   //For UA mode, myMediaDevice should have been initialised
   //to a sound card

   bool cmdLineMode = false;
   if (UaCommandLine::instance()->getIntOpt("cmdline")) {
      cmdLineMode = true;
   }

#ifdef USE_MPEGLIB
   int video  = atoi(UaConfiguration::instance().getValue(VideoTag).c_str());
   if (video && (!cmdLineMode) && (myMediaDevice == 0)) {
      myMediaDevice = new VideoDevice(0);
   }
#endif

   //set the sound-card media device
   string devName = UaConfiguration::instance().getValue(DeviceNameTag);
   if (myMediaDevice ==0) {
      if (cmdLineMode)
         cerr << "Opening audio device " << devName.c_str() << endl;
      cpLog(LOG_INFO, "Using sound device [%s]", devName.c_str());
      if (devName == "/dev/null") {
         cpLog(LOG_INFO, "Opening NULL device");
         myMediaDevice = new NullDevice();
      }
      else {
#if defined(__linux__)
         myMediaDevice = new LinAudioDevice( devName.c_str());
#endif
      }
   }
   cpLog(LOG_DEBUG, "returning MediaDevice");
   return myMediaDevice; 
}

void
UaFacade::releaseMediaDevice(int id) {
   MediaDeviceMap::iterator itr = myMediaDeviceMap.find(id);
   if (itr != myMediaDeviceMap.end()) {
      cpLog(LOG_DEBUG, "Releasing media device with Id:%d", id);
      
      if ((myMediaDevice != 0)
          && ((itr->second)->getSessionId() == myMediaDevice->getSessionId())) {
         myMediaDevice = NULL;
      }

      myMediaDeviceMap.erase(itr);
   }
   else {
      cpLog(LOG_DEBUG, "WARNING:  Could not find media device: %d to release it!\n", id);
   }
}//releaseMediaDevice

void UaFacade::queueEvent(Sptr <SipProxyEvent> event) {
   eventQueue.push(event);
}

void UaFacade::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                    uint64 now) {
   mySipThread->tick(input_fds, output_fds, exc_fds, now);
   myRegistrationManager->tick(input_fds, output_fds, exc_fds, now);
#ifdef USE_LANFORGE
   myLFThread->tick(input_fds, output_fds, exc_fds, now);
#endif
   
   MediaController::instance().tick(input_fds, output_fds, exc_fds, now);

   while (eventQueue.size()) {
      if (eventQueue.top()->getRunTimer() > now) {
         // Wait a while, not ready to process any more events at this time
         break;
      }
      Sptr<SipProxyEvent> ev = eventQueue.top();
      UaCallControl::instance().processEvent(ev);
      eventQueue.pop();
   }
}

int UaFacade::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     int& maxdesc, uint64& timeout, uint64 now) {
   mySipThread->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   myRegistrationManager->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
#ifdef USE_LANFORGE
   myLFThread->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
#endif

   MediaController::instance().setFds(input_fds, output_fds, exc_fds, maxdesc,
                                      timeout, now);
   if (eventQueue.size()) {
      uint64 ntx = eventQueue.top()->getRunTimer();
      if (ntx < now) {
         timeout = 0;
      }
      else {
         timeout = min(timeout, ntx - now);
      }
   }
   return 0;
}


Sptr<CachedEncodedRtp> UaFacade::findRtpCache(const string& fname,
                                              VCodecType codec) {
   // Ok..need to add this to our cache
   char buf[fname.size() + 100];
   if (vad_options.getVADOn()) {
      sprintf(buf, "%s.VAD.%i.%i.%i",
              fname.c_str(), vad_options.getVADMsBeforeSuppression(),
              vad_options.getForceSendMs(), (int)(codec));
   }
   else {
      sprintf(buf, "%s.%i", fname.c_str(), (int)(codec));
   }

   string k(buf);
   Sptr<CachedEncodedRtp> rv = rtp_cache_map[k];
   char tmp[1024];

   if (! rv.getPtr()) {

      // Lock the base file so we don't conflict w/other processes.
      int fd = -1;
      FILE* lck = fopen(fname.c_str(), "r");
      if (lck) {
         fd = fileno(lck);
         flock(fd, LOCK_EX);
      }

      // Couldn't find anything...see if we can read it from disk.
      rv = new CachedEncodedRtp(k);
      ifstream f(buf, ios::binary | ios::in );
      int sofar = 0;
      if (f) {
         RtpPldBufferStorage b;
         // Ok then..read it in.
         while (f) {
            if (f.eof()) {
               break;
            }
            f.read((char*)(&b), sizeof(b));
            if (f.gcount() != sizeof(b)) {
               if (f.gcount() == 0) {
                  // Maybe it does eof handling wierd?
                  cpLog(LOG_ERR, "WARNING:  Read zero..must be end of file, sofar: %d\n",
                        sofar);
               }
               else {
                  cpLog(LOG_ERR, "ERROR:  Failed to read complete buffer, read: %d  file: %s  assuming cache corrupted, sofar: %d\n",
                        f.gcount(), k.c_str(), sofar);
                  rv = NULL;
               }
               break;
            }
            else {
               sofar += sizeof(b);
               cpLog(LOG_DEBUG, "read in header, length: %d, b.len: %d\n",
                     sizeof(b), b.len);
               if (b.len > 1024) {
                  cpLog(LOG_ERR, "ERROR:  buffer length too long while reading rtp buffer file: %s  assuming corrupted.\n", k.c_str());
                  rv = NULL;
                  break;
               }
               else {
                  if (b.len > 0) {
                     f.read(tmp, b.len);
                     if (f.gcount() != b.len) {
                        cpLog(LOG_ERR, "ERROR:  Failed to read complete buffer payload, read: %d file: %s  assuming cache corrupted, tried: %d  sofar: %d\n",
                              f.gcount(), k.c_str(), b.len, sofar);
                        rv = NULL;
                        break;
                     }
                     else {
                        sofar += b.len;
                        cpLog(LOG_DEBUG, "read in payload, length: %d  codecType: %d\n",
                              b.len, b.ct);
                        rv->addBuffer(new RtpPldBuffer(tmp, b.len, b.samples, b.ct));
                     }
                  }
                  else {
                     // VAD pkt
                     cpLog(LOG_DEBUG, "read in VAD pkt, length: %d  codecType: %d\n",
                           b.len, b.ct);
                     rv->addBuffer(new RtpPldBuffer(NULL, b.len, 0, b.ct));
                  }
               }
            }
         }
      }
      else {
         cpLog(LOG_ERR, "WARNING: Could not find RTP cache file: %s  error: %s  Will rebuild.\n",
               k.c_str(), VSTRERROR);
         rv = NULL;
      }

      if (lck) {
         flock(fd, LOCK_UN); // Unlock it..
         fclose(lck); // Seems to free up the pointer...
         lck = NULL;
      }
   }
   return rv;
}//findRtpCache


void UaFacade::updateRtpCache(const string& fname, list<RtpPldBuffer*> encoded_rtp) {

   VCodecType codec = G711U;
   list<RtpPldBuffer*>::const_iterator cii;
   for(cii = encoded_rtp.begin(); cii != encoded_rtp.end(); cii++) {
      codec = ((*cii)->getCodecType());
      break;
      //if ((*cii)->getLength() == 0) {
      //   // Silence
      //   vad = true;
      //   break;
      //}
   }

   Sptr<CachedEncodedRtp> c = findRtpCache(fname, codec);

   if (!c.getPtr()) {
      // Ok..need to add this to our cache
      char buf[fname.size() + 100];
      if (vad_options.getVADOn()) {
         sprintf(buf, "%s.VAD.%i.%i.%i", fname.c_str(),
                 vad_options.getVADMsBeforeSuppression(),
                 vad_options.getForceSendMs(), (int)(codec));
      }
      else {
         sprintf(buf, "%s.%i", fname.c_str(), (int)(codec));
      }

      //cpLog(LOG_ERR, "Creating RTP cache, fname: %s\n", buf);

      c = new CachedEncodedRtp(buf, encoded_rtp);
      string k(buf);
      rtp_cache_map[k] = c;

      // Lock the base file so we don't conflict w/other processes.
      int fd = -1;
      FILE* lck = fopen(fname.c_str(), "r");
      if (lck) {
         fd = fileno(lck);
         flock(fd, LOCK_EX);
      }

      // Make sure nothing sneaked in and created the file...
      ifstream ifs(buf);
      if (ifs) {
         cpLog(LOG_ERR, "WARNING:  Another process created rtp cache file...will ignore our copy.\n");
      }
      else {
         // Now, save it to disk.
         ofstream f(buf, ios::binary | ios::out);
         if (f) {
            list<RtpPldBuffer*>::const_iterator cii;
            for(cii = encoded_rtp.begin(); cii != encoded_rtp.end(); cii++) {
               RtpPldBuffer* bc = *cii;
               RtpPldBufferStorage b = bc->getStorage();
               //cpLog(LOG_ERR, "Writing buffer-storage, size: %i\n", sizeof(b));
               f.write((char*)(&b), sizeof(b));
               if (bc->getLength()) {
                  //cpLog(LOG_ERR, "Writing buffer payload, length: %i\n", bc->getLength());
                  f.write(bc->getBuffer(), bc->getLength());
               }
               if (f.fail()) {
                  cpLog(LOG_ERR, "ERROR:  Failed to write rtp cache, err: %s\n",
                        strerror(errno));
                  f.close();
                  unlink(buf);
                  break;
               }
            }
         }
         else {
            cpLog(LOG_ERR, "ERROR:  Failed to open rtp buffer cache for writing, file: %s  error: %s\n",
                  buf, strerror(errno));
         }
      }

      if (lck) {
         flock(fd, LOCK_UN); // Unlock it..
         fclose(lck); // Seems to free up the pointer...
         lck = NULL;
      }

   }
}
