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

static const char* const phoneJack_cxx_Version =
    "$Id: soundcardTest.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";
#include <iostream.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/soundcard.h>

#include "RtpSession.hxx"
#include "cpLog.h"

#define NETWORK_RTP_RATE 160

static int deviceFD;
RtpSession* audioStack;
RtpPacket* outRtpPkt;
RtpPacket* inRtpPkt;

RtpPayloadType apiCodec;
static int apiRate;

int openAudioHardware( const char* pname )
{
    cerr <<"Opening audio hardware\n";

    int audioFD;
    int ioctlParam;

    // open audio device
    if( ( audioFD = open( pname, O_RDWR, 0 ) ) == -1 )
    {
        perror("Open failed:");
        return -1;
    }

    // query audio formats
    ioctlParam = -1;
    if( ioctl( audioFD, SNDCTL_DSP_GETFMTS, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_GETFMTS");
        return -1;
    }

    // choose codec
    if( ioctlParam & AFMT_MU_LAW )
    {
        cpLog(LOG_DEBUG,"Setting sound card to ulaw");
        ioctlParam = AFMT_MU_LAW;
        apiCodec = rtpPayloadPCMU;
        apiRate = 160;
    }
    else if( ioctlParam & AFMT_S16_LE )
    {
        cpLog(LOG_DEBUG,"Setting sound card to linear16");
        ioctlParam = AFMT_S16_LE;
        apiCodec = rtpPayloadL16_mono;
        apiRate = 320;
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
        return -1;
    }

    // setting audio device parameters
    int ioctlParamSave = ioctlParam;
    if( ioctl( audioFD, SNDCTL_DSP_SETFMT, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_SETFMT");
        return -1;
    }
    if( ioctlParam != ioctlParamSave )
    {
        perror("Failed to set DSP Format, sound card returned format");
        return -1;
    }
    ioctlParam = 1;
    if( ioctl( audioFD, SNDCTL_DSP_CHANNELS, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_CHANNELS");
        return -1;
    }
    if( ioctlParam != 1)
    {
        perror("Failed to set channels");
        return -1;
    }

    ioctlParam = 8000;
    if( ioctl( audioFD, SNDCTL_DSP_SPEED, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_SPEED");
        return -1;
    }
    if( ioctlParam != 8000 )
    {
        perror("Failed to set sample rate");
        return -1;
    }

    return audioFD;
}


int closeAudioHardware( int audioFD )
{
    cerr <<"Closing audio hardware\n";

    if( ioctl( audioFD, SNDCTL_DSP_RESET, 0 ) == -1 )
    {
        perror("SNDCTL_DSP_RESET");
    }

    close( audioFD );

    return 1;
}


int audioStart ( char* rmtHost, int rmtPort, int lclPort )
{
    cerr << "%%%   Starting audio   %%%\n";

    // open RTP session
    cerr <<"Opening RtpSession\n";
    audioStack = new RtpSession( rmtHost,
                                 rmtPort, lclPort, 0, 0 );
    assert( audioStack );

    // setup network paramaters
    cerr <<"Setting up network paramaters\n";
    audioStack->setApiFormat ( apiCodec, apiRate );
    audioStack->setNetworkFormat ( rtpPayloadPCMU, NETWORK_RTP_RATE );
    //if( lclPort )
    //{
    //    inRtpPkt = new RtpPacket( NETWORK_RTP_RATE );
    //}
    inRtpPkt = 0;
    outRtpPkt = 0;

    return 0;
}


int audioStop ()
{
    cerr << "%%%   Stopping audio   %%%\n";

    // close RTP session
    audioStack->transmitRTCPBYE();

    delete audioStack; audioStack = NULL;

    if ( outRtpPkt )
    {
        delete outRtpPkt;
        outRtpPkt = NULL;
    }
    if ( inRtpPkt )
    {
        delete inRtpPkt;
        inRtpPkt = NULL;
    }

    return 0;
}


int main ( int argc, char* argv[] )
{
    if( argc < 3 )
    {
        cerr <<"Usage: "<<argv[0]<<" remoteHost remotePort localPort\n";
        cerr <<"Usage: "<<argv[0]<<" 0 0 localPort\n";
        cerr <<"Usage: "<<argv[0]<<" remoteHost remotePort localPort\n";
        return 0;
    }

    // turn on cplog
    cpLogSetLabel ( "sampleUsage" );
    cpLogSetPriority ( LOG_DEBUG_STACK );


    // open audio session
    deviceFD = openAudioHardware( "/dev/dsp" );
    if( deviceFD < 0 )
    {
        cerr <<"Error opening audio hardware\n";
        return -1;
    }
    audioStart( argv[ 1 ], atoi( argv[ 2 ] ), atoi( argv[ 3 ] ) );
    usleep( 1000 );

    int maxFD = 0;
    fd_set rfds;
    struct timeval tv;
    if ( maxFD < deviceFD ) maxFD = deviceFD;
    maxFD++;

    char outBuffer[1024];
    int cc;

    while ( 1 )
    {
        FD_ZERO( &rfds );
        FD_SET( deviceFD, &rfds );
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        // transmit an audio packet
        if( atoi( argv[ 2 ] ) != 0 )
        {
            if ( select( maxFD, &rfds, NULL, NULL, &tv ) >= 0 )
            {
                cerr << "t";
                cc = read ( deviceFD, outBuffer, apiRate );
                if ( cc > 0 )
                {
                    audioStack->transmitRaw( outBuffer, cc );
                }

            }
            else cerr << "T";
        }


        // receive an audio packet
        if( atoi( argv[ 3 ] ) != 0 )
        {
            inRtpPkt = audioStack->receive();
            if( inRtpPkt )
            {
                cerr << "r";
                write ( deviceFD, inRtpPkt->getPayloadLoc(),
                                  inRtpPkt->getPayloadUsage() );
                delete inRtpPkt;
            }
            //else cerr << "R";
        }
    }

    // close audio session
    audioStop();
    closeAudioHardware( deviceFD );
}
