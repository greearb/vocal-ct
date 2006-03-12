
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

#ifdef USE_VOICE_AGE

#include "global.h"
#include <cassert>

#include "codec/g711.h"
#include "CodecG729a.hxx"
#include "cpLog.h"

using namespace Vocal::UA;

CodecG729a::CodecG729a(const CodecG729a &src)
      : CodecAdaptor(src) {
   commonInit();
}

CodecG729a::CodecG729a(int priority)
      : CodecAdaptor(G729, priority) 
{ 
   myClockRate = 8000;
   myAttrValueMap["ptime"] = "20";
   myCompressionRatio = 1;

   commonInit();
};

void CodecG729a::commonInit() {

   myMediaType = AUDIO;
   myEncodingName = "G729";

   // Construct G729a members.
   codec c = (codec)(0);
   codecLibOpen(&dec_handle, c/* G729 */, DEC, 0);
   codecLibOpen(&enc_handle, c/* G729 */, ENC, 0);

   memset(&enc_option, 0, sizeof(enc_option));
   memset(&dec_option, 0, sizeof(dec_option));

   enc_option.mode = 0;
   enc_option.bfi = 0;
   enc_option.packing = 0;
   enc_option.DTX_mode = 0;

   enc_option.mode = 0;
   enc_option.bfi = 0;
   enc_option.packing = 0;
   enc_option.DTX_mode = 0;

   // get frame size from codec properties
   codecLibQuery((codec)(0)/* G729 */, ENC, &enc_properties, &enc_option, 0);
   inputFrameSize = enc_properties.codecInput.samplesPerFrame;

   codecLibQuery((codec)(0)/* G729 */, DEC, &dec_properties, &dec_option, 0);
   outputFrameSize = enc_properties.codecInput.samplesPerFrame;

   overflow_catcher = 0xd5d5d5d5;
}//commonInit


CodecG729a::~CodecG729a() {
   codecLibClose(&enc_handle, 0);
   codecLibClose(&dec_handle, 0);

   enc_handle = NULL;
   dec_handle = NULL;
}

int CodecG729a::encode(char* data, int num_samples, int per_sample_size,
                       char* encBuf, int &encodedLength) {
    cpLog(LOG_DEBUG_STACK,"CodecG729a::encode: %d, per_sample_size: %d encodedLength: %d",
          num_samples, per_sample_size, encodedLength);
    
    short* shorts = (short*)(data);
    if (per_sample_size == 1) {
       shorts = new short[num_samples];
       for (int i = 0; i<num_samples; i++) {
          shorts[i] = (short)(data[i]);
       }
    }

    Word32 ns = num_samples;
    Word32 el = encodedLength;
    int rv = codecLibEncode(&enc_handle, shorts, &ns, encBuf, &el, &enc_option, 0);

    if (rv < 0) {
       cpLog(LOG_ERR, "codecLibEncode rv < 0: %i, num_samples: %d  ns: %d  el: %d\n",
             rv, num_samples, ns, el);
    }
    
    num_samples = ns;
    encodedLength = el;

    return rv;
}


int CodecG729a::decode(char* data, int length, char* decBuf, int decBufLen,
                       int& decodedSamples, int& decodedPerSampleSize,
                       bool is_silence) {
   cpLog(LOG_DEBUG_STACK, "CodecG729a::decode: %d, decBufLen: %d  silence: %d",
         length, decBufLen, is_silence);

   Word32 l = length;
   Word32 dbl = decBufLen;
   // Set appropriate flags in the dec_option structure
   // TODO: This doesn't actually work with the current codec. --Ben
   if (is_silence) {
      cpLog(LOG_ERR, "CodecG729a decoding silence...\n");
      dec_option.bfi = 1;

      // Per VoiceAge support:  Have to lie and tell it that we
      // actually have some length...
      l = 20;
   }
   else {
      dec_option.bfi = 0;
   }

   int rv = codecLibDecode(&dec_handle, data, &l, ((short*)(decBuf)),
                           &dbl, &dec_option, 0);
   
   if (rv < 0) {
      cpLog(LOG_ERR, "codecLibDecode rv < 0: %i, length: %d decBufLen: %d l: %d dbl: %d\n",
            rv, length, decBufLen, l, dbl);
   }
   else {
      cpLog(LOG_DEBUG, "codecLibDecode OK, rv: %i, length: %d decBufLen: %d l: %d dbl: %d\n",
            rv, length, decBufLen, l, dbl);
   }
   
   length = l;
   decBufLen = dbl;
   
   assert((unsigned)overflow_catcher == (unsigned)0xd5d5d5d5);
   
   decodedPerSampleSize = 2;
   
   decodedSamples = decBufLen;// / 2;
   
   //cpLog(LOG_ERR, "g729a decode, samples: %i  perSampleSize: %i, rv: %i\n",
   //      decodedSamples, decodedPerSampleSize, rv);
   
   return rv;
}

char* CodecG729a::getSilenceFill(int& len) {
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

      //cpLog(LOG_ERR, "Encoding for silence fill, samples: %d\n", samples);
      last_len = 2048; //sizeof silence
      encode((char*)(tmp), samples, 2, silence, last_len);
      //cpLog(LOG_ERR, "Done encoding...\n");
      ms = last_ms;
   }
   len = last_len;
   return silence;
}//getSilenceFill


#endif
