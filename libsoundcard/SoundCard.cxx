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

static const char* const SoundCard_cxx_Version =
    "$Id: SoundCard.cxx,v 1.3 2004/06/22 02:24:04 greear Exp $";

#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <csignal>
#include <errno.h>

/* sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef WIN32
#include <sys/soundcard.h>
#endif

#include "SoundCard.hxx"
#include "cpLog.h"
#include <cassert>
#include "../util/misc.hxx"

using namespace Vocal;
using namespace Vocal::UA;

extern "C"
{
    unsigned char linear2ulaw( int pcm_val );
    int ulaw2linear( unsigned char u_val );
}


SoundCard::SoundCard( const char* deviceName )
    : myFD(-1),
      myDeviceName(deviceName),
      myFormat(SoundCardInvalid),
      myNumChannels(1),
      myMultiplier(1),
      myReadBuffer(2048),
      myWriteBuffer(2048),
      bug_catcher(0xabcdef12)
{
    assert(deviceName);
}


int SoundCard::reopen()
{
    int retval;
    retval = close();
    retval = retval | open();
    return retval;
}

int 
SoundCard::open()
{
    cpLog(LOG_DEBUG, "Opening audio hardware" );

    // open audio device
#ifndef WIN32
    //if( ( myFD = ::open( myDeviceName.getData(lo), O_RDWR | O_NDELAY, 0 ) ) == -1 )
    if( ( myFD = ::open( myDeviceName.c_str(), O_RDWR , 0 ) ) == -1 )
    {
        perror("Open failed:");
	myFD = -1;
	return -1;
    }

#else
    if (waveInGetNumDevs() <= 0) 
    {
        cpLog(LOG_DEBUG, "could not get any devices");
	myFD = -1;
	return -1;
    }
    if (!m_aSoundCardWinOut.Open(SoundOut)) 
    {
        perror("Open openAudioHardware failed (not found input sound card device) : ");
	myFD = -1;
	return -1;
    }
    if (!m_aSoundCardWinIn.Open(SoundIn)) 
    {
#ifndef _DEBUG
        //AND:Need for run 2 exemplars program on one computer (debug version)
        m_aSoundCardWinOut.Close();
        perror("Open openAudioHardware failed (not found output sound card device) : ");
	myFD = -1;
	return -1;
#endif
    }
    myFD = 1;
#endif

#ifndef WIN32

    // reset the thing
    if( ioctl( myFD, SNDCTL_DSP_RESET, 0 ) == -1 ) 
    {
        perror("could not reset DSP");
        ::close(myFD);
	myFD = -1;
        return -1;        
    }

    // query capabilities
    int caps;
    if ( ioctl ( myFD, SNDCTL_DSP_GETCAPS, &caps ) == -1 )
    {
        perror("SNDCTL_DSP_GETCAPS");
        ::close(myFD);
	myFD = -1;
        return -1;
    }

    if( ! (caps & DSP_CAP_DUPLEX)) 
    {
        cout << "warning:  this card does not support full duplex.\n"
             << "we will try to proceed anyway.\n";
    }

    // query audio formats
    int ioctlParam = -1;
    if( ioctl( myFD, SNDCTL_DSP_GETFMTS, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_GETFMTS");
        ::close(myFD);
	myFD = -1;
        return -1;
    }

    // choose codec
    if( ioctlParam & AFMT_MU_LAW )
    {
        cpLog(LOG_DEBUG,"Setting sound card to ulaw");
        ioctlParam = AFMT_MU_LAW;
	myFormat = SoundCardUlaw;

    }
    else if( ioctlParam & AFMT_S16_LE )
    {
        cpLog(LOG_DEBUG,"Setting sound card to linear16");
        ioctlParam = AFMT_S16_LE;
	myFormat = SoundCardSigned16LE;
    }
    else
    {
        cpLog(LOG_ERR,"Sound card doesn't support any known encodings");
        printf("Sound card supports the following (Mask:%d:)\n", ioctlParam);
        if( ioctlParam & AFMT_MU_LAW )     printf("  AFMT_MU_LAW\n");
        if( ioctlParam & AFMT_A_LAW )      printf("  AFMT_A_LAW\n");
        if( ioctlParam & AFMT_IMA_ADPCM )  printf("  AFMT_IMA_ADPCM\n");
        if( ioctlParam & AFMT_U8 )         printf("  AFMT_U8\n");
        if( ioctlParam & AFMT_S16_LE )     printf("  AFMT_S16_LE\n");
        if( ioctlParam & AFMT_S16_BE )     printf("  AFMT_S16_BE\n");
        if( ioctlParam & AFMT_S8 )         printf("  AFMT_S8\n");
        if( ioctlParam & AFMT_U16_LE )     printf("  AFMT_U16_LE\n");
        if( ioctlParam & AFMT_U16_BE )     printf("  AFMT_U16_BE\n");
        if( ioctlParam & AFMT_MPEG )       printf("  AFMT_MPEG\n");
        ::close(myFD);
	myFD = -1;
        return -1;
    }

    // setting audio device parameters
    //ioctlParam = AFMT_MU_LAW;  //test setting to ulaw
    int ioctlParamSave = ioctlParam;
    if( ioctl( myFD, SNDCTL_DSP_SETFMT, &ioctlParam ) == -1 ) {
       cpLog(LOG_ERR, "SNDCTL_DSP_SETFMT: %s", strerror(errno));
       ::close(myFD);
       myFD = -1;
       return -1;
    }
    if( ioctlParam != ioctlParamSave ) {
       cpLog(LOG_ERR, "Failed to set DSP Format, sound card returned format: %s",
             strerror(errno));
       ::close(myFD);
       myFD = -1;
       return -1;
    }

    ioctlParam = 1;

    // set the number of channels to 1.  if this is an error, it
    // should probably not be a major failure.

    myNumChannels = 1;

    if ( ioctl( myFD, SNDCTL_DSP_CHANNELS, &ioctlParam ) == -1 ) {
       cpLog(LOG_ERR,"SNDCTL_DSP_CHANNELS: %s", strerror(errno));
       ::close(myFD);
       myFD = -1;
       return -1;
    }

    if( ioctlParam != 1) {
       myNumChannels = ioctlParam;
       cout << "warning:  unable to set audio output to mono,"
            << " may cause problems later. Set to " 
            << myNumChannels << " channels\n";
    }

    ioctlParam = 8000;
    if( ioctl( myFD, SNDCTL_DSP_SPEED, &ioctlParam ) == -1 ) {
       cpLog(LOG_ERR, "SNDCTL_DSP_SPEED: %s", strerror(errno));
       ::close(myFD);
       myFD = -1;
       return -1;
    }
    if( ioctlParam != 8000 ) {
       cpLog(LOG_ERR, "Failed to set sample rate: %s, it is instead: %d",
             strerror(errno), ioctlParam);
    }

    int blockSize;
    if ( ioctl ( myFD, SNDCTL_DSP_GETBLKSIZE, &blockSize) == -1 ) {
       cpLog(LOG_ERR, "SNDCTL_DSP_GETBLKSIZE: %s", strerror(errno));
       ::close(myFD);
       myFD = -1;
       return -1;
    }

    if ( ioctl( myFD, SNDCTL_DSP_SYNC, 0 ) == -1 ) {
       cpLog(LOG_ERR, "SNDCTL_DSP_SYNC: %s", strerror(errno));
       ::close(myFD);
       myFD = -1;
       return -1;
    }

    cpLog(LOG_ERR, "soundcard opened successfully, blockSize = %d\n", blockSize);

#else
    myFormat = SoundCardSigned16LE;
#endif

    myMultiplier = 1;

    return 0;
}


bool
SoundCard::writerBuffered ()
{
    int bufbytes = 0;
    ::ioctl(myFD, SNDCTL_DSP_GETODELAY, &bufbytes);

    return (bufbytes > 0);
}


void SoundCard::write ( const unsigned char* data, int samples ) {
    int cc = 0;
    int p = 0;
    int j = 0;
    int i;

    // check codec encodings
    switch( myFormat )
    {
    case SoundCardUlaw:
        // no conversion needed
        cpLog(LOG_ERR, "Writing: %d ULAW samples to sound card buffer.\n", samples);
	myWriteBuffer.put(data, samples);
        break;
        
    case SoundCardSigned16LE:
        assert( (int)(sizeof(longdataBuffer)) >= (samples * myNumChannels * 2));
        // convert sound sample from ULAW to Linear16
        for(p = 0; p < samples; p++ )
        {
            int c = ulaw2linear( data[p] );
            for(i = 0; i < (myNumChannels * myMultiplier) ; i++)
            {
                longdataBuffer[j++] = c & 0xff ;
                longdataBuffer[j++] = (c & 0xff00) >> 8;
            }
        }

        samples *= 2 * myNumChannels;
        cpLog(LOG_DEBUG, "Writing: %d CONVERTED ULAW samples to sound card buffer.\n", samples);
        myWriteBuffer.put(longdataBuffer, samples);
        break;

    default:
        cpLog(LOG_ERR,"Failed to provide correct codec encoding");
        return;
    }

    cc = myWriteBuffer.get(dataBuffer, samples);

#ifndef WIN32

#if 0
    audio_buf_info info;
    ::ioctl(myFD, SNDCTL_DSP_GETOSPACE, &info);

    int bytecount = info.fragments * info.fragsize;
        
    if (cc > bytecount) {
        //cout << "write will block" << endl;
    }

    if (myWriteBuffer.bytesUsed() > 100) {
        // cout << "bytesUsed: " << myWriteBuffer.bytesUsed() << endl;
    }

    int bufbytes = 0;
    ::ioctl(myFD, SNDCTL_DSP_GETODELAY, &bufbytes);

    if(bufbytes > 256) {
        //cout << "bufbytes: " << bufbytes << endl;
    }
#endif

    // NOTE:  Inefficient to convert this..remove before shipping.
    //string dmp;
    //hexDump((const char*)(dataBuffer), cc, dmp);
    //cpLog(LOG_DEBUG, "Writing to sound card fd: %d, len: %d hex:\n%s\n",
    //      myFD, cc, dmp.c_str());

    cpLog(LOG_DEBUG, "Writing to sound card fd: %d, len: %d\n",
          myFD, cc);

    int rv = ::write( myFD, dataBuffer, cc );

    if (rv < 0) {
        cpLog(LOG_ERR, "ERROR writing to sound card, fd: %d  len: %d, error: %s\n",
              myFD, cc, strerror(errno));
    }

#else
    m_aSoundCardWinOut.Write(reinterpret_cast<char*>(dataBuffer), cc);
#endif

    return;
}


int 
SoundCard::read( unsigned char* data,
                 const int samples )
{
    int cc = -1;
    int p = 0;
    int q = 0;
    int bigs;

    //check codec encodings
    switch( myFormat )
    {
#ifndef WIN32
    case SoundCardUlaw:
        // no conversion needed
        cc = 0;
        audio_buf_info info;
        ::ioctl(myFD, SNDCTL_DSP_GETISPACE, &info);

        {
            int bytecount = info.fragments * info.fragsize;

            if (samples > bytecount)
            {
//                cout << "read: " << bytecount << endl;
            }
        }

        assert(samples <= (int)(sizeof(rdataBuffer)));
        cc = ::read( myFD, rdataBuffer, samples );


	myReadBuffer.put(rdataBuffer, cc);

        // TODO:  This is a wasted memory copy unless cc == requested
        // read size.  Could optimize this if desired... --Ben
	cc = myReadBuffer.get(data, samples);

        break;
#endif
    case SoundCardSigned16LE:
        // convert sound sample from Liner16 to ULAW
#ifndef WIN32
        ::ioctl(myFD, SNDCTL_DSP_GETISPACE, &info);

        bigs = samples * 2 * myNumChannels;
        {
            if (bigs > info.bytes)
            {
                bigs = info.bytes;
            }
        }

        if(bigs == 0)
        {
//            cout << "bigs is 0..." << endl;
            bigs = 10;
//            cc = 0;
        }

        cc = ::read( myFD, rawAudio, bigs );

        if(cc)
        {
            myReadBuffer.put(rawAudio, cc);
        }

	cc = myReadBuffer.get(rawAudio, samples * 2 * myNumChannels);

#else
        cc = m_aSoundCardWinIn.Read( (char*)rawAudio, samples * 2 );
#endif
        q = 0;
        for(p = 0; p < cc; p += (myNumChannels * myMultiplier * 2) )
        {
            data[q] 
                = linear2ulaw((rawAudio[p+1] << 8) | rawAudio[p]);
            q += 1;
        }
        cc = q;
        break;

    default:
        cpLog(LOG_DEBUG,"Failed to provide correct encoding");
        break;
    }
#ifndef WIN32
    if( samples != cc )
    {
        cpLog(LOG_DEBUG,"Incomplete read from Sound Card (%d)", cc);
    }
#else
    // AND:prevent - many garbage on screen (when run 2 programs and
    // input from sound card already use one from him) :-/

    if( m_aSoundCardWinIn.IsOpen() && samples != cc )
    {
        cpLog(LOG_ERR,"Incomplete read from Sound Card (%d)", cc);
    }
#endif

    // NOTE:  Inefficient to convert this..remove before shipping.
    //string dmp;
    //hexDump((const char*)(data), cc, dmp);
    //cpLog(LOG_DEBUG, "Read from sound card fd: %d, len: %d hex:\n%s\n",
    //      myFD, cc, dmp.c_str());

    return cc;
}


int 
SoundCard::close()
{
    int retval = 0;

    cpLog(LOG_DEBUG, "Closing audio hardware" );

#ifndef WIN32
    if (myFD >= 0) {
        if ( ioctl( myFD, SNDCTL_DSP_RESET, 0 ) == -1 ) {
            cpLog(LOG_ERR, "ERROR: trying to reset SND-Device, fd: %d  error: %s\n",
                  myFD, strerror(errno));
        }
        retval = ::close( myFD );
    }

#else
    m_aSoundCardWinOut.Close();
    m_aSoundCardWinIn.Close();
#endif

    myFD = -1;

    return retval;
}


SoundCardFormat
SoundCard::getFormat() const
{
    return myFormat;
}


int 
SoundCard::getFd() const
{
    return myFD;
}
