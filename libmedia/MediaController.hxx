#ifndef MediaController_hxx
#define MediaController_hxx

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


static const char* const MediaController_hxx_Version = 
    "$Id: MediaController.hxx,v 1.3 2004/06/22 02:24:04 greear Exp $";

#include "global.h"
#include <list>
#include <map>
#include "Sptr.hxx"
#include "Rtp.hxx"
#include "VException.hxx"
#include "MediaDevice.hxx"
#include "NetworkRes.hxx"
#include "MediaSession.hxx"
#include "Sdp2Session.hxx"
#include "MediaCapability.hxx"
#include "MediaException.hxx"

using Vocal::SDP::SdpSession;

namespace Vocal
{

namespace UA
{


/** MediaController is a singleton object to manage Media session between two or more
    parties. The Controller provide interfaces for managing various
    media devices and allocate network resources for a given session. 

 */
class MediaController
{
public:
   ///Interface to access the Singelton object.
   static MediaController& instance();

   /**Interface to initialize the Controller. The interface should be called
    *by the application only once to create and intialize the Controller object.
    * @param local_ip  Local IP to bind to, use "" for system default.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    * @param prio_map  A mapping of codec types to priorities.  If a codec is not in
    *   the priority map, then it will not be used at all.
    */
   static void initialize(const string& local_ip,
                          const string& local_dev_to_bind_to,
                          int minRtpPort, int maxRtpPort,
                          map<VCodecType, int>& prio_map);

   ///
   string className() { return "MediaController"; }

   /** Virtual destructor
    */
   virtual ~MediaController();

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);


   ///Create a compatibl;e session after negotiating with remote SDP
   SdpSession createSession(const SdpSession& remoteSdp)
      throw (MediaException&) ;

   /**Create a brand new SDP session
    * using the RTP port range and MediaCapabilities registered.
    */
   SdpSession createSession()
      throw (MediaException&);

   ///Get SDP for an existing session identified by sId.
   SdpSession getSdp(unsigned int sId, VSdpMode=VSDP_SND_RECV);

   /**Get SDP for an existing session identified by sId, after negotiatng
    * with given remoteSdp.
    */
   SdpSession getSdp(unsigned int sId, SdpSession& resmoteSdp);

   ///Free a media session and all its resources identified by sId.
   void freeSession(int sessionId);

   ///Join a remote site to a session identified by localSdp 
   void addToSession(SdpSession& localSdp, SdpSession& remoteSdp);

   ///Add a device to session identified by the sessionId
   void addDeviceToSession(unsigned int sessionId, Sptr<MediaDevice> mDevice);

   ///Register a Codec 
   void registerCodec(Sptr<CodecAdaptor> cAdp);

   ///Get the MediaCapability object that holds the list of codecs supported
   MediaCapability& getMediaCapability() { return myMediaCapability; };

   ///Get a session associated with a sessionId.
   Sptr<MediaSession> getSession(unsigned int sessionId);

   ///Starts a session identified by sId and given mode.
   // mode used to default to VSDP_SND_RECV
   void startSession(unsigned int sId, VSdpMode mode);

   ///
   void suspendSession(unsigned int sId);

   ///
   void resumeSession(unsigned int sId, SdpSession& remoteSdp);

private:
   /// NOTE:  localAddr and port will be modified accordingly.
   int createSessionImpl(string& localAddr,
                         int& port);

   /** Create the Instance of MediaController
    * @param local_ip  Local IP to bind to, use "" for system default.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    * @param prio_map  A mapping of codec types to priorities.  If a codec is not in
    *   the priority map, then it will not be used at all.
    */
   MediaController(const string& local_ip,
                   const string& local_dev_to_bind_to,
                   int minRtpPort, int maxRtpPort,
                   map<VCodecType, int>& prio_map);

   ///
   static MediaController* myInstance;

   /**List of NetworkResource objects ( encapsulates IP,Port 
    * pair of listening sending media, and also the availability
    * information)
    */
   list<Sptr<NetworkRes> > myNetworkResList;

   ///
   map<int, Sptr<MediaSession> > myMediaSessionMap;

   /** Suppress copying
    */
   MediaController(const MediaController &);
        
   /** Suppress copying
    */
   const MediaController & operator=(const MediaController &);

   // Suppress default construction
   MediaController();

   ///
   int   myRollingSessionId;

   ///
   MediaCapability myMediaCapability;

   string local_ip;
   string localDevToBindTo;
};

}
}

#endif
