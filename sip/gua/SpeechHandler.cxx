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


/* ====================================================================
 * Copyright (c) 1999-2001 Carnegie Mellon University.  All rights
 * reserved.
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
 * This work was supported in part by funding from the Defense Advanced
 * Research Projects Agency and the National Science Foundation of the
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 */

static const char* const SpeechHandler_cxx_Version =
    "$Id: SpeechHandler.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#ifdef WIN32
#include <posixwin32.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <stdio.h>

extern "C" {
#include "s2types.h"
#include "basic_types.h"
#include "CM_macros.h"
#include "err.h"
#include "ad.h"
#include "cont_ad.h"
#include "search_const.h"
#include "msd.h"
#include "list.h"
#include "hash.h"
#include "lmclass.h"
#include "lm_3g.h"
#include "dict.h"
#include "kb.h"
#include "fbs.h"
#include "mulaw.h"

    unsigned char linear2ulaw( int pcm_val );
    int ulaw2linear( unsigned char u_val );

};

#include "CircularBuffer.hxx"
#include "SpeechHandler.hxx"
#include "cpLog.h"
#include "Lock.hxx"
#include "UaConfiguration.hxx"


#define MIN_ENDSIL      5000    //samples of silence to declare end utterance
#define MAX_ALT         30

int G_tCount=0;
int G_tCount2=0;


using Vocal::Threads::Lock;
using Vocal::UA::UaConfiguration;
using namespace Vocal::UA;

Vocal::ADT::CircularBuffer<int16>* xyzbuf = 0;
Mutex xyzmutex;

extern "C" {
    int fake_read(ad_rec_t *handle, int16 *buf, int32 max);
};

int fake_write(ad_rec_t *handle, int16 *buf, int32 len)
{
    Lock x(xyzmutex);
    if(!xyzbuf)
    {
	xyzbuf = new Vocal::ADT::CircularBuffer<int16>(250000);
    }

    xyzbuf->put(buf, len);
    return len;
}

int fake_read(ad_rec_t *handle, int16 *buf, int32 max)
{
    Lock x(xyzmutex);
    if(!xyzbuf)
    {
	xyzbuf = new Vocal::ADT::CircularBuffer<int16>(250000);
    }
    int len = xyzbuf->bytesUsed();

    if(len > max)
    {
	len = max;
    }
    return xyzbuf->get(buf, len);
//    return len;
}


using namespace Vocal;

void
SpeechHandler::initialize()
{
    myInitFlag = true;

    myModelDataDir = UaConfiguration::instance().getValue(SphinxDataDirTag);
    myModelDataDir += "/lm/";
    myModelDataDir += UaConfiguration::instance().getValue(SphinxModelNameTag);
    myModelDataDir += "/";

    myHMMDir = UaConfiguration::instance().getValue(SphinxHMMDirTag) + "/";

    Data filePrefix(myModelDataDir);
    filePrefix += UaConfiguration::instance().getValue(SphinxModelNameTag);

    /* Open audio device and calibrate for background noise level */
#if 0
    if ((myAd = ad_open ()) == 0)
    {
        cpLog(LOG_ERR, "Failed to open the audio device");
        return;
    }
#endif
    if ((myCont = cont_ad_init (myAd, fake_read)) == 0)
    {
        cpLog(LOG_ERR, "cont_ad_init failed");
        return;
    }
    //Opened the device successfully, calibrate
//    calibrateDevice();

    //make the argument list for fbs_init
    char** argBuf  = new char*[80];
    int argCount = 0;
    for(int i =0; i < 80; i++)
       argBuf[i] = new char[128];

    sprintf(argBuf[argCount], "-verbose"); argCount++;
    sprintf(argBuf[argCount], "9"); argCount++;

    sprintf(argBuf[argCount], "-adcin"); argCount++;
    sprintf(argBuf[argCount], "TRUE"); argCount++;

    sprintf(argBuf[argCount], "-adcext"); argCount++;
    sprintf(argBuf[argCount], "8k"); argCount++;

    sprintf(argBuf[argCount], "-agcmax"); argCount++;
    sprintf(argBuf[argCount], "TRUE"); argCount++;



    sprintf(argBuf[argCount], "-langwt"); argCount++;
    sprintf(argBuf[argCount], "6.5"); argCount++;

    sprintf(argBuf[argCount], "-fwdflatlw"); argCount++;
    sprintf(argBuf[argCount], "8.5"); argCount++;




    sprintf(argBuf[argCount], "-kbdumpdir"); argCount++;
    sprintf(argBuf[argCount], "%s", myModelDataDir.logData()); argCount++;

    sprintf(argBuf[argCount], "-lmfn"); argCount++;
    sprintf(argBuf[argCount], "%s", Data( filePrefix+ ".lm").logData()); argCount++;

    sprintf(argBuf[argCount], "-dictfn"); argCount++;
    sprintf(argBuf[argCount], "%s", Data( filePrefix+ ".dic").logData()); argCount++;

    sprintf(argBuf[argCount], "-noisedict"); argCount++;
    sprintf(argBuf[argCount], "%s", Data( myHMMDir+ "noisedic").logData()); argCount++;

    sprintf(argBuf[argCount], "-phnfn"); argCount++;
    sprintf(argBuf[argCount], "%s", Data( myHMMDir+ "phone").logData()); argCount++;

    sprintf(argBuf[argCount], "-mapfn"); argCount++;
    sprintf(argBuf[argCount], "%s", Data( myHMMDir+ "map").logData()); argCount++;

    sprintf(argBuf[argCount], "-hmmdir"); argCount++;
    sprintf(argBuf[argCount], "%s", myHMMDir.logData()); argCount++;

    sprintf(argBuf[argCount], "-hmmdirlist"); argCount++;
    sprintf(argBuf[argCount], "%s", myHMMDir.logData()); argCount++;

    sprintf(argBuf[argCount], "-8bsen"); argCount++;
    sprintf(argBuf[argCount], "TRUE"); argCount++;

    sprintf(argBuf[argCount], "-sendumpfn"); argCount++;
    sprintf(argBuf[argCount], "%s", Data( myHMMDir + "sendump").logData()); argCount++;

    sprintf(argBuf[argCount], "-cbdir"); argCount++;
    sprintf(argBuf[argCount], "%s", myHMMDir.logData()); argCount++;

    sprintf(argBuf[argCount], "-verbose"); argCount++;
    sprintf(argBuf[argCount], "9"); argCount++;

//    sprintf(argBuf[argCount], "-agcnoise"); argCount++;
//    sprintf(argBuf[argCount], "TRUE"); argCount++;

//    sprintf(argBuf[argCount], "-samp"); argCount++;
//    sprintf(argBuf[argCount], "8000"); argCount++;

    /* Initialize recognition engine */
    fbs_init (argCount, argBuf);
    delete []argBuf;
    myStartwid = kb_get_word_id ("<s>");

    cpLog(LOG_DEBUG, "Done initializing SpeechHandler");
    run();
}

void
SpeechHandler::calibrateDevice()
{
    cpLog (LOG_INFO, "Calibrating background noise level...");
//    ad_start_rec (myAd);
    cont_ad_calib (myCont);
//    ad_stop_rec (myAd);
    cpLog (LOG_INFO, "done.");
}

SpeechHandler::~SpeechHandler()
{
    /* Close recognition engine */
    fbs_end ();
}

void
SpeechHandler::thread()
{
    ///Listen for utterences
    int16 adbuf[4096];
    int32 n_alt=0;
    int32 k, fr, ts, rem;
    char *hyp;
    char word1[1024], word2[1024], word3[1024];
    search_hyp_t **alt;

    calibrateDevice();

    for (;;) {
        if(isShutdown()) break;
	cpLog(LOG_DEBUG, "\n\n\n\nREADY\n\n\n\n");

        // Await beginning of next utterance 
        while ((k = cont_ad_read (myCont, adbuf, 4096)) == 0)
	{
	    if(isShutdown()) break;
            usleep(100000);
	}

        if (k < 0)
        {
            cpLog(LOG_ERR, "Continuous read failed");
            return;
        }

	cpLog(LOG_DEBUG_STACK, "got data!");
        // Non-zero amount of data received; start recognition of new utterance.
        // NULL argument to uttproc_begin_utt => automatic generation of 
        // utterance-id.
        if (uttproc_begin_utt (0) < 0)
        {
            cpLog(LOG_ERR, "uttproc_begin_utt() failed");
            return;
        }
        uttproc_rawdata (adbuf, k, 0);

        cpLog(LOG_DEBUG, "Listening...");

        // Note timestamp for this first block of data 
        ts = myCont->read_ts;

        // Decode utterance until end (marked by a "long" silence, >1sec)
        for (;;) {
	    if(isShutdown()) break;
            // Read non-silence audio data, if any, from continuous 
            // listening module 
            if ((k = cont_ad_read (myCont, adbuf, 4096)) < 0)
            {
                cpLog(LOG_ERR, "cont_ad_read failed");
                break;
            }

	    cpLog(LOG_DEBUG_STACK, "got data!");


            if (k == 0) 
            {
                // No speech data available; check current timestamp 
                // with most recent speech to see if more than min_endsil.  
                // If so, end of utterance.
                if ((myCont->read_ts - ts) > MIN_ENDSIL) break;
            } 
            else 
            {
                // New speech data received; note current timestamp
                ts = myCont->read_ts;
            }

            // Decode whatever data was read above.  NOTE: Non-blocking mode!!
            // rem = #frames remaining to be decoded upon return from the 
            // function.
            rem = uttproc_rawdata (adbuf, k, 0);

            // If no work to be done, sleep a bit 
            if ((rem == 0) && (k == 0))
                usleep (100000);
        }

        // Utterance ended; flush any accumulated, unprocessed A/D data and stop
        // listening until current utterance completely decoded
        cont_ad_reset (myCont);

        cpLog(LOG_DEBUG, "Stopped listening"); 

        // Finish decoding, obtain result 
        uttproc_end_utt ();
        if (uttproc_result (&fr, &hyp, 1) < 0)
        {
            cpLog(LOG_ERR, "uttproc_result failed");
        }
        cpLog(LOG_INFO, "Result %d: %s\n", fr, hyp);

        k = sscanf (hyp, "%s %s %s", word1, word2, word3);
        if (k > 0) {
            search_save_lattice ();
            n_alt = search_get_alt (MAX_ALT, 0, fr-1, -1, myStartwid, &alt);

            if (!playWhatYouGot(hyp, alt, n_alt))
            {
                cpLog(LOG_ERR, "Failed to play");
                return;
            }
        }

        // Exit if the utterance was "BYE or QUIT" 
        if ((k == 1) &&
            ((strcmp (word1, "bye") == 0) || (strcmp (word1, "BYE") == 0)) &&
            ((strcmp (word2, "quit") == 0) || (strcmp (word2, "QUIT") == 0)))
            return;
    }
    return;
}

bool
SpeechHandler::playWhatYouGot(char* best, search_hyp_t **alt, int32 n_alt)
{
    //Play back using TEXT -> Speech 
    //TODO

    //For debug perposes, just dump the words also
    // best result first
    search_hyp_t *h;
    cpLog(LOG_DEBUG, "Best result: %s", best);

    // xxx i should convert this into something playable
    myUtteranceFifo.add(best);

    // Compose strings for and print each of the additional hypotheses 
    cpLog(LOG_DEBUG, "Additional hypotheses...");
    for (int i = 0; i < n_alt; i++) 
    {
        for (h = alt[i]; h; h = h->next) 
        {
            if (h->wid != myStartwid) 
            { // Filter out the startword <s>
                cpLog(LOG_DEBUG, "%d. %s", i, h->word);
            }
        }
    }
    cpLog(LOG_DEBUG, "Additional hypotheses done.");
    
    return true;
}

void
SpeechHandler::recognize(unsigned char* buf, int size)
{
    int16 adbuf[65000];

    for(int i = 0; i < size ; i++) 
    {
	adbuf[i] = muLaw[buf[i]];
    }

    cpLog(LOG_DEBUG_STACK, "wrote buf");
    fake_write(0, adbuf, size);

#if 0
    if(myBuf == 0)
    {
        myBuf = new unsigned char[25000];
        myPktSize = 0;
    }
  

    //Check for noise/silence packet
    bool silence = true;
    for(int i = 0; i < size; i++)
    {
        unsigned char c = *(buf+i);
        c &= 0xf0;
        if(!(!(c ^ 0x70) ||
             !(c ^ 0xf0) ))
        {
            silence = false;
            //break;
        }
        else
        {
            silence = true;
        }
    }
   
    if(!silence)
    {
        cpLog(LOG_DEBUG, "Not silence");
        if(!myStartUtterance)
        {
            myStartUtterance = true;
            G_tCount = 0;
            G_tCount2 = 0;
            myPktSize = 0;
        }
        memcpy(myBuf+(myPktSize), buf, size);
        myPktSize += size;
        G_tCount2++;
    }
    else
    {
        cpLog(LOG_DEBUG, "Filtering silence");
        G_tCount++;

        if(myStartUtterance && (G_tCount > 2 ))
        {
            //Give data for recognition
            if(G_tCount2 > 5)
            {
               recognize_impl(myBuf, myPktSize);
            }
            myPktSize = 0;
            G_tCount = 0;
            myStartUtterance = false ;
        }
        else if((G_tCount < 2) && G_tCount2 > 0)
        {
            memcpy(myBuf+(myPktSize), buf, size);
            myPktSize += size;
        }

    }
#endif
}

Data
SpeechHandler::recognize_impl(unsigned char* buf, int size)
{
    cpLog(LOG_INFO, "In SpeechHandler::recognize_impl: %d", size);
    Data retData;
    int16* adbuf = new int16[size];
    int32 n_alt=0;
    int32 k;
    int32 fr;
    char *hyp;
    char word1[1024], word2[1024], word3[1024];
    search_hyp_t **alt;
    if (uttproc_begin_utt (0) < 0)
    {
        cpLog(LOG_ERR, "uttproc_begin_utt() failed");
        return retData;
    }

//    ad_mu2li(adbuf, buf, size);
    // this code is equivalent to ad_mu2li
    for(int i = 0; i < size ; i++) 
    {
	adbuf[i] = muLaw[buf[i]];
    }

    uttproc_rawdata (adbuf, size, 0);
    // Finish decoding, obtain result 
    uttproc_end_utt ();
    if (uttproc_result (&fr, &hyp, 1) < 0)
    {
        cpLog(LOG_ERR, "uttproc_result failed");
        return retData;
    }
    cpLog(LOG_INFO, "Result %d: %s\n", fr, hyp);

    k = sscanf (hyp, "%s %s %s", word1, word2, word3);
    if (k > 0) 
    {
        search_save_lattice ();
        n_alt = search_get_alt (MAX_ALT, 0, fr-1, -1, myStartwid, &alt);

        retData = hyp;
    }
    delete []adbuf;
    return retData;
}

