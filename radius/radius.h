#ifndef radius_h
#define radius_h

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2001 Vovida Networks, Inc.  All rights reserved.
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

static const char* const radius_h_Version =
    "$Id: radius.h,v 1.1 2004/05/01 04:15:22 greear Exp $";


#include "vtypes.h"


/**
   RADIUS data format

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   |                         Authenticator                         |
   |                                                               |
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Attributes ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-
 */

/** Lengths define in RFC 2865 Section 3 
 */
const u_int16_t RadiusPacketHeaderSize = 20;
const u_int16_t RadiusMinPacketSize = RadiusPacketHeaderSize;
const u_int16_t RadiusMaxPacketSize = 4096;
const u_int16_t RadiusAuthenticatorLength = 16;

struct RadiusHdr
{
    u_int8_t  code;
    u_int8_t  identifier;
    u_int16_t length;
    u_int8_t  authenticator[ RadiusAuthenticatorLength ];
};

/** Raw Radius message data buffer
 */
union RawMessage
{
    /* Packet buffer */
    u_int8_t buffer[ RadiusMaxPacketSize ];
    /* TODO: Buffer size not big enough for Calculating authenticator */

    /* Message header */
    RadiusHdr msgHdr;
};


/** RADIUS Codes
 */
typedef u_int8_t RadiusPacketType;

const u_int8_t RP_ACCESS_REQUEST      =  1;
const u_int8_t RP_ACCESS_ACCEPT       =  2;
const u_int8_t RP_ACCESS_REJECT       =  3;
const u_int8_t RP_ACCOUNTING_REQUEST  =  4;
const u_int8_t RP_ACCOUNTING_RESPONSE =  5;
const u_int8_t RP_ACCESS_CHALLENGE    = 11;


// Attribute format
//
//    0                   1                   2
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//   |     Type      |    Length     |  Value ...
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

const u_int16_t RadiusMaxAttributeLength = 255;    // 256 - 2 + 1 for \0

/** RADIUS Attribute Types
    TODO: Add other attributes
 */
typedef u_int8_t RadiusAttributeType;

const u_int8_t RA_USER_NAME            =  1;
const u_int8_t RA_USER_PASSWORD        =  2;
const u_int8_t RA_CHAP_PASSWORD        =  3;
const u_int8_t RA_NAS_IP_ADDRESS       =  4;
const u_int8_t RA_NAS_PORT             =  5;
const u_int8_t RA_SERVICE_TYPE         =  6;
const u_int8_t RA_REPLY_MESSAGE        = 18;
const u_int8_t RA_STATE                = 24;
const u_int8_t RA_CLASS                = 25;
const u_int8_t RA_VENDOR_SPECIFIC      = 26;
const u_int8_t RA_SESSION_TIMEOUT      = 27;
const u_int8_t RA_CALLED_STATION_ID    = 30;
const u_int8_t RA_CALLING_STATION_ID   = 31;
const u_int8_t RA_ACCT_STATUS_TYPE     = 40;
const u_int8_t RA_ACCT_DELAY_TIME      = 41;
const u_int8_t RA_ACCT_INPUT_OCTETS    = 42;
const u_int8_t RA_ACCT_OUTPUT_OCTETS   = 43;
const u_int8_t RA_ACCT_SESSION_ID      = 44;
const u_int8_t RA_ACCT_AUTHENTIC       = 45;
const u_int8_t RA_ACCT_SESSION_TIME    = 46;
const u_int8_t RA_ACCT_INPUT_PACKETS   = 47;
const u_int8_t RA_ACCT_OUTPUT_PACKETS  = 48;
const u_int8_t RA_ACCT_TERMINATE_CAUSE = 49;
const u_int8_t RA_NAS_PORT_TYPE        = 61;
const u_int8_t RA_DIGEST_RESPONSE      = 206;
const u_int8_t RA_DIGEST_ATTRIBUTES    = 207;
const u_int8_t    RA_DIGEST_REALM      = 1;
const u_int8_t    RA_DIGEST_NONCE      = 2;
const u_int8_t    RA_DIGEST_METHOD     = 3;
const u_int8_t    RA_DIGEST_URI        = 4;
const u_int8_t    RA_DIGEST_ALGORITHM  = 6;
const u_int8_t    RA_DIGEST_USER_NAME  = 10;

/** RA_ACCT_STATUS_TYPE Values
 */
typedef u_int8_t RadiusAcctStatusType;

const u_int32_t RAS_START   = 1;
const u_int32_t RAS_STOP    = 2;
const u_int32_t RAS_INTERIM = 3;
const u_int32_t RAS_ON      = 7;
const u_int32_t RAS_OFF     = 8;


/** RA_ACCT_TERMINATE_CAUSE Values (incomplete)
 */
typedef u_int8_t RadiusAcctTerminateCauseType;

const u_int32_t RATC_USER_REQUEST         =  1;
const u_int32_t RATC_LOST_CARRIER         =  2;
const u_int32_t RATC_LOST_SERVICE         =  3;
const u_int32_t RATC_IDLE_TIMEOUT         =  4;
const u_int32_t RATC_SESSION_TIMEOUT      =  5;
const u_int32_t RATC_ADMIN_RESET          =  6;
const u_int32_t RATC_ADMIN_REBOOT         =  7;
const u_int32_t RATC_PORT_ERROR           =  8;
const u_int32_t RATC_NAS_ERROR            =  9;
const u_int32_t RATC_NAS_REQUEST          = 10;
const u_int32_t RATC_NAS_REBOOT           = 11;
const u_int32_t RATC_SERVICE_UNAVAILABLE  = 15;


/** RA_SERVICE_TYPE Values (incomplete)
 */
typedef u_int8_t RadiusServiceTypeType;

const u_int32_t RAST_LOGIN         =  1;


/** RA_NAS_PORT_TYPE Values (incomplete)
 */
typedef u_int8_t RadiusNasPortTypeType;

const u_int32_t RANPT_VIRTUAL         =  5;

#endif
