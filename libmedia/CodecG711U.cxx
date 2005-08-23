
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

static const char* const CodecG711U_cxx_Version =
    "$Id: CodecG711U.cxx,v 1.2 2005/08/23 00:27:54 greear Exp $";

#include "global.h"
#include <cassert>

#include "CodecG711U.hxx"
#include "codec/g711.h"
#include "cpLog.h"

using namespace Vocal::UA;

int CodecG711U::encode(char* data, int num_samples, int per_sample_size,
                       char* encBuf, int& encodedLength) {
    cpLog(LOG_DEBUG_STACK,"CodecG711U::encode: %d %d", num_samples, per_sample_size);
    if (encodedLength < num_samples) {
       cpLog(LOG_ERR, "Not enough space to put encoded data");
       return -1; 
    }

    // NOTE:  Not sure how this would handle other sample sizes,
    //  could add support as needed.
    assert(per_sample_size == 2);

    short* srcData = reinterpret_cast<short*>(data);
    for(int i = 0; i < num_samples; i++)
    {
        encBuf[i] = linear2ulaw(srcData[i]);
    }
    encodedLength = num_samples;
    return (0);
}
 
int CodecG711U::decode(char* data, int length, char* decBuf, int decBufLen,
                       int &decodedSamples, int& decodedPerSampleSize) {
    cpLog(LOG_DEBUG_STACK,"CodecG711U::decode: %d", length);
    if(decBufLen < length * 2)
    {
       cpLog(LOG_ERR, "Not enough space to put decoded data");
       return -1; 
    }
    short * retData = reinterpret_cast<short*>(decBuf);
    char* srcData = (data);
    for(int i = 0; i < length; i++)
    {
        retData[i] = ulaw2linear(srcData[i]);
    }
    decodedSamples = length;
    decodedPerSampleSize = 2;
    return (0);
}


char* CodecG711U::getSilenceFill(int& len) {
   static char silence[2048];
   int ms = atoi(myAttrValueMap["ptime"].c_str());
   int samples = getClockRate() / (1000 / ms);
   if (samples > 2048) {
      cpLog(LOG_ERR, "ERROR:  too many samples, clockRate: %i  ptime: %i\n",
            getClockRate(), ms);
      samples = 2048;
   }

   memset(silence, 0xFF, samples);
   len = samples;
   return silence;
}//getSilenceFill
