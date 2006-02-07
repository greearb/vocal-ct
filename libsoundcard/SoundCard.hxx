#ifndef SOUNDCARD_HXX_
#define SOUNDCARD_HXX_
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

static const char* const SoundCard_hxx_Version =
    "$Id: SoundCard.hxx,v 1.3 2006/02/07 01:33:21 greear Exp $";

#include "Data.hxx"
#include "CircularBuffer.hxx"

using Vocal::ADT::CircularBuffer;

#ifdef WIN32
#include <Mmsystem.h>
#include "ASoundCardWin.hxx"
#endif


namespace Vocal
{

namespace UA
{

enum SoundCardFormat
{
    SoundCardUlaw,
    SoundCardSigned16LE,
    SoundCardInvalid
};

/**

   The SoundCard class is designed as an abstraction for sound
   hardware.  It implements a few simple methods to open, close,
   etc. the hardware and thus allow users to ignore the actual API for
   the hardware.<p>
   
   <b>Sample usage</b><p>
   To record:<p>
   <pre>
   SoundCard mySoundCard("/dev/dsp");
   
   if( mySoundCard.open() == -1 )
   {
        exit( -1 );
   }
   int wav = creat("test.raw", 0666);
   unsigned char dataBuffer[10];
   for(int i = 0 ; i < 1000; i++)
   {
        int cc = mySoundCard.read( dataBuffer, 10 );
        if(cc > 0)
        {
            write(wav, dataBuffer, cc);
        }
   }
   close(wav);
   mySoundCard.close();
   </pre>


   <p>
   To play:<p>
   <pre>
   SoundCard mySoundCard("/dev/dsp");
   
   if( mySoundCard.open() == -1 )
   {
       exit( -1 );
   }
   int wav = open("test.raw", O_RDONLY);
   unsigned char dataBuffer[1024];
   int bytes;
   while( (bytes = read(wav, dataBuffer, 160)) > 0 )
   {
       mySoundCard.write (dataBuffer, bytes );
   }
   close(wav);
   mySoundCard.close();
   </pre>

   <p>
   Please see testsound.cxx for an extended example.
*/

class SoundCard
{
    public:
        SoundCard( const char* deviceName );

        int  open ();
        void write ( const unsigned char* data, int samples );
        int  read ( unsigned char* data, int samples );
        int  close();
        int  reopen();
        SoundCardFormat getFormat() const;
        int getFd() const;
        bool readerBuffered();
        bool writerBuffered();
        bool isClosed() { return (myFD < 0); }

    private:
        int myFD;
        Data myDeviceName;
        SoundCardFormat myFormat;
        int myNumChannels;
        int myMultiplier;

#ifdef WIN32
        //AND:Win32 specific members (handles, etc...)
        ASoundCardWin	m_aSoundCardWinIn;
        ASoundCardWin	m_aSoundCardWinOut;
#endif

        CircularBuffer<unsigned char> myReadBuffer;
        CircularBuffer<unsigned char> myWriteBuffer;

        // Get these off of the stack!
        unsigned char longdataBuffer[1024];
        unsigned char dataBuffer[1024];

        unsigned char rawAudio[2048];
#ifndef WIN32
        unsigned char rdataBuffer[480];
#endif
        int bug_catcher; /* catch overflows */
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
