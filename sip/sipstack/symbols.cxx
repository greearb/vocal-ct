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

#include "symbols.hxx"

namespace Vocal
{
const char * const SP = " ";
const char * const EMPTY = "";
const char * const CRLF = "\r\n";
const char * const COMMA = ",";
const char * const SEMICOLON = ";";
const char * const EQUAL = "=";
const char * const COLON = ":";
const char * const CR = "\r";
const char * const LF = "\n";
const Literal TCP = "TCP";
const char * const CRLF2 = "\r\n\r\n";
const char * const CRLF3 = "\n\r\n";
const char * const CRLF4 = "\r\r\n";
const char * const CRLF5 = "\n\n";
const int HEADERS = 1;
const int URLPARMS=2;
const Literal INVITE_METHOD = "INVITE";
const Literal ACK_METHOD = "ACK";
const Literal OPTIONS_METHOD = "OPTIONS";
const Literal BYE_METHOD = "BYE";
const Literal CANCEL_METHOD = "CANCEL";
const Literal REGISTER_METHOD = "REGISTER";
const Literal INFO_METHOD = "INFO";
const Literal TRANSFER_METHOD = "TRANSFER";
const Literal REFER_METHOD = "REFER";
const Literal SUBSCRIBE_METHOD = "SUBSCRIBE";
const Literal NOTIFY_METHOD = "NOTIFY";
const Literal MESSAGE_METHOD = "MESSAGE";
const Literal UNKNOWN_METHOD = "UNKNOWN";



const Literal SIP_PGP = "pgp";
const Literal AUTH_BASIC = "Basic";
const Literal AUTH_DIGEST = "Digest";
const Literal AUTH_PGP = "pgp";
const Literal AUTH_CHAP = "CHAP";
const Literal QVALUE = "q";
const Literal ACTION = "action";
const Literal ALSO = "Also:";
const Literal ACTION_PROXY = "proxy";
const Literal ACTION_REDIRECT = "redirect";
const Literal EXPIRE = "expires";
const Literal URI = "uri";
const Literal REALM = "realm";
const Literal VERSION = "sip_version";
const Literal SIGNATURE = "signature";
const Literal SIGNED_BY = "signed-by";
const Literal NONCE = "nonce";
const Literal ALGORITHM = "algorithm";
const Literal MD5 = "MD5";

const Literal SIP_DURATION = "duration";

const Literal HIDE_ROUTE = "route";
const Literal HIDE_HOP = "hop";
const Literal TAG_CHECK = "tag";


const Literal DEFAULT_TTL = "1";
const Literal HIDDEN = "hidden";
const Literal RECEIVED = "received";
const Literal BRANCH = "branch";

const Literal TRANSFER_TO = "Transfer-To:";

const Literal SIP_EMERGENCY = "emergency";
const Literal SIP_URGENT = "urgent";
const Literal SIP_NORMAL = "normal";
const Literal SIP_NONURGENT = "non-urgent";
const Literal TRANSPORT = "transport";
const Literal TRANSPORT_UDP = "udp";
const Literal TRANSPORT_TCP = "tcp";
const Literal USER = "user";
const Literal USERNAME = "username";
const Literal USER_PHONE = "phone";
const Literal USER_IP = "ip";
const Literal METHOD_PARM = "method";
const Literal MADDR_PARM = "maddr";
const Literal TTL = "ttl";
const Literal ISDN_ADDR = "isub";
const Literal POST_DIAL = "postd";
const Literal ONE_SECOND_PAUSE = "p";
const Literal WAIT_FOR_DIAL_TONE = "w";
const Literal DTMF1 = "*";
const Literal DTMF2 = "#";
const Literal DTMF3 = "A";
const Literal DTMF4 = "B";
const Literal DTMF5 = "C";
const Literal DTMF6 = "D";
const Literal DIGIT = "0";

const Literal SIP_ISUBV = "isub=";
const Literal SIP_POSTDIAL = "postd=";
const Literal SIP_PHCONTXT = "phone-context=";
const Literal WEEKDAY1 = "Mon";
const Literal WEEKDAY2 = "Tue";
const Literal WEEKDAY3 = "Wed";
const Literal WEEKDAY4 = "Thu";
const Literal WEEKDAY5 = "Fri";
const Literal WEEKDAY6 = "Sat";
const Literal WEEKDAY7 = "Sun";

const Literal TIMEZONE1 = "UT";
const Literal TIMEZONE2 = "GMT";
const Literal TIMEZONE3 = "CST";
const Literal TIMEZONE4 = "CDT";
const Literal TIMEZONE5 = "EST";
const Literal TIMEZONE6 = "EDT";
const Literal TIMEZONE7 = "PST";
const Literal TIMEZONE8 = "PDT";
const Literal TIMEZONE9 = "MST";
const Literal TIMEZONE10 = "MDT";


const Literal MONTH1 = "Jan";
const Literal MONTH2 = "Feb";
const Literal MONTH3 = "Mar";
const Literal MONTH4 = "Apr";
const Literal MONTH5 = "May";
const Literal MONTH6 = "Jun";
const Literal MONTH7 = "Jul";
const Literal MONTH8 = "Aug";
const Literal MONTH9 = "Sep";
const Literal MONTH10 = "Oct";
const Literal MONTH11 = "Nov";
const Literal MONTH12 = "Dec";

const Literal DIVERSIONREASON1 = "unuvailable";
const Literal DIVERSIONREASON2 = "unconditional";
const Literal DIVERSIONREASON3 = "time-of-day";
const Literal DIVERSIONREASON4 = "do-not-disturb";
const Literal DIVERSIONREASON5 = "deflection";
const Literal DIVERSIONREASON6 = "follow-me";
const Literal DIVERSIONREASON7 = "away";
const Literal DIVERSIONREASON8 = "out-of-service";
   
const Literal DIVERSION_REASON = "reason=";
const Literal COUNTER = "counter=";
const Literal LIMIT = "limit=";


//this is initialized so that when we
//check the Method for a  status Msg, we know the msg is a status msg,
//by looking at this.

const Literal DEFAULT_PROTOCOL = "SIP";
const Literal DEFAULT_VERSION = "2.0";

const Literal DEFAULT_TRANSPORT = "UDP";

const Literal DEFAULT_SIP_PORT = "5060";
const Literal DEFAULT_TRANSPORT_URL = "udp";
const Literal APPLICATION_SDP = "application/sdp";
const Literal APPLICATION_MGCP = "application/mgcp";


/* used in SipAccept */
const Literal DEFAULT_MEDIATYPE = "application";
const Literal DEFAULT_MEDIASUBTYPE = "sdp";
const Literal ALL_MEDIATYPES = "*";
const Literal ALL_MEDIASUBTYPES = "*";
const Literal DEFAULT_QVALUE = "1";
const Literal ASTERISK = "*";
const Literal SERVICE_PROVIDER = "tsp";
const Literal PHONE_CONTEXT_TAG = "phone-context";
const Literal HANDLING_PARM = "handling";
const Literal CONT_REQUIRED = "optional";
const Literal CONT_OPTIONAL = "required";


//headers
const Literal TRANSFER_TO1 = "Transfer-To";
const Literal SIP_UNSUPPORTED1 = "Unsupported";
const Literal FROM1 = "From";
const Literal FROM_SHORT1 = "f";
const Literal ACCEPT1 = "Accept";
const Literal ACCEPT_ENCODING1 = "Accept-Encoding";
const Literal ACCEPT_LANGUAGE1 = "Accept-Language";
const Literal RECORD_ROUTE1 = "Record-Route";
const Literal RECORD_ROUTE_SHORT1 = "r";
const Literal EXPIRES1 = "Expires";
const Literal CONTENT_ENCODING1 = "Content-Encoding";
const Literal CONTENT_ENCODING_SHORT1 = "e";
const Literal AUTHORIZATION1 = "Authorization";
const Literal ALLOW1 = "Allow";
const Literal HIDE1 = "Hide";
const Literal USERAGENT1 = "User-Agent";
const Literal VIA1 = "Via";
const Literal VIA_SHORT1 = "v";
const Literal TO1 = "To";
const Literal TO_SHORT1 = "t";
const Literal CSEQ1 = "CSeq";
const Literal CALLID1 = "Call-ID";
const Literal CALLID_SHORT1 = "i";
const Literal CONTACT1 = "Contact";
const Literal CONTACT_SHORT1 = "m";
const Literal DATE1 = "Date";
const Literal TIMESTAMP1 = "Timestamp";
const Literal ENCRYPTION1 = "Encryption";
const Literal CONTENT_LENGTH1 = "Content-Length";
const Literal CONTENT_LENGTH_SHORT1 = "l";
const Literal CONTENT_TYPE1 = "Content-Type";
const Literal CONTENT_TYPE_SHORT1 = "c";
const Literal MAX_FORWARDS1 = "Max-Forwards";
const Literal ORGANIZATION1 = "Organization";
const Literal SIP_PRIORITY1 = "Priority";
const Literal SIP_PROXYAUTHORIZATION1 = "Proxy-Authorization";
const Literal SIP_PROXYREQUIRE1 = "Proxy-Require";
const Literal SIP_ROUTE1 = "Route";
const Literal SIP_REQUIRE1 = "Require";
const Literal SIP_RESPONSEKEY1 = "Response-Key";
const Literal SUBJECT1 = "Subject";
const Literal SUBJECT_SHORT1 = "s";
const Literal SIP_PROXYAUTHENTICATE1 = "Proxy-Authenticate";
const Literal SIP_RETRYAFTER1 = "Retry-After";
const Literal SIP_SERVER1 = "Server";
const Literal UNSUPPORTED1 = "Unsupported";
const Literal SUPPORTED1 = "Supported";
const Literal SUPPORTED_SHORT1 = "k";
const Literal SIP_WARNING1 = "Warning";
const Literal SIP_WWWAUTHENTICATE1 = "WWW-Authenticate" ;
const Literal SIP_EXPIRES1 = "Expires";
const Literal EVENT1 = "Event";
const Literal SipUrlParamTransport = "transport=";
const Literal SipUrlParamTtl = "ttl=";
const Literal SipUrlParamMaddr = "maddr=";
const Literal SipUrlParamUser = "user=";
const Literal SipUrlParamMethod = "method=";
const Literal SipUrlParamTag = "tag=";
const Literal SessionTimerParam = "refresher=";
const Literal SIPDIVERSION = "Diversion:";
//const Literal SIPDIVERSION = "CC-Diversion:";
const Literal EVENT_MWI = "Message-Waiting-Indicator";
const Literal OSP_AUTH1 = "OSP-Authorization-Token";
const Literal SIP_CONTDISPOSITION1 = "Content-Disposition";
const Literal SIP_INREPLYTO1="In-Reply-To";
const Literal CONTENTLANGUAGE1="Content-Language";
const Literal SESSIONEXPIRES1="Session-Expires";
const Literal MIN_SE1="Min-SE";





const Literal CONTENTLANGUAGE="Content-Language:";
const Literal SIP_UNSUPPORTED = "Unsupported:";
const Literal FROM = "From:";
const Literal FROM_SHORT = "f:";
const Literal ACCEPT = "Accept:";
const Literal ACCEPT_ENCODING = "Accept-Encoding:";
const Literal ACCEPT_LANGUAGE = "Accept-Language:";
const Literal RECORD_ROUTE = "Record-Route:";
const Literal RECORD_ROUTE_SHORT = "r:";
const Literal EXPIRES = "Expires:";
const Literal CONTENT_ENCODING = "Content-Encoding:";
const Literal CONTENT_ENCODING_SHORT = "e:";
const Literal AUTHORIZATION = "Authorization:";
const Literal ALLOW = "Allow:";
const Literal HIDE = "Hide:";
const Literal USERAGENT = "User-Agent:";
const Literal VIA = "Via:";
const Literal VIA_SHORT = "v:";
const Literal TO = "To:";
const Literal TO_SHORT = "t:";
const Literal CSEQ = "CSeq:";
const Literal CALLID = "Call-ID:";
const Literal CALLID_SHORT = "i:";
const Literal CONTACT = "Contact:";
const Literal CONTACT_SHORT = "m:";
//const Literal DATE = "Date:";
const Literal TIMESTAMP = "Timestamp:";
const Literal ENCRYPTION = "Encryption:";
const Literal CONTENT_LENGTH = "Content-Length:";
const Literal CONTENT_LENGTH_SHORT = "l:";
const Literal CONTENT_TYPE = "Content-Type:";
const Literal CONTENT_TYPE_SHORT = "c:";
const Literal MAX_FORWARDS = "Max-Forwards:";
const Literal ORGANIZATION = "Organization: ";
const Literal SIP_PRIORITY = "Priority:";
const Literal SIP_PROXYAUTHORIZATION = "Proxy-Authorization:";
const Literal SIP_PROXYREQUIRE = "Proxy-Require:";
const Literal SIP_ROUTE = "Route:";
const Literal SIP_REQUIRE = "Require:";
const Literal SIP_RESPONSEKEY = "Response-Key:";
const Literal SUBJECT = "Subject:";
const Literal SUBJECT_SHORT = "s:";
const Literal SIP_PROXYAUTHENTICATE = "Proxy-Authenticate:";
const Literal SIP_RETRYAFTER = "Retry-After:";
const Literal SIP_SERVER = "Server:";
const Literal UNSUPPORTED = "Unsupported:";
const Literal SUPPORTED = "Supported:";
const Literal SUPPORTED_SHORT = "k:";
const Literal SIP_WARNING = "Warning:";
const Literal SIP_WWWAUTHENTICATE = "WWW-Authenticate:" ;
const Literal SIP_EXPIRES = "Expires:";
const Literal EVENT = "Event:";
const Literal OSP_AUTH = "OSP-Authorization-Token:";
const Literal REFER_TO = "Refer-To";
const Literal REFERRED_BY = "Referred-By";
const Literal REPLACES = "Replaces";
const Literal SESSION = "Session:";
const Literal SESSION1 = "Session";   
const Literal SESSIONEXPIRES="Session-Expires:";
const Literal SESSIONEXPIRES_SHORT="x:";
const Literal MIN_SE="Min-SE:";

//sub items in the Session Header as given in draft on 183 msgs.    
const Literal SESSION_MEDIA = "Media";
const Literal SESSION_QOS = "Qos";
const Literal SESSION_SECURITY = "Security";
   
const Literal ZERO_CONTENT_LENGTH = "0";
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
