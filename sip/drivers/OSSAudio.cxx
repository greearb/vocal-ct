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

static const char* const OSSAudio_cxx_version =
    "$Id: OSSAudio.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include <assert.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "OSSAudio.hxx"
#include "codec/g711.h"

OSSAudio::OSSAudio(const char* device)
    :
    myDevice(0),
    myState(SOUNDCARD_CLOSED)
{
    myDevice = strdup(device);
}


bool 
OSSAudio::open()
{
    if(myState == SOUNDCARD_OPEN)
    {
	return false;
    }
    else
    {
	myFd = ::open(myDevice, O_RDWR, 0);
	if(myFd == -1)
	{
	    perror("Open of device failed: ");
	    return false;
	}

	int ioctlParam = -1;
	if( ioctl( myFd, SNDCTL_DSP_GETFMTS, &ioctlParam ) == -1 )
	{
	    perror("Could not get audio formats: ");
	    return false;
	}

	// choose codec
	if( ioctlParam & AFMT_MU_LAW )
	{
	    //cpLog(LOG_DEBUG,"Setting sound card to ulaw");
	    ioctlParam = AFMT_MU_LAW;
	    myCodec = CODEC_ULAW;
	}
	else if( ioctlParam & AFMT_S16_LE )
	{
	    //cpLog(LOG_DEBUG,"Setting sound card to linear16");
	    ioctlParam = AFMT_S16_LE;
	    myCodec = CODEC_L16;
	}
	else
	{
	    printf("Sound card doesn't support mu law or linear 16");
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
	    return false;
	}

	// setting audio device parameters
	//ioctlParam = AFMT_MU_LAW;  //test setting to ulaw
	int ioctlParamSave = ioctlParam;
	if( ioctl( myFd, SNDCTL_DSP_SETFMT, &ioctlParam ) == -1 )
	{   
	    perror("Could not set DSP format: ");
	    return false;
	}

	if( ioctlParam != ioctlParamSave )
	{   
	    perror("Failed to set DSP Format: ");
	    return false;
	}

	ioctlParam = 1;

	// set the number of channels to 1.  if this is an error, it
	// should probably not be a major failure.

	if( ioctl( myFd, SNDCTL_DSP_CHANNELS, &ioctlParam ) == -1 )
	{
	    perror("could not set right number of channels:");
	    return false;
	}

	if( ioctlParam != 1)
	{
	    printf("warning:  unable to set audio output to mono, may cause problems later\n");
	}

	ioctlParam = 8000;
	if( ioctl( myFd, SNDCTL_DSP_SPEED, &ioctlParam ) == -1 )
	{
	    perror("could not set sample rate: ");
	    return false;
	}
	if( ioctlParam != 8000 )
	{
	    perror("Failed to set sample rate");
	    return false;
	}

	myState = SOUNDCARD_OPEN;
    }
    return true;
}


bool
OSSAudio::close()
{
    if(myState == SOUNDCARD_OPEN)
    {
	myState = SOUNDCARD_CLOSED;
	int retCode = ::close(myFd);
	if(retCode != 0)
	{
	    perror("Close of device failed: ");
	    return false;
	}
    }
    return true;
}


int
OSSAudio::read(char* buf, int size)
{
}


int
OSSAudio::write(char* buf, int size)
{
    if(myState == SOUNDCARD_OPEN)
    {
	signed short convertedBuf[1024];
	int realSize = size;

	switch( myCodec )
	{
	case CODEC_ULAW:
	    // no conversion needed
	    break;
        
	case CODEC_L16:
	    assert(size < 1024);
	    // convert sound sample from ULAW to Linear16
	    for(int p = 0; p < size; p++ )
		convertedBuf[p]  = ulaw2linear( buf[p] );
	    realSize = size * 2;
	    buf = (char*)convertedBuf;
	    break;

	default:
	    return -1;
	}
	int retVal = ::write(myFd, buf, realSize);
	return retVal;
    }
    else
    {
	printf("sound card is not open, not writing.\n"); 
	return -1;
    }
    
}


/* Local Variables: */
/* c-file-style:"stroustrup" */
/* c-basic-offset:4 */
/* c-file-offsets:((inclass . ++)) */
/* indent-tabs-mode:nil */
/* End: */
