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


#include <stdlib.h>

#include "global.h"
#include "Data.hxx"
#include "support.hxx"

#include "Sdp2Exceptions.hxx"
#include "Sdp2Media.hxx"

using namespace Vocal::SDP;


const int SdpMedia::MaxFormatIntValue = 15;


///
SdpMedia::SdpMedia()
{
    mediaType = MediaTypeAudio;
    mediaTypeString = "audio";
    port = -1;
    numPorts = 0;
    transportType = TransportTypeRTP;
    formatList.clear();
    formatList.push_back(0);
    mediaAttrib = 0;
    connection = 0;
    bandwidth = 0;
    encryptkey = 0;
    mediaInfo = "";
}    // SdpMedia::SdpMedia


SdpMedia::~SdpMedia() {
   if (mediaAttrib) {
      delete mediaAttrib;
   }
   if (connection) {
      delete connection;
   }
   if (bandwidth) {
      delete bandwidth;
   }
   if (encryptkey) {
      delete encryptkey;
   }
}//destructor


///
SdpMedia::SdpMedia (list<Data>& mediaDetails)
{
    //initialize all pointers
    bandwidth = 0;
    mediaAttrib = 0;
    connection = 0;
    encryptkey = 0;
    list<Data>::iterator linecount;

    linecount = mediaDetails.begin();
    //print Media details
    list<Data>::iterator tempcount;
    tempcount = mediaDetails.begin();
    while (tempcount != mediaDetails.end())
    {
        cpLog(LOG_DEBUG_STACK, "SdpMedia: %s", (*tempcount).logData());
        tempcount++;
    }

    while (linecount != mediaDetails.end())
    {
        if ((*linecount)[0] == 'm')
        {
            Data lineStr = (*linecount);
	    lineStr.parse("=");

            list<Data> mList;
	    bool finished = false;
	    while(!finished) 
	    {
		Data x = lineStr.parse(" ", &finished);
		if(finished)
		{
		    x = lineStr;
		}
		mList.push_back(x);
	    }

            if (mList.size() < 4)
            {
                cpLog(LOG_ERR, "SdpMedia: m= size < 4");
                throw SdpExceptions(PARAM_NUMERR);
            }
            else
	    {
                cpLog(LOG_DEBUG_STACK, "SdpMedia: m= size >4");
	    }

            //parse media items.
            list<Data>::iterator mListIter;

            mListIter = mList.begin();

            mediaTypeString = *mListIter;
            if (mediaTypeString == Data("audio"))
            {
                mediaType = MediaTypeAudio;
                cpLog(LOG_DEBUG_STACK, "Found m=audio");
            }
            else if (mediaTypeString == Data("video"))
            {
                mediaType = MediaTypeVideo;
                cpLog(LOG_DEBUG_STACK, "Found m=video");
            }
            else
            {
                mediaType = MediaTypeUnknown;
                cpLog( LOG_DEBUG_STACK, "Found m=%s", mediaTypeString.logData() );
            }

            ++mListIter;
            /** port may be <port>/<num of ports>
                So, split mListIter again on /  */
            Data portDetails = (*mListIter);

            list<Data> portList;

	    finished = false;
	    while(!finished) 
	    {
		Data x = portDetails.parse("/", &finished);
		if(finished)
		{
		    x = portDetails;
		}
		portList.push_back(x);
	    }

	    if(portList.begin() != portList.end())
	    {
		LocalScopeAllocator lPort;
		port = strtol((*portList.begin()).getData(lPort), (char**)NULL, 10);
		cpLog(LOG_DEBUG_STACK, "SDP port: %d " , port);
	    }

            if (portList.size() == 2)
            {
                //contains the number of ports.
		LocalScopeAllocator lNumPorts;
                numPorts = atoi((++portList.begin())->getData(lNumPorts));
            }
            else
            {
                numPorts = 0;
            }

            ++mListIter;

            if (*mListIter == SdpMediaTransportRTPAVP)
            {
                transportType = TransportTypeRTP;
            }
            else if (*mListIter == SdpMediaTransportUDP)
            {
                transportType = TransportTypeUDP;
            }

            //handle the format list.
            formatList.clear();
            ++mListIter;
            while (mListIter != mList.end())
            {
                //these are all format list items
                cpLog( LOG_DEBUG_STACK, "Media format: %s", (*mListIter).c_str() );
                formatList.push_back( atoi((*mListIter).c_str()) );

                ++mListIter;
            }

        }// if line = m.
        else if ((*linecount)[0] == 'a')
        {
            try
            {
                if (!mediaAttrib)
                {
                    //create the MediaAttribute object.
                    mediaAttrib = new MediaAttributes();
                    assert(mediaAttrib);
                }
                Data attrib = (*linecount);
		attrib.parse("=");
		
//                chomp (&attrib);
                mediaAttrib->setAttribute(attrib);
            }
            catch (SdpExceptions& )
            {
                throw SdpExceptions(FAILED);
            }
        }
        else if ((*linecount)[0] == 'b')
        {
            try
            {
                Data bw = (*linecount);
		bw.parse("=");

//                bw.erase(0, 2);
//                chomp (&bw);
                cpLog(LOG_DEBUG_STACK, "Bandwidth, before sending to C'tor:%s", bw.logData());
                bandwidth = new SdpBandwidth(bw);
                assert(bandwidth);
            }
            catch (SdpExceptions& )
            {
                throw SdpExceptions(FAILED);
            }
        }
        else if ((*linecount)[0] == 'k')
        {
            try
            {
                if (encryptkey)
                {
                    delete encryptkey;
                    encryptkey = 0;
                }

                Data key = (*linecount);

		key.parse("=");

//                key.erase(0, 2);
//                chomp (&key);
                cpLog(LOG_DEBUG, "Encryption key, before sending to C'tor:%s", key.logData());
                encryptkey = new SdpEncryptkey(key);
                assert(encryptkey);
            }
            catch (SdpExceptions& )
            {
                throw SdpExceptions(FAILED);
            }
        }
        else if ((*linecount)[0] == 'c')
        {
            try
            {
                Data conn = (*linecount);
		conn.parse("=");
//                conn.erase(0, 2);
//                chomp (&conn);

                connection = new SdpConnection(conn);
                assert(connection);
            }
            catch (SdpExceptions& )
            {
                throw SdpExceptions(FAILED);
            }
        }
        else if ((*linecount)[0] == 'i')
        {
            Data info = (*linecount);
	    info.parse("=");
//            info.erase(0, 2);
//            chomp (&info);

            mediaInfo = info;
        }
        else if ((*linecount)[0] == '\r' || (*linecount)[0] == '\n' )
        {
            //TODO make sure this is ok
            cpLog( LOG_DEBUG_STACK, "Empty line" );
        }
        else
        {
            cpLog(LOG_ERR, "Unknown field in the media portion %s",
                  (*linecount).logData());
            throw SdpExceptions(UNKNOWN_FIELD);
        }
        linecount++;
    }    // while
}    // SdpMedia::SdpMedia(list<Data>& mediaDetails)


///
const SdpMedia&
SdpMedia::operator=(SdpMedia& newMedia)
{
    mediaType = newMedia.mediaType;
    mediaTypeString = newMedia.mediaTypeString;
    port = newMedia.port;
    numPorts = newMedia.numPorts;
    transportType = newMedia.transportType;
    mediaInfo = newMedia.mediaInfo;
    cpLog(LOG_DEBUG_STACK, "Copied media, port, numPorts, transportType");

    vector < int > * newFormatList = newMedia.getFormatList();
    assert(newFormatList);

    formatList = *newFormatList;
    cpLog(LOG_DEBUG_STACK, "SdpMedia: copied the format list");

    if (newMedia.mediaAttrib)
    {
        if (!mediaAttrib)
        {
            mediaAttrib = new MediaAttributes;
            assert(mediaAttrib);
        }
        *(mediaAttrib) = *(newMedia.mediaAttrib);
        //copy parent details.
//        *((SdpAttributes*)(mediaAttrib)) = *((SdpAttributes*)(newMedia.mediaAttrib));
        cpLog(LOG_DEBUG_STACK, "SdpMedia: Copied Media attributes");
    }
    else //if not defined now, but previously defined, remove previous definition.
    {
        if (mediaAttrib)
        {
            delete mediaAttrib;
            mediaAttrib = 0;
        }
    }
    if (newMedia.connection)
    {
        if (!connection)
        {
            connection = new SdpConnection;
            assert(connection);
        }
        *(connection) = *(newMedia.connection);
        cpLog(LOG_DEBUG_STACK, "SdpMedia: copied connection");
    }
    else
    {
        if (connection)
        {
            delete connection;
            connection = 0;
        }
    }
    if (newMedia.bandwidth)
    {
        if (!bandwidth)
        {
            bandwidth = new SdpBandwidth;
            assert(bandwidth);
        }
        *(bandwidth) = *(newMedia.bandwidth);
        cpLog(LOG_DEBUG_STACK, "SdpMedia:copied bandwidth");
    }
    else
    {
        if (bandwidth)
        {
            delete bandwidth;
            bandwidth = 0;
        }
    }
    if (newMedia.encryptkey)
    {
        if (!encryptkey)
        {
            encryptkey = new SdpEncryptkey;
            assert(encryptkey);
        }
        *(encryptkey) = *(newMedia.encryptkey);
        cpLog(LOG_DEBUG, "SdpEncryptkey:copied encryptkey");
    }
    else
    {
        if (encryptkey)
        {
            delete encryptkey;
            encryptkey = 0;
        }
    }
    return *(this);
}


///
Data
SdpMedia::transportTypeString ()
{
    Data s;

    switch (transportType)
    {
        case TransportTypeRTP:
        {
            s = SdpMediaTransportRTPAVP;
            break;
        }
        case TransportTypeUDP:
        {
            s = SdpMediaTransportUDP;
            break;
        }
        default:
        {
            //TODO Throw some exception
            break;
        }
    }
    return s;
}    // SdpMedia::transportTypeString


///
void
SdpMedia::encodeMediaAnnouncement (ostrstream& s)
{
    s << "m="
      << mediaTypeString
      << ' '
      << port;

    if (numPorts > 0)
    {
        s << '/' << numPorts;
    }
    s << ' ' << transportTypeString();

    vector < int > ::iterator iter = formatList.begin();
    while (iter != formatList.end())
    {
        s << ' ' << *iter;
        ++iter;
    }
    s << "\r\n";
}    // SdpMedia::encodeMediaAnnouncement


///
void
SdpMedia::setTransportType(TransportType newtransport)
{
    //TODO relax these checks?
    switch (newtransport)
    {
        case TransportTypeRTP:
        if (((port > 1023) && (port < 65536)) && (0 == (port % 2)))
            transportType = newtransport;
        else
            cpLog(LOG_ERR, "for RTP, port: %d should be within 1024 - 65536 and even", port);
        break;
        case TransportTypeUDP:
        if ((port > 1023) && (port < 65536))
            transportType = newtransport;
        else
            cpLog(LOG_ERR, "for UDP, port: %d should be within 1024 - 65536", port);
        break;
        default:
        cpLog(LOG_ERR, "Unknown transport type %d", newtransport);
    }
}


///
void
SdpMedia::encode (ostrstream& s)
{
    // m= media name and transport address
    encodeMediaAnnouncement (s);

    // i=* media title
    if (mediaInfo.length() > 0)
    {
        s << "i=" << mediaInfo << "\r\n";
    }

    // c=* connection information (not required if included at session-level)
    if (connection)
    {
        connection->encode (s);
    }

    // b=* bandwidth information
    if (bandwidth)
    {
        bandwidth->encode (s);
    }

    // k=* encryption key
    if (encryptkey)
    {
        encryptkey->encode (s);
    }

    // a=* zero or more session attribute lines
    if (mediaAttrib)
    {
        mediaAttrib->encode (s);
    }
}    // SdpMedia::encode


// payload_type would be like the 97 in:  a=rtmap:97 speex/8000
// Returns >= 0 on success, < 0 on failure.
int SdpMedia::getRtpmapAttributeValue(int payload_type, SdpRtpMapAttribute& rv) {
   if (mediaAttrib) {
      return mediaAttrib->getRtpmapAttributeValue(payload_type, rv);
   }
   return -1;
}


void
SdpMedia::setMediaType( SdpMediaType newType )
{
    switch( newType )
    {
        case MediaTypeAudio:
        {
            mediaTypeString = "audio";
        }
        break;
        case MediaTypeVideo:
        {
            mediaTypeString = "video";
        }
        break;
        default:
        {
            cpLog( LOG_ERR, "Unknown media type %d", newType );
            mediaTypeString = "unknown";
        }
    }
}    // SdpMedia::setMediaType


void
SdpMedia::setMediaType( Data newType )
{
    mediaTypeString = newType;
    if( newType == "audio" )
    {
        mediaType = MediaTypeAudio;
    }
    else if( newType == "video" )
    {
        mediaType = MediaTypeVideo;
    }
    else
    {
        mediaType = MediaTypeUnknown;
    }
}    // SdpMedia::setMediaType
