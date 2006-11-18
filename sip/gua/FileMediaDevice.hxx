#ifndef FileMediaDevice_hxx
#define FileMediaDevice_hxx

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

#include "RtpSession.hxx"
#include "UaHardwareEvent.hxx"
#include "PlayQueue.h"
#include "MediaDevice.hxx"

namespace Vocal
{

namespace UA
{    

/**
 * This class is used as a interface to play from a media
 * file.
 */
class FileMediaDevice : public MediaDevice {
public:
   ///Creates a FileMediaDevice object identified by id.
   FileMediaDevice(int id);
   
   ///
   virtual ~FileMediaDevice(void);

   ///set the file name to play
   void setFileToPlay(const string fName) { myFileToPlay = fName; };

   /** start audio channel on device
    *  returns 0 if successful, errorcode otherwise
    */
   int start(VCodecType codec_type);

   /** stops audio channel on device
    *  returns 0 if successful, errorcode otherwise
    */
   int stop(const char* reason);

   ///Returns the device ID
   int getMyId() const { return myId; }

   /** Sinks received data to a file (recording). The function is not
    *  fully implemented.
    */
   void sinkData(char* data, int length, VCodecType type,
                 Sptr<CodecAdaptor> codec, bool silence_pkt,
                 RtpPayloadCache* payload_cache);

   // Do work, maybe can read or write now, check the file descriptors.
   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   // Set max timeout and/or set file descriptors we are interested in.
   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);


private:

   /** Read the media file and play.
    */
   void processAudio ();


   int  resume(VCodecType codec_type) { return 0; }
   int  suspend() { return 0; }

   int audioActive;

   ///
   bool hasPlayed; 

   ///Player
   PlayQueue player;
   ///
   uint64 nextTime;

   ///File to play
   string myFileToPlay;
   ///
   uint32 networkPktSize;
   ///
   int myId;
};
 
}

}


#endif
