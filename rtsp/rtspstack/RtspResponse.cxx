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

static const char* const RtspResponse_cxx_Version =
    "$Id: RtspResponse.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <string.h>
#include <stdio.h>
#include "cpLog.h"
#include "RtspResponse.hxx"
#include "RtspBadDataException.hxx"

RtspResponse::RtspResponse()
    : RtspMsg(),
      myRtspRequest(0),
      myStatusCodeType(RTSP_NULL_STATUS),
      myStatusCode(0),
      myFileLocation(0)
{
}

RtspResponse::RtspResponse(Sptr< RtspRequest > rtspRequest,
                           RtspStatusCodesType statusCode)
    : RtspMsg(),
      myRtspRequest(rtspRequest),
      myStatusCodeType(statusCode),
      myStatusCode(0),
      myFileLocation(0)
{
    assert(rtspRequest != 0);

    myTransConnPtr = rtspRequest->getTransConnPtr();

    setStatusLine();
    appendCSeqHdr(rtspRequest);
    appendSessionHdr(rtspRequest);
}

RtspResponse::RtspResponse( const RtspResponse& src)
    : RtspMsg(src),
      myRtspRequest(0),
      myStatusCodeType(src.myStatusCodeType),
      myStatusCode(src.myStatusCode),
      myFileLocation(0)
{
    if (src.myRtspRequest != 0)
    {
        myRtspRequest = new RtspRequest();
        *myRtspRequest = *src.myRtspRequest;
    }
    if (src.myFileLocation != 0)
    {
        myFileLocation = new RtspLocationHdr();
        *myFileLocation = *src.myFileLocation;
    }
}

RtspResponse&
RtspResponse::operator= (const RtspResponse& src)
{
    if (&src != this)
    {
        RtspMsg::operator=(src);

        myStatusCodeType = src.myStatusCodeType;
        myStatusCode = src.myStatusCode;
        myRtspRequest = 0;
        myFileLocation = 0;

        if (src.myRtspRequest != 0)
        {
            myRtspRequest = new RtspRequest();
            *myRtspRequest = *src.myRtspRequest;
        }
        if (src.myFileLocation != 0)
        {
            myFileLocation = new RtspLocationHdr();
            *myFileLocation = *src.myFileLocation;
        }
    }
    return (*this);
}

void
RtspResponse::setStatusLine()
{
    char tmpBuf[64];
    u_int32_t len = 0;
    CharData version = RtspUtil::getVersion();
    CharData statusCode = RtspUtil::getStatusCodeInString(myStatusCodeType);
    CharData statusString = RtspUtil::getStatusInString(myStatusCodeType);
    u_int32_t versionLen = version.getLen();
    u_int32_t statusCodeLen = statusCode.getLen();
    u_int32_t statusStringLen = statusString.getLen();

    memcpy(tmpBuf, version.getPtr(), versionLen);
    len = versionLen;
    tmpBuf[len++] = ' ';
    memcpy(tmpBuf + len, statusCode.getPtr(), statusCodeLen);
    len += statusCodeLen;
    tmpBuf[len++] = ' ';
    memcpy(tmpBuf + len, statusString.getPtr(), statusStringLen);
    len += statusStringLen;
    memcpy(tmpBuf + len, "\r\n", 2);
    len += 2;

    Data statusLine(tmpBuf, len);
    myStartLine = statusLine;
}

void
RtspResponse::appendCSeqHdr(Sptr< RtspRequest > rtspRequest)
{
    if ( (rtspRequest->getCSeq()).length() > 0)
    {
        myHeaders += "CSeq: ";
        myHeaders += rtspRequest->getCSeq();
        myHeaders += "\r\n";
    }
}

void
RtspResponse::appendSessionHdr(Sptr< RtspRequest > rtspRequest)
{
    if ( (rtspRequest->getSessionId()).length() > 0)
    {
        myHeaders += "Session: ";
        myHeaders += rtspRequest->getSessionId();
        myHeaders += "\r\n";
    }
}

void
RtspResponse::appendSessionId(u_int32_t sessionId)
{
    char tmpBuf[32];
    sprintf(tmpBuf, "%d", sessionId);

    myHeaders += "Session: ";
    myHeaders += tmpBuf;
    myHeaders += "\r\n";
}

void
RtspResponse::appendTransportHdr(Sptr< RtspRequest > rtspRequest, 
                                 u_int32_t serverPortA, u_int32_t serverPortB)
{
    Sptr< RtspTransportSpec > spec = rtspRequest->getTransport();
    spec->myServerPortA = serverPortA;
    spec->myServerPortB = serverPortB;

    RtspTransportHdr transportHdr;
    transportHdr.appendTransportSpec(spec);

    myHeaders += transportHdr.encode();
}

void
RtspResponse::appendTransportHdr(Sptr< RtspTransportSpec > spec)
{
    RtspTransportHdr transportHdr;
    transportHdr.appendTransportSpec(spec);

    myHeaders += transportHdr.encode();
}

void
RtspResponse::appendNPTRangeHdr(double startTime, double endTime)
{
   RtspRangeHdr rangeHdr(startTime, endTime);

   myHeaders += rangeHdr.encode();
}

void
RtspResponse::appendLocationHdr(Sptr< RtspLocationHdr > hdr)
{
    myFileLocation = hdr;
    myHeaders += hdr->encode();
}

void
RtspResponse::appendRtpInfoHdr(Sptr< RtspRtpInfoHdr > hdr)
{
    myRtpInfo = hdr;
    myHeaders += hdr->encode();
}

void
RtspResponse::appendConnectionCloseHdr()
{
    myHeaders += "Connection: close\r\n";
}

void
RtspResponse::appendEndOfHeaders()
{
    myHeaders += "\r\n";
}

void
RtspResponse::appendSdpData( RtspSdp& sdp )
{
    myHasBody = true;
    Data contData = sdp.encode();
    if (contData.length() > 0)
    {
        char tmpBuf[32];
        sprintf(tmpBuf, "%d", contData.length());

        myHeaders += "Content-Type: application/sdp\r\n";
        myHeaders += "Content-Length: ";
        myHeaders += tmpBuf;
        myHeaders += "\r\n\r\n";
        myMsgBody = contData;
    }
}

Data
RtspResponse::encode()
{
    if (!myHasBody)
        appendEndOfHeaders();

    Data msg = myStartLine + myHeaders;

    if (myHasBody)
        msg += myMsgBody;
        
    return msg;
}


const u_int32_t
RtspResponse::getStatusCode() throw (RtspBadDataException&)
{
    if (myStatusCode == 0)
    {
        if (myStartLine.length() == 0)
            return myStatusCode;

        CharData strtLine;
        LocalScopeAllocator lo;
        const char* buf = myStartLine.getData(lo);

        strtLine.set(buf, myStartLine.length());
        CharDataParser charParser(&strtLine);
        CharData version;
        u_int32_t len = 8;
        if (charParser.getThruLength(&version, len) == 0)
        {
            throw RtspBadDataException( "Bad RTSP message", __FILE__, __LINE__, 0 );
        }

        //TODO can add check for version here

        charParser.getThruSpaces(NULL);
        u_int32_t statusCodeNum;
        if (charParser.getNextInteger( statusCodeNum ))
        {
            myStatusCode = statusCodeNum;
        }
        else
        {
            cpLog(LOG_ERR, "error read status code number");
        }
    }
    return myStatusCode;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
