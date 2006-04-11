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

#include "global.h"
#include "SipHeader.hxx"
#include "SipAccept.hxx"
#include "SipAcceptEncoding.hxx"
#include "SipAcceptLanguage.hxx"
#include "SipAllow.hxx"
#include "SipAlso.hxx"
#include "SipAuthorization.hxx"
#include "SipCallId.hxx"
#include "SipCallLeg.hxx"
#include "SipContact.hxx"
#include "SipContentData.hxx"
#include "SipContentDisposition.hxx"
#include "SipContentEncoding.hxx"
#include "SipContentLanguage.hxx"
#include "SipContentLength.hxx"
#include "SipContentType.hxx"
#include "SipDate.hxx"
#include "SipCSeq.hxx"
#include "SipDiversion.hxx"
#include "SipEncryption.hxx"
#include "SipExpires.hxx"
#include "SipFrom.hxx"
#include "SipHide.hxx"
#include "SipInReplyTo.hxx"
#include "SipMaxForwards.hxx"
#include "SipMimeVersion.hxx"
#include "SipOrganization.hxx"
#include "SipOspAuthorization.hxx"
#include "SipPriority.hxx"
#include "SipProxyAuthorization.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipProxyRequire.hxx"
#include "SipRecordRoute.hxx"
#include "SipReferredBy.hxx"
#include "SipReferTo.hxx"
#include "SipRequire.hxx"
#include "SipReplaces.hxx"
#include "SipResponseKey.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipSession.hxx"
#include "SipSubject.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SipSupported.hxx"
#include "SipTimestamp.hxx"
#include "SipTo.hxx"
#include "SipTransferTo.hxx"
#include "SipUnsupported.hxx"
#include "SipUserAgent.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipSessionExpires.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SipSessionExpires.hxx"
#include "SipMinSE.hxx"

using namespace Vocal;

unsigned int SipHeader::_cnt = 0;

// We're going to maintain two tables, one for easy maintenance
// and one for use. The first table, of SipHeaderTableEntry, is
// constructed from native types. The second table, of SipHeaderNames,
// is constructed of composite types. 
//
// The reason for the two tables is that the Windows compiler (version 6.0), 
// cannot initize an array with compositite types without choking.
//
struct SipHeaderTableEntry {
        SipHeaderType type;
        const char * lowerName;
        const char * upperName;
        const char * shortName;
};

struct SipHeaderNames {
        SipHeaderType type;
        Data lowerName;
        Data upperName;
        Data shortName;

        SipHeaderNames() {}
        SipHeaderNames(const SipHeaderTableEntry & src)
            :   type(src.type), lowerName(src.lowerName), 
                upperName(src.upperName), shortName(src.shortName)
        {}
};


// This table is used to populate the sipHeaderNames table, which is
// used to determine which type a given sipHeader is, particularly for
// the purpose of constructing a specific type of SipHeader based on
// its text.

// This table should be maintained in alphabetical order.  The entry
// you add needs to be in the exact same place as the entry in
// SipHeaderType in SipHeader.hxx.  


static const SipHeaderTableEntry sipHeaderTable[sipHeaderNameCount] =
{
    { SIP_ACCEPT_HDR,             "accept",             "Accept"          ,"" },
    { SIP_ACCEPT_ENCODING_HDR,    "accept-encoding",    "Accept-Encoding" ,"" },
    { SIP_ACCEPT_LANGUAGE_HDR,    "accept-language",    "Accept-Language" ,"" },
    { SIP_ALLOW_HDR,              "allow",              "Allow"           ,"" },
    { SIP_ALSO_HDR,               "also",               "Also"            ,"" },
    { SIP_AUTHORIZATION_HDR,      "authorization",      "Authorization"   ,"" },
    { SIP_CALLID_HDR,             "call-id",            "Call-ID"         ,"i" },
    { SIP_CALL_LEG_HDR,           "call-leg",           "Call-Leg"        ,""},
    { SIP_CONTACT_HDR,            "contact",            "Contact"         ,"m"},
    { SIP_CONTENT_DATA_HDR,       "content-data",       "Content-Data"    ,""},
    { SIP_CONTENT_DISPOSITION_HDR, "content-disposition",                 
                                                     "Content-Disposition","" },
    { SIP_CONTENT_ENCODING_HDR,   "content-encoding",   "Content-Encoding","e" },
    { SIP_CONTENT_LANGUAGE_HDR,   "content-language",   "Content-Language","" },
    { SIP_CONTENT_LENGTH_HDR,     "content-length",     "Content-Length"  ,"l" },
    { SIP_CONTENT_TYPE_HDR,       "content-type",       "Content-Type"    ,"c" },
    { SIP_CSEQ_HDR,               "cseq",               "CSeq"            ,"" },
    { SIP_DATE_HDR,               "date",               "Date"            ,"" },
    { SIP_DIVERSION_HDR,          "diversion",          "Diversion"       ,"" },
//    { SIP_DIVERSION_HDR,          "cc-diversion",       "CC-Diversion"    ,"" },
    { SIP_ENCRYPTION_HDR,         "encryption",         "Encryption"      ,"" },
    { SIP_EXPIRES_HDR,            "expires",            "Expires"         ,"" },
    { SIP_EVENT_HDR,              "event",              "Event"           ,"" },
    { SIP_FROM_HDR,               "from",               "From"            ,"f" },
    { SIP_HIDE_HDR,               "hide",               "Hide"            ,"" },
    { SIP_IN_REPLY_TO_HDR,        "in-reply-to",        "In-Reply-To"     ,"" },
    { SIP_MAX_FORWARDS_HDR,       "max-forwards",       "Max-Forwards"    ,"" },
    { SIP_MIME_VERSION_HDR,       "mime-version",       "Mime-Version"    ,"" },
    { SIP_ORGANIZATION_HDR,       "organization",       "Organization"    ,"" },
    { SIP_OSP_AUTHORIZATION_HDR,  "osp-authorization-token",             
                                                        "OSP-Authorization-Token" ,"" },
    { SIP_PRIORITY_HDR,           "priority",           "Priority"        ,"" },
    { SIP_PROXY_AUTHENTICATE_HDR, "proxy-authenticate", "Proxy-Authenticate" ,""  },
    { SIP_PROXY_AUTHORIZATION_HDR, "proxy-authorization", 
                                                        "Proxy-Authorization" ,""},
    { SIP_PROXY_REQUIRE_HDR,      "proxy-require",      "Proxy-Require"    ,""},
    { SIP_RECORD_ROUTE_HDR,       "record-route",       "Record-Route"     ,""},
    { SIP_REFERRED_BY_HDR,        "referred-by",        "Referred-By"      ,"b"},
    { SIP_REFER_TO_HDR,           "refer-to",           "Refer-To"         ,"r"},
    { SIP_REPLACES_HDR,           "replaces",           "Replaces"         ,""},
    { SIP_REQUIRE_HDR,            "require",            "Require"          ,""},
    { SIP_RESPONSE_KEY_HDR,       "response-key",       "Response-Key"     ,""},
    { SIP_RETRY_AFTER_HDR,        "retry-after",        "Retry-After"      ,""},
    { SIP_ROUTE_HDR,              "route",              "Route"            ,""},
    { SIP_SERVER_HDR,             "server",             "Server"           ,""},
    { SIP_SESSION_HDR,            "session",            "Session"          ,""},
    { SIP_SUBJECT_HDR,            "subject",            "Subject"          ,"s"},
    { SIP_SUBSCRIPTION_NOTIFY_HDR, "subscription-notify",                  
                                                        "Subscription-Notify" ,"" },
    { SIP_SUPPORTED_HDR,          "supported",          "Supported"      ,"k"  },
    { SIP_TIMESTAMP_HDR,          "timestamp",          "Timestamp"      ,"" },
    { SIP_TO_HDR,                 "to",                 "To"             ,"t" },
    { SIP_TRANSFER_TO_HDR,        "transfer-to",        "Transfer-To"    ,"" },
    { SIP_UNSUPPORTED_HDR,        "unsupported",        "Unsupported"    ,"" },
    { SIP_USER_AGENT_HDR,         "user-agent",         "User-Agent"     ,"" },
    { SIP_VIA_HDR,                "via",                "Via"            ,"v" },
    { SIP_WARNING_HDR,            "warning",            "Warning"         ,"" },
    { SIP_WWW_AUTHENTICATE_HDR,   "www-authenticate",   "WWW-Authenticate" ,""},
    { SIP_SESSION_EXPIRES_HDR,   "session-expires",   "Session-Expires" ,"x"},
    { SIP_MIN_SE_HDR,   "min-se",   "Min-SE" ,""},
    { SIP_UNKNOWN_HDR,            "",                   "" ,""}  // leave this last
};


static SipHeaderNames sipHeaderNames[sipHeaderNameCount];

// This is evil, but it guarantees initialization of the 
// sipHeaderNames after it has been allocated and initialized.
//
struct SipHeaderInit
{
    public:
        SipHeaderInit()
        {
            SipHeader::init();
        }
};

static SipHeaderInit sipHeaderInit;


void SipHeader::init()
{
    for ( int i = 0; i < sipHeaderNameCount; ++i )
    {
        assert(static_cast<int>(sipHeaderTable[i].type) == i);
        sipHeaderNames[i] = SipHeaderNames(sipHeaderTable[i]);
    }
}


    
SipHeaderType 
Vocal::headerTypeDecode(const Data& headerName)
{
    int jump = 0;
    
    while (jump < sipHeaderNameCount)
    {
        if ((headerName == sipHeaderNames[jump].lowerName) || 
            (headerName == sipHeaderNames[jump].shortName))
            return sipHeaderNames[jump].type;
        else
            ++jump;
    }
    return SIP_UNKNOWN_HDR;
}


Data 
Vocal::headerTypeEncode(const SipHeaderType type)
{   
    return sipHeaderNames[type].upperName;
}


SipHeader*
SipHeader::decodeHeader( SipHeaderType type, const Data& value, const string& local_ip )
{
    SipHeader* header = 0;

    switch(type)
    {
    case    SIP_ACCEPT_HDR:
        header = new SipAccept(value, local_ip);
        break;
    case    SIP_ACCEPT_ENCODING_HDR:
        header = new SipAcceptEncoding(value, local_ip);
        break;
    case    SIP_ACCEPT_LANGUAGE_HDR:
        header = new SipAcceptLanguage(value, local_ip);
        break;
    case    SIP_ALLOW_HDR:
        header = new SipAllow(value, local_ip);
        break;
    case    SIP_ALSO_HDR:
        header = new SipAlso(value, local_ip);
        break;
    case    SIP_AUTHORIZATION_HDR:
        header = new SipAuthorization(value, local_ip);
        break;
    case    SIP_CALLID_HDR:
        header = new SipCallId(value, local_ip);
        break;
    case    SIP_CALL_LEG_HDR:
        if(value == "")
        {
            header = new SipCallLeg(local_ip);
        }
        else
        {
            assert(0); // Code below used to just assert, so moving it up to here.
            //header = new SipCallLeg(value);
        }
        break;
    case    SIP_CONTACT_HDR:
        header = new SipContact(value, local_ip);
        break;
    case    SIP_CONTENT_DATA_HDR:
        break;
    case    SIP_CONTENT_DISPOSITION_HDR:
        header = new SipContentDisposition(value, local_ip);
        break;
    case    SIP_CONTENT_ENCODING_HDR:
        header = new SipContentEncoding(value, local_ip);
        break;
    case    SIP_CONTENT_LANGUAGE_HDR:
        header = new SipContentLanguage(value, local_ip);
        break;
    case    SIP_CONTENT_LENGTH_HDR:
        header = new SipContentLength(value, local_ip);
        break;
    case    SIP_CONTENT_TYPE_HDR:
        header = new SipContentType(value, local_ip, 0);
        break;
    case    SIP_CSEQ_HDR:
        header = new SipCSeq(value, local_ip);
        break;
    case    SIP_DATE_HDR:
        header = new SipDate(value, local_ip);
        break;
    case    SIP_DIVERSION_HDR:
        header = new SipDiversion(value, local_ip);
        break;
    case    SIP_ENCRYPTION_HDR:
        header = new SipEncryption(value, local_ip);
        break;
    case    SIP_EXPIRES_HDR:
        header = new SipExpires(value, local_ip);
        break;
    case    SIP_EVENT_HDR:
        header = new SipSubsNotifyEvent(value, local_ip);
        break;
    case    SIP_FROM_HDR:
        header = new SipFrom(value, local_ip);
        break;
    case    SIP_HIDE_HDR:
        header = new SipHide(value, local_ip);
        break;
    case    SIP_IN_REPLY_TO_HDR:
        header = new SipInReplyTo(value, local_ip);
        break;
    case    SIP_MAX_FORWARDS_HDR:
        header = new SipMaxForwards(value, local_ip);
        break;
    case    SIP_MIME_VERSION_HDR:
        header = new SipMimeVersion(value, local_ip);
        break;
    case    SIP_ORGANIZATION_HDR:
        header = new SipOrganization(value, local_ip);
        break;
    case    SIP_OSP_AUTHORIZATION_HDR:
        header = new SipOspAuthorization(value, local_ip);
        break;
    case    SIP_PRIORITY_HDR:
        header = new SipPriority(value, local_ip);
        break;
    case    SIP_PROXY_AUTHENTICATE_HDR:
        header = new SipProxyAuthenticate(value, local_ip);
        break;
    case    SIP_PROXY_AUTHORIZATION_HDR:
        header = new SipProxyAuthorization(value, local_ip);
        break;
    case    SIP_PROXY_REQUIRE_HDR:
        header = new SipProxyRequire(value, local_ip);
        break;
    case    SIP_RECORD_ROUTE_HDR:
        header = new SipRecordRoute(value, local_ip);
        break;
    case    SIP_REFERRED_BY_HDR:
        header = new SipReferredBy(value, local_ip);
        break;
    case    SIP_REFER_TO_HDR:
        header = new SipReferTo(value, local_ip);
        break;
    case    SIP_REPLACES_HDR:
        header = new SipReplaces(value, local_ip);
        break;
    case    SIP_REQUIRE_HDR:
        header = new SipRequire(value, local_ip);
        break;
    case    SIP_RESPONSE_KEY_HDR:
        header = new SipResponseKey(value, local_ip);
        break;
    case    SIP_RETRY_AFTER_HDR:
        header = new SipRetryAfter(value, local_ip);
        break;
    case    SIP_ROUTE_HDR:
        header = new SipRoute(value, local_ip);
        break;
    case    SIP_SERVER_HDR:
        header = new SipServer(value, local_ip);
        break;
    case    SIP_SESSION_HDR:
        header = new SipSession(value, local_ip);
        break;
    case    SIP_SUBJECT_HDR:
        header = new SipSubject(value, local_ip);
        break;
    case    SIP_SUBSCRIPTION_NOTIFY_HDR:
        header = new SipSubsNotifyEvent(value, local_ip); //TODO:  Used twice???
        break;
    case    SIP_SUPPORTED_HDR:
        header = new SipSupported(value, local_ip);
        break;
    case    SIP_TIMESTAMP_HDR:
        header = new SipTimestamp(value, local_ip);
        break;
    case    SIP_TO_HDR:
        header = new SipTo(value, local_ip);
        break;
    case    SIP_TRANSFER_TO_HDR:
        header = new SipTransferTo(value, local_ip);
        break;
    case    SIP_UNKNOWN_HDR:
        // right now, this does nothing, which is fine, but you can't
        // access such unknown headers -- they are merely passed
        // around silently
        break;
    case    SIP_UNSUPPORTED_HDR:
        header = new SipUnsupported(value, local_ip);
        break;
    case    SIP_USER_AGENT_HDR:
        header = new SipUserAgent(value, local_ip);
        break;
    case    SIP_VIA_HDR:
        header = new SipVia(value, local_ip);
        break;
    case    SIP_WARNING_HDR:
        header = new SipWarning(value, local_ip);
        break;
    case    SIP_WWW_AUTHENTICATE_HDR:
        header = new SipWwwAuthenticate(value, local_ip);
        break;
    case    SIP_SESSION_EXPIRES_HDR:
        header = new SipSessionExpires(value, local_ip);
        break;
    case    SIP_MIN_SE_HDR:
        header = new SipMinSE(value, local_ip);
        break;

    default:
	break;
    }
    return header;
}
