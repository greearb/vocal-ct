#ifndef MediaSession_hxx
#define MediaSession_hxx

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


static const char* const MediaSession_hxx_Version = 
    "$Id: MediaSession.hxx,v 1.1 2004/05/01 04:15:16 greear Exp $";

#include "global.h"
#include "Sptr.hxx"
#include "NetworkRes.hxx"
#include "MediaDevice.hxx"
#include "Sdp2Session.hxx"
#include "VThread.hxx"
#include "Def.hxx"
#include "MRtpSession.hxx"

using Vocal::SDP::SdpSession;

namespace Vocal
{

namespace UA
{


class Adaptor;

/** Class identified by a unique session-id, implements a Media session between
  * two parties. The participants can be a physical Media device and/or a RTP
  * session between two parties. Each participants plays both the role of a source
  * and a sink of data. For example, in case of a User Agent, the 
  * participants would we a sound-card device and an RTP session to send and 
  * receive RTP packets to/from the other side. 
  * 
 */
class MediaSession : public BugCatcher
{
    public:
      /** Constructor- creates a MediaSession object with a given session ID and
        * reserve the local network resource ( IP and PORT) to use for RTP.
        * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
        */
      MediaSession(int sessionId,
                   Sptr<NetworkRes> localRes,
                   const string& local_dev_to_bind_to);


      ///Add a media device to the session
      void addToSession( Sptr<MediaDevice> mDevice);

      /**Based on the Local and remote SDP, creates an RTP session and
       * and associates the RTP session as one of the participants.
       */
      void addToSession( SdpSession& localSdp, SdpSession& remoteSdp);

      ///
      string className() { return "MediaSession"; }

      /** Virtual destructor
       */
      virtual ~MediaSession();

      ///Start the session, also indicate the session mode, default is send-recv
      // Mode used to default to VSDP_SND_RECV
      void startSession(VSdpMode mode);

      ///Teardown the entire session, returns 1 on success
      int tearDown();
      ///Suspend a session, can be resumed by calling resume()
      void suspend();
      ///Resume a suspended session
      void resume(SdpSession& remoteSdp);

      /**Process data from a Media device or RTP stream
        * @param data - The raw data received from device or via RTP
        * @param len  - Length of the raw data
        * @param cType - Codec type used to encode data
        * @param adp - The Adaptor(souncard, RTP etc)that generated the data
        * @param codec - Some codecs need state, so pass this codec, which can decode
        *        the raw, if possible.  If null, a codec will be found, but it will
        *        not necessarily have the right state.  Speex currently needs this
        *        functionality.
        * @param silence_pkt - The packet is silence, generated locally, probably because
        *        we never received the real packet.
        */
      void processRaw (char *data, int len, VCodecType cType, Sptr<CodecAdaptor> codec,
                       Adaptor* adp, bool silence_pkt);

      /** Allow the receiver to throttle based on the current size of the
       * jitter buffer.  This timeout will be passed to select.
       */
      virtual struct timeval getPreferredTimeout(unsigned int jitter_pkts_in_queue,
                                                 unsigned int queue_max);

      /**Based on the given mode ( VSDP_SND, VSDP_RECV,VSDP_SND_RECV), generate
        *local SDP data. Used when initiating a call.
        */
      SdpSession getSdp(VSdpMode mode);

      /**Based on the remote SDP, negotiate SDP and generate the Local SDP.
       * Uses MediaCapability to query the local codec capabilities.
       * Used when accepting a call into session.
       */
      SdpSession getSdp(SdpSession& remoteSdp);

   //MRtpSession* getRtpSession() { return myRtpSession; }

      virtual int getSessionId() const { return mySessionId; }

      static int getInstanceCount() { return _cnt; }

    private:
      static int _cnt;

      ///
      int mySessionId;
      ///
      MRtpSession*  myRtpSession;

      ///
      Sptr<NetworkRes>         myLocalRes;
      ///
      Sptr<MediaDevice>        myMediaDevice;

      //Suppress copying
      MediaSession(const MediaSession &);
        
      // Suppress copying
      const MediaSession & operator=(const MediaSession &);

      ///
      bool myShutdownFlg;

      ///
      u_int32_t mySSRC;
      string localDevToBindTo;
};


}
}

#endif
