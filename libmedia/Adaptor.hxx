#ifndef Adaptor_hxx
#define Adaptor_hxx

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


static const char* const Adaptor_hxx_Version = 
    "$Id: Adaptor.hxx,v 1.7 2006/02/24 22:27:52 greear Exp $";

#include "global.h"
#include <string>
#include <list>
#include "NetworkRes.hxx"
#include "CodecAdaptor.hxx"
#include "Def.hxx"
#include <BugCatcher.hxx>
#include <misc.hxx>

namespace Vocal
{

namespace UA
{

   struct RtpPldBufferStorage {
      int flags; //None defined at this moment..must be zero.
      int len;
      int samples;
      VCodecType ct;
   };

   /** Holds Encoded RTP data.  Used when we are re-transmitting
    * same file over and over:  No need to re-encode each time.
    */
   class RtpPldBuffer : public BugCatcher {
   protected:
      RtpPldBufferStorage s;
      char* buf;

   public:
      RtpPldBuffer(const char* buffer, int length, int sampleCount,
                   VCodecType codec_type) {
         s.flags = 0;
         s.len = length;
         s.samples = sampleCount;
         s.ct = codec_type;
         buf = new char[length];
         memcpy(buf, buffer, length);
      }

      RtpPldBuffer(const RtpPldBuffer& rhs) {
         s = rhs.s;
         buf = new char[s.len];
         memcpy(buf, rhs.buf, s.len);
      }

      virtual ~RtpPldBuffer() {
         if (buf) {
            delete[] buf;
         }
      }

      const char* getBuffer() const { return buf; }
      VCodecType getCodecType() const { return s.ct; }
      int getLength() const { return s.len; }
      int getSampleCount() const { return s.samples; }
      RtpPldBufferStorage getStorage() { return s; }
   };

   /** Interface class, used for callback. */
   class RtpPayloadCache {
   public:
      virtual ~RtpPayloadCache() { }
      virtual void addRtpPldBuffer(const char* msg, int len, int samples,
                                   VCodecType t) = 0;
   };

   class CachedEncodedRtp : public BugCatcher {
   protected:
      string key; //file_name.codec.VAD.cache
      list<RtpPldBuffer*> buffers;

   public:
      CachedEncodedRtp(const string& _key, const list<RtpPldBuffer*>& _buffers);
      CachedEncodedRtp(const string& _key);
      ~CachedEncodedRtp();

      void addBuffer(RtpPldBuffer* b);
      const list<RtpPldBuffer*>& getBufferList() const { return buffers; }
      const string& getKey() const { return key; }
   };

/** Abstract class,defines interface to plug a Media Source/Sink. A Media Handling device 
    implementation must derive from this interface in order to plug into the 
    Media Framework.
 */
class Adaptor : public BugCatcher {
public:
   /// Virtual destructor
   virtual ~Adaptor() { }

   ///
   VDeviceType getDeviceType() const { return myDeviceType; };
   ///
   VMediaType getMediaType() const { return myMediaType; };
   
   ///
   Sptr<CodecAdaptor> getCodec() { return myCodec; };
   
   //The rate at which the Adaptor is expecting data, default is 20ms
   // Units are miliseconds.
   virtual int getDataRate() const { return myDataRate; };

   ///
   const string& getInstanceName() const { return _instanceName; }
   ///
   const string& getDescription() const {  return _description; }

   /** Consume the data
    * @param silence_pkt - The packet is silence, generated locally, probably because
    *        we never received the real packet.
    * @param codec - Some codecs need state, so pass this codec, which can decode
    *        the raw, if possible.  If null, a codec will be found, but it will
    *        not necessarily have the right state.  Speex currently needs this
    *        functionality.
    * 
    */
   virtual void sinkData(char* data, int length, VCodecType type,
                         Sptr<CodecAdaptor> codec, bool silence_pkt,
                         RtpPayloadCache* payload_cache) = 0;

   // Do work, maybe can read or write now, check the file descriptors.
   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now) = 0;

   // Set max timeout and/or set file descriptors we are interested in.
   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now) = 0;



protected:
   /**Constructor to create a Adaptor, the priority indicate
    * the pref when multiple codecs can be used, a 0 priority means
    * equal preferrence
    */
   Adaptor(const char* instanceName, const char* desc,
           VDeviceType dType, VMediaType mType)
         : myDeviceType(dType), myMediaType(mType), myDataRate(20) {
      _instanceName = instanceName;
      _description = desc;
   };

   ///
   VDeviceType myDeviceType; 
   ///
   VMediaType  myMediaType;
   
   ///
   Sptr<CodecAdaptor> myCodec;
   ///
   int   myDataRate;

   /** Suppress copying
    */
   Adaptor(const Adaptor &);
   const Adaptor & operator=(const Adaptor &);

   string _instanceName;
   string _description;

};

}
}

#endif
