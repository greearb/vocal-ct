
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

static const char* const SipStatusLine_cxx_Version =
    "$Id: SipStatusLine.cxx,v 1.2 2004/11/19 01:54:38 greear Exp $";

#include "global.h"
#include "SipStatusLine.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipParserMode.hxx"

#define MAX_STATUS_CODE_LEN 128

using namespace Vocal;


string
SipStatusLineParserException::getName( void ) const
{
    return "SipStatusLineParserException";
}

typedef struct _StatusCode
{
    int statusCode;
    char reasonPhrase[MAX_STATUS_CODE_LEN];
}
StatusCode;

static StatusCode statusCodes[] =
    {
        //Informational
        { 100, "Trying" },
        { 180, "Ringing" },
        { 181, "Call is Being Forwarded" },
        { 182, "Queued" },
        { 183, "Session Progress" },
        //Success
        { 200, "OK" },
        { 202, "Accepted" },
        //Redirection
        { 300, "Multiple Choices" },
        { 301, "Moved Permanently" },
        { 302, "Moved Temporarily" },
        //{ 303, "See Other" }, removed in August 6-SIP draft.
        { 305, "Use Proxy" },
        { 380, "Alternative Service" },
        //Client-Error
        { 400, "Bad Request" },
        { 401, "Unauthorized" },
        { 402, "Payment Required" },
        { 403, "Forbidden" },
        { 404, "Not Found" },
        { 405, "Method Not Allowed" },
        { 406, "Not Acceptable" },
        { 407, "Proxy Authentication Required" },
        { 408, "Request Timeout" },
        { 409, "Conflict" },
        { 410, "Gone" },
        //{ 411, "Length Required" },    removed in bis-03 (11/2001)
        { 413, "Request Entity Too Large" },
        { 414, "Request-URI Too Large" },
        { 415, "Unsupported Media Type" },
        { 420, "Bad Extension" },
        { 480, "Temporarily Unavailable" },
        { 481, "Call Leg/Transaction Does Not Exist" },
        { 482, "Loop Detected" },
        { 483, "Too Many Hops" },
        { 484, "Address Incomplete" },
        { 485, "Ambiguous" },
        { 486, "Busy Here" },
        { 487, "Request Terminated" },
        { 488, "Not Acceptable Here" },
        //Server-Error
        { 500, "Server Internal Error" },
        { 501, "Not Implemented" },
        { 502, "Bad Gateway" },
        { 503, "Service Unavailable" },
        { 504, "Server Timed-out" },
        { 505, "Version Not Supported" },
        { 513, "Message Too Large" },
        //Global-Failure
        { 600, "Busy Everywhere"},
        { 603, "Decline"},
        { 604, "Does Not Exist Anywhere"},
        { 606, "Not Acceptable"},
        { 0, "Unknown"},
    };


SipStatusLine::SipStatusLine()
{
    version = DEFAULT_VERSION;
    protocol = DEFAULT_PROTOCOL;
}


SipStatusLine::SipStatusLine( const Data& srcData)
        : statusCode(),
        reasonPhrase(),
        version(),
        protocol(),
        statusLine()
{
    decode(srcData);
}


void
SipStatusLine::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipStatusLine(nData);
    }
    catch (SipStatusLineParserException&)
    {
        cpLog(LOG_ERR, "Failed to decode data: <%s>", data.logData());
        assert(0);
        if (SipParserMode::sipParserMode())
        {
            throw;
        }

    }
}


void
SipStatusLine::scanSipStatusLine(const Data &tmpdata)
{
    Data reqdata = tmpdata;
    Data reqvalue;
    int ret = reqdata.match(" ", &reqvalue, true);
    if (ret == FOUND)
    {
        parseVersion(reqvalue);
        Data reqsdata = reqdata;
        Data reqsvalue;
        int retn = reqsdata.match(" ", &reqsvalue, true);
        if (retn == FOUND)
        {
            setStatusCode(reqsvalue.convertInt());
            setReasonPhrase(reqsdata);
        }
        else if (retn == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to scan StatusLine: <%s>", tmpdata.logData());
                throw SipStatusLineParserException(
                    "failed to decode the StatusLine string",
                    __FILE__,
                    __LINE__, DECODE_STATUSLINE_FAILED);
            }
        }
        else if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to scan StatusLine: <%s>", tmpdata.logData());
                throw SipStatusLineParserException(
                    "failed to decode the StatusLine string",
                    __FILE__,
                    __LINE__, DECODE_STATUSLINE_FAILED);
            }
        }
    }
    else if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to scan StatusLine: <%s>", tmpdata.logData());
            throw SipStatusLineParserException(
                "failed to decode the StatusLine string",
                __FILE__,
                __LINE__, DECODE_STATUSLINE_FAILED);
        }

    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to scan StatusLine: <%s>", tmpdata.logData());
            throw SipStatusLineParserException(
                "failed to decode the StatusLine string",
                __FILE__,
                __LINE__, DECODE_STATUSLINE_FAILED);
        }
    }
}


SipStatusLine::SipStatusLine( const SipStatusLine& src )
{
    statusCode = src.statusCode;
    reasonPhrase = src.reasonPhrase;
    version = src.version;
    protocol = src.protocol;
    statusLine = src.statusLine;
}


SipStatusLine&
SipStatusLine::operator=( const SipStatusLine& src )
{
    if (this != &src)
    {
        statusCode = src.statusCode;
        reasonPhrase = src.reasonPhrase;
        version = src.version;
        protocol = src.protocol;
        statusLine = src.statusLine;
    }

    return *this;
}


bool 
SipStatusLine::operator ==(const SipStatusLine& src) const
{
  return
    (
      ( statusCode == src.statusCode) &&
      ( reasonPhrase == src.reasonPhrase) &&
      ( version == src.version) &&
      ( protocol == src.protocol) &&
      ( statusLine == src.statusLine )
      );
}


Data 
SipStatusLine::encode() const
{
    Data data;
    data = protocol;
    data += "/";
    data += version;

    data += SP;
    data += statusCode;
    data += SP;

    data += reasonPhrase;
    data += CRLF;

    return data;
}


void 
SipStatusLine::setStatusCode( const int& newStatusCode)
{
    statusCode = Data(newStatusCode);
    setDefaultReason(statusCode);
}


void 
SipStatusLine::setStatusCodeData( const Data& newStatusCode)
{
    statusCode = newStatusCode;
}


const Data& 
SipStatusLine::getReasonPhrase( ) const
{
    return reasonPhrase;
}


void 
SipStatusLine::setReasonPhrase( const Data& newReasonPhrase )
{
    reasonPhrase = newReasonPhrase;
}


int 
SipStatusLine::getStatusCode() const
{
    return statusCode.convertInt();
}


const Data& 
SipStatusLine::getStatusCodeData()
{
    return statusCode;
}


const Data& 
SipStatusLine::getVersion()
{
    return version;
}


void 
SipStatusLine::parseVersion( const Data& newver )
{
    Data finaldata = newver;
    Data finalvalue ;
    int test = finaldata.match("/", &finalvalue, true);
    if (test == FIRST)
    {
        //EXp
    }

    if (test == FOUND)
    {
        setVersion(finaldata);
        setProtocol(finalvalue);
    }

    if (test == NOT_FOUND)
    {
        //EXP
    }

}


void 
SipStatusLine::setVersion( const Data& newversion )
{
    version = newversion;
}


const Data& 
SipStatusLine::getProtocol()
{
    return protocol;
}

///
void SipStatusLine::setProtocol( const Data& newprotocol )
{
    protocol = newprotocol;
}

const Data& SipStatusLine::getStatusLine()
{
    return statusLine;
}

///
void SipStatusLine::setStatusLine( const Data& newstatusLine )
{
    statusLine = newstatusLine;
}

///
void SipStatusLine::setDefaultReason(Data findStatusCode)
{
    // private member function.

    cpLog(LOG_DEBUG_STACK, "In setDefaultReason");
    int index = 0;
    bool found = false;

    while (statusCodes[index].statusCode != 0)
    {

        if (statusCodes[index].statusCode == findStatusCode.convertInt())
        {
            //set the reason phrase.
            reasonPhrase = statusCodes[index].reasonPhrase;
            found = true;
            break;
        }
        index++;
    }
    if (!found)
    {
        //unknown message , get the default message.
        Data statusStr = Data(findStatusCode);
        LocalScopeAllocator lo;
        char info = statusStr.getData(lo)[0];

        cpLog(LOG_DEBUG_STACK, "Class of Response:%c", info);

        if (info == '1')
        {
            setDefaultReason(Data(100));
        }
        else if (info == '2')
        {
            setDefaultReason(Data(200));
        }
        else if (info == '3')
        {
            setDefaultReason(Data(300));
        }
        else if (info == '4')
        {
            setDefaultReason(Data(400));
        }
        else if (info == '5')
        {
            setDefaultReason(Data(500));
        }
        else if (info == '6')
        {
            setDefaultReason(Data(600));
        }
        else
        {
            cpLog(LOG_INFO, "Status Code is not recognizable");
        }
    }
}

//End of File

/* Local Variables: */
/* c-file-style:"stroustrup" */
/* c-basic-offset:4 */
/* c-file-offsets:((inclass . ++)) */
/* indent-tabs-mode:nil */
/* End: */
