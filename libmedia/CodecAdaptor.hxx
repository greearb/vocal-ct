#ifndef CodecAdaptor_hxx
#define CodecAdaptor_hxx

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


static const char* const CodecAdaptor_hxx_Version = 
    "$Id: CodecAdaptor.hxx,v 1.1 2004/05/01 04:15:16 greear Exp $";

#include "global.h"
#include <string>
#include <map>
#include "Def.hxx"

namespace Vocal
{

namespace UA
{



/** CodecAdaptor - Interface to plugin codecs, a Codec implementation must derive
    from this interface in order to plug in into Media Framework
 */
class CodecAdaptor {
public:
   // Used when we are negotiating the protocol, ie type >= 96
   int getRtpType() const {
      if (rtpType == -1) {
         return getType(); // Will work for non-negotiated protocols at least
      }
      return rtpType;
   }

   VMediaType getMediaType() const { return myMediaType; }

   virtual const string& getEncodingName() const { return myEncodingName; };
   
   int getClockRate() const { return myClockRate; }
   int getSampleSize() const { return sampleSize; }
   VCodecType getType() const { return myType; };
   int getPerSampleSize() const { return perSampleSize; }

   void setRtpType(int i) { rtpType = i; }

   void setClockRate(int c) { myClockRate = c; }

   ///
   const map<string, string>& getAttrValueMap() const { return myAttrValueMap; };

   /**Decode form codec type to raw data (PCMU), caller should supply the buffer of
    *size atleast length * 2. Returns 0 if successfull or -1 on failure
    * @param length is in bytes
    * @param decBufLen is the space available in decBuf, in units of bytes.
    * @param decodedSamples  Upon return, specifies number of samples that were decoded.
    * @param decodedPerSampleSize Upon return, 
    */
   virtual int decode(char* data, int length, char* decBuf, int decBufLen,
                      int &decodedSamples, int& decodedPerSampleSize) = 0;

   /**Encode from raw data (PCMU) to codec type, caller should supply the buffer of
    * size atleast > length/2. Returns 0 if successfull  or -1 on failure.
    * @ num_samples is not necessarily the length of 'data'.  num_samples * per_sample_size
    *     is the length of 'data' in bytes.
    * @ param encodedLength should be max size of encBuf, in bytes, upon calling,
    *     and will be the actuall number of bytes encoded upon return.
    */
   virtual int encode(char* data, int num_samples, int per_sample_size,
                      char* encBuf, int& encodedLength) = 0;

   
   int getPriority() const { return myPriority; };
   
   void setPriority(int pri) { myPriority = pri; };

   /// TODO:  I bet we can get rid of these.
   int getCompressionRatio() const { return myCompressionRatio; };
   ///
   void setCompressionRatio(int cr) { myCompressionRatio = cr; };

   ///
   virtual string className() { return "CodecAdaptor"; }
   
   /// Virtual destructor
   virtual ~CodecAdaptor() { };
   
protected:
   /**Constructor to create a CodecAdaptor, the priority indicate
    * the pref when multiple codecs can be used, a 0 priority means
    * equal preferrence
    */
   CodecAdaptor(VCodecType cType, int priority=0)
         : myType(cType), myPriority(priority), 
           myEncodingName("UNKNOWN"), 
           myClockRate(0), perSampleSize(1),
           sampleSize(160), rtpType(-1), myMediaType(AUDIO)
      { };
   

   CodecAdaptor(const CodecAdaptor &);
   
   VCodecType myType; 
   int myPriority;
   string myEncodingName;   
   int myClockRate;
   int perSampleSize;
   int sampleSize; // How many samples per packet, 160 usually.

   // For negotiated protocols.
   int rtpType;
   
   /*
    * C.Cameron -- added in Compression Ratio to allow G729 to work properly
    */
   int    myCompressionRatio;
   VMediaType myMediaType;

   map<string, string> myAttrValueMap;
   

private:
   /** Suppress copying
    */
   const CodecAdaptor & operator=(const CodecAdaptor &);
};

}
}

#endif
