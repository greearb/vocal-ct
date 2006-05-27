
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
static const char* const FileMediaDevice_cxx_Version = 
    "$Id: FileMediaDevice.cxx,v 1.8 2006/05/27 00:02:01 greear Exp $";



#include "global.h"
#include <cassert>
#include "FileMediaDevice.hxx"
#include "UaFacade.hxx"
#include "cpLog.h"
#include "UaConfiguration.hxx"
                             
using namespace Vocal;
using namespace Vocal::UA;

static const string WaveFilePath="/usr/local/vocal/etc/";


//***************************************************************************
// FileMediaDevice::FileMediaDevice
// description:  Constructor. Initialize the hookstate.
//               Resets the device to get it ready.
//***************************************************************************

FileMediaDevice::FileMediaDevice(int id)
    : MediaDevice("FileMediaDevice", WAVE, AUDIO), hasPlayed(false), myId(id)
{
    cpLog(LOG_DEBUG, "Opened wave device with id:%d", id);
    audioActive = false;
    myFileToPlay="default.wav";

}  // end FileMediaDevice::FileMediaDevice()


//***************************************************************************
// FileMediaDevice::~FileMediaDevice
// description:  Destructor
//***************************************************************************


FileMediaDevice::~FileMediaDevice(void)
{
    player.stop();
} // end FileMediaDevice::~FileMediaDevice()



// Do work, maybe can read or write now, check the file descriptors.
void FileMediaDevice::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                           uint64 now) {
   if (audioActive) {
      if (now <= nextTime) {
         nextTime += networkPktSize;
         processAudio();
      }
   }
}

// Set max timeout and/or set file descriptors we are interested in.
int FileMediaDevice::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                            int& maxdesc, uint64& timeout, uint64 now) {
   if (nextTime > now) {
      timeout = min(nextTime - now, timeout);
   }
   else {
      timeout = 0;
   }
   return 0;
}


//***************************************************************************
// FileMediaDevice::processRTP
// description:  main processing loop for RTP
//***************************************************************************

void
FileMediaDevice::processAudio () {
   unsigned char buffer[networkPktSize*8];
   if ( !player.getData(buffer, networkPktSize*8) ) {
      Sptr < UaHardwareEvent > signal = new UaHardwareEvent( myId );
      signal->type = HardwareAudioType;
      signal->request.type = AudioStop;
      UaFacade::instance().queueEvent(signal.getPtr());
   }
   Sptr<CodecAdaptor> nll;
   processRaw((char*)buffer, networkPktSize*8, G711U, nll, false, NULL);
}

//***************************************************************************
// FileMediaDevice::audioStart
//
// description:  creates a new rtp session and also allocates memory for
//               incoming and outgoing rtp packet. 
//***************************************************************************

int
FileMediaDevice::start(VCodecType codec_type) {
   if ( audioActive ) {
      cpLog(LOG_ERR, "Audio channel is already active. Ignoring");
      return 0;
   }

   MediaDevice::start(codec_type);

   audioActive = true;
   // mark audio as active
   cpLog(LOG_DEBUG, "setting audio active");

   // allocate RTP packet spaces
   networkPktSize = atoi(UaConfiguration::instance().getValue(NetworkRtpRateTag).c_str());

   nextTime = vgetCurMs();
   hasPlayed = false;
   string aName(WaveFilePath);
   aName += myFileToPlay;
   player.add(aName);
   if (!player.start()) {
      cpLog(LOG_ERR, "Failed to start playing of sound file");
      Sptr < UaHardwareEvent > signal =
         new UaHardwareEvent( myId );
      signal->type = HardwareAudioType;
      signal->request.type = AudioStop;
      UaFacade::instance().queueEvent( signal.getPtr() );
   };

   return 0;
}

//***************************************************************************
// FileMediaDevice::audioStop
//
// description:  tears down audio.  cleans up by delete objects created when
//               audioStart was called...  inBuffer, outBuffer, audioStack,
//               rtp packets.
//***************************************************************************

int FileMediaDevice::stop(const char* reason) {
   if (!audioActive) {
      cpLog(LOG_DEBUG, "stop: No audio active, ignored the request");
      return 1;
   }

   MediaDevice::stop(reason );

   // mark audio as deactivated.
   cpLog(LOG_DEBUG, "Audio Stop received, reason: %s", reason);
   audioActive = false;

   player.stop();

   cpLog(LOG_DEBUG,"End of session");  
   return 0;
} 


void
FileMediaDevice::sinkData(char* data, int length, VCodecType type,
                          Sptr<CodecAdaptor> codec, bool silence_pkt,
                          RtpPayloadCache* payload_cache) {
   cpLog(LOG_DEBUG_STACK, "Sink Data: length %d", length);
   if(type == DTMF_TONE) {
      char digit;
      memcpy(&digit, data, 1);
      cpLog( LOG_DEBUG, "***  DTMF %c  ***", digit );
   }

   // TODO:  Implement something here!!
}
