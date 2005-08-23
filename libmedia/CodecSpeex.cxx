
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

static const char* const CodecSpeex_cxx_Version =
    "$Id: CodecSpeex.cxx,v 1.3 2005/08/23 06:39:42 greear Exp $";

#include "global.h"
#include <cassert>

#include "codec/g711.h"
#include "CodecSpeex.hxx"
#include "speex.h"  // Found in the contrib directory
#include "cpLog.h"

using namespace Vocal::UA;

CodecSpeex::CodecSpeex(const CodecSpeex &src)
      : CodecAdaptor(src) {
   commonInit();
}

CodecSpeex::CodecSpeex(int priority)
      : CodecAdaptor(SPEEX, priority) 
{ 
   myClockRate = 8000;
   myAttrValueMap["ptime"] = "20";
   myCompressionRatio = 1;

   commonInit();
};

void CodecSpeex::commonInit() {

   myMediaType = AUDIO;
   myEncodingName = "SPEEX";

   // Construct Speex members.
   speex_bits_init(&dec_bits);
   speex_bits_init(&enc_bits);
   enc_state = speex_encoder_init(&speex_nb_mode);
   dec_state = speex_decoder_init(&speex_nb_mode);

   int frame_size = 0;
   speex_encoder_ctl(enc_state, SPEEX_GET_FRAME_SIZE, &frame_size);

   assert(frame_size == VSPEEX_FRAME_SIZE);

   /*Set the perceptual enhancement on*/
   int tmp = 1;
   speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &tmp);

    /*Set the quality to 8 (15 kbps)*/
   tmp = 8;
   speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &tmp);

   overflow_catcher = 0xd5d5d5d5;
}//commonInit


CodecSpeex::~CodecSpeex() {
   speex_bits_destroy(&enc_bits);
   speex_bits_destroy(&dec_bits);
   speex_encoder_destroy(enc_state); 
   speex_decoder_destroy(dec_state); 
   enc_state = NULL;
   dec_state = NULL;
}

int CodecSpeex::encode(char* data, int num_samples, int per_sample_size,
                       char* encBuf, int &encodedLength) {
    cpLog(LOG_DEBUG_STACK,"CodecSpeex::encode: %d, per_sample_size: %d encodedLength: %d",
          num_samples, per_sample_size, encodedLength);

    // First, convert the data from shorts into floats, as Speex likes
    // floats at this point.

    float fdata[num_samples];
    if (per_sample_size == 2) {
       short* shorts = (short*)(data);
       for (int i = 0; i<num_samples; i++) {
          fdata[i] = (float)(shorts[i]);
       }
    }
    else if (per_sample_size == 1) {
       for (int i = 0; i<num_samples; i++) {
          fdata[i] = (float)(data[i]);
       }
    }

    speex_bits_reset(&enc_bits);

    speex_encode(enc_state, fdata, &enc_bits);

    // Make sure we have plenty of room to work.
    assert(encodedLength >= (VSPEEX_FRAME_SIZE * 2));

    int nbBytes = speex_bits_write(&enc_bits, encBuf, encodedLength);

    encodedLength = nbBytes;

    return (0);
}
 
int CodecSpeex::decode(char* data, int length, char* decBuf, int decBufLen,
                       int& decodedSamples, int& decodedPerSampleSize) {
    cpLog(LOG_DEBUG_STACK,"CodecSpeex::decode: %d, decBufLen: %d",
          length, decBufLen);

    speex_bits_read_from(&dec_bits, data, length);

    speex_decode(dec_state, &dec_bits, decode_floats); 

    assert((unsigned)overflow_catcher == (unsigned)0xd5d5d5d5);

    decodedPerSampleSize = 2;

    assert(decBufLen > (VSPEEX_FRAME_SIZE * decodedPerSampleSize));

    short* decshorts = (short*)(decBuf);
    for (int i = 0; i<VSPEEX_FRAME_SIZE; i++) {
       decshorts[i] = (short)(decode_floats[i]);
    }

    decodedSamples = VSPEEX_FRAME_SIZE;

    return (0);
}


char* CodecSpeex::getSilenceFill(int& len) {
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
