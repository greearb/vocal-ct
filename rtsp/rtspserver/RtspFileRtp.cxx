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

static const char* const RtspFileRtp_cxx_version =
    "$Id: RtspFileRtp.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspFileRtp.hxx"
#include "RtpFileTypes.hxx"
#include "RtspConfiguration.hxx"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


RtspFileRtp::RtspFileRtp( const string& shortFilename )
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


RtspFileRtp::~RtspFileRtp()
{
    close();
}


bool
RtspFileRtp::saveHeader( const int ftIndex, RtspSdp& rtspSdp )
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
    dummyWrite( xAudioFile, rtpFileTypeInfo[ftIndex].packetSize);

    // close audio file
    fclose( xAudioFile );

    return true;
}


bool
RtspFileRtp::loadHeader( int* ftIndex, int* lengthInMs )
{
    *ftIndex = myFtIndex;
    *lengthInMs = 0;

    LocalScopeAllocator lo;

    // read packet size from first rtp packet
    int dummyBufLen = 2000;
    char dummyBuf[dummyBufLen];
    unsigned short seqNum;
    unsigned int ts;
    m_xAudioFile = fopen( localFilename().getData(lo),"rb" );
    if( !m_xAudioFile )
    {
        cpLog( LOG_ERR, "fopen %s - failed ",
               localFilename().getData(lo));
        return false;
    }
    int cc = read( dummyBuf, dummyBufLen, &seqNum, &ts );
    fclose( m_xAudioFile );
    m_xAudioFile = 0;
    if( cc < 0 )
    {
        cpLog( LOG_ERR, "error reading first rtp packet from file" );
        return false;
    }


    // read total file length
    struct stat ifstat;
    if( stat( localFilename().getData(lo), &ifstat ) )
    {
        cpLog( LOG_ERR, "Error in stat()" );
        return false;
    }

    // calculate file length
    *lengthInMs = int( (float)ifstat.st_size /
                       ( cc +
                         4 + /*length*/
                         2 + /*sequence number*/
                         4 /*time stamp*/ )
                     );
    *lengthInMs = *lengthInMs * rtpFileTypeInfo[*ftIndex].intervalMs;

    cpLog( LOG_DEBUG, "File lengths is %d ms", *lengthInMs );

    return true;
}


bool
RtspFileRtp::open( bool bReadWrite, int ftIndex )
{
    LocalScopeAllocator lo;

    if( !bReadWrite ) //read mode
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
    else //write mode
    {
        m_xAudioFile = fopen( localFilename().getData(lo), "wb" );
        if( !m_xAudioFile )
        {
            cpLog( LOG_ERR, "fopen %s - failed",
                   filename().getData(lo) );
            return false;
        }

        myFileMode = RTSP_FILE_MODE_REC;
    }
    return true;
}


bool
RtspFileRtp::close()
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
RtspFileRtp::read( void* data, int max, 
                   unsigned short* pSeqNum, unsigned int* pTS )
{
    const unsigned int MaxRtpPacketSize=2000;

    unsigned short tempSeqNum;
    unsigned int tempTS;
    //if(pSeqNum==0 || pTS==0) return -1;
    if(pSeqNum==0) pSeqNum=&tempSeqNum;
    if(pTS==0) pTS=&tempTS;

    unsigned int uRtpBlockSize;
    int cc = fread((void*)&uRtpBlockSize, 4, 1, m_xAudioFile);
    if(cc<=0)
    {
        cpLog(LOG_DEBUG, "Can't read Rtp file" );
        return -1;
    }
    uRtpBlockSize=ntohl(uRtpBlockSize);
    if(uRtpBlockSize>MaxRtpPacketSize)
    {
        cpLog(LOG_DEBUG, "Can't read Rtp file - incorrect size of Rtp packet (%d)\n", uRtpBlockSize);
        return -1;
    }
    if(uRtpBlockSize>(unsigned(max+2+4))) // 2 bytes for SeqNum and 4 bytes for TS
    {
        cpLog(LOG_DEBUG, "Read buffer(%d) not large enough for this RTP packet(%d)\n",max, uRtpBlockSize);
        return -1;
    }
    cc = fread((void*)pSeqNum, 2, 1, m_xAudioFile);
    if(cc<=0)
    {
        cpLog(LOG_DEBUG, "Can't read Rtp file (SeqNum)" );
        return -1;
    }
    *pSeqNum=ntohs(*pSeqNum);
    cc = fread((void*)pTS, 4, 1, m_xAudioFile);
    if(cc<=0)
    {
        cpLog(LOG_DEBUG, "Can't read Rtp file (TS)" );
        return -1;
    }
    *pTS=ntohl(*pTS);
    cc = fread( data, uRtpBlockSize-2-4, 1, m_xAudioFile);
    if(cc>0) cc=uRtpBlockSize-2-4;

    return cc;

/*
    if( pSeqNum==0 || pTS==0 ) return -1;
    int uRtpBlockSize;
    int cc = fread((void*)&uRtpBlockSize, 4, 1, m_xAudioFile);
    uRtpBlockSize=ntohl(uRtpBlockSize);
    if(cc<=0) return -1;
    if(uRtpBlockSize>max+2+4) // 2 bytes for SeqNum and 4 bytes for TS
    {
        cpLog(LOG_DEBUG, "Read buffer(%d) not large enough for this RTP packet(%d)",max, uRtpBlockSize);
        return -1;
    }
    cc = fread((void*)&pSeqNum, 2, 1, m_xAudioFile);
    if(cc<=0) return -1;
    cc = fread((void*)&pTS, 4, 1, m_xAudioFile);
    if(cc<=0) return -1;
    cc = fread( data, uRtpBlockSize-2-4, 1, m_xAudioFile);
    if(cc>0) cc=max;

    return cc;
*/
}


int
RtspFileRtp::write( void* data, int max, 
                        unsigned short uSeqNum, unsigned int uTS )
{
    unsigned int uRtpBlockLen=max+2+4; //2 bytes for SeqNum + 4 bytes for TS
    uRtpBlockLen=htonl(uRtpBlockLen);
    //uSeqNum=htons(uSeqNum);
    //uTS=htonl(uTS);
    int result = fwrite( (void*)&uRtpBlockLen, 4, 1, m_xAudioFile);
    if(result>0) result = fwrite( (void*)&uSeqNum, 2, 1, m_xAudioFile);
    else       return -1; // TODO add debug info
    if(result>0) result = fwrite( (void*)&uTS, 4, 1, m_xAudioFile);
    else       return -1; // TODO add debug info
    result = fwrite( data, max, 1, m_xAudioFile);
    result = result>0?(int)max:-1;

    return result;   
}


long
RtspFileRtp::seek( long samples, int whence )
{
    long packets = (long)( (float) samples /
                           (float) rtpFileTypeInfo[myFtIndex].intervalMs );
    long seek_size = packets * ( rtpFileTypeInfo[myFtIndex].packetSize +
                                 4 + /*size*/
                                 2 + /*SeqNum*/
                                 4   /*TS*/ );

    return fseek( m_xAudioFile, seek_size, whence );
}


void
RtspFileRtp::dummyWrite( FILE* xAudioFile, const int dummyLen )
{
    unsigned int uRtpBlockLen = dummyLen + 2 + 4; //2 bytes for SeqNum + 4 bytes for TS
    uRtpBlockLen = htonl( uRtpBlockLen );

    // write some dummy data
    cpLog( LOG_DEBUG, "Writing %d bytes of dummy data", dummyLen );
    char dummyBuffer[dummyLen];
    memset( dummyBuffer, 0, dummyLen );

    if( fwrite( (void*)&uRtpBlockLen, 4, 1, xAudioFile ) <=0 )
    {
        cpLog( LOG_DEBUG, "fwrite error ");
    }

    if( fwrite( dummyBuffer, 2+4, 1, xAudioFile ) <=0 )
    {
        cpLog( LOG_DEBUG, "fwrite error ");
    }

    if( fwrite( dummyBuffer, dummyLen, 1, xAudioFile ) <=0 )
    {
        cpLog( LOG_DEBUG, "fwrite error ");
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
