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

#include "global.h"

#include "Rtp.hxx"
#include "codec/g711.h"
#include "LinAudioDevice.hxx"
#include "MediaController.hxx"
#include "CodecG711U.hxx"

using namespace Vocal;
using namespace Vocal::UA;


LinAudioDevice::LinAudioDevice( const char* deviceName)
    : MediaDevice("LinAudioDevice", SOUNDCARD, AUDIO), 
      mySoundCard(deviceName)
{
    // open audio hardware device
    if( mySoundCard.open() == -1 )
    {
        cpLog( LOG_ERR, "Failed to open %s", deviceName );
        exit( -1 );
    }

    cpLog(LOG_DEBUG, "Opened audio device");
    myCodec = new CodecG711U();
    audioActive = false;

} // end LinAudioDevice::LinAudioDevice()



//***************************************************************************
// LinAudioDevice::~LinAudioDevice
// description:  Destructor
//***************************************************************************

LinAudioDevice::~LinAudioDevice()
{
    mySoundCard.close();
} // end LinAudioDevice::~LinAudioDevice()


void LinAudioDevice::processOutgoingAudio() {

    int cc;
    
    int pld_size = myCodec->getSampleSize();

    cc = mySoundCard.read( dataBuffer, myCodec->getSampleSize());
    if ((cc > 0)) {
        if (cc >= pld_size) {
            //Send raw packet to Controller
            cpLog(LOG_DEBUG_STACK, "LinAudioDevice::processOutgoingAudio");
            processRaw((char*)dataBuffer, cc, G711U, NULL, false);
        }
        else {
            // Don't bother the lower stack..it needs the rest of the pkt
            // before it can do something with it...
        }
    }
    else {
        cpLog(LOG_DEBUG, "ERROR: failed to read data from sound card");
        if (mySoundCard.getFd() < 0) {
            cpLog(LOG_ERR, "ERROR:  Soundcard fd is negative, making audio inactive...\n");
            audioActive = false;
        }
    }
}


void LinAudioDevice::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                                uint64 now) {
   if (audioActive) {
       if (mySoundCard.getFd() < 0) {
           cpLog(LOG_ERR, "ERROR:  Soundacard's FD < 0, stopping!\n");
           stop();
       }
       else {
           if (FD_ISSET(mySoundCard.getFd(), input_fds)) {
               processOutgoingAudio();
           }
       }
   }
}

int LinAudioDevice::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                                 int& maxdesc, uint64& timeout, uint64 now) {
    int fd = mySoundCard.getFd();
    if (fd > 0) {
        FD_SET(fd, input_fds);
        maxdesc = max(fd, maxdesc);
    }
    return 0;
}


int
LinAudioDevice::start(VCodecType codec_type)
{
    cerr << "%%% Starting audio" << endl;
    if( audioActive )
    {
        return 1;
    }

    MediaDevice::start(codec_type);

    mySoundCard.reopen();

    audioActive = true;
    return 0;
} // end LinAudioDevice::start()

int
LinAudioDevice::stop()
{

    if( !audioActive )
    {
        return 1;
    }

    //Stop the base class
    MediaDevice::stop();

    cerr << "%%% Stopping audio" << endl;
    audioActive = false;
    mySoundCard.reopen();
    return 0;
} 

void 
LinAudioDevice::sinkData(char* data, int length, VCodecType type,
                         Sptr<CodecAdaptor> codec, bool silence_pkt)
{
    cpLog(LOG_DEBUG_STACK, "Sink Data: length %d", length);
    if(type == DTMF_TONE)
    {
        char digit;
        memcpy(&digit, data, 1);
        cpLog( LOG_DEBUG, "***  DTMF %c  ***", digit );
        return;
    }
    
    if(type != myCodec->getType())
    {
        //First convert the data from type to myType and then feed
        //it to the soundcard
        //cpLog(LOG_ERR, "Need codec conversion!!!!");
        if (codec.getPtr() == 0) {
            codec = MediaController::instance().getMediaCapability().getCodec(type);
        }
        ///Convert from codec type to PCM
        int decLen = 1024;
        char decBuf[1024];
        int decodedSamples = 0;
        int decodedPerSampleSize = 0;
        codec->decode(data, length, decBuf, decLen, decodedSamples, decodedPerSampleSize);
        int encLen = 1024;
        char encBuf[1024];
        myCodec->encode(decBuf, decodedSamples, decodedPerSampleSize, encBuf, encLen);
        mySoundCard.write( (unsigned char*) encBuf, encLen);
    }
    else
    {
        mySoundCard.write( (unsigned char*) data, length);
    }
}

int
LinAudioDevice::suspend()
{
    cerr << "%%% Suspending audio" << endl;
    audioActive = false;
    return(mySoundCard.close());
}

int
LinAudioDevice::resume()
{
    cerr << "%%% Resuming audio" << endl;
    audioActive = true;
    return(mySoundCard.reopen());
}
