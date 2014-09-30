
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

#include <stdlib.h>
#include "global.h"
#include <cassert>

#include "codec/g711.h"
#include "CodecG726_16.hxx"
#include <cpLog.h>

using namespace Vocal::UA;

CodecG726_16::CodecG726_16(int priority)
      : CodecAdaptor(G726_16, priority) {

   commonInit();
}

CodecG726_16::CodecG726_16(const CodecG726_16& src)
      : CodecAdaptor(src) {

   commonInit();
}

void CodecG726_16::commonInit() {
   myEncodingName = "g726-16";
   myClockRate = 8000;
   myMediaType = AUDIO;
   myAttrValueMap["ptime"] = "20";
   myCompressionRatio = 4;
   
   g726_init_state(&enc_state);
   g726_init_state(&dec_state);

};


int CodecG726_16::encode(char* data, int num_samples, int per_sample_size,
                       char* encBuf, int& encodedLength) {
    cpLog(LOG_DEBUG_STACK,"CodecG726_16::encode: %d %d", num_samples, per_sample_size);
    if (encodedLength < num_samples) {
       cpLog(LOG_ERR, "Not enough space to put encoded data");
       return -1; 
    }

    int j = 0;
    if (per_sample_size == 2) {
       short* srcData = reinterpret_cast<short*>(data);
       for (int i = 0; i < num_samples; i++) {
          encBuf[j] = g726_16_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state);

          encBuf[j] |= (g726_16_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state) << 2);
          encBuf[j] |= (g726_16_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state) << 4);
          encBuf[j] |= (g726_16_encoder(srcData[i], AUDIO_ENCODING_LINEAR, &enc_state) << 6);
          j++;
       }
    }
    else {
       assert("Can't handle samples that are other than 16-bit at this time.");
    }

    encodedLength = j;
    return (0);
}
 
int CodecG726_16::decode(char* data, int length, char* decBuf, int decBufLen,
                         int &decodedSamples, int& decodedPerSampleSize,
                         bool is_silence) {
   cpLog(LOG_DEBUG_STACK,"CodecG726_16::decode: %d", length);
   
   if (is_silence) {
      cpLog(LOG_ERR, "ERROR:  G726_16 Codec does not support silence decode.\n");
      return -1;
   }

   if (decBufLen < length * 8) {
      cpLog(LOG_ERR, "Not enough space to put decoded data");
      return -1; 
   }
   short * retData = reinterpret_cast<short*>(decBuf);
   char* srcData = (data);
   int j = 0;
   for (int i = 0; i < length; i++) {
      int sample = srcData[i];
      retData[j++] = g726_16_decoder(sample & 0x3, AUDIO_ENCODING_LINEAR, &dec_state);
      retData[j++] = g726_16_decoder((sample >> 2) & 0x3, AUDIO_ENCODING_LINEAR, &dec_state);
      retData[j++] = g726_16_decoder((sample >> 4) & 0x3, AUDIO_ENCODING_LINEAR, &dec_state);
      retData[j++] = g726_16_decoder((sample >> 6) & 0x3, AUDIO_ENCODING_LINEAR, &dec_state);
   }
   decodedSamples = j;
   decodedPerSampleSize = 2;
   return (0);
}


char* CodecG726_16::getSilenceFill(int& len) {
   static char silence[2048];
   static int last_ms = 0;
   static int last_len = 0;
   int ms = atoi(myAttrValueMap["ptime"].c_str());
   if (ms != last_ms) {
      int samples = getClockRate() / (1000 / ms);
      if (samples > 2048) {
         cpLog(LOG_ERR, "ERROR:  too many samples, clockRate: %i  ptime: %i\n",
               getClockRate(), ms);
         samples = 2048;
      }
      short tmp[samples];
      for (int i = 0; i<samples; i++) {
         tmp[i] = ulaw2linear(0xFF); // This will be a silence word in linear encoding
      }

      last_len = 2048; //sizeof silence
      encode((char*)(tmp), samples, 2, silence, last_len);
      ms = last_ms;
   }
   len = last_len;
   return silence;
}//getSilenceFill

#if 0
char* CodecG726_16::getSilenceFill(int& len) {
   // Coppied from rtpCodec.cxx, no idea how they were derived. --Ben
   static char[] sample_20_ms = {
      0x03, 0x33, 0xC3, 0x3C, 0x3C, 0xCF, 0x3C, 0x03,
      0xCF, 0x00, 0xF0, 0xF3, 0x3F, 0xFF, 0xC0, 0xCF,
      0xF0, 0xF3, 0x00, 0xFC, 0xFF, 0x0C, 0x3C, 0x00,
      0x0C, 0xFF, 0xCC, 0x03, 0x0C, 0xC0, 0x0C, 0xF0,
      0xC3, 0x30, 0x30, 0x00, 0xCC, 0xF0, 0xF3, 0x0C };

   static char[] sample_100_ms = {
      0x30, 0xCF, 0xCF, 0xC0, 0x00, 0x00, 0x0C, 0x00,
      0x0C, 0xCC, 0x30, 0xCC, 0xC3, 0xFC, 0x3C, 0x00,
      0xCF, 0x00, 0x0C, 0x03, 0x03, 0x03, 0xFC, 0x3F,
      0xFF, 0x33, 0xF3, 0x30, 0xFF, 0x30, 0xFF, 0xF3,
      0xF3, 0xF3, 0xFC, 0xFC, 0xF0, 0xC0, 0xC3, 0x00,

      0x00, 0x00, 0xCC, 0x0C, 0xC3, 0x0F, 0x0C, 0x0F,
      0x00, 0x30, 0xC0, 0xF0, 0x30, 0xCC, 0x30, 0xF0,
      0x0C, 0xFF, 0xC0, 0x3F, 0xF3, 0xFC, 0x33, 0xFF,
      0x3F, 0xCF, 0x3F, 0x3F, 0x33, 0x3C, 0xC0, 0xF3,
      0x03, 0xFC, 0x00, 0xC0, 0xCC, 0xF3, 0xCC, 0xC0,

      0x00, 0x00, 0x03, 0x00, 0x03, 0x0F, 0x00, 0x30,
      0x30, 0xC3, 0xF3, 0xF3, 0x0F, 0xF3, 0xCF, 0xCF,
      0x3F, 0xFF, 0x3F, 0xFC, 0xFC, 0xF3, 0xCF, 0xF3,
      0x03, 0xCC, 0x00, 0x30, 0x0C, 0x0C, 0xF0, 0x00,
      0xCC, 0xC3, 0x00, 0xCC, 0x00, 0x33, 0x00, 0x3C,

      0x30, 0x3F, 0xC0, 0xFF, 0xFC, 0xCF, 0x3C, 0xFF,
      0x33, 0xF0, 0x3F, 0x3C, 0xFF, 0x3F, 0x3F, 0xCF,
      0x3C, 0xFF, 0x30, 0x3C, 0x00, 0x0C, 0xC0, 0x00,
      0xC3, 0x00, 0xCC, 0xC0, 0x0C, 0x0C, 0x30, 0x30,
      0x30, 0xCF, 0xC3, 0x3C, 0xF0, 0xFF, 0xFF, 0x33,

      0x3F, 0x00, 0xF3, 0xF0, 0xFC, 0x3F, 0xC3, 0x0F,
      0xF3, 0x3F, 0x33, 0x0F, 0xFC, 0x30, 0xC0, 0xFC,
      0x0C, 0xF3, 0x00, 0x33, 0x00, 0x00, 0x00, 0xC3,
      0x30, 0x03, 0xC3, 0x0C, 0xCF, 0xC3, 0xF3, 0xF0,
      0xCF, 0xC0, 0xC3, 0x0C, 0xC0, 0xFF, 0xC0, 0x30 };

   int ms = atoi(myAttrValueMap["ptime"]);
   if (ms == 20) {
      len = 40;
      return sample_20_ms;
   }
   else if (ms = 100) {
      len = 200;
      return sample_100_ms;
   }
   else {
      cpLog(LOG_ERR, "ERROR:  unsupported ptime: %i\n", ms);
      len = 40;
      return sample_20_ms;
   }
}//getSilenceFill
#endif
