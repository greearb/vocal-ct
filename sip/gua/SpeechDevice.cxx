/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2003 Vovida Networks, Inc.  All rights reserved.
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

#include <sys/times.h>

#include "Rtp.hxx"
#include "codec/g711.h"
#include "VCondition.h"
#include "SpeechDevice.hxx"
#include "MediaController.hxx"
#include "CodecG711U.hxx"
#include "UaConfiguration.hxx"
#include "UaFacade.hxx"
#include "LookupTable.hxx"

using namespace Vocal;
using namespace Vocal::UA;

#define RESID_RTP_RATE 160      // 320 byte packet size


#define DATADIR "/auto/vvs/bko/sphinx"
#define MODEL_NAME  "7646"
#define HMMDIR "/usr/local/communicator-2000-11-17-2/sphinx_2_format"
#define WAVEFILE_PATH "/usr/local/vocal/etc/"


SpeechDevice::SpeechDevice(  int id )
    : MediaDevice(SOUNDCARD, AUDIO), myHandler(utteranceFifo), myId(id)
{
    // open audio hardware device
    myHandler.initialize();

    myHandler.run();

    cpLog(LOG_DEBUG, "Opened audio device");
    myCodec = new CodecG711U();
    audioActive = false;

    cpLog(LOG_DEBUG, "Opened wave device with id:%d", id);
    myFileToPlay="default.wav";


} // end SpeechDevice::SpeechDevice()

//***************************************************************************
// SpeechDevice::~SpeechDevice
// description:  Destructor
//***************************************************************************

SpeechDevice::~SpeechDevice()
{
    player.stop();
    myTFifo.addDelayMs(0, 0);
} // end SpeechDevice::~SpeechDevice()


void
SpeechDevice::processOutgoingAudio()
{
    if(!audioActive)
        return;

    // do something practical here

    return;
}

void
SpeechDevice::processAudio()
{
    Lock lock(myMutex);
    if ( !audioActive )
    {
        vusleep(30000);
        return ;
    }

    // check to see if there is anything new to add to the queue
    while(utteranceFifo.size())
    {
        string aName(
            UaConfiguration::instance().getValue(SphinxWavefileDirTag));
        aName += "/";

        // remove something and do something about it
        Data x = utteranceFifo.getNext();
        x.removeSpaces();


        cpLog(LOG_DEBUG, "got utterance: %s", x.c_str());
        LookupTableItem item;

        bool found =
            LookupTable::instance(UaConfiguration::instance().getValue(SphinxUtteranceFileTag).c_str())->lookup(x, item);
        if(found)
        {
            aName += item.wav.logData();
            //reportEvent(DeviceEventReferUrl, item.url.logData());
        }
        else
        {
            aName += "unknown.wav";
        }

        player.add(aName);
        player.start();
    }

    int wait = networkPktSize - (getTimeOfDay() - nextTime);

    if ( wait > 0 ) 
    {
	vusleep(wait*1000);
    }

    nextTime += networkPktSize;


    char buffer[10240];
    memset(buffer, 0xFE, networkPktSize*8);
    if ( !player.getData(buffer, networkPktSize*8) )
    {
        // i don't think i want to hang up here
#if 0
         Sptr < UaHardwareEvent > signal = 
                 new UaHardwareEvent( myId );
         signal->type = HardwareAudioType;
         signal->request.type = AudioStop;
         UaFacade::instance().getEventFifo()->add( signal );
#endif
    }
    processRaw((char*)buffer, networkPktSize*8);
}


int
SpeechDevice::start(VCodecType codec_type)
{
    Lock lock(myMutex);
    if ( audioActive )
    {
        cpLog(LOG_ERR, "Audio channel is already active. Ignoring");
        return 0;
    }

    cerr << "%%% Starting audio" << endl;

    audioActive = true;

    MediaDevice::start(codec_type);

    // mark audio as active
    cpLog(LOG_DEBUG, "setting audio active");

    // allocate RTP packet spaces
    networkPktSize = atoi(Vocal::UA::UaConfiguration::instance().getValue(NetworkRtpRateTag).c_str());

    nextTime = getTimeOfDay();
    hasPlayed = false;
    string aName(UaConfiguration::instance().getValue(SphinxWavefileDirTag));
    aName += "/";
    aName += myFileToPlay;
    player.add(aName);
    if(!player.start())
    {
#if 0
        cpLog(LOG_ERR, "Failed to start playing of sound file");
        Sptr < UaHardwareEvent > signal =
                     new UaHardwareEvent( myId );
        signal->type = HardwareAudioType;
        signal->request.type = AudioStop;
        UaFacade::instance().getEventFifo()->add( signal );
#endif
    };

    return 0;
} // end SpeechDevice::start()

int
SpeechDevice::stop()
{
    Lock lock(myMutex);
    if (!audioActive) {
        cpLog(LOG_DEBUG, "stop: No audio active, ignored the request");
        return 1;
    }

    MediaDevice::stop();

    // mark audio as deactivated.
    cpLog(LOG_DEBUG, "Audio Stop received.");
    audioActive = false;

    player.stop();

    cpLog(LOG_DEBUG,"End of session");  
    cerr << "%%% Stopping audio" << endl;

    return 0;
} 


int 
SpeechDevice::shutdownThreads()
{
    MediaDevice::shutdownThreads();
    // now shutdown my own threads, specifically the speech recognizer
    myHandler.shutdown();
    myHandler.join();
    /* Close recognition engine */
    fbs_end ();
    return 0;
}


void 
SpeechDevice::sinkData(char* data, int length, VCodecType type)
{
    Lock lock(myMutex);
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
        cpLog(LOG_ERR, "Need codec conversion!!!!");
        Sptr<CodecAdaptor> cAdp =
        MediaController::instance().getMediaCapability().getCodec(type);
        ///Convert from codec type to PCM
        int decLen = 1024;
        char decBuf[1024];
        cAdp->decode(data, length, decBuf, decLen);
        int encLen = 1024;
        char encBuf[1024];
        myCodec->encode(decBuf, decLen, encBuf, encLen);
        //Give the RTP to recognizer
        myHandler.recognize((unsigned char*) encBuf , encLen );

    }
    else
    {
        //Give the RTP to recognizer
        myHandler.recognize((unsigned char*) data , length );
    }
}


VmTime
SpeechDevice::getTimeOfDay()
{
    VmTime tm;
    struct tms t;

    tm = times(&t) * 10;
    return tm;
}


int
SpeechDevice::suspend()
{
    Lock lock(myMutex);
    cerr << "%%% Suspending audio" << endl;
    audioActive = false;
    return 0;
}

int
SpeechDevice::resume()
{
    Lock lock(myMutex);
    cerr << "%%% Resuming audio" << endl;
    audioActive = true;
    return 0;
}


void
SpeechDevice::reportEvent(DeviceEventType eventType, Data item )
{
    cpLog( LOG_INFO, "Got device event: %d", eventType );
    Sptr < DeviceEvent > event = new DeviceEvent( UaFacade::instance().getEventFifo() );
    event->type = eventType;
    event->id = myId;
    event->item = item;
    UaFacade::instance().getEventFifo()->add( event );
}



/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
