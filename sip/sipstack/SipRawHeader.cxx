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
 * IN EXCESS OF ,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const SipRawHeader_cxx_Version =
    "$Id: SipRawHeader.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipRawHeader.hxx"
#include "cpLog.h"

using namespace Vocal;

bool 
SipRawHeader::shallowEquals(const SipRawHeader& src) const
{
    if((src.header == 0) && (header == 0))
    {
        return ((headerType == src.headerType) &&
                (headerValue == src.headerValue) &&
                ( (headerType != SIP_UNKNOWN_HDR) ||
                  (headerName == src.headerName)));
    }
    else
    {
        // at least one is parsed -- parse both and go for it
        src.parse();
        parse();
        return (header->compareSipHeader(src.header.getPtr()));
    }
}


bool 
SipRawHeader::shallowEqualsNul() const
{
    // a header is equal to the NUL pointer if 
    if(header == 0)
    {
        // in this case, something is going to get encoded, so it
        // can't possibly equal the NUL pointer.
        cpLog(LOG_DEBUG_STACK, "no header so returning false");
        return false;
    }
    else if ((headerType == SIP_CONTENT_TYPE_HDR) || 
             (headerType == SIP_CONTENT_LENGTH_HDR) ||
             (headerType == SIP_CONTENT_DISPOSITION_HDR) ||
             (headerType == SIP_CONTENT_ENCODING_HDR) ||
             (headerType == SIP_CONTENT_LANGUAGE_HDR) ||
             (headerType == SIP_MAX_FORWARDS_HDR)
        )
    {
        // for purposes of comparison w/ a null header, these always
        // are "true" (because they may not yet have been set
        // correctly)
        cpLog(LOG_DEBUG_STACK, "content header so returning true");
        return true;
    }
    else // compare for real
    {
        // at least one is parsed, so there is a chance that this is
        // the right one
        Data encoding;
        encoding = header->encode();
        cpLog(LOG_DEBUG_STACK, "encoded header: \"%s\"", encoding.logData());
        return (encoding == "");
    }
}


bool
SipRawHeader::operator==(const SipRawHeader& src) const
{
    // need to figure this one out
    bool compare = shallowEquals(src);

    if(compare)
    {
        Sptr<SipRawHeader> left = next;
        Sptr<SipRawHeader> right = src.next;

        while( compare && ((left != 0) && (right != 0)))
        {
            // keep comparing children
            compare = left->shallowEquals(*right);
            left = left->next;
            right = right->next;
        }
        if( compare && (left == 0) && (right == 0))
        {
            // in this case, both ended at the same time, so if
            // compare is true, both are "equal"
            compare = true;
        }
        else
        {
            // otherwise, false
            compare = false; 
        }
    }

    return compare;
}


bool
SipRawHeader::operator!=(const SipRawHeader& src) const
{
    return !(*this == src);
}


static void encodeShallowPutsName(const SipRawHeader& header, Data* msg)
{
    if (header.header != 0)
    {
        // call the parsed header encode
        *msg += header.header->encode();
    }
    else
    {
        // no parsed header, reconstruct
        if(header.headerType != SIP_UNKNOWN_HDR)
        {
            *msg += headerTypeEncode(header.headerType);
        }
        else
        {
            *msg += header.headerNameOriginal;
        }
        *msg += ": ";
        Data headerVal = header.headerValue;
        if (headerVal.length())
        {
            *msg += header.headerValue;
        }
        *msg += "\r\n";
    }
}


static void encodeShallowNoName(const SipRawHeader& header, Data* msg, bool comma)
{
    if(header.header != 0)
    {
        if(comma)
        {
            *msg += ",";
        }
        *msg += header.header->encode();
    }
    else if(header.headerValue.length() != 0)
    {
        if(comma)
        {
            *msg += ",";
        }
        *msg += header.headerValue;
    }
}


bool
SipRawHeader::encode(Data* msg) const
{
    switch(headerType)
    {
    case SIP_ACCEPT_HDR:
    case SIP_ACCEPT_ENCODING_HDR:
    case SIP_ACCEPT_LANGUAGE_HDR:
    case SIP_CONTENT_ENCODING_HDR:
    case SIP_RECORD_ROUTE_HDR:
    case SIP_SUPPORTED_HDR:
        
    case SIP_CONTENT_LANGUAGE_HDR:
    case SIP_ALLOW_HDR:
        
    case SIP_PROXY_REQUIRE_HDR:
    case SIP_REQUIRE_HDR:
    case SIP_ROUTE_HDR:
        // in this case, this is a list -- encode that way!
        *msg += headerTypeEncode(headerType);
        *msg += ": ";
        encodeShallowNoName(*this, msg, false);
        // keep encoding if this is a list
        {
            Sptr<SipRawHeader> tmp = next;
            while(tmp != 0)
            {
                encodeShallowNoName(*tmp, msg, true);
                tmp = tmp->next;
            }
            *msg += "\r\n";
        }
        break;
    default:
        encodeShallowPutsName(*this, msg);
        // keep encoding if this is a list
        {
            Sptr<SipRawHeader> tmp = next;
            while(tmp != 0)
            {
                encodeShallowPutsName(*tmp, msg);
                tmp = tmp->next;
            }
        }
        break;
    }
    
    return false;
}


bool
SipRawHeader::decode(const Data& headerData)
{
    bool parseError = false;
    bool noMatch;

    // decode the next line of the message
    headerValue = headerData;
    headerName = headerValue.parse(": \t", &noMatch);
    headerNameOriginal = headerName;
    headerName.lowercase();
    headerType = headerTypeDecode(headerName);
    if(noMatch)
    {
	// some sort of error
	parseError = true;
    }

    // check for possible multiple headers

    switch(headerType)
    {
        // all of these are multiple headers, so do the multiple
        // header case for them.

    case SIP_VIA_HDR:
    case SIP_DIVERSION_HDR:
    case SIP_ACCEPT_HDR:
    case SIP_ACCEPT_ENCODING_HDR:
    case SIP_ACCEPT_LANGUAGE_HDR:
    case SIP_RECORD_ROUTE_HDR:
    case SIP_SUPPORTED_HDR:
    case SIP_CONTENT_LANGUAGE_HDR:
    case SIP_ALLOW_HDR:
    case SIP_CONTENT_ENCODING_HDR:
    case SIP_PROXY_REQUIRE_HDR:
    case SIP_REQUIRE_HDR:
    case SIP_ROUTE_HDR:
        
    case SIP_UNSUPPORTED_HDR:
    case SIP_IN_REPLY_TO_HDR:
    case SIP_WARNING_HDR:
    case SIP_CONTACT_HDR:
    {
        // multiple header parsing
        SipRawHeader* current = this;
        bool notFound;

        do
        {
            Data tmp = current->headerValue;
            Data found = tmp.parseOutsideQuotes(",", true, true, &notFound);
            if(!notFound)
            {
                if(found != "")
                {
                    // found, do something
                    current->headerValue = found;
                    current->next = new SipRawHeader(local_ip);
                    
                    current->next->headerValue = tmp;
                    current->next->headerValue.removeSpaces();
                    current->next->headerName = headerName;
                    current->next->headerNameOriginal = headerNameOriginal;
                    current->next->headerType = headerType;
                    current = current->next.getPtr();
                }
                else
                {
                    // there is a problem here, in that there was no
                    // text prior to the comma, e.g

                    // Route : ,<sip:tisong@1.1.1.1>

                    current->headerValue = tmp;
                    cpLog(LOG_DEBUG_STACK, "empty header separated by comma");
                }
            }
        }
        while(!notFound);
    }
    break;
    default:
        // do nothing here for single headers 
        break;
    }


    return parseError;
}


bool
SipRawHeader::parse() const
{
    if(header == 0)
    {
        header = SipHeader::decodeHeader(headerType, headerValue, local_ip);
    }
    return true;
}


bool
SipRawHeader::parseDeep() const
{
    const SipRawHeader* tmp;
    tmp = this;
    while(tmp != 0)
    {
        tmp->parse();
        tmp = tmp->next.getPtr();
    }

    return true;
}


void
SipRawHeader::deepCopy(const SipRawHeader& src)
{
    assert(0);
    *this = src; // shallow copy
    if(src.header != 0)
    {
        this->header = src.header->duplicate();
    }
//    this->next = deepCopy
    if(src.next != 0)
    {
        this->next = src.next->duplicate();
    }
}


SipRawHeader*
SipRawHeader::duplicate() const
{
    SipRawHeader* tmp = new SipRawHeader(*this); // make a shallow copy
    if(this->header != 0)
    {
        tmp->header = this->header->duplicate();
    }
    if(this->next != 0)
    {
        tmp->next = this->next->duplicate();
    }
    return tmp;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
