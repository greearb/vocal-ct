#ifndef UaFacade_hxx
#define UaFacade_hxx

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
#include <map>
#include "Sptr.hxx"
#include "StatusMsg.hxx"
#include "SipThread.hxx"
#include "RegistrationManager.hxx"
#include "SipTransceiver.hxx"
#include "MediaDevice.hxx"
#include "BaseFacade.hxx"

#ifdef USE_LANFORGE
class LFVoipThread;
#endif

using Vocal::SipTransceiver;
using Vocal::SipThread;
using Vocal::UA::RegistrationManager;

namespace Vocal
{

namespace UA
{

typedef enum
{
  CALL_MODE_UA = 0,
  CALL_MODE_ANNON,
  CALL_MODE_SPEECH,
  CALL_MODE_VMAIL,
  CALL_MODE_LANFORGE
} CallControlMode;


class UaCli;

/** UaFacade is the Facade wrapper, that wraps all the components needed
    for UA.
 */
class UaFacade : public BaseFacade {
public:

   static const char* VOIP_ID_STRING;

   ///
   static UaFacade& instance();

   ///Constructor to initialze UaFacade global instance.Should be called only once
#ifdef USE_LANFORGE
   static void initialize(const Data& applName, unsigned short  defaultSipPort,
                          bool nat, LFVoipThread* lfthread);
#else
   static void initialize(const Data& applName,
                          unsigned short  defaultSipPort, bool nat);
#endif

   static string getBareUserName();

   ///
   string className() { return "UaFacade"; }

   static Sptr<SipFrom> generateSipFrom();

   /**
      Frees the memory associated with singelton instance.
      gets register to atexit() function at the time of creation.
   */
   static void destroy();

   /** Virtual destructor
    */
   virtual ~UaFacade();

   ///
   Sptr<SipTransceiver> getSipTransceiver() { return mySipStack; };

   // Returns **OR CREATES*** device with specified ID.  If you just want to
   // get a handle to the last thing created, try peekMediaDevice.
   Sptr<MediaDevice> getMediaDevice(int id);

   Sptr<MediaDevice> peekMediaDevice() { return myMediaDevice; }   

   ///
   void releaseMediaDevice(int id);

   ///
   Sptr<RegistrationManager> getRegistrationManager() const { return myRegistrationManager; };

   // Received from the stack
   virtual void process(Sptr < SipProxyEvent > event);

   /** If !sending, assume we are receiving this */
   void postMsg(Sptr<SipMsg> sMsg, bool sending, const char* debug);

   ///
   void postMsg(const string& msg, const char* debug);

   void notifyCallEnded();

   // Add the event to our queue.  Will process it next tick()
   void queueEvent(Sptr <SipProxyEvent> event);

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);


   ///
   CallControlMode getMode() const { return myMode; }
      
   /**Set the mode of the user agent i.e. a phone, an announcement
    * server or interface to VoiceMail server.
    */
   //void setMode(CallControlMode mode) { myMode = mode; };

   bool isVadOn() { return vad_options.getVADOn(); }

   void updateRtpCache(const string& fname, list<RtpPldBuffer*> encoded_rtp);
   Sptr<CachedEncodedRtp> findRtpCache(const string& fname, VCodecType codec);

#ifdef USE_LANFORGE
   void setLFThread(LFVoipThread* lft);
   LFVoipThread* getLFThread();
#endif

private:

   /** Create the Instance of UaFacade, to wrap up the application wide
    *  sipStack .
    */
#ifdef USE_LANFORGE
   UaFacade(const Data& applName, uint16 tos, uint32 priority,
            const string& _localIp,
            unsigned short _localSipPort, const string& _natIp,
            int _transport, const NetworkAddress& proxyAddr,
            bool nat, VADOptions& vadOptions, LFVoipThread* lfthread);
   LFVoipThread* myLFThread;
#else
   UaFacade(const Data& applName, uint16 tos, uint32 priority,
            const string& _localIp,
            unsigned short _localSipPort, const string& _natIp,
            int _transport, const NetworkAddress& proxyAddr,
            bool nat, VADOptions& vadOptions, LFVoipThread* lfthread);
#endif

   ///
   static Sptr<UaFacade> myInstance;
   ///
   Sptr<SipThread> mySipThread;

   ///
   Sptr<SipTransceiver> mySipStack;

   ///
   Sptr<RegistrationManager> myRegistrationManager;

   // Queue incomming events, handle them all in the tick() method.
   priority_queue<Sptr<SipProxyEvent>,
                  vector< Sptr<SipProxyEvent> >,
                  SipProxyEventComparitor> eventQueue;

   // Last thing we created/returned.  Is contained in the myMediaDeviceMap
   Sptr<MediaDevice> myMediaDevice;

   ///
   Sptr<UaCli>  myUaCli;

   /** Suppress copying
    */
   UaFacade(const UaFacade &);
        
   /** Suppress copying
    */
   const UaFacade & operator=(const UaFacade &);
   
   ///
   CallControlMode myMode;

   ///Mapping from device Id to MediaDevice
   typedef map< int, Sptr<MediaDevice> > MediaDeviceMap;

   ///
   MediaDeviceMap myMediaDeviceMap;

   VADOptions vad_options;

   map<string, Sptr<CachedEncodedRtp> > rtp_cache_map;
};

}
}

#endif
