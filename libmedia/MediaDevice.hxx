#ifndef MediaDevice_hxx
#define MediaDevice_hxx

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


static const char* const MediaDevice_hxx_Version = 
    "$Id: MediaDevice.hxx,v 1.3 2004/06/21 19:33:20 greear Exp $";

#include <stdio.h>
#include "Sptr.hxx"
#include "cpLog.h"
#include "Adaptor.hxx"
#include "Sptr.hxx"
#include "MediaSession.hxx"

namespace Vocal
{

namespace UA
{

class MediaSession;

/** Defines interface to plugin a Media device implementation into libmedia.
 */
class MediaDevice : public Adaptor {
public:
   ///
   void setBusy(bool val) { busy = val; };
   
   ///Assign device to a MediaSession identified by sessionId
   void assignedTo(int sessionId) { mySessionId = sessionId; };
   
   ///Returns true, if device is allocated to a session
   bool isBusy() const { return busy; };

   ///Returns the MediaSession Id, device is associated with
   int getSessionId() const { return mySessionId; };

   /// Virtual destructor
   virtual ~MediaDevice();

   ///
   string className() { return "MediaDevice"; }

   ///
   string description() { 
      char buf[56];
      snprintf(buf, 55, "%d,%d", myDeviceType, myMediaType); 
      return buf;
   }

   /**@name Virtual Interfaces
    * Following interfaces needs to be defined by the derived
    * device implementation.
    */
   //@{

   /**Called by the device itself when input data is ready to be shipped
    * @param data - input data bytes
    * @param length - Length of data in bytes
    * @param type - Codec type used to encode data
    * @param silence_pkt - The packet is silence, generated locally, probably because
    *        we never received the real packet.
    * @param codec - Some codecs need state, so pass this codec, which can decode
    *        the raw, if possible.  If null, a codec will be found, but it will
    *        not necessarily have the right state.  Speex currently needs this
    *        functionality.
    */
   virtual void processRaw(char* data, int length, VCodecType type,
                           Sptr<CodecAdaptor> codec, bool silence_pkt);

   //Returns 0 if successfully started
   // Specify the codec to use.
   virtual int start(VCodecType codec_type);

   ///Returns 0 if successfully stopped
   virtual int stop();
   ///Returns 0 if successfull
   virtual int suspend() = 0;
   ///Returns 0 if successfull
   virtual int resume() = 0;


protected:
   MediaDevice(VDeviceType dType, VMediaType mType) 
         : Adaptor(dType, mType), busy(false) , mySessionId (-1),
           mySession(0) {
   };

   ///
   bool busy;

   ///
   int mySessionId;

   /** Suppress copying
    */
   MediaDevice(const MediaDevice &);
   const MediaDevice & operator=(const MediaDevice &);

   ///
   Sptr<MediaSession> mySession;
};

}
}

#endif
