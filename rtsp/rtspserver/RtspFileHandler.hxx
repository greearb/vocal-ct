#ifndef RtspFileHandler_HXX
#define RtspFileHandler_HXX
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

static const char* const RtspFileHandler_hxx_version =
    "$Id: RtspFileHandler.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "cpLog.h"
#include "Data.hxx"
#include "Rtp.hxx"
#include "RtspSdp.hxx"
#include "RtpFileTypes.hxx"
#include "RtspFile.hxx"
#include "RtspMsgInterface.hxx"
#include "Sptr.hxx"

/** Interface into RtspFile class.  Handles reading and writing to audio file
    and audio header.  Container for parameters used by rtp processor.
 */
class RtspFileHandler
{
    public:
        /** Opens the specified descriptor file.
            Parses from filename file extension, has default if not found.
            Parses from filename myFtIndex(codec type), has default if not found.
            Note, myFtIndex might not match format type in header of audio file.

            Sets up the following private data members.
            - myFileExtension
            - myFtIndex ( my be incorrect )
            - myFilename
            @param filename filename.[wav|au|raw|...]
            @param contentBase rtsp file path
         */
        RtspFileHandler( const Data& filename, const Data& contentBase );
        /** deconstructor */
        virtual ~RtspFileHandler();

        /** Return name of audio file. */
        Data filename() const;

        /** return contentbase(rtsp file path) + filename + extension */
        Data filenameUrl() const;

        /** Write sdp block into audio header and/or *.sdp file.  This
            function will check codec in sdp block is valid and supported.  
            Audio file and header will be created as needed.
            @param sdp sdp block (ie content body of rtsp ANNOUNCE)
         */
        bool writeAudioHeader( const Data& sdp );

        /** Read sdp block from audio header or *.sdp file.
            @return sdp block.  Can be casted into char* or string - see
                    RtspSdp class.
         */
        RtspSdp& readAudioHeader();

        /** Builds myRtspSdp from audio header or *.sdp file.
            @return false on failure
         */
        bool loadAudioHeader( Sptr<RtspMsg> = 0 );

        /** Open file with given mode and sizing.
            @param mode <code>false</code> read, <code>true>/code>write
            @param packetSize bytes per packet
         *  @return false on failure
         */
        bool openAudioFile( const bool mode, const int packetSize );

        /** Closes audio file.  Updates audio header and *.sdp file
         *  @return false on failure
         */
        bool closeAudioFile();

        /** Return length of audio file in ms, -1 if unknown.  */
        long length() const
            { return myLength; }

        /** Read from audio file.
            @param data return param preallocationed memory location for 
                        new data
            @param max size of preallocationed memory location in bytes
            @param pSeqNum rtp sequence number of new data
            @param pTs rtp time stamp of new data
            @return bytes of new data read, -1 if error
         */
        int read( void* data, size_t max,
                  unsigned short* pSeqNum = 0, unsigned int* pTS = 0 );

        /** Write to audio file.
            @param data new data to write 
            @param max size of new data in bytes
            @param uSeqNum rtp sequence number of new data
            @param uTs rtp time stamp of new data
            @return bytes of new data written, -1 if error
         */
        int write( void* data, size_t max,
                   unsigned short uSeqNum = 0, unsigned int uTS = 0 );

        /** Seek read/write pointer into file.
         #  @param npt time in ms
         *  @return actual npt in milliseconds, -1 if error
         */
        long seek( long npt );

        /** set dynamicCodec */
        void setDynamicCodec( int dynCodec ) { myDynamicCodec = dynCodec; }

        /** rtp payload codec */
        int packetCodec() const
            { return myCodec; }

        /** rtp packet size in bytes */
        int packetSize() const
            { return myPacketSize; }

        /** samples per rtp packet */
        int packetSamples() const
            { return rtpFileTypeInfo[myFtIndex].samples; }

        /** packet interval in milliseconds */
        int packetInterval() const
            { return myPacketInterval; }

        /** codec parameters index */
        int ftIndex() const
            { return myFtIndex; }

        /** get dynamic payload type */
        int dynamicCodec() const
            { return myDynamicCodec; }

    private:
        /** file extension types */
        enum eExtension
        {
            UNKNOWN_EXT = 0,
            AU_EXT,
            WAV_EXT,
            RAW_EXT,
            RTP_EXT
        };

    private:
        /** rtsp file path */
        string myContentBase;

        /** file extension type of audio file */
        eExtension myFileExtension;

        /** sdp data */
        RtspSdp myRtspSdp;

        /** RtspFile interface */
        RtspFile* m_xRtspFile;

        /** file access mode. 
            <code>false</code> if read; <code>true</code> if write */
        bool myReadWrite;

        /** codec parameters index */
        int myFtIndex;

        /** rtp payload codec */
        int myCodec;

        /** length of audio file in ms */
        int myLength;

        /** packet interval in milliseconds */
        int myPacketInterval;

        /** rtp packet size in bytes */
        int myPacketSize;

        /** dynamic payload type needed to be remembered */
        int myDynamicCodec;

    protected:
        /** suppress copy constructor */
        RtspFileHandler( const RtspFileHandler& );
        /** suppress assignment operator */
        RtspFileHandler& operator=( const RtspFileHandler& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
