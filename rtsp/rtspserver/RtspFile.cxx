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

static const char* const RtspFile_cxx_version =
    "$Id: RtspFile.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspFile.hxx"

#include <sys/stat.h>
#include <unistd.h>

bool
RtspFile::writeSdpFile( RtspSdp& rtspSdp )
{
    string sdpFilename = RtspConfiguration::instance().audioDirectory;
    if( !sdpFilename.empty() )
    {
        sdpFilename += "/";
    }
    sdpFilename += myShortFilename;
    sdpFilename += ".sdp";

    cpLog( LOG_DEBUG, "Writing sdp to descriptor file: %s", sdpFilename.c_str() );

    /*
    FILE* fSdpDescFile = fopen( sdpFilename.c_str(), "rb" );
    if( fSdpDescFile )
    {
        cpLog( LOG_ERR, "Sdp file already exists" );
        cpLog( LOG_ERR, "Hacked to overwrite existing sdp file" );
        //return false;
    }
    fclose( fSdpDescFile );

    fSdpDescFile = fopen( sdpFilename.c_str(), "wb" );
    */

    FILE* fSdpDescFile = fopen( sdpFilename.c_str(), "wb+" );
    if( !fSdpDescFile )
    {
        cpLog( LOG_ERR, "Fail to open sdp file" );
        return false;
    }

    Data newSdpData = rtspSdp.encode();

    LocalScopeAllocator lo;

    int writeRes = fwrite( newSdpData.getData(lo), newSdpData.length(),
                           1, fSdpDescFile );
    fclose( fSdpDescFile );
    if( writeRes <= 0 )
    {
        cpLog( LOG_DEBUG, "Fail to write sdp data to file" );
        return false;
    }

    return true;
}


bool
RtspFile::readSdpFile( RtspSdp& rtspSdp )
{
    string sdpFilename = RtspConfiguration::instance().audioDirectory;
    if( !sdpFilename.empty() )
    {
        sdpFilename += "/";
    }
    sdpFilename += myShortFilename;
    sdpFilename += ".sdp";

    FILE* fSdpDescFile = fopen( sdpFilename.c_str(), "rb" );
    if( !fSdpDescFile )
    {
        // failure is ok, RtspFileHandler will try reading
        // audio header for sdp information
        return false;
    }

    cpLog( LOG_DEBUG, "Generating sdp from descriptor file: %s",
           sdpFilename.c_str() );

    // read information from sdp file
    struct stat ifstat;
    if( stat( sdpFilename.c_str(), &ifstat ) )
    {
        cpLog( LOG_ERR, "Error in stat()" );
        return false;
    }
    int iSdpFileSize = ifstat.st_size;

    char* tmpBuff = new char[iSdpFileSize+1];
    fread( (void*)tmpBuff, iSdpFileSize, 1, fSdpDescFile );
    fclose( fSdpDescFile );
    tmpBuff[iSdpFileSize] = '\0';

    cpLog(LOG_DEBUG, "Sdp file length is: %d", iSdpFileSize);
    cpLog(LOG_DEBUG, "Sdp file has data: %s", tmpBuff);

    Data dataTmp(tmpBuff, iSdpFileSize);
    delete [] tmpBuff;

    // build sdp object
    rtspSdp.decode( dataTmp );
    if( !rtspSdp.isValid() )
    {
        cpLog( LOG_ERR, "Sdp data from file is invalid" );
        return false;
    }

    return true;
}


bool
RtspFile::appendSdpFile( const char* data, const int len )
{
#ifndef SDP_FILE
    return true;
#endif

    string sdpFilename = RtspConfiguration::instance().audioDirectory;
    if( !sdpFilename.empty() )
    {
        sdpFilename += "/";
    }
    sdpFilename += myShortFilename;
    sdpFilename += ".sdp";

    cpLog( LOG_DEBUG, "Appending data to descriptor file: %s", sdpFilename.c_str() );

    FILE* fSdpDescFile = fopen( sdpFilename.c_str(), "ab" );
    if( !fSdpDescFile )
    {
        cpLog( LOG_DEBUG, "Error opening descriptor file with append mode" );
        return false;
    }

    fwrite( data, len, 1, fSdpDescFile );
    fclose( fSdpDescFile );

    return true;
}


bool
RtspFile::deleteLineSdpFile( const char* data )
{
#ifndef SDP_FILE
    return true;
#endif

    //TODO read entire sdp file.  If any line begin
    //     with data, then delete that line.
    //     return true if a line was deleted
    return false;
}


void
RtspFile::updateSdpFileRange()
{
#ifndef SDP_FILE
    return;
#endif

    if( myFileMode == RTSP_FILE_MODE_REC )
    {
        // reload updated header
        int ftIndex;
        int lengthInMs;
        loadHeader( &ftIndex, &lengthInMs );

        // write to sdp file
        char rangeBuffer[20];
        sprintf( rangeBuffer, "a=range:npt=0.0-%.3f\r\n",
                 lengthInMs / 1000.0 );
        deleteLineSdpFile( "a=range:" );
        appendSdpFile( rangeBuffer, strlen( rangeBuffer) );
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
