#ifndef SYMBOLS_HXX_
#define SYMBOLS_HXX_

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

static const char* const symbolsVersion = "$Id: symbols.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "Literal.hxx"

namespace Vocal
{
    extern const char * const SP;
    extern const char * const EMPTY;
    extern const char * const CRLF;
    extern const char * const COMMA;
    extern const char * const SEMICOLON;
    extern const char * const EQUAL;
    extern const char * const COLON;
    extern const char * const CR;
    extern const char * const LF;
    extern const Literal TCP;
    extern const char * const CRLF2;
    extern const char * const CRLF3;
    extern const char * const CRLF4;
    extern const char * const CRLF5;
    extern const int HEADERS;
    extern const int URLPARMS;
    extern const Literal INVITE_METHOD;
    extern const Literal ACK_METHOD;
    extern const Literal OPTIONS_METHOD;
    extern const Literal BYE_METHOD;
    extern const Literal CANCEL_METHOD;
    extern const Literal REGISTER_METHOD;
    extern const Literal INFO_METHOD;
    extern const Literal TRANSFER_METHOD;
    extern const Literal REFER_METHOD;
    extern const Literal SUBSCRIBE_METHOD;
    extern const Literal NOTIFY_METHOD;
    extern const Literal MESSAGE_METHOD;
    extern const Literal UNKNOWN_METHOD;



    extern const Literal SIP_PGP;
    extern const Literal AUTH_BASIC;
    extern const Literal AUTH_DIGEST;
    extern const Literal AUTH_PGP;
    extern const Literal AUTH_CHAP;
    extern const Literal QVALUE;
    extern const Literal ACTION;
    extern const Literal ALSO;
    extern const Literal ACTION_PROXY;
    extern const Literal ACTION_REDIRECT;
    extern const Literal EXPIRE;
    extern const Literal URI;
    extern const Literal REALM;
    extern const Literal VERSION;
    extern const Literal SIGNATURE;
    extern const Literal SIGNED_BY;
    extern const Literal NONCE;
    extern const Literal ALGORITHM;
    extern const Literal MD5;

    extern const Literal SIP_DURATION;

    extern const Literal HIDE_ROUTE;
    extern const Literal HIDE_HOP;
    extern const Literal TAG_CHECK;


    extern const Literal DEFAULT_TTL;
    extern const Literal HIDDEN;
    extern const Literal RECEIVED;
    extern const Literal BRANCH;

    extern const Literal TRANSFER_TO;

    extern const Literal SIP_EMERGENCY;
    extern const Literal SIP_URGENT;
    extern const Literal SIP_NORMAL;
    extern const Literal SIP_NONURGENT;
    extern const Literal TRANSPORT;
    extern const Literal TRANSPORT_UDP;
    extern const Literal TRANSPORT_TCP;
    extern const Literal USER;
    extern const Literal USERNAME;
    extern const Literal USER_PHONE;
    extern const Literal USER_IP;
    extern const Literal METHOD_PARM;
    extern const Literal MADDR_PARM;
    extern const Literal TTL;
    extern const Literal ISDN_ADDR;
    extern const Literal POST_DIAL;
    extern const Literal ONE_SECOND_PAUSE;
    extern const Literal WAIT_FOR_DIAL_TONE;
    extern const Literal DTMF1;
    extern const Literal DTMF2;
    extern const Literal DTMF3;
    extern const Literal DTMF4;
    extern const Literal DTMF5;
    extern const Literal DTMF6;
    extern const Literal DIGIT;

    extern const Literal SIP_ISUBV;
    extern const Literal SIP_POSTDIAL;
    extern const Literal SIP_PHCONTXT;
    extern const Literal WEEKDAY1;
    extern const Literal WEEKDAY2;
    extern const Literal WEEKDAY3;
    extern const Literal WEEKDAY4;
    extern const Literal WEEKDAY5;
    extern const Literal WEEKDAY6;
    extern const Literal WEEKDAY7;

    extern const Literal TIMEZONE1;
    extern const Literal TIMEZONE2;
    extern const Literal TIMEZONE3;
    extern const Literal TIMEZONE4;
    extern const Literal TIMEZONE5;
    extern const Literal TIMEZONE6;
    extern const Literal TIMEZONE7;
    extern const Literal TIMEZONE8;
    extern const Literal TIMEZONE9;
    extern const Literal TIMEZONE10;


    extern const Literal MONTH1;
    extern const Literal MONTH2;
    extern const Literal MONTH3;
    extern const Literal MONTH4;
    extern const Literal MONTH5;
    extern const Literal MONTH6;
    extern const Literal MONTH7;
    extern const Literal MONTH8;
    extern const Literal MONTH9;
    extern const Literal MONTH10;
    extern const Literal MONTH11;
    extern const Literal MONTH12;

    extern const Literal DIVERSIONREASON1;
    extern const Literal DIVERSIONREASON2;
    extern const Literal DIVERSIONREASON3;
    extern const Literal DIVERSIONREASON4;
    extern const Literal DIVERSIONREASON5;
    extern const Literal DIVERSIONREASON6;
    extern const Literal DIVERSIONREASON7;
    extern const Literal DIVERSIONREASON8;
   
    extern const Literal DIVERSION_REASON;
    extern const Literal COUNTER;
    extern const Literal LIMIT;


    //this is initialized so that when we
    //check the Method for a  status Msg, we know the msg is a status msg,
    //by looking at this.

    extern const Literal DEFAULT_PROTOCOL;
    extern const Literal DEFAULT_VERSION;

    extern const Literal DEFAULT_TRANSPORT;

    extern const Literal DEFAULT_SIP_PORT;
    extern const Literal DEFAULT_TRANSPORT_URL;
    extern const Literal APPLICATION_SDP;
    extern const Literal APPLICATION_MGCP;


    /* used in SipAccept */
    extern const Literal DEFAULT_MEDIATYPE;
    extern const Literal DEFAULT_MEDIASUBTYPE;
    extern const Literal ALL_MEDIATYPES;
    extern const Literal ALL_MEDIASUBTYPES;
    extern const Literal DEFAULT_QVALUE;
    extern const Literal ASTERISK;
    extern const Literal SERVICE_PROVIDER;
    extern const Literal PHONE_CONTEXT_TAG;
    extern const Literal HANDLING_PARM;
    extern const Literal CONT_REQUIRED;
    extern const Literal CONT_OPTIONAL;


    //headers
    extern const Literal TRANSFER_TO1;
    extern const Literal SIP_UNSUPPORTED1;
    extern const Literal FROM1;
    extern const Literal FROM_SHORT1;
    extern const Literal ACCEPT1;
    extern const Literal ACCEPT_ENCODING1;
    extern const Literal ACCEPT_LANGUAGE1;
    extern const Literal RECORD_ROUTE1;
    extern const Literal RECORD_ROUTE_SHORT1;
    extern const Literal EXPIRES1;
    extern const Literal CONTENT_ENCODING1;
    extern const Literal CONTENT_ENCODING_SHORT1;
    extern const Literal AUTHORIZATION1;
    extern const Literal ALLOW1;
    extern const Literal HIDE1;
    extern const Literal USERAGENT1;
    extern const Literal VIA1;
    extern const Literal VIA_SHORT1;
    extern const Literal TO1;
    extern const Literal TO_SHORT1;
    extern const Literal CSEQ1;
    extern const Literal CALLID1;
    extern const Literal CALLID_SHORT1;
    extern const Literal CONTACT1;
    extern const Literal CONTACT_SHORT1;
    extern const Literal DATE1;
    extern const Literal TIMESTAMP1;
    extern const Literal ENCRYPTION1;
    extern const Literal CONTENT_LENGTH1;
    extern const Literal CONTENT_LENGTH_SHORT1;
    extern const Literal CONTENT_TYPE1;
    extern const Literal CONTENT_TYPE_SHORT1;
    extern const Literal MAX_FORWARDS1;
    extern const Literal ORGANIZATION1;
    extern const Literal SIP_PRIORITY1;
    extern const Literal SIP_PROXYAUTHORIZATION1;
    extern const Literal SIP_PROXYREQUIRE1;
    extern const Literal SIP_ROUTE1;
    extern const Literal SIP_REQUIRE1;
    extern const Literal SIP_RESPONSEKEY1;
    extern const Literal SUBJECT1;
    extern const Literal SUBJECT_SHORT1;
    extern const Literal SIP_PROXYAUTHENTICATE1;
    extern const Literal SIP_RETRYAFTER1;
    extern const Literal SIP_SERVER1;
    extern const Literal UNSUPPORTED1;
    extern const Literal SUPPORTED1;
    extern const Literal SUPPORTED_SHORT1;
    extern const Literal SIP_WARNING1;
    extern const Literal SIP_WWWAUTHENTICATE1;
    extern const Literal SIP_EXPIRES1;
    extern const Literal EVENT1;
    extern const Literal SipUrlParamTransport;
    extern const Literal SipUrlParamTtl;
    extern const Literal SipUrlParamMaddr;
    extern const Literal SipUrlParamUser;
    extern const Literal SipUrlParamMethod;
    extern const Literal SipUrlParamTag;
    extern const Literal SessionTimerParam;
    extern const Literal SIPDIVERSION;
    extern const Literal EVENT_MWI;
    extern const Literal OSP_AUTH1;
    extern const Literal SIP_CONTDISPOSITION1;
    extern const Literal SIP_INREPLYTO1;
    extern const Literal CONTENTLANGUAGE1;
    extern const Literal SESSIONEXPIRES1;
    extern const Literal MIN_SE1;





    extern const Literal CONTENTLANGUAGE;
    extern const Literal SIP_UNSUPPORTED;
    extern const Literal FROM;
    extern const Literal FROM_SHORT;
    extern const Literal ACCEPT;
    extern const Literal ACCEPT_ENCODING;
    extern const Literal ACCEPT_LANGUAGE;
    extern const Literal RECORD_ROUTE;
    extern const Literal RECORD_ROUTE_SHORT;
    extern const Literal EXPIRES;
    extern const Literal CONTENT_ENCODING;
    extern const Literal CONTENT_ENCODING_SHORT;
    extern const Literal AUTHORIZATION;
    extern const Literal ALLOW;
    extern const Literal HIDE;
    extern const Literal USERAGENT;
    extern const Literal VIA;
    extern const Literal VIA_SHORT;
    extern const Literal TO;
    extern const Literal TO_SHORT;
    extern const Literal CSEQ;
    extern const Literal CALLID;
    extern const Literal CALLID_SHORT;
    extern const Literal CONTACT;
    extern const Literal CONTACT_SHORT;
    //extern const Literal DATE[] = "Date:";
    extern const Literal TIMESTAMP;
    extern const Literal ENCRYPTION;
    extern const Literal CONTENT_LENGTH;
    extern const Literal CONTENT_LENGTH_SHORT;
    extern const Literal CONTENT_TYPE;
    extern const Literal CONTENT_TYPE_SHORT;
    extern const Literal MAX_FORWARDS;
    extern const Literal ORGANIZATION;
    extern const Literal SIP_PRIORITY;
    extern const Literal SIP_PROXYAUTHORIZATION;
    extern const Literal SIP_PROXYREQUIRE;
    extern const Literal SIP_ROUTE;
    extern const Literal SIP_REQUIRE;
    extern const Literal SIP_RESPONSEKEY;
    extern const Literal SUBJECT;
    extern const Literal SUBJECT_SHORT;
    extern const Literal SIP_PROXYAUTHENTICATE;
    extern const Literal SIP_RETRYAFTER;
    extern const Literal SIP_SERVER;
    extern const Literal UNSUPPORTED;
    extern const Literal SUPPORTED;
    extern const Literal SUPPORTED_SHORT;
    extern const Literal SIP_WARNING;
    extern const Literal SIP_WWWAUTHENTICATE;
    extern const Literal SIP_EXPIRES;
    extern const Literal EVENT;
    extern const Literal OSP_AUTH;
    extern const Literal REFER_TO;
    extern const Literal REFERRED_BY;
    extern const Literal REPLACES;
    extern const Literal SESSION;
    extern const Literal SESSION1;
    extern const Literal SESSIONEXPIRES;
    extern const Literal SESSIONEXPIRES_SHORT;
    extern const Literal MIN_SE;
    //sub items in the Session Header as given in draft on 183 msgs.    
    extern const Literal SESSION_MEDIA;
    extern const Literal SESSION_QOS;
    extern const Literal SESSION_SECURITY;
   
    extern const Literal ZERO_CONTENT_LENGTH;

    /// possible valid methods
    typedef enum
    {
        SIP_UNKNOWN,
        SIP_INVITE,
        SIP_ACK,
        SIP_OPTIONS,
        SIP_BYE,
        SIP_CANCEL,
        SIP_REGISTER,
        SIP_INFO,
        SIP_TRANSFER,
        SIP_REFER,
        SIP_STATUS,
        SIP_SUBSCRIBE,
        SIP_NOTIFY,
        SIP_MESSAGE,
        SIP_UNKNOWNEXTN,
        SIP_INVALIDMSG
    } Method;

    /// enum for SNMP.
    typedef enum
    {
        INS,
        OUTS
    } SnmpType;
 
} // namespace Vocal



/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */


#endif
