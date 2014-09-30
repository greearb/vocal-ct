#ifndef MediaCapability_hxx
#define MediaCapability_hxx

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


static const char* const MediaCapability_hxx_Version = 
    "$Id: MediaCapability.hxx,v 1.1 2004/05/01 04:15:16 greear Exp $";

#include "global.h"
#include <string>
#include <map>
#include <list>
#include "Sptr.hxx"
#include "CodecAdaptor.hxx"
#include "Sdp2Session.hxx"

using Vocal::SDP::SdpSession;

namespace Vocal
{

namespace UA
{

/** Container to hold the CodecAdaptors supported by the libmedia.
 */
class MediaCapability 
{
   public:
      ///
      MediaCapability() { };

      ///
      void addCodec(Sptr<CodecAdaptor> cAdp);

      ///
      Sptr<CodecAdaptor> getCodec(VCodecType cType);
      Sptr<CodecAdaptor> getCodec(const string& cType);

      ///
      list<Sptr<CodecAdaptor> > getSupportedAudioCodecs();
      ///
      list<Sptr<CodecAdaptor> > getSupportedVideoCodecs();

      /// Virtual destructor
      virtual ~MediaCapability() { };

      ///
      virtual string className() { return "MediaCapability"; }

      /**Based on the payload type (96 and higher is treated as dynamic type) and 
        *remoteSdp, returns true or false. 
        *@param supportedType - returns the actual payload type that is equivalent
        *                       to the given payload type
        * for example is we support DTMF payload as 100 (dynamic type) and offered is
        * 101 payload type, it is still supported and the supportedType = 100
       */
      bool isSupported(int payloadType, const SdpSession& remoteSdp, int& supportedType);

    protected:
      ///
      typedef map<VCodecType, Sptr<CodecAdaptor> > CodecMap;

      ///
      CodecMap myCodecMap;

      /** Suppress copying
      */
      MediaCapability(const MediaCapability &);
        
      /** Suppress copying
      */
      const MediaCapability & operator=(const MediaCapability &);
};

}
}

#endif
