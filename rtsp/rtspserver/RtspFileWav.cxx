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

static const char* const RtspFileWav_cxx_version =
    "$Id: RtspFileWav.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspFileWav.hxx"
#include "RtpFileTypes.hxx"


RtspFileWav::RtspFileWav(const string& shortFilename)
    : RtspFile( shortFilename ),
      m_xAudioFile( 0 )
{
    myFileMode = RTSP_FILE_MODE_UNKNOWN;
}


RtspFileWav::~RtspFileWav()
{
    close();
}


bool
RtspFileWav::saveHeader( const int ftIndex, RtspSdp& rtspSdp )
{
    // check filemap contains full codec information
    if( rtpFileTypeInfo[ftIndex].WavFileFormat == 0x0000 )
    {
        cpLog( LOG_ERR, "Incomplete rtpFileTypeInfo map for codec %s",
               rtpFileTypeInfo[ftIndex].name.c_str() );
        return false;
    }

    mySfInfo.format = SF_FORMAT_WAV | SF_FORCE_OPEN;
    mySfInfo.format_tag = rtpFileTypeInfo[ftIndex].WavFileFormat;
    mySfInfo.samplerate = 8000;
    mySfInfo.channels = 1;
    mySfInfo.pcmbitwidth = 16;
    mySfInfo.samples = 0;
    mySfInfo.sections = 1;
    mySfInfo.seekable = 1;
    mySfInfo.bytewidth = rtpFileTypeInfo[ftIndex].bytewidth;

    //TODO add WAVEFORMATEX extra bytes here
    mySfInfo.avg_bytes_per_sec = (int)( (
                                  (float)rtpFileTypeInfo[ftIndex].packetSize /
                                  (float)rtpFileTypeInfo[ftIndex].intervalMs ) *
                                  1000.0 );
    cpLog( LOG_DEBUG, "mySfInfo.avg_bytes_per_sec %d", mySfInfo.avg_bytes_per_sec );
    if( mySfInfo.avg_bytes_per_sec > 500 )
    {
        cpLog( LOG_DEBUG, "fixing mySfInfo.avg_bytes_per_sec to 0" );
        mySfInfo.avg_bytes_per_sec = 0;
    }

    LocalScopeAllocator lo;

    // save sf_info to file
    SNDFILE* xAudioFile = sf_open_write( localFilename().getData(lo), &mySfInfo );
    if( !xAudioFile )
    {
        cpLog( LOG_DEBUG, "sf_open_write %s - failed while saving header",
               localFilename().getData(lo) );
        return false;
    }
    dummyWrite( xAudioFile, rtpFileTypeInfo[ftIndex].packetSize );

    if( cpLogGetPriority() >= LOG_DEBUG )
    {
        printSfInfo();
    }

    // close audio file
    sf_close( xAudioFile );

    return true;
}


bool
RtspFileWav::loadHeader( int* ftIndex, int* lengthInMs )
{
    *ftIndex = 0;
    *lengthInMs = 0;

    // read sf_info from file
    memset( &mySfInfo, 0, sizeof( mySfInfo ) );
    mySfInfo.format = SF_FORCE_OPEN;
    LocalScopeAllocator lo;

    SNDFILE* xAudioFile = sf_open_read( localFilename().getData(lo), &mySfInfo );
    if( !xAudioFile )
    {
        char errbuff[80];
        sf_error_str( xAudioFile, errbuff, 80 );
        cpLog( LOG_ERR, "sf_open_read %s - failed: %s ",
               localFilename().getData(lo), errbuff );
        return false;
    }

    if( ( mySfInfo.format & SF_FORMAT_TYPEMASK ) != SF_FORMAT_WAV )
    {
        cpLog( LOG_ERR, "Audio file header doesn't match file extension" );
        sf_close( xAudioFile );
        return false;
    }
    
    // read codec number from sf_info
    *ftIndex = 0;
    for( int i = 0; i< myNumberOfCodecs; i++ )
    {
        if( rtpFileTypeInfo[i].WavFileFormat == mySfInfo.format_tag )
        {
            *ftIndex = i;
            cpLog( LOG_DEBUG, "Read index %d from audio map", i );
            break;
        }
    }
    if( *ftIndex == 0 )
    {
        cpLog( LOG_DEBUG, "Wav codec %d not found", mySfInfo.format_tag );
        if( cpLogGetPriority() >= LOG_DEBUG )
        {
            printSfInfo();
        }
        sf_close( xAudioFile );
        return false;
    }

    //dirty hack of libsndfile
    cpLog( LOG_DEBUG, "Filling incomplete SF_INFO structure" );
    mySfInfo.pcmbitwidth = 16;
    mySfInfo.bytewidth = rtpFileTypeInfo[*ftIndex].bytewidth;
    sf_set_bytewidth( xAudioFile, &mySfInfo );

    sf_close( xAudioFile );

    if( cpLogGetPriority() >= LOG_DEBUG )
    {
        printSfInfo();
    }

    // calculate file length
    if( rtpFileTypeInfo[*ftIndex].bytewidth > 2 )
    {
        *lengthInMs = int( mySfInfo.samples *
                           rtpFileTypeInfo[*ftIndex].intervalMs );
    }
    else
    {
        *lengthInMs = int( mySfInfo.samples /
                           ( (float)rtpFileTypeInfo[*ftIndex].packetSize /
                             (float)rtpFileTypeInfo[*ftIndex].intervalMs ) );
    }

    cpLog( LOG_DEBUG, "File length is %d ms", *lengthInMs );

    return true;
}


bool
RtspFileWav::open( bool bReadWrite, int ftIndex )
{
    if( !bReadWrite ) //read mode
    {
        LocalScopeAllocator lo;

        m_xAudioFile = sf_open_read( localFilename().getData(lo), &mySfInfo );
        if( !m_xAudioFile )
        {
            cpLog( LOG_ERR, "sf_open_read %s - failed",
                   localFilename().getData(lo) );
            return false;
        }

        cpLog( LOG_ERR, "Filling incomplete SF_INFO structure:" );
        //dirty hack of libsndfile
        mySfInfo.pcmbitwidth = 16;
        mySfInfo.bytewidth = rtpFileTypeInfo[ftIndex].bytewidth;
        sf_set_bytewidth( m_xAudioFile, &mySfInfo );

        myFileMode = RTSP_FILE_MODE_PLAY;
    }
    else //write mode
    {
        mySfInfo.format = SF_FORMAT_WAV | SF_FORCE_OPEN;
        mySfInfo.bytewidth = rtpFileTypeInfo[ftIndex].bytewidth;

        LocalScopeAllocator lo;

        m_xAudioFile = sf_open_write( localFilename().getData(lo), &mySfInfo );
        if( !m_xAudioFile )
        {
            cpLog( LOG_ERR, "sf_open_write %s - failed",
                   localFilename().getData(lo) );
            return false;
        }
        dummyWrite( m_xAudioFile, rtpFileTypeInfo[ftIndex].packetSize );

        myFileMode = RTSP_FILE_MODE_REC;
    }
    return true;
}


bool
RtspFileWav::close()
{
    if( m_xAudioFile )
    {
        if( sf_close( m_xAudioFile ) != 0 )
        {
            cpLog( LOG_ERR, "Error in sf_close()" );
        }
        m_xAudioFile = 0;

        updateSdpFileRange();

        return true;
    }

    return false;
}


int
RtspFileWav::read( void* data, int max, 
                   unsigned short* pSeqNum, unsigned int* pTS )
{
    return sf_read_raw( m_xAudioFile, data, max );
}


int
RtspFileWav::write( void* data, int max, 
                    unsigned short uSeqNum, unsigned int uTS )
{
    return sf_write_raw( m_xAudioFile, data, max );
}


long
RtspFileWav::seek( const long samples, const int whence )
{
    return sf_seek( m_xAudioFile, samples, whence );
}


void
RtspFileWav::printSfInfo() const
{
    printf( "    Sample Rate : %d\n",   mySfInfo.samplerate );
    printf( "    Samples     : %d\n",   mySfInfo.samples );
    printf( "    Channels    : %d\n",   mySfInfo.channels );
    printf( "    Bit Width   : %d\n",   mySfInfo.pcmbitwidth );
    printf( "    Format      : 0x%x\n", mySfInfo.format );
    printf( "    Format_tag* : 0x%x\n", mySfInfo.format_tag );
    printf( "    Bytewidth*  : %d\n",   mySfInfo.bytewidth );
    printf( "    Sections    : %d\n",   mySfInfo.sections );
    printf( "    Seekable    : %d\n",   mySfInfo.seekable );
    printf( "    Avg_bytes   : %d\n",   mySfInfo.avg_bytes_per_sec );
}


void
RtspFileWav::dummyWrite( SNDFILE* xAudioFile, const int dummyLen )
{
    // write some dummy data
    cpLog( LOG_DEBUG, "Writing %d bytes of dummy data", dummyLen );
    char dummyBuffer[dummyLen];
    memset( dummyBuffer, 0, dummyLen );
    if( (int)sf_write_raw( xAudioFile, dummyBuffer, dummyLen ) != dummyLen )
    {
        cpLog( LOG_DEBUG, "sf_write_raw result error %d", dummyLen );
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
