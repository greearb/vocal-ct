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

static const char* const RtspFileHandler_cxx_version =
    "$Id: RtspFileHandler.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "RtspFileHandler.hxx"
#include "RtspConfiguration.hxx"
#include "RtspFileManager.hxx"
#include "RtpFileTypes.hxx"
#include "RtspRangeHdr.hxx"

#include "RtspFileAu.hxx"
#include "RtspFileWav.hxx"
#include "RtspFileRaw.hxx"
#include "RtspFileRtp.hxx"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


//using Vocal::SDP::SdpRtpMapAttribute;

RtspFileHandler::RtspFileHandler( const Data& filename, const Data& contentBase )
    : myContentBase ( ),
      myFileExtension( UNKNOWN_EXT ),
      myRtspSdp(),
      m_xRtspFile( 0 ),
      myReadWrite( 0 ),
      myFtIndex( 0 ),
      myCodec( -1 ),
      myPacketInterval( -1 ),
      myPacketSize( -1 ),
      myDynamicCodec( -1 )
{
    LocalScopeAllocator lo;
    myContentBase = contentBase.getData(lo);
    cpLog( LOG_DEBUG, "File handler created for %s", filename.logData() );

    string name = filename.getData(lo);

    while(1)
    {
        // determine file extension
        // has default value if none found
        myFileExtension = UNKNOWN_EXT;
        string::size_type size = name.size();
        if( size>2 && name[size-3]=='.' )
        {
            if( ( (name[size-2]=='a') || (name[size-2]=='A') ) &&
                ( (name[size-1]=='u') || (name[size-1]=='U') )   )
            {
                myFileExtension = AU_EXT;
                name = name.substr( 0, size-3 );
            }
        }
        else
        {
            if(size>3 && name[size-4]=='.')
            {
                if( ( (name[size-3]=='r') || (name[size-3]=='R') ) &&
                    ( (name[size-2]=='a') || (name[size-2]=='A') ) &&
                    ( (name[size-1]=='w') || (name[size-1]=='W') )   )
                {
                    myFileExtension = RAW_EXT;
                    name = name.substr( 0, size-4 );
                }
                else if( ( (name[size-3]=='r') || (name[size-3]=='R') ) &&
                         ( (name[size-2]=='t') || (name[size-2]=='T') ) &&
                         ( (name[size-1]=='p') || (name[size-1]=='P') )   )
                {
                    myFileExtension = RTP_EXT;
                    name = name.substr( 0, size-4 );
                }
                else if( ( (name[size-3]=='w') || (name[size-3]=='W') ) &&
                         ( (name[size-2]=='a') || (name[size-2]=='A') ) &&
                         ( (name[size-1]=='v') || (name[size-1]=='V') )   )
                {
                    myFileExtension = WAV_EXT;
                    name = name.substr( 0, size-4 );
                }
            }
        }
        if( myFileExtension == UNKNOWN_EXT )
        {
            // real networks hack
            // try strip junk like streamid=0
            string::size_type pos = name.find( "/streamid", string::npos );
            if( pos != string::npos )
            {
                name=name.substr( 0, pos );
                cpLog( LOG_DEBUG, "Stripped name: %s", name.c_str());
            }
            else
            {
                cpLog( LOG_DEBUG, "Setting file extension to default value" );
                myFileExtension = (RtspFileHandler::eExtension)
                    ( RtspConfiguration::instance().defaultFileExtension );
                break;
            }
        }
        else
        {
            break;
        }
    }

    switch( myFileExtension )
    {
        case AU_EXT:
        {
            cpLog( LOG_DEBUG, "Opening %s as *.au", name.c_str() );
            m_xRtspFile = new RtspFileAu( name );
            assert( m_xRtspFile );
            break;
        }
        case WAV_EXT:
        {
            cpLog( LOG_DEBUG, "Opening %s as *.wav", name.c_str() );
            m_xRtspFile = new RtspFileWav( name );
            assert( m_xRtspFile );
            break;
        }
        case RAW_EXT:
        {
            cpLog( LOG_DEBUG, "Opening %s as *.raw", name.c_str() );
            m_xRtspFile = new RtspFileRaw( name );
            assert( m_xRtspFile );
            break;
        }
        case RTP_EXT:
        {
            cpLog( LOG_DEBUG, "Opening %s as *.rtp", name.c_str() );
            m_xRtspFile = new RtspFileRtp( name );
            assert( m_xRtspFile );
            break;
        }
        default:
            assert( 0 );
    }
}


RtspFileHandler::~RtspFileHandler()
{
}


bool
RtspFileHandler::writeAudioHeader( const Data& sdp )
{
    // build sdp object
    myRtspSdp.decode( sdp );
    if( !myRtspSdp.isValid() )
    {
        cpLog( LOG_ERR, "Sdp block is invalid" );
        return false;
    }

    if(!myRtspSdp.getMediaFormatList())
    {
        cpLog( LOG_ERR, "No media in Sdp" );
        return false;
    }

    // read codec number
    int codec = myRtspSdp.getMediaFormatList()->front();

    // read codec string
    string codecString = "UNKNOWN";
    SdpRtpMapAttribute* rtpMapAttribute = 0;
    vector<SdpRtpMapAttribute*>* rtpMapList = myRtspSdp.getRtpmapList();
    if( rtpMapList == 0 )
    {
        if( RtspConfiguration::instance().rtpMapRequired )
        {
            cpLog( LOG_ERR, "RtpMap not found" );
            return false;
        }
        else
        {
            cpLog( LOG_DEBUG, "Assuming rtp map values" );
        }
    }
    else
    {
        rtpMapAttribute = rtpMapList->front();
        assert( rtpMapAttribute );
        if( codec != rtpMapAttribute->getPayloadType() ||
            8000 != rtpMapAttribute->getClockRate() )
        {
            cpLog( LOG_ERR, "Sdp data is incorrect in codec(%d/%d) or clock rate(%d)",
                   codec, rtpMapAttribute->getPayloadType(),
                   rtpMapAttribute->getClockRate() );
            return false;
        }
        codecString = rtpMapAttribute->getEncodingName();
    }

    // reindex myFtIndex with codec name from sdp
    // find exact name matchin first if possible
    bool supported = false;
    for( int i = 0; i< myNumberOfCodecs; i++ )
    {
        if( rtpFileTypeInfo[i].payloadType == codec &&
            rtpFileTypeInfo[i].name == codecString )
        {
            myFtIndex = i;
            supported = true;
            break;
        }
    }

    // do first codec matching if no name can be matched
    if( !supported && !RtspConfiguration::instance().rtpMapRequired )
    {
        for( int i = 0; i< myNumberOfCodecs; i++ )
        {
            if( rtpFileTypeInfo[i].payloadType == codec )
            {
                myFtIndex = i;
                supported = true;
                break;
            }
        }
    }

    // support for dynamic payload type, name need to be matched
    if ( !supported &&
         ((codec >= rtpPayloadDynMin) && (codec <= rtpPayloadDynMax)) )
    {
        for( int i = 0; i< myNumberOfCodecs; i++ )
        {
            if( rtpFileTypeInfo[i].name == codecString )
            {
                myFtIndex = i;
                supported = true;
                break;
            }
        }
    }

    if( !supported )
    {
        cpLog( LOG_DEBUG, "Codec %d/%s not found in RtpFileTypeInfo map",
               codec, codecString.c_str() );
        return false;
    }

    // add rtpmap if none was set
    if( rtpMapAttribute == 0 )
    {
        codecString = rtpFileTypeInfo[myFtIndex].name;
        myRtspSdp.addRtpmap( codec, codecString.c_str(), 8000 );

        vector<SdpRtpMapAttribute*>* rtpMapList = myRtspSdp.getRtpmapList();
        assert( rtpMapList);
        rtpMapAttribute = rtpMapList->front();
    }

    cpLog( LOG_DEBUG, "Codec %d/%s index as %d",
           codec, codecString.c_str(), myFtIndex );

#ifdef SDP_FILE
    if( !m_xRtspFile->writeSdpFile( myRtspSdp ) )
    {
        cpLog( LOG_ERR, "Error in writeSdpFile()" );
        return false;
    }
#ifndef REAL_AUDIO
    Data controlData = "a=control:rtsp://";
    controlData += filenameUrl();
    controlData += "\r\n";
    m_xRtspFile->deleteLineSdpFile( "a=control:" );
    LocalScopeAllocator lo;

    m_xRtspFile->appendSdpFile( controlData.getData(lo), controlData.length() );
#endif
#endif

    // create audio file with audio header
    cpLog( LOG_DEBUG, "Saving new audio file header" );
    if( !m_xRtspFile->saveHeader( myFtIndex, myRtspSdp ) )
    {
        cpLog( LOG_DEBUG, "Failed to write audio file header" );
        return false;
    }

    return true;
}


bool RtspFileHandler::loadAudioHeader( Sptr<RtspMsg> msg )
{
    LocalScopeAllocator lo;

    cpLog( LOG_DEBUG, "Generating sdp for audio file %s",
           m_xRtspFile->filename().getData(lo) );

#ifdef SDP_FILE
    if( m_xRtspFile->readSdpFile( myRtspSdp ) )
    {
        // read codec number
        vector<int>* formatList = myRtspSdp.getMediaFormatList();
        if( !formatList )
        {
            return false;
        }
        int codec = formatList->front();

        // read codec string
        SdpRtpMapAttribute* rtpMapAttribute =
                            myRtspSdp.getRtpmapList()->front();
        if( codec != rtpMapAttribute->getPayloadType() ||
            8000 != rtpMapAttribute->getClockRate() )
        {
            cpLog( LOG_ERR, "Sdp data is incorrect in codec or clock rate" );
            return false;
        }
        string codecString = rtpMapAttribute->getEncodingName();
        
        // reindex myFtIndex with codec name from sdp
        // find exact name matchin first if possible
        bool supported = false;
        for( int i = 0; i< myNumberOfCodecs; i++ )
        {
            if( rtpFileTypeInfo[i].payloadType == codec &&
                rtpFileTypeInfo[i].name == codecString )
            {
                myFtIndex = i;
                supported = true;
                break;
            }
        }

        // do first codec matching if no name can be matched
        if( !supported && !RtspConfiguration::instance().rtpMapRequired )
        {
            for( int i = 0; i< myNumberOfCodecs; i++ )
            {
                if( rtpFileTypeInfo[i].payloadType == codec )
                {
                    myFtIndex = i;
                    supported = true;
                    break;
                }
            }
        }

        // support for dynamic payload type, name need to be matched
        if ( !supported &&
             ((codec >= rtpPayloadDynMin) && (codec <= rtpPayloadDynMax)) )
        {
            for( int i = 0; i< myNumberOfCodecs; i++ )
            {
                if( rtpFileTypeInfo[i].name == codecString )
                {
                    myFtIndex = i;
                    supported = true;
                    break;
                }
            }
        }

        if( !supported )
        {
            cpLog( LOG_DEBUG, "Codec %d/%s not found in RtpFileTypeInfo map",
                   codec, codecString.c_str() );
            return false;
        }
        cpLog( LOG_DEBUG, "Codec %d/%s index as %d",
           codec, codecString.c_str(), myFtIndex );

        //TODO update myLength from "a=range:ntp=0.0-10.0"

        return true;
    }
#endif

    // get codec and codecString from the sdp block
    int sdpCodec = -1;
    string sdpCodecString = "UNKNOWN";
    if (msg->getMsgBody().length() > 0)
    {
        RtspSdp rtspSdp( msg->getMsgBody() );
        rtspSdp.getCodecNCodecString( sdpCodec, sdpCodecString );
    }
    else
    {
        cpLog(LOG_DEBUG, "no sdp info.");
    }

    // read information from audio file
    if( !m_xRtspFile->loadHeader( &myFtIndex, &myLength ) )
    {
        cpLog( LOG_ERR, "Fail to load header info from audio file" );
        return false;
    }

    // compare sdpCodec and the audio file codec info
    if (sdpCodec >= rtpPayloadDynMin && sdpCodec <= rtpPayloadDynMax)
    {
       if (sdpCodecString == rtpFileTypeInfo[myFtIndex].name)
       {
           myDynamicCodec = sdpCodec;
           cpLog(LOG_DEBUG, "dynamic Codec is: %d", myDynamicCodec);
       }
       else
       {
           cpLog(LOG_DEBUG, "sdp codec string is not match with audio file.");
       }
    }

    cpLog( LOG_DEBUG, "Sdp was created from audio header" );

    // build sdp block
    int codec = rtpFileTypeInfo[myFtIndex].payloadType;
    myRtspSdp.setSessionName( RtspConfiguration::instance().serverName );
    myRtspSdp.setMediaPort( 0 );
    if (( myDynamicCodec > 0 ) &&
        ((codec == rtpPayloadG726_16) ||
        (codec == rtpPayloadG726_24) ||
        (codec == rtpPayloadGSMEFR)) )
    {
        myRtspSdp.addMediaFormat(myDynamicCodec);
        myRtspSdp.addRtpmap( myDynamicCodec,
                             rtpFileTypeInfo[myFtIndex].name, 8000 );
    }
    else
    {

        myRtspSdp.addMediaFormat( codec );
        myRtspSdp.addRtpmap( codec,
                             rtpFileTypeInfo[myFtIndex].name, 8000 ); 
    }
#ifndef REAL_AUDIO
    Data controlData = "rtsp://";
    controlData += filenameUrl();

    myRtspSdp.addMediaAttribute( "control", controlData.getData(lo) );
#endif

    char rangeBuffer[20];
    float nptEnd = myLength / 1000.0;
    sprintf( rangeBuffer, "npt=0.0-%.3f", nptEnd ); 
    myRtspSdp.addMediaAttribute( "range", rangeBuffer );

    return true;
}


RtspSdp&
RtspFileHandler::readAudioHeader()
{
    if( myFtIndex == 0 )
    {
        cpLog( LOG_ERR, "myFtIndex was unset" );
        assert(0);
    }
    return myRtspSdp;
}


bool
RtspFileHandler::openAudioFile( bool mode, int packetSize )
{
    myReadWrite = mode;

    if( myFtIndex == 0 || myLength == 0 )
    {
        /* why did mera want this?
        if( !loadAudioHeader() )
        {
            cpLog( LOG_ERR, "Fail to load header info from audio header" );
            return false;
        }
        */

        if( !m_xRtspFile->loadHeader( &myFtIndex, &myLength) )
        {
            cpLog( LOG_ERR, "Fail to load header info from audio file" );
            return false;
        }
    }

    // open audio file
    if( !myReadWrite ) //read mode
    {
        cpLog( LOG_DEBUG, "Opening %s for reading",
               m_xRtspFile->filename().logData() );

        // check file is unlocked
        if( RtspConfiguration::instance().lockRecordingFiles )
        {
            LocalScopeAllocator lo;

            if( RtspFileManager::instance().find(
                m_xRtspFile->filename().getData(lo) ) )
            {
                cpLog( LOG_ERR, "File is locked" );
                return false;
            }
        }

        // open audio file
        if( !m_xRtspFile->open( myReadWrite, myFtIndex ) )
        {
            cpLog( LOG_ERR, "open for read %s - failed",
                   m_xRtspFile->filename().logData() );
            return false;
        }
    }
    else  //write mode
    {
        cpLog( LOG_DEBUG, "Opening %s for writing",
               m_xRtspFile->filename().logData() );;

        // lock audio file
        if( RtspConfiguration::instance().lockRecordingFiles )
        {
            LocalScopeAllocator lo;
    
            if( !RtspFileManager::instance().add(
                m_xRtspFile->filename().getData(lo) ) )
            {
                cpLog( LOG_ERR, "File already locked" );
                return false;
            }
        }

        // open audio file
        if( !m_xRtspFile->open( myReadWrite, myFtIndex ) )
        {
            cpLog( LOG_ERR, "Opening %s for writing - failed",
                   m_xRtspFile->filename().logData() );
            return false;
        }
    }
    
    // set private data members for rtp processor
    myCodec = rtpFileTypeInfo[myFtIndex].payloadType;
    if( packetSize <= 0 )
    {
        packetSize = rtpFileTypeInfo[myFtIndex].packetSize;
    }
    else
    {
        // reindex myFtIndex with desired packet size
        while( ( myFtIndex < myNumberOfCodecs ) &&
               ( rtpFileTypeInfo[myFtIndex].payloadType == myCodec ) &&
               ( rtpFileTypeInfo[myFtIndex].packetSize != packetSize ) )
        {
            myFtIndex++;
        }
        if( ( rtpFileTypeInfo[myFtIndex].packetSize != packetSize ) ||
            ( rtpFileTypeInfo[myFtIndex].payloadType != myCodec ) )
        {
            cpLog( LOG_ERR, "Codec %d doesn't support packetSize %d",
                   myCodec, packetSize );
        }
    }

    myPacketInterval = rtpFileTypeInfo[myFtIndex].intervalMs;
    myPacketSize = rtpFileTypeInfo[myFtIndex].packetSize;
    cpLog( LOG_DEBUG, "Setting packetSize to %d for %d ms interval",
           myPacketSize, myPacketInterval );

    //seek to beginning
    if( m_xRtspFile->seek( 0, SEEK_SET ) < 0 )
    {
        cpLog( LOG_ERR, "Fail to seek to beginning of file" );
    }

    return true;
}


bool
RtspFileHandler::closeAudioFile()
{
    cpLog( LOG_DEBUG, "Closing audio file %s",
           m_xRtspFile->filename().logData() );

    m_xRtspFile->close();

    if( myReadWrite )  // recording mode
    {
        // unlock audio file
        if( RtspConfiguration::instance().lockRecordingFiles )
        {
            LocalScopeAllocator lo;

            if( !RtspFileManager::instance().del(
                m_xRtspFile->filename().getData(lo) ) )
            {
                cpLog( LOG_ERR, "File was not locked" );
            }
        }
    }

    return true;
}


int
RtspFileHandler::read( void* data, size_t max,
                       unsigned short* pSeqNum, unsigned int* pTS )
{
    if( myPacketSize > (int)max )
    {
        cpLog( LOG_DEBUG,
               "Read buffer(%d) not large enough for given codec(%d) and packet size (%d)",
               max, myCodec, myPacketSize );
        return -1;
    }

    int cc = m_xRtspFile->read( data, myPacketSize, pSeqNum, pTS );

#ifdef INFINITE_READ
    if( cc < myPacketSize )
    {
        cpLog( LOG_DEBUG_HB, "Infinite read hack" );
        m_xRtspFile->seek( 0, SEEK_SET );
        cc = m_xRtspFile->read( data, myPacketSize, pSeqNum, pTS );
        if( cc < myPacketSize )
        {
            cpLog( LOG_ERR, "Infinite read hack didn't work" );
            assert(0);
            return -1;
        }
    }
#endif

    if( cpLogGetPriority() >= LOG_DEBUG_HB )
    {
        cerr<<"r";
    }

    return cc;
}


int RtspFileHandler::write( void* data, size_t max,
                            unsigned short uSeqNum, unsigned int uTS )
{
#ifdef DUMB_WRITE
    int cc = max;
#else
    int cc = m_xRtspFile->write( data, max, uSeqNum, uTS );
#endif
    
    if( cpLogGetPriority() >= LOG_DEBUG_HB )
    {
        cerr<<"w";
    }

    return cc;
}


long RtspFileHandler::seek( long npt )
{
    //long samples = (int)( (float)npt * rtpFileTypeInfo[myFtIndex].sizePerMs 
    //               / (float)rtpFileTypeInfo[myFtIndex].blockSize );
    long samples = npt / rtpFileTypeInfo[myFtIndex].intervalMs;
    long result = m_xRtspFile->seek( samples, SEEK_SET );

    cpLog( LOG_DEBUG, "Seeking to %dms result in %dms", npt, result );

    return result;
}


Data 
RtspFileHandler::filename() const
{
    return m_xRtspFile->filename();
}


Data
RtspFileHandler::filenameUrl() const
{
    Data result = myContentBase;
    result += m_xRtspFile->filename();
    return result;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
