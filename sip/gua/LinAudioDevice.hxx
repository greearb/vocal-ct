#ifndef LinAudioDevice_HXX_
#define LinAudioDevice_HXX_

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
static const char* const LinAudioDeviceVersion =
    "$Id: LinAudioDevice.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include "Rtp.hxx"
#include "SoundCard.hxx"
#include "MediaDevice.hxx"
#include "SipProxyEvent.hxx"

#include "NetworkRes.hxx"

#ifdef USE_WINRTP
#include "AudioSample.h"
#include "PCM2G711Transformer.h"
#include "G7112PCMTransformer.h"
#include "AudioSampleManager.h"
#endif

using Vocal::SipProxyEvent;

namespace Vocal
{

namespace UA
{

/**
 * Implements a Sound card device on Linux
 */

class LinAudioDevice : public MediaDevice
{
    public:
        ///
        LinAudioDevice( const char* deviceName);

        /// Destructor
        virtual ~LinAudioDevice();

        /** Start audio channel on device
       	  * @return 0 if successful, errorcode otherwise
	  */
        int start(VCodecType codec_type);

        /** Stops audio channel on device
       	 * @return 0 if successful, errorcode otherwise
    	 */
        int stop();

        /** suspend audio channel on device
       	 * @return 0 if successful, errorcode otherwise
    	 */
        int suspend();

        /** resume audio channel on device
       	 * @return 0 if successful, errorcode otherwise
    	 */
        int resume();
        

        ///process data from audio device
        void processAudio();

        ///Play data to the soundcard device
        void sinkData(char* data, int length, VCodecType type,
                      Sptr<CodecAdaptor> codec, bool silence_pkt);

#ifdef USE_WINRTP
        int RenderAudioSamples(std::vector<std::pair<AudioSample*, AudioSource* > > &data);
        int GenerateData(AudioSample **ppAudioSample);
        void addMixer(Sptr<PCMMixer> mmixer);
        void setSinkMixer(Sptr<PCMMixer> mmixer);
#endif


    private:
#ifdef USE_WINRTP
       Sptr<PCM2G711Transformer> myPCMtoUlawTransformer;
       Sptr<G7112PCMTransformer> myUlawToPCMTransformer;
#endif
        ///
	void processOutgoingAudio();

        //
        bool audioActive;

        ///
        unsigned char dataBuffer[480];

        ///
        SoundCard mySoundCard;

};
 
}
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
