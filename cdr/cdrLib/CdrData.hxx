#ifndef  CdrData_h
#define CdrData_h

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


static const char* const CdrData_hxx_Version =
    "$Id: CdrData.hxx,v 1.1 2004/05/01 04:14:55 greear Exp $";

#include "global.h"
#include <string>
#include <list>

/** Enum's from MIND SDK Reference Guide, CDR Format section **/

///
enum CdrCallDirection
{ ///
    DIRECTION_IN = 'I',  ///
    DIRECTION_OUT = 'O'  ///
};

///
enum CdrCallType
{ ///
    TYPE_VOICE = 'V',  ///
    TYPE_FAX = 'F',  ///
    TYPE_DATA = 'D' ///
};

///
enum CdrCallParties
{ ///
    PHONE_TO_PHONE = 1,   ///
    DESK_TO_PHONE = 2,   ///
    PHONE_TO_DESK = 3,   ///
    PHONE_TO_PBX = 10,  ///
    DESK_TO_PBX = 11,  ///
    WEB_TO_PBX = 20  ///
};

///
enum CdrCallEnd
{ ///
    END_NORMAL = 'N',  ///
    END_DISCONNECTED = 'D',  ///
    END_ABANDONED = 'A',  ///
    END_BUSY = 'B',  ///
    END_ERROR = 'E'  ///
};

///
enum CdrErrorType
{ ///
    ERR_NORMAL = 0,   ///
    ERR_BUSY = 1,   ///
    ERR_ABANDONED = 2,   ///
    ERR_INVALID_PHONE = 3,   ///
    ERR_INVALID_USER = 4,   ///
    ERR_CARD_EXPIRED = 5,   ///
    ERR_CREDIT_LIMIT = 6,   ///
    ERR_BLOCKED_USER = 7,   ///
    ERR_RESTRICTED_PHONE = 8,   ///
    ERR_NO_FAX_PAPER = 9,   ///
    ERR_NO_ANSWER = 10,  ///
    ERR_NO_DIAL_TONE = 11,  ///
    ERR_NETWORK_ERROR = 12,  ///
    ERR_NO_ROUTE = 13,  ///
    ERR_NO_PORTS = 14,  ///
    ERR_GENERAL = 15,  ///
    ERR_USER_DEFINED = 1000 ///
};

/** Enums for MIND VSA Attributes **/

///
enum MindVsaCallerIdType
{ ///
    MIND_VSA_USER_ID = 0,  ///
    MIND_VSA_PIN_CODE = 1,  ///
    MIND_VSA_ANI_CODE = 2,  ///
    MIND_VSA_DOMAIN = 4  ///
};

///
enum MindVsaProtocol
{ ///
    MIND_VSA_DTMF = 0,  ///
    MIND_VSA_E164 = 1  ///
};

///
enum MindVsaStatus
{ ///
    MIND_VSA_OK = 0,   ///
    MIND_VSA_FAIL = 1,   ///
    MIND_VSA_INVALID_ARG = 2,   ///
    MIND_VSA_UNKNOWN_USER = 3,   ///
    MIND_VSA_ACCOUNT_USE = 4,   ///
    MIND_VSA_CARD_EXPIRED = 5,   ///
    MIND_VSA_CREDIT_LIMIT = 6,   ///
    MIND_VSA_USER_BLOCKED = 7,   ///
    MIND_VSA_BAD_LINE_NUM = 8,   ///
    MIND_VSA_INVALID_NUM = 11,  ///
    MIND_VSA_NO_RATE = 12,  ///
    MIND_VSA_NO_AUTH = 13,  ///
    MIND_VSA_NO_MONEY = 14,  ///
    MIND_VSA_ACCT_INACTIVE = 15  ///
};

/** Client specific enum **/

///
enum CdrCallEvent
{ ///
    CALL_UNKNOWN = 0,  ///   Message received out of order
    CALL_START = 1,    ///   Received an Invite SIP message
    CALL_UPDATE = 2,   ///   Received Re-invite to refresh session timer
    CALL_RING = 3,     ///   Received 18x status message
    CALL_END = 7,      ///   Received Bye SIP message
    CALL_COMPLETE = 8, ///   Obsolete
    CALL_BILL = 9      ///   Reconciled record for entire call
};


/**
   CdrClient is the minimum data required from the client
 **/

struct CdrClient
{ ///
    char m_callId[256];          /// CDR Server unique id
    char m_userId[128];          /// caller
    char m_recvId[128];          /// callee NOTE:Set m_protocolNum field
    unsigned long m_gwStartRing;          /// start time for ringing
    unsigned int m_gwStartRingMsec;      ///
    unsigned long m_gwStartTime;          /// seconds since Jan 1,1970
    unsigned int m_gwStartTimeMsec;      ///
    unsigned long m_gwEndTime;            /// seconds since Jan 1,1970
    unsigned int m_gwEndTimeMsec;        ///
    char m_originatorIp[24];     /// IP of the originator gw
    short m_originatorLine;       /// for IN calls only
    char m_terminatorIp[24];     /// IP of the terminating gw
    short m_terminatorLine;       /// for OUT calls only
    CdrCallType m_callType;             ///
    CdrCallParties m_callParties;          ///
    CdrCallEnd m_callDisconnect;       ///
    CdrCallEvent m_callEvent;            ///
    MindVsaProtocol m_protocolNum;        /// Applies to m_recvId
    CdrCallDirection m_callDirection;      ///

    ///
    CdrClient()
    {
        memset(this, 0, sizeof(CdrClient));
    }
};


/**
   CdrRadius contains the required fields to create a Radius accounting message.
   See Mind-iPhonEx SDK Reference Guide (CDR Format and Radius Protocol sections).
 **/

struct CdrRadius
{
    //// Client fields

    char m_callId[256];          /// CDR Server unique id
    unsigned long m_gwStartRing;          /// start time for ringing
    unsigned int m_gwStartRingMsec;      ///
    unsigned long m_gwEndTime;            /// needed to calculate duration
    unsigned int m_gwEndTimeMsec;        ///
    CdrCallEvent m_callEvent;            ///

    //// Required CDR Fields

    char m_userId[128];          /// code of user originating call
    unsigned long m_gwStartTime;          /// seconds since Jan 1,1970 (GMT+Bias)
    unsigned int m_gwStartTimeMsec;      ///
    unsigned int m_billingDuration;      /// call duration after connection established
    unsigned int m_billingDurationMsec;  ///
    char m_originatorIp[24];     /// IP of the originator gw
    short m_originatorLine;       /// for IN calls only
    char m_terminatorIp[24];     /// IP of the terminating gw
    short m_terminatorLine;       /// for OUT calls only
    char m_E164CalledNum[32];    /// in E.164 format, optional if DTMF exists
    char m_DTMFCalledNum[32];    /// number as dialed by user, optional if E.164
    CdrCallType m_callType;             ///
    CdrCallParties m_callParties;          ///
    CdrCallEnd m_callDisconnect;       ///
    CdrCallDirection m_callDirection;      ///

    //// Optional CDR Fields

    CdrErrorType m_errorType;            ///
    char m_errorDesc[32];        ///
    int m_faxPages;             /// number of pages faxed
    short m_faxPriority;          /// priority of fax transfer
    char m_ANI[32];              /// calling number id
    char m_DNIS[32];             /// destination party number
    int m_bytesSent;            /// number of bytes sent
    int m_bytesReceived;        /// number of bytes received
    int m_sequenceNumber;       /// sequence number of CDR record

    ///
    CdrRadius()
    {
        memset(this, 0, sizeof(CdrRadius));
    }
    ///
    void update(const CdrClient &cli);
    ///
    int mindCdrVsaFormat(unsigned char *buf) const;

    /// read from billing file, same format as dumpFlds()
    void setValues(list < string > &tokenList);

    /// write to billing file, same format as setValues()
    int dumpFlds(char *buf, int maxLen) const;
};

/// Global functions

///
string enumToString(const MindVsaStatus e);
///
string enumToString(const CdrCallEvent e);
///
CdrCallEvent stringToEnum(const string &s);
///
ostream& operator<< (ostream &strm, const CdrRadius &dat);

#endif
