#ifndef RtspFileWav_HXX
#define RtspFileWav_HXX
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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspFileWav_hxx_version =
    "$Id: RtspFileWav.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "RtspFile.hxx"
#include <sndfile.h>


/** Child class for *.wav file type.
    Uses libsndfile to read wav header and audio data.
 */
class RtspFileWav : public RtspFile
{
    public:
        /** constructor
            @param shortFilename rtsp based filename of audio file
         */
        RtspFileWav( const string& shortFilename );

        /** deconstructor */
        virtual ~RtspFileWav();

        /** see parent class for more info, rtspSdp isn't used here */
        virtual bool saveHeader( const int ftIndex, RtspSdp& rtspSdp );

        /** see parent class for more info */
        virtual bool loadHeader( int* ftIndex, int* lengthInMs );

        /** see parent class for more info */
        virtual bool open( bool bReadWrite, int ftIndex );

        /** see parent class for more info */
        virtual bool close();

        /** see parent class for more info, pSeqNum and pTS not used */
        virtual int read( void* data, int max, 
                          unsigned short* pSeqNum = 0, unsigned int* pTS = 0);
        
        /** see parent class for more info, pSeqNum and pTS not used */
        virtual int write( void* data, int max,
                           unsigned short uSeqNum = 0, unsigned int uTS = 0 );

        /** see parent class for more info */
        virtual long seek( const long samples, const int whence );
       
        /** see parent class for more info */
        string fileExtensionString() const
            { return ".wav"; }

    private:
        /** Print sf info block.  Used for debugging */
        void printSfInfo() const;

        /** Write dummy data to audio file.  Used to work around libsndfile 
            limitaiton - can't reopen an empty file. */
        void dummyWrite( SNDFILE* xAudioFile, const int dummyLen );

    protected:
        /** libsndfile file pointer handler */
        SNDFILE* m_xAudioFile;

        /** copy of libsndfile formated audio header */
        SF_INFO mySfInfo;
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
