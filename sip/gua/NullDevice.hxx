#ifndef NULLDEVICE_HXX
#define NULLDEVICE_HXX

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
static const char* const NullDeviceVersion =
    "$Id: NullDevice.hxx,v 1.6 2006/05/27 00:02:01 greear Exp $";

#include "MediaDevice.hxx"
#include <iostream>

using Vocal::SipProxyEvent;

namespace Vocal
{

namespace UA
{

/**
 * Impelments a dummy soundcard device that does not require
 * any hardware audo device. The device is useful when just testing
 * the call signalling.
 */

class NullDevice : public MediaDevice {
public:
   ///Just create a dummy soundcard device
   NullDevice():
         MediaDevice("NullDevice", SOUNDCARD, AUDIO)
      { };
   
   ///
   void processAudio() { };
 
   ///Returns 0 if successfully started
   int start(VCodecType codec_type) {
      nextTime = vgetCurMs();
      cerr << "Connected" << endl;
      return 0;
   }

   ///Returns 0 if successfully stopped
   int stop(const char* reason) {
      MediaDevice::stop(reason);
      cerr << "NullDevice stopped, reason: " << reason << endl;
      return 0;
   }

   ///Returns 0 if successfully suspended
   int suspend() { return 0; }
   ///Returns 0 if successfully resumed
   int resume() { return 0; };

   ///
   void sinkData(char* data, int length, VCodecType type,
                 Sptr<CodecAdaptor> codec, bool silence_pkt,
                 RtpPayloadCache* payload_cache) {
      //vusleep(20000);
   };

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now) {
      if (now <= nextTime) {
         // Do nothing, but do it 50 times a second!
         nextTime += 20;
      }
   }

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now) {
      if (nextTime > now) {
         timeout = min(nextTime - now, timeout);
      }
      else {
         timeout = 0;
      }
      return 0;
   }


private:
   uint64 nextTime;

};
 
}
}



#endif
