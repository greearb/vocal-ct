#ifndef RtspFile_HXX
#define RtspFile_HXX
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

static const char* const RtspFile_hxx_version =
    "$Id: RtspFile.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "cpLog.h"
#include "Data.hxx"
#include "Rtp.hxx"
#include "RtspSdp.hxx"
#include "RtspConfiguration.hxx"

/** Rtsp file modes.
 */
enum RtspFileMode
{
    RTSP_FILE_MODE_PLAY,
    RTSP_FILE_MODE_REC,
    RTSP_FILE_MODE_UNKNOWN
};

/** Base class for all the file types ( *.au, *.wav, *.raw, *.rtp ).
    Handles reading and writing to audio file and audio header.
 */
class RtspFile
{
    public:
        /** constructor
            @param shortFilename rtsp based filename of audio file
         */
        RtspFile( const string& shortFilename )
            { myShortFilename = shortFilename; }

        /** deconstructor */
        virtual ~RtspFile() {};

        /** Save sdp block into audio header.
            Child class either uses <code>ftIndex</code> or
            <code>rtspSdp</code> to determine correct header to write.  
            Overwrite any existing header information.
            @param ftIndex new codec parameters indexed base on RtpFileTypes 
            @param rtspSdp sdp block to write into sdp file (if configured)
         */
        virtual bool saveHeader( const int ftIndex, RtspSdp& rtspSdp ) = 0;

        /** Load audio header codec information.
            @param ftIndex return param of new codec index
            @param lenghtInMs return param of file length in milliseconds.
            @return false on failure (ie can't open audio header or fail to
                read audio header)
         */
        virtual bool loadHeader( int* ftIndex, int* lengthInMs ) = 0;

        /** Opens audio file for reading or writing.
            @param bReadWrite <code>true</code> for write;
                              <code>false</code> for reading
            @param ftIndex codec index of audio file.
                           Value obtainable from loadHeader().
            @return false on failure (ie can't find audio file)
         */
        virtual bool open( bool bReadWrite, int ftIndex ) = 0;

        /** Close audio file and updates audio header.
            @return false on failure. (ie file not open)
         */
        virtual bool close() = 0;

        /** Read from audio file.
            @param data return param preallocationed memory location for 
                        new data
            @param max size of preallocationed memory location in bytes
            @param pSeqNum rtp sequence number of new data
            @param pTs rtp time stamp of new data
            @return bytes of new data read
         */
        virtual int read( void* data, int max,
                          unsigned short* pSeqNum, unsigned int* pTS ) = 0;        

        /** Write to audio file.
            @param data new data to write 
            @param max size of new data in bytes
            @param uSeqNum rtp sequence number of new data
            @param uTs rtp time stamp of new data
            @return bytes of new data written
         */
        virtual int write( void* data, int max,
                           unsigned short uSeqNum = 0,
                           unsigned int uTS = 0 ) = 0;

        /** Seek read/write pointer into file.
            @param seekToMs new read/write pointer location
            @param whence relative seek
                          [<code>SEEK_SET</code>|<code>SEEK_CUR</code>]
            @return actual read/write pointer location in milliseconds
         */
        virtual long seek( const long seekToMs, const int whence ) = 0;


        /** File extension string.
            @return file extension (ie "*.au")
         */
        virtual string fileExtensionString() const = 0;

        /** Filename string.  Used for rtsp messages.
            @return file name with rtsp file path and extension.
                    (ie "/samples/foo.au")
         */
        Data filename() const
            {
                Data result = myShortFilename;
                result += fileExtensionString();
                return result;
            }

        /** Local filename string.  Used for local file access.
            @return filename with local file path and extension.
                    (ie "/usr/local/sounds/foo.au")
         */
        Data localFilename() const
            {
                Data result = RtspConfiguration::instance().audioDirectory;
                if( result.length() )
                {
                    result += "/";
                }
                result += filename();
                return result;
            }

        /** Filename sting only.  Used for printing or debug messages.
            @return filename without extension
                    (ie "foo")
         */
        const string& shortFilename()
            { return myShortFilename; }


    public:
        /** Writes rtspSdp to *.sdp file.
            @param rtspSdp sdp block
            @return false on failure
         */
        bool writeSdpFile( RtspSdp& rtspSdp );

        /** Reads rtspSdp from *.sdp file.
            @param rtspSdp return param of sdp block
            @return false on failure
         */
        bool readSdpFile( RtspSdp& rtspSdp );

        /** Append data to *.sdp file.  Data should end with "\n".
            @param data new line to be writen to file
            @param len length of new data
            @return false on failure
         */
        bool appendSdpFile( const char* data, const int len );

        /** Delete line from *.sdp file.  (TODO function not completed!)
            @param data beginning of which line to delete (null terminated)
            @return false on failure (ie can't match line)
         */
        bool deleteLineSdpFile( const char* data );


    protected:
        /** filename only (stripped of codec string and file extension) */
        string myShortFilename;

        /** play/rec/unknown file modes */
        RtspFileMode myFileMode;

        /** codec parameters of audio file (index based on RtpFileTypes) */
        int myFtIndex;

        /** Updates sdp file with new audio length.
            Called while closing audio file. */
        void updateSdpFileRange();

    private:
        /** suppress copy constructor */
        RtspFile( const RtspFile& rhs );
        /** suppress assignment operator */
        RtspFile& operator=( const RtspFile& rhs );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
