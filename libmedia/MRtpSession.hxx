#ifndef MRtpSession_HXX_
#define MRtpSession_HXX_

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

#include "NetworkRes.hxx"
#include "RtpSession.hxx"
#include "Sdp2Session.hxx"
#include "Adaptor.hxx"
#include "Def.hxx"

static const char* const MRtpSessionVersion =
    "$Id: MRtpSession.hxx,v 1.4 2004/06/22 02:24:04 greear Exp $";

#include "Sptr.hxx"

class RtpSession;

using Vocal::SDP::SdpSession;
using Vocal::UA::VSdpMode;

namespace Vocal
{


namespace UA
{

class CodecAdaptor;
class MediaSession;
///Sample class to abstract RTP data handling. Currently uses Vovida's RTP stack.
class MRtpSession  : public Adaptor {
public:
   /**Constructor
    * @param sessionId - MediaSesion Id, the RTP session is associated with.
    * @param local - Local network resource ( IP,PORT)
    * @param remote - Remote resource ( IP,PORT)
    * @param cAdp - Codec adaptor to use for encodning/decoding data
    * @param ssrc - (Optional), set the unique src id in RTP packets, if RTP stack
    *               does not take care by itself.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    * @param rtpPayloadType  For non-standard codecs, the number must be
    *               negotiated.  If so, pass something in here.  Currently
    *               the Speex codec requires it.
    */
   MRtpSession(int sessionId, NetworkRes& local, 
               const string& local_dev_to_bind_to,
               NetworkRes& remote ,
               Sptr<CodecAdaptor> cAdp, int rtpPayloadType,
               u_int32_t ssrc);

   virtual ~MRtpSession() {
      delete myRemoteAddress;
      delete rtpStack;
      delete myLocalAddress;
   }

   ///
   string className() { return "MRtpSession"; }  

   ///
   string description();

   ///
   const NetworkRes& getRemoteAddress() const { return *myRemoteAddress; };
   ///
   int getSessionId() const { return mySessionId; };

   /** Send the data over the wire
    */
   void sinkData(char* data, int length, VCodecType type,
                 Sptr<CodecAdaptor> codec, bool silence_pkt);

   ///Re-initialise connection to remote party based on the remote SDP
   void adopt(SdpSession& remoteSdp);

   ///Set the operation mode (VSDP_SND, VSDP_RECV,VSDP_SND_RECV)
   void setMode(VSdpMode mode);

   ///Called when DTMF is received as RTP Payload (RFC 2833)
   void recvDTMF(int event);

   RtpSession* getRtpSession() { return rtpStack; }

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);

protected:
   ///Process the data received over the wire
   void processRecv(RtpPacket& packet, const NetworkRes& sentBy);

   ///Interface to receive DTMF events from RTP stack
   class MDTMFInterface: public DTMFInterface {
   public:
      MDTMFInterface(MRtpSession* s): mySession(s) { };
      ///Do not know what to do with second argument
      virtual void sendDTMF( int a, int b ) { mySession->recvDTMF(a); };
      MRtpSession* mySession;
   };
   ///
   void processIncomingRTP(fd_set* fds);

   ///
   int mySessionId;

   ///
   NetworkRes* myRemoteAddress;

   /// RTP session variables
   RtpSession* rtpStack;

   ///
   MDTMFInterface* myDTMFInterface;

private:
   ///
   Sptr<MediaSession> mySession;
   ///
   NetworkRes* myLocalAddress;
   string localDevToBindTo;

   RtpPacket rtp_rx_packet;
};

 
}

}

#endif
