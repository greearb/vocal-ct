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


static const char* const MediaController_cxx_Version = 
    "$Id: MediaController.cxx,v 1.7 2005/03/03 19:59:49 greear Exp $";


#include "MediaController.hxx"
#include "SdpHandler.hxx"
#include "CodecG711U.hxx"
#include "CodecG726_16.hxx"
#include "CodecG726_24.hxx"
#include "CodecG726_32.hxx"
#include "CodecG726_40.hxx"
#include "CodecSpeex.hxx"
#include "CodecG729a.hxx"
#include "CodecTelEvent.hxx"

using namespace Vocal;
using namespace Vocal::UA;

MediaController* MediaController::myInstance = 0;

MediaController&
MediaController::instance() {
   assert(myInstance != 0);
   return *myInstance;
}

void MediaController::destroy() {
   delete myInstance;
   myInstance = NULL;
}

void MediaController::initialize(uint16 tos, uint32 priority,
                                 const string& local_ip,
                                 const string& local_dev_to_bind_to,
                                 int minRtpPort, int maxRtpPort,
                                 map<VCodecType, int>& prio_map)
{
   assert(myInstance == 0);
   if (myInstance == 0) {
      myInstance = new MediaController(tos, priority, local_ip, local_dev_to_bind_to,
                                       minRtpPort, maxRtpPort, prio_map);
   }
}

MediaController::MediaController(uint16 tos, uint32 priority,
                                 const string& _local_ip,
                                 const string& local_dev_to_bind_to,
                                 int minRtpPort, int maxRtpPort,
                                 map<VCodecType, int>& prio_map)
      : local_ip(_local_ip),
        localDevToBindTo(local_dev_to_bind_to),
        _tos(tos), _skb_priority(priority)
{
   cpLog(LOG_DEBUG, "MediaController::MediaController");
   myRollingSessionId = 1;
   //get the list of reserved ports
   for(int i = minRtpPort; i < maxRtpPort; i = i+2) {
      //Check to see if port is free
      try {
         UdpStack uStack(_tos, _skb_priority, false, local_ip, localDevToBindTo, 0, i , i );
         Sptr<NetworkRes> res = new NetworkRes(local_ip, i);
         myNetworkResList.push_back(res);
      }
      catch(...) {
         cpLog(LOG_ERR, "Port %d is busy, try next", i);
      }
   }

   // Create the list of supported codec, default list
   Sptr<CodecAdaptor> cAdp = new CodecG711U();
   map<VCodecType, int >::iterator itr = prio_map.find(cAdp->getType());
   if (itr != prio_map.end()) {
      cAdp->setPriority((*itr).second);
      registerCodec(cAdp);
   }


#ifdef USE_VOICE_AGE
   Sptr<CodecAdaptor> cAdp2 = new CodecG729a();
   itr = prio_map.find(cAdp2->getType());
   if (itr != prio_map.end()) {
      cpLog(LOG_ERR, "G729a is in priority map.\n");
      cAdp2->setPriority((*itr).second);
      registerCodec(cAdp2);
   }
   else {
      cpLog(LOG_ERR, "G729a not in priority map.\n");
   }
#endif

   Sptr<CodecAdaptor> cAdp4 = new CodecSpeex();
   itr = prio_map.find(cAdp4->getType());
   if (itr != prio_map.end()) {
      cAdp4->setPriority((*itr).second);
      registerCodec(cAdp4);
   }
   
   Sptr<CodecAdaptor> cAdp5 = new CodecG726_16();
   itr = prio_map.find(cAdp5->getType());
   if (itr != prio_map.end()) {
      cAdp5->setPriority((*itr).second);
      registerCodec(cAdp5);
   }

   cAdp5 = new CodecG726_24();
   itr = prio_map.find(cAdp5->getType());
   if (itr != prio_map.end()) {
      cAdp5->setPriority((*itr).second);
      registerCodec(cAdp5);
   }

   cAdp5 = new CodecG726_32();
   itr = prio_map.find(cAdp5->getType());
   if (itr != prio_map.end()) {
      cAdp5->setPriority((*itr).second);
      registerCodec(cAdp5);
   }

   cAdp5 = new CodecG726_40();
   itr = prio_map.find(cAdp5->getType());
   if (itr != prio_map.end()) {
      cAdp5->setPriority((*itr).second);
      registerCodec(cAdp5);
   }

   Sptr<CodecAdaptor> cAdp3 = new CodecTelEvent();
   itr = prio_map.find(cAdp3->getType());
   if (itr != prio_map.end()) {
      cAdp3->setPriority((*itr).second);
      registerCodec(cAdp3);
   }

   cpLog(LOG_DEBUG, "Initialized MediaController");
}

MediaController::~MediaController()
{
}


void MediaController::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                           uint64 now) {
   if (myMediaSessionMap.size()) {
      // No need to spam the logs when nothing happening.
      cpLog(LOG_DEBUG_STACK, "MediaController tick, session size: %d\n",
            myMediaSessionMap.size());
   }
   map<int, Sptr<MediaSession> >::iterator i;
   for (i = myMediaSessionMap.begin(); i != myMediaSessionMap.end(); i++) {
      i->second->tick(input_fds, output_fds, exc_fds, now);
   }
}

int MediaController::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                            int& maxdesc, uint64& timeout, uint64 now) {
   map<int, Sptr<MediaSession> >::iterator i;
   for (i = myMediaSessionMap.begin(); i != myMediaSessionMap.end(); i++) {
      i->second->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   }
   return 0;
}


SdpSession
MediaController::createSession(const SdpSession& remoteSdp, const char* debug) 
    throw (MediaException&) 
{
    //Based on the remote SDP and capability, create a
    //compatible Local SDP and reserve associated Media
    //Resources 
    //reserve a local audio device
    string localAddr;
    int    localPort;
    
    int sId = createSessionImpl( localAddr, localPort, debug);
    SdpSession sdp;
    negotiateSdp(sdp, localAddr, localPort, remoteSdp);
    sdp.setSessionId(sId);

    return sdp;
}

SdpSession
MediaController::createSession(const char* debug) throw (MediaException&) {
    string localAddr;
    int    localPort;
    
    int sId = createSessionImpl( localAddr, localPort, debug);
    SdpSession sdp;
    setStandardSdp(sdp, localAddr, localPort);
    sdp.setSessionId(sId);
    return sdp;
}

int MediaController::createSessionImpl(string& localAddr, int& port, const char* debug) {
   //Get a NetworkRes from the list of resources and devices
   //and create a session Id to assign
   int sId = myRollingSessionId++;
   //Get a free NetworkRes and a device
   Sptr<NetworkRes> localRes; 
   for(list<Sptr<NetworkRes> >::iterator itr = myNetworkResList.begin();
       itr != myNetworkResList.end(); itr++) {
      if (!(*itr)->isBusy(local_ip, localDevToBindTo)) {
         localRes = *itr;
         localAddr = localRes->getIpName().c_str();
         port = localRes->getPort();
         break;
      }
      else {
         cpLog(LOG_ERR, "localRes: %s is busy: flag %d  probe: %d, mediaSessioMapSize: %d  owner: %p  MediaSession count: %d, debug: %s\n",
               (*itr)->toString().c_str(), (*itr)->getBusyFlag(),
               (*itr)->isPortTaken(local_ip), myMediaSessionMap.size(),
               (*itr)->getOwner(), MediaSession::getInstanceCount(), debug);
      }
   }
   if (localRes == 0) {
      cpLog(LOG_ERR, "No Network resource is free, debug: %s", debug);
      assert("No Ntwk Resource free" == "bug");

      throw MediaException("No Network resource is free",
                           __FILE__, __LINE__);
   }

   //TODO:  There is still a race here, cause another process could jump in
   // and steal the port! --Ben
   Sptr<MediaSession> mSession = new MediaSession(sId, localRes, _tos, _skb_priority,
                                                  localDevToBindTo, debug);

   assert(myMediaSessionMap.count(sId) == 0);
   myMediaSessionMap[sId] = mSession;
   return sId;
}//createSessionImpl

void 
MediaController::freeSession(int sId)
{
    //Free the resources associated with the session Id
   map<int, Sptr<MediaSession> >::iterator itr = myMediaSessionMap.find(sId);
   if(itr == myMediaSessionMap.end()) {
      cpLog(LOG_ERR, "Session %d does not exists", sId);
      return;
   }

   Sptr<MediaSession> ms = itr->second;

   if (ms->tearDown()) {
      myMediaSessionMap.erase(itr);
   }
   else {
      cpLog(LOG_ERR, "NOT Freeing session %d, teardDown failed", sId);
   }

}

void
MediaController::addDeviceToSession(unsigned int sessionId, Sptr<MediaDevice> mDevice)
{
   if (myMediaSessionMap.count(sessionId) == 0) {
      cpLog(LOG_ERR, "No session exists for if (%d), ignoring the add device request",
            sessionId);
      return;
   }
   Sptr<MediaSession> mSession = myMediaSessionMap[sessionId];
   mSession->addToSession(mDevice);
}

void
MediaController::addToSession(SdpSession& localSdp, SdpSession& remoteSdp)
{
    unsigned int sId = localSdp.getSessionId();
    cpLog(LOG_DEBUG, "Adding to session (%d)", sId);
    assert(myMediaSessionMap.count(sId));
    Sptr<MediaSession> mSession = myMediaSessionMap[sId];
    mSession->addToSession(localSdp, remoteSdp);
}

Sptr<MediaSession>
MediaController::getSession(unsigned int sessionId)
{
   map<int, Sptr<MediaSession> >::iterator itr = myMediaSessionMap.find(sessionId);
   assert(itr != myMediaSessionMap.end());
   return((*itr).second);
}

void 
MediaController::registerCodec(Sptr<CodecAdaptor> cAdp)
{
    myMediaCapability.addCodec(cAdp);
}

void
MediaController::startSession(unsigned int sId, VSdpMode mode)
{
    getSession(sId)->startSession(mode);
}

void
MediaController::suspendSession(unsigned int sId)
{
    getSession(sId)->suspend();
}
void
MediaController::resumeSession(unsigned int sId, SdpSession& remoteSdp)
{
    getSession(sId)->resume(remoteSdp);
}

SdpSession
MediaController::getSdp(unsigned int sId, VSdpMode mode)
{
    return (getSession(sId)->getSdp(mode));
}

SdpSession
MediaController::getSdp(unsigned int sId, SdpSession& remoteSdp)
{
    return (getSession(sId)->getSdp(remoteSdp));
}


