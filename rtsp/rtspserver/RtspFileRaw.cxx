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

static const char* const RtspFileRaw_cxx_version =
    "$Id: RtspFileRaw.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspFileRaw.hxx"
#include "RtpFileTypes.hxx"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


RtspFileRaw::RtspFileRaw( const string& shortFilename )
    : RtspFile( shortFilename ),
      m_xAudioFile( 0 )
{
    myFileMode = RTSP_FILE_MODE_UNKNOWN;

    // read codec from filename - has default value if none found
    string name = shortFilename;
    string::size_type pos = name.find_last_of( '-' );
    if( pos != string::npos )
    {
        // some type of codec string was found
        bool success = false;
        string codecString2 = name.substr( pos+1, string::npos );
        cpLog( LOG_DEBUG, "Codec string %s", codecString2.c_str());
        for( int i = 0; i< myNumberOfCodecs; i++ )
        {
            if( rtpFileTypeInfo[i].name == codecString2 )
            {
                myFtIndex = i;
                name = name.substr( 0, pos );
                success = true;
                break;
            }
        }
        if( !success )
        {
            // non codec string was found
            cpLog( LOG_DEBUG, "Non-codec string found, using default value" );
            myFtIndex = RtspConfiguration::instance().defaultCodec;
        }
    }
    else
    {
        // no codec string was found
        cpLog( LOG_DEBUG, "No codec string found, using default value" );
        myFtIndex = RtspConfiguration::instance().defaultCodec;
    }

    // save stripped name
    myShortFilename = name;
}


RtspFileRaw::~RtspFileRaw()
{
    close();
}


bool
RtspFileRaw::saveHeader( const int ftIndex, RtspSdp& rtspSdp )
{
    //TODO
    if( myFtIndex != ftIndex )
    {
        cpLog( LOG_ERR, "%d %d", myFtIndex, ftIndex );
    }

    LocalScopeAllocator lo;

// RtspFileHandler will call this exact code if SDP_FILE defined
#ifndef SDP_FILE
    if( !writeSdpFile( rtspSdp ) )
    {
        cpLog( LOG_ERR, "Error in writeSdpFile()" );
        return false;
    }
#ifndef REAL_AUDIO
    Data controlData = "a=control:rtsp://";
    //TODO need to get this data into this class
    //controlData += filenameUrl();
    controlData += "\r\n";
    deleteLineSdpFile( "a=control:" );

    appendSdpFile( controlData.getData(lo), controlData.length() );
#endif
#endif


    FILE* xAudioFile = fopen( localFilename().getData(lo), "wb" );
    if( !xAudioFile )
    {
        cpLog( LOG_DEBUG, "fopen %s - failed while saving header",
               localFilename().getData(lo) );
        return false;
    }
    dummyWrite( xAudioFile, rtpFileTypeInfo[ftIndex].packetSize );

    // close audio file
    fclose( xAudioFile );

    return true;
}


bool
RtspFileRaw::loadHeader( int* ftIndex, int* lengthInMs )
{
    LocalScopeAllocator lo;
    // begin kim
    *ftIndex = myFtIndex;
    *lengthInMs = 0;
    // end kim
/* // begin mera
        if(myFtIndex)
        *ftIndex=myFtIndex;
    else
        myFtIndex=*ftIndex;


    FILE* audioFile = fopen(filename().getData(lo),"rb");
    if( !audioFile )
    {
        cpLog( LOG_ERR, "fopen %s - failed ", filename().getData(lo));
        return false;
    }
    fclose( audioFile );
    // end mera
*/

    // read total file length
    struct stat ifstat;
    if( stat( localFilename().getData(lo), &ifstat ) )
    {
        cpLog( LOG_ERR, "Error in stat()" );
        return false;
    }

    // calculate file length
    *lengthInMs = ( (int)( (float)ifstat.st_size /
                    (float)rtpFileTypeInfo[*ftIndex].packetSize ) ) *
                  rtpFileTypeInfo[*ftIndex].intervalMs;

    cpLog( LOG_DEBUG, "File length is %d ms", *lengthInMs );

    return true;
}


bool
RtspFileRaw::open( bool bReadWrite, int ftIndex )
{
    LocalScopeAllocator lo;

    if( !bReadWrite ) // read mode
    {
        m_xAudioFile = fopen( localFilename().getData(lo), "rb" );
        if( !m_xAudioFile )
        {
            cpLog( LOG_ERR, "fopen %s - failed",
                   localFilename().getData(lo) );
            return false;
        }

       myFileMode = RTSP_FILE_MODE_PLAY;
    }
    else // write mode
    {
        m_xAudioFile = fopen( localFilename().getData(lo), "wb" );
        if( !m_xAudioFile )
        {
            cpLog( LOG_ERR, "fopen %s - failed",
                   localFilename().getData(lo) );
            return false;
        }

        myFileMode = RTSP_FILE_MODE_REC;
    }
    return true;
}


bool
RtspFileRaw::close()
{
    if( m_xAudioFile )
    {
        if( fclose( m_xAudioFile ) != 0 )
        {
            cpLog( LOG_ERR, "Error in fclose()" );
        }
        m_xAudioFile = 0;

        updateSdpFileRange();

        return true;
    }

    return false;
}


int
RtspFileRaw::read( void* data, int max, 
                   unsigned short* pSeqNum, unsigned int* pTS )
{
    int cc = fread( data, max, 1, m_xAudioFile );
    if( cc > 0 )
    {
        cc = max;
    }

    return cc;
}


int
RtspFileRaw::write( void* data, int max, 
                    unsigned short uSeqNum, unsigned int uTS )
{
    int cc = fwrite( data, max, 1, m_xAudioFile );
    if( cc > 0 )
    {
        cc = max;
    }

    return cc;
}


long
RtspFileRaw::seek( long samples, int whence )
{
    long seek_size = ( (long)( (float)samples /
                       (float)rtpFileTypeInfo[myFtIndex].intervalMs ) ) *
                     rtpFileTypeInfo[myFtIndex].packetSize;
    return fseek( m_xAudioFile, seek_size, whence );
}


void
RtspFileRaw::dummyWrite( FILE* xAudioFile, const int dummyLen )
{
    // write some dummy data
    cpLog( LOG_DEBUG, "Writing %d bytes of dummy data", dummyLen );
    char dummyBuffer[dummyLen];
    memset( dummyBuffer, 0, dummyLen );
    if( !fwrite( dummyBuffer, dummyLen, 1, xAudioFile ) )
    {
        cpLog( LOG_DEBUG, "fwrite result error %d", dummyLen );
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
