
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

static const char* const CodecG726_40_cxx_Version =
    "$Id: CodecG726_40.cxx,v 1.2 2005/08/23 00:27:54 greear Exp $";

#include "global.h"
#include <cassert>

#include "codec/g711.h"
#include "CodecG726_40.hxx"
#include <cpLog.h>

using namespace Vocal::UA;

CodecG726_40::CodecG726_40(int priority)
      : CodecAdaptor(G726_40, priority) {

   commonInit();
}

CodecG726_40::CodecG726_40(const CodecG726_40& src)
      : CodecAdaptor(src) {

   commonInit();
}

void CodecG726_40::commonInit() {
   myEncodingName = "g726-40";
   myClockRate = 8000;
   myMediaType = AUDIO;
   myAttrValueMap["ptime"] = "20";
   myCompressionRatio = 3;
   
   g726_init_state(&enc_state);
   g726_init_state(&dec_state);

};


int CodecG726_40::encode(char* data, int num_samples, int per_sample_size,
                         char* encBuf2, int& encodedLength) {
    cpLog(LOG_DEBUG_STACK,"CodecG726_40::encode: %d %d", num_samples, per_sample_size);
    if (encodedLength < num_samples) {
       cpLog(LOG_ERR, "Not enough space to put encoded data");
       return -1; 
    }

    unsigned char* encBuf = (unsigned char*)(encBuf2);

    int j = 0;
    if (per_sample_size == 2) {
       short* srcData = reinterpret_cast<short*>(data);
       unsigned int tmp;
       for (int i = 0; i < num_samples; i++) {
          encBuf[j] = g726_40_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state);
          tmp = g726_40_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state);
          encBuf[j] |= ((tmp & 0x7) << 5);
          j++;

          encBuf[j] = (tmp >> 3); //save 2 overflow bits
          encBuf[j] |= (g726_40_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state) << 2);
          tmp = g726_40_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state);
          encBuf[j] |= ((tmp & 0x1) << 7); // Need single bit
          j++;

          encBuf[j] = (tmp >> 1); // save 4 overflow bits
          tmp = g726_40_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state);
          encBuf[j] |= ((tmp & 0xF) << 4); // Need 4 bits
          j++;

          encBuf[j] = (tmp >> 4); // save 1 overflow bits
          encBuf[j] |= (g726_40_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state) << 1);
          tmp = g726_40_encoder(srcData[i++], AUDIO_ENCODING_LINEAR, &enc_state);
          encBuf[j] |= ((tmp & 0x3) << 6); // Need 2 bits
          j++;

          encBuf[j] = (tmp >> 2); // save 3 overflow bits
          encBuf[j] |= (g726_40_encoder(srcData[i], AUDIO_ENCODING_LINEAR, &enc_state) << 3);

          j++;
       }
    }
    else {
       assert("Can't handle samples that are other than 16-bit at this time.");
    }

    encodedLength = j;
    return (0);
}
 
int CodecG726_40::decode(char* data, int length, char* decBuf, int decBufLen,
                       int &decodedSamples, int& decodedPerSampleSize) {
    cpLog(LOG_DEBUG_STACK,"CodecG726_40::decode: %d", length);
    if (decBufLen < length * 6) {
       cpLog(LOG_ERR, "Not enough space to put decoded data");
       return -1; 
    }
    short * retData = reinterpret_cast<short*>(decBuf);
    unsigned char* srcData = (unsigned char*)(data);
    int j = 0;
    unsigned int tmp;
    for (int i = 0; i < length; i++) {
       int sample = srcData[i];
       retData[j++] = g726_40_decoder(sample & 0x1f, AUDIO_ENCODING_LINEAR, &dec_state);
       tmp = sample >> 5; // 3 bits left over
       i++;

       sample = srcData[i];
       tmp |= ((sample & 0x3) << 3); // Grab two bits
       retData[j++] = g726_40_decoder(tmp & 0x1f, AUDIO_ENCODING_LINEAR, &dec_state);
       retData[j++] = g726_40_decoder((sample >> 2) & 0x1f, AUDIO_ENCODING_LINEAR, &dec_state);
       tmp = sample >> 7; // 1 bit left over
       i++;

       sample = srcData[i];
       tmp |= ((sample & 0xf) << 1); // grab 4 bits
       retData[j++] = g726_40_decoder(tmp & 0x1f, AUDIO_ENCODING_LINEAR, &dec_state);
       tmp = sample >> 4; // 4 bits left over
       i++;

       sample = srcData[i];
       tmp |= ((sample & 0x1) << 4); // Grab 1 bit
       retData[j++] = g726_40_decoder(tmp & 0x1f, AUDIO_ENCODING_LINEAR, &dec_state);
       retData[j++] = g726_40_decoder((sample >> 1) & 0x1f, AUDIO_ENCODING_LINEAR, &dec_state);
       tmp = sample >> 6; // 2 bits left over
       i++;

       sample = srcData[i];
       tmp |= ((sample & 0x7) << 2); // grab 3 bits
       retData[j++] = g726_40_decoder(tmp & 0x1f, AUDIO_ENCODING_LINEAR, &dec_state);
       retData[j++] = g726_40_decoder((sample >> 3) & 0x1f, AUDIO_ENCODING_LINEAR, &dec_state);

    }
    decodedSamples = j;
    decodedPerSampleSize = 2;
    return (0);
}


char* CodecG726_40::getSilenceFill(int& len) {
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

      encode((char*)(tmp), samples, 2, silence, last_len);
      ms = last_ms;
   }
   len = last_len;
   return silence;
}//getSilenceFill
