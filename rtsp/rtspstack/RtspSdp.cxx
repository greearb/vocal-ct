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

static const char* const RtspSdp_cxx_Version =
    "$Id: RtspSdp.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspSdp.hxx"
#include "SdpMedia.hxx"
#include "SdpExceptions.hxx"
#include "cpLog.h"
#include <list>

//using Vocal::SDP::SdpSession;
//using Vocal::SDP::SdpConnection;
//using Vocal::SDP::SdpMedia;
//using Vocal::SDP::SdpRtpMapAttribute;
//using Vocal::SDP::SdpAttributes;
//using Vocal::SDP::ValueAttribute;
//using Vocal::SDP::MediaAttributes;

RtspSdp::RtspSdp()
{
    // TODO trick sdp that it's valid
    mySdpSession.setVersion( 0 );
}


RtspSdp::RtspSdp( const Data& contentData )
{
    decode( contentData );
}

/*
RtspSdp&
RtspSdp::operator=(const RtspSdp& src)
{
    if (&src != this )
    {
        mySdpSession = src.mySdpSession;
    }
    return (*this);
}
*/

void
RtspSdp::decode( const Data& contentData )
{
    mySdpSession.decode( contentData.convertString() );
    if( !mySdpSession.isValidDescriptor() )
    {
        Data content2 = "v=0\r\no=- 12345 12345 IN IP4 0.0.0.0\r\ns=Vovida RTSP Server\r\nt=0 0\r\n";
        content2 += contentData;

        cpLog( LOG_DEBUG, "Building sdp object from partial contentData\n%s",
               content2.logData() );
        mySdpSession.decode( content2.convertString() );
        if( !mySdpSession.isValidDescriptor() )
        {
            cpLog(LOG_ERR, "Sdp data is not valid");
            return;
        }
    }
}

void
RtspSdp::setConnAddress(const Data& connAddr)
{
    SdpConnection sdpConn;
    LocalScopeAllocator lo;

    sdpConn.setUnicast(connAddr.getData(lo));
    mySdpSession.setConnection(sdpConn);
}

void
RtspSdp::setOwnerAddress(const Data& ownerAddr)
{
    LocalScopeAllocator lo;
    mySdpSession.setAddress(ownerAddr.getData(lo));
}

void
RtspSdp::setUserName(const Data& userName)
{
    LocalScopeAllocator lo;
    mySdpSession.setUserName(userName.getData(lo));
}

void
RtspSdp::setSessionName(const Data& sessionName)
{
    LocalScopeAllocator lo;

    mySdpSession.setSessionName(sessionName.getData(lo));
}

void
RtspSdp::addAttribute(const char* attribute, const char* value)
{
    SdpAttributes* attributes = mySdpSession.getAttribute();
    ValueAttribute* attrib = new ValueAttribute();
    attrib->setAttribute( attribute );
    attrib->setValue( value );
    if ( attributes == 0 )
    {
        attributes = new SdpAttributes();
        assert( attributes );
        mySdpSession.setAttribute(attributes);
    }
    attributes->addValueAttribute( attrib );
}

void
RtspSdp::setMediaPort(int port)
{
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() == 0)
    {
        SdpMedia* media = new SdpMedia;
        //due to SdpMedia has default format 0
        media->clearFormatList();
        mySdpSession.addMedia (media);
        mediaList = mySdpSession.getMediaList();
    }
    (*mediaList.begin())->setPort(port);
}

void
RtspSdp::setMediaConnAddress(const Data& connAddr)
{
    SdpConnection sdpConn;
    LocalScopeAllocator lo;
    sdpConn.setUnicast(connAddr.getData(lo));
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() == 0)
    {
        SdpMedia* media = new SdpMedia;
        //due to SdpMedia has default format 0
        media->clearFormatList();
        mySdpSession.addMedia (media);
        mediaList = mySdpSession.getMediaList();
    }
    (*mediaList.begin())->setConnection(sdpConn);
}

void
RtspSdp::addMediaAttribute(const char* attribute, const char* value)
{
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() == 0)
    {
        SdpMedia* media = new SdpMedia;
        //due to SdpMedia has default format 0
        media->clearFormatList();
        mySdpSession.addMedia (media);
        mediaList = mySdpSession.getMediaList();
    }
    ValueAttribute* attrib = new ValueAttribute();
    attrib->setAttribute( attribute );
    attrib->setValue( value );
    MediaAttributes* mediaAttrib = (*mediaList.begin())->getMediaAttributes();
    if ( mediaAttrib == 0 )
    {
        mediaAttrib = new MediaAttributes();
        assert( mediaAttrib );
        (*mediaList.begin())->setMediaAttributes( mediaAttrib );
    }
    mediaAttrib->addValueAttribute( attrib );
}

void
RtspSdp::addMediaFormat(const int payloadType)
{
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() == 0)
    {
        SdpMedia* media = new SdpMedia;
        //due to SdpMedia has default format 0
        media->clearFormatList();
        mySdpSession.addMedia (media);
        mediaList = mySdpSession.getMediaList();
    }
    (*mediaList.begin())->addFormat(payloadType);
}

void
RtspSdp::addRtpmap(const int payloadType,
                                const Data& encodeName,
                                const int cloackRate)
{
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() == 0)
    {
        SdpMedia* media = new SdpMedia;
        //due to SdpMedia has default format 0
        media->clearFormatList();
        mySdpSession.addMedia (media);
        mediaList = mySdpSession.getMediaList();
    }
    SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
    rtpMapAttrib->setPayloadType( payloadType );
    LocalScopeAllocator lo;
    rtpMapAttrib->setEncodingName( encodeName.getData(lo) );
    rtpMapAttrib->setClockRate( cloackRate );
    MediaAttributes* mediaAttrib = (*mediaList.begin())->getMediaAttributes();
    if ( mediaAttrib == 0 )
    {
        mediaAttrib = new MediaAttributes();
        assert( mediaAttrib );
        (*mediaList.begin())->setMediaAttributes( mediaAttrib );
    }
    mediaAttrib->addmap( rtpMapAttrib );
}

Data
RtspSdp::getConnAddress()
{
    SdpConnection* conn = mySdpSession.getConnection();
    if (conn)
    {
        return Data( conn->getUnicast() );
    }
    
    return "";
}

Data
RtspSdp::getOwnerAddress()
{
    return Data(mySdpSession.getAddress());
}

Data
RtspSdp::getUserName()
{
    return Data(mySdpSession.getUserName());
}

Data
RtspSdp::getSessionName()
{
    return Data(mySdpSession.getSessionName());
}

Data
RtspSdp::getAttributeValue(const char* attribute)
{
    Data value;

    SdpAttributes* attributes = mySdpSession.getAttribute();
    if (attributes != 0)
    {
        vector < ValueAttribute* > * valueAttribList = 
            attributes->getValueAttributes();
    
        if ( valueAttribList->size() == 0 )
        {
            cpLog( LOG_DEBUG, "No value attribute in the sdp session" );
        }
        else
        {
            vector < ValueAttribute* > ::iterator attribIterator = 
                valueAttribList->begin();
    
            while ( attribIterator != valueAttribList->end() )
            {
                char* attribName = (*attribIterator)->getAttribute();
                if ( strcmp( attribName, attribute ) == 0 )
                {
                    value = Data( (*attribIterator)->getValue() );
                    break;
                }
                attribIterator++;
            }
        }
    }
    else
    {
        cpLog( LOG_DEBUG, "No session level attribute in this sdp" );
    }
    return value;
}

int
RtspSdp::getMediaPort()
{
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() == 0)
    {
        cpLog(LOG_DEBUG, "There is no mediaList.");
        return 0;
    }
    return (*mediaList.begin())->getPort();
}

Data
RtspSdp::getMediaConnAddress()
{
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() > 0)
    {
        SdpConnection* conn = (*mediaList.begin())->getConnection();
        if (conn)
        {
            return Data( conn->getUnicast() );
        }
    }
    
    return "";
}

Data
RtspSdp::getMediaAttributeValue(const char* attribute)
{
    Data value;

    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() > 0)
    {
        MediaAttributes* mediaAttrib = (*mediaList.begin())->getMediaAttributes();
        if ( mediaAttrib != 0 )
        {
            vector < ValueAttribute* > * valueAttribList = 
                mediaAttrib->getValueAttributes();
    
            if ( valueAttribList->size() == 0 )
            {
                cpLog( LOG_DEBUG, "No value attribute in the first media" );
            }
            else
            {
                vector < ValueAttribute* > ::iterator attribIterator = 
                    valueAttribList->begin();
    
                while ( attribIterator != valueAttribList->end() )
                {
                    char* attribName = (*attribIterator)->getAttribute();
                    if ( strcmp( attribName, attribute ) == 0 )
                    {
                        value = Data( (*attribIterator)->getValue() );
                        break;
                    }
                    attribIterator++;
                }
            }
        }
        else
        {
            cpLog( LOG_DEBUG, "No media attributes in first SdpMedia" );
        }
    }
    else
    {
        cpLog( LOG_DEBUG, "No media in this sdp" );
    }
    return value;
}

vector < int > *
RtspSdp::getMediaFormatList()
{
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() > 0)
    {
        return (*mediaList.begin())->getFormatList();
    }
    else
    {
        cpLog(LOG_DEBUG, "no formatList.");
        return 0;
    }
}

vector < SdpRtpMapAttribute* > *
RtspSdp::getRtpmapList()
{
    vector < SdpMedia* > mediaList = mySdpSession.getMediaList();
    if (mediaList.size() > 0)
    {
        if ((*mediaList.begin())->getMediaAttributes() == 0)
        {
            cpLog(LOG_DEBUG, "media attribute is empty");
            return 0;
        }
        return ((*mediaList.begin())->getMediaAttributes())->getmap();
    }
    else
    {
        cpLog(LOG_DEBUG, "media list is empty");
        return 0;
    }
}

void RtspSdp::getCodecNCodecString( int& sdpCodec, string& sdpCodecString )
{
    if( isValid() )
    {
        if(getMediaFormatList())
        {
            // read codec number
            sdpCodec = getMediaFormatList()->front();
            // read codec string
            SdpRtpMapAttribute* rtpMapAttribute = 0;
            vector<SdpRtpMapAttribute*>* rtpMapList = getRtpmapList();
            if( rtpMapList != 0 )
            {
                rtpMapAttribute = rtpMapList->front();
                assert( rtpMapAttribute );
                if( sdpCodec == rtpMapAttribute->getPayloadType() &&
                    8000 == rtpMapAttribute->getClockRate() )
                {
                    sdpCodecString = rtpMapAttribute->getEncodingName();
                    cpLog(LOG_DEBUG, "sdpCodecString is %s",
                          sdpCodecString.c_str());
                }
                else
                {
                    cpLog( LOG_ERR, "Sdp data is incorrect in codec(%d/%d) or clock rate(%d)",
                           sdpCodec, rtpMapAttribute->getPayloadType(),
                           rtpMapAttribute->getClockRate() );
                }
            }
            else
            {
                cpLog(LOG_ERR, "sdp has not rtpMap list.");
            }
        }
        else
        {
            cpLog(LOG_ERR, "sdp has no media list.");
        }
    }
    else
    {
        cpLog(LOG_ERR, "sdp is not valid.");
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
