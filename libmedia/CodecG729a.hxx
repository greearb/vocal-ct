#ifndef CodecG729a_hxx
#define CodecG729a_hxx

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

static const char* const CodecG729a_hxx_Version = 
    "$Id: CodecG729a.hxx,v 1.2 2004/06/20 22:28:00 greear Exp $";

#ifdef USE_VOICE_AGE

#include "CodecAdaptor.hxx"
#include <codecLib_if.h> // Voice-age specific file.


namespace Vocal
{

namespace UA
{

class CodecG729a  : public CodecAdaptor
{
public:

   CodecG729a(const CodecG729a &);

   /**Constructor to create a CodecAdaptor, the priority indicate
    * the pref when multiple codecs can be used, a 0 priority means
    * equal preferrence
    */
   CodecG729a(int priority=0);

   //From codec type  to raw data (PCMU)
   virtual int decode(char* data, int length, char* decBuf, int decBufLen,
                      int &decodedSamples, int& decodedPerSampleSize);

   //from raw data (PCMU) to codec type 
   virtual int encode(char* data, int num_samples, int per_sample_size,
                      char* encBuf, int& encodedLength);

   /// Virtual destructor
   virtual ~CodecG729a();

   ///
   virtual string className() { return "CodecG729a"; }

protected:

   void commonInit();

   void *enc_handle;
   void *dec_handle;
   codecOption enc_option;
   codecOption dec_option;
   codecProperties enc_properties;
   codecProperties dec_properties;

   int inputFrameSize;
   int outputFrameSize;

   int overflow_catcher;

private:

   /** Suppress copying
    */
   const CodecG729a & operator=(const CodecG729a &);
};

}
}

#endif

#endif
