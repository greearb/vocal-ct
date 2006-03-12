#ifndef VmcpDevice_HXX
#define VmcpDevice_HXX

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
static const char* const VmcpDevice_hxx_Version =
    "$Id: VmcpDevice.hxx,v 1.5 2006/03/12 07:41:28 greear Exp $";

//#include "Vmcp.h"
#include "PlayQueue.h"
#include "Recorder.h"
#include "DeviceEvent.hxx"
#include "MediaDevice.hxx"

class Vmcp;

namespace Vocal
{


namespace UA
{
    

/**
 * This class is used as a SIP talking front end for voice mail server.
 * It talks Voice Mail Control Protocol with voicemail server.
 * It is used to play/record messages controlled by the VmServer.
 * It also handles the DTMF digits.
 */
class VmcpDevice: public MediaDevice {
public:
   ///
   VmcpDevice(int id);

   ///
   virtual ~VmcpDevice(void);

   /** process all events received from VmServer. returns 0 if successful,
    *  otherwise returns an errorcode.
    */
   int process (fd_set* fd);

   /** start the device for audio processing
    * returns 0 if successful, errorcode otherwise
    */
   int start (VCodecType codec_type);

   /** stops the device
    * returns 0 if successful, errorcode otherwise
    */
   int stop (void);

   ///Set the call-info for the call received
   void provideCallInfo(Data sCallerId, 
                        Data sCalleeId, Data sForwardReason);

   ///Sinks the received data to the recorder
   void sinkData(char* data, int length, VCodecType type,
                 Sptr<CodecAdaptor> codec, bool silence_pkt,
                 RtpPayloadCache* payload_cache);


   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);


private:
   int  resume() { return 0; }
   int  suspend() { return 0; }

   ///Play/record messages
   void processAudio ();

   /** add VmServer connection socket to the fd set
    * returns 0 if successful, errorcode otherwise
    */
   int addToFdSet (fd_set* fd, int& maxdesc);

   ///
   int connectToVmServer();
   ///
   void reportEvent( DeviceEventType eventType );

   bool hasPlayed; 

   // VoiceMail Control Protocol stack
   Vmcp* myVmStack;

   int myId;

   bool audioActive;
   bool hookStateOffhook;
   bool serverAvailable;

   struct timeval prevRtpPacketTime;
   Data CallerId;
   Data CalleeId;
   Data ForwardReason;
   int NumberOfForwards;
   int fwdFlag;

   // Vmcp socket
   int ss; 	

   ///Player
   PlayQueue player;
   ///recorder
   Recorder recorder;

   uint64 nextTime;
   uint64 nextRecTime;
   uint64 networkPktSize;
};
 
}

}


#endif
