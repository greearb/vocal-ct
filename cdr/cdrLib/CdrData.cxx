
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


static const char* const CdrData_cxx_Version =
    "$Id: CdrData.cxx,v 1.1 2004/05/01 04:14:55 greear Exp $";

#include "global.h"
#include <time.h>
#include <strstream>
#include <strings.h>
#include <stdio.h>
#include <iostream.h>

#include "CdrData.hxx"


void
CdrRadius::update( const CdrClient &cli )
{
    m_callEvent = cli.m_callEvent;

    if ( cli.m_callEvent == CALL_RING )
    {
        strncpy( m_callId, cli.m_callId, sizeof(m_callId) );
        strncpy( m_userId, cli.m_userId, sizeof(m_userId) );

        m_gwStartRing = cli.m_gwStartRing;
        m_gwStartRingMsec = cli.m_gwStartRingMsec;

        m_gwStartTime = cli.m_gwStartRing;
        m_gwStartTimeMsec = cli.m_gwStartRingMsec;

        m_gwEndTime = 0;
        m_billingDuration = 0;
        m_billingDurationMsec = 0;

        strncpy( m_originatorIp, cli.m_originatorIp, sizeof(m_originatorIp) );
        m_originatorLine = cli.m_originatorLine;
        strncpy( m_terminatorIp, cli.m_terminatorIp, sizeof(m_terminatorIp) );
        m_terminatorLine = cli.m_terminatorLine;

        if (cli.m_protocolNum == MIND_VSA_DTMF)
        {
            strncpy( m_DTMFCalledNum, cli.m_recvId, sizeof(m_DTMFCalledNum) );
        }
        else
        {
            strncpy( m_E164CalledNum, cli.m_recvId, sizeof(m_E164CalledNum) );
        }

        m_callType = cli.m_callType;
        m_callParties = cli.m_callParties;
        m_callDirection = cli.m_callDirection;
        m_callDisconnect = END_NORMAL;
    }
    else if ( cli.m_callEvent == CALL_START )
    {
        strncpy( m_callId, cli.m_callId, sizeof(m_callId) );
        strncpy( m_userId, cli.m_userId, sizeof(m_userId) );

        m_gwStartTime = cli.m_gwStartTime;
        m_gwStartTimeMsec = cli.m_gwStartTimeMsec;
        m_gwEndTime = 0;
        m_billingDuration = 0;
        m_billingDurationMsec = 0;

        strncpy( m_originatorIp, cli.m_originatorIp, sizeof(m_originatorIp) );
        m_originatorLine = cli.m_originatorLine;
        strncpy( m_terminatorIp, cli.m_terminatorIp, sizeof(m_terminatorIp) );
        m_terminatorLine = cli.m_terminatorLine;

        if (cli.m_protocolNum == MIND_VSA_DTMF)
        {
            strncpy( m_DTMFCalledNum, cli.m_recvId, sizeof(m_DTMFCalledNum) );
        }
        else
        {
            strncpy( m_E164CalledNum, cli.m_recvId, sizeof(m_E164CalledNum) );
        }

        m_callType = cli.m_callType;
        m_callParties = cli.m_callParties;
        m_callDirection = cli.m_callDirection;
        m_callDisconnect = END_NORMAL;
    }
    else if ( cli.m_callEvent == CALL_END )
    {
        m_gwEndTime = cli.m_gwEndTime;
        m_gwEndTimeMsec = cli.m_gwEndTimeMsec;
        m_callDisconnect = cli.m_callDisconnect;

        // bill the ring time
        if ( m_gwStartRing > 0 )
        {
            m_gwStartTime = m_gwStartRing;
            m_gwStartTimeMsec = m_gwStartRingMsec;
        }

        if ( m_gwStartTime > m_gwEndTime )
        {
            m_billingDuration = 0;
            m_billingDurationMsec = 0;
        }
        else
        {
            m_billingDuration = m_gwEndTime - m_gwStartTime;

            if ( m_billingDuration > 0 && m_gwEndTimeMsec < m_gwStartTimeMsec )
            {
                m_billingDuration--;
                m_billingDurationMsec = m_gwEndTimeMsec + 1000;
            }

            if ( m_gwStartTimeMsec > m_gwEndTimeMsec )
                m_billingDurationMsec = 0;
            else
                m_billingDurationMsec = m_gwEndTimeMsec - m_gwStartTimeMsec;
        }
    }
    else
    {
        // copy all the fields
        strncpy( m_callId, cli.m_callId, sizeof(m_callId) );
        strncpy( m_userId, cli.m_userId, sizeof(m_userId) );

        m_gwStartRing = cli.m_gwStartRing;
        m_gwStartRingMsec = cli.m_gwStartRingMsec;

        m_gwStartTime = cli.m_gwStartTime;
        m_gwStartTimeMsec = cli.m_gwStartTimeMsec;

        m_gwEndTime = cli.m_gwEndTime;
        m_gwEndTimeMsec = cli.m_gwEndTimeMsec;
        m_callDisconnect = cli.m_callDisconnect;

        m_billingDuration = 0;
        m_billingDurationMsec = 0;

        strncpy( m_originatorIp, cli.m_originatorIp, sizeof(m_originatorIp) );
        m_originatorLine = cli.m_originatorLine;
        strncpy( m_terminatorIp, cli.m_terminatorIp, sizeof(m_terminatorIp) );
        m_terminatorLine = cli.m_terminatorLine;

        if ( cli.m_protocolNum == MIND_VSA_DTMF )
        {
            strncpy( m_DTMFCalledNum, cli.m_recvId, sizeof(m_DTMFCalledNum) );
        }
        else
        {
            strncpy( m_E164CalledNum, cli.m_recvId, sizeof(m_E164CalledNum) );
        }

        m_callType = cli.m_callType;
        m_callParties = cli.m_callParties;
        m_callDirection = cli.m_callDirection;
        m_callDisconnect = END_ERROR;
    }
}

int
CdrRadius::mindCdrVsaFormat( unsigned char *buf ) const
{
    if (buf == 0)
    {
        return 0;
    }

    unsigned char *ptr = buf;
    char dataFld[1024];
    unsigned char fldLen = 0;
    int cdrLen = 0;

    tm *gwTime = gmtime((time_t*) & m_gwStartTime);

    int hour = (int) m_billingDuration / 360;
    int min = (int) ((m_billingDuration % 360) / 60);
    int sec = m_billingDuration - (min * 60);

    // Gateway start date/time
    sprintf( dataFld, "\"%02d-%02d-%02d %02d:%02d:%02d\";",
             (1900 + gwTime->tm_year), (gwTime->tm_mon + 1), gwTime->tm_mday,
             gwTime->tm_hour, gwTime->tm_min, gwTime->tm_sec );

    fldLen = strlen( dataFld );
    cdrLen += fldLen;
    memcpy( ptr, dataFld, fldLen );
    ptr += fldLen;

    // Gw start time (secs since epoch); billing duration;
    sprintf( dataFld, "%lu;\"%03d:%02d:%02d\";",
             m_gwStartTime, hour, min, sec );

    fldLen = strlen(dataFld);
    cdrLen += fldLen;
    memcpy( ptr, dataFld, fldLen );
    ptr += fldLen;

    // orig IP; orig Line; term IP; term Line;
    sprintf( dataFld, "%s;%d;%s;%d;",
             m_originatorIp, m_originatorLine,
             m_terminatorIp, m_terminatorLine );

    fldLen = strlen(dataFld);
    cdrLen += fldLen;
    memcpy(ptr, dataFld, fldLen);
    ptr += fldLen;

    // userId; E.164; DTMF; calltype; callparties; calldisconnect;
    // skip 4 fields; ANI
    sprintf( dataFld, "%s;%s;%s;%c%c;%02d;%c;;;;;%s",
             m_userId, m_E164CalledNum, m_DTMFCalledNum,
             m_callDirection, m_callType,
             (int)m_callParties, m_callDisconnect, m_ANI );

    fldLen = strlen(dataFld);
    cdrLen += fldLen;
    memcpy( ptr, dataFld, fldLen );
    ptr += fldLen;

    return cdrLen;
}

void
CdrRadius::setValues( list < string > &tokenList )
{
    string dateStr;
    string timeStr;
    string durStr;

    list < string > ::iterator itr = tokenList.begin();

    // m_callEvent
    if (itr != tokenList.end())
    {
        m_callEvent = stringToEnum(*itr);
        itr++;
    }

    // m_callId
    if (itr != tokenList.end())
    {
        strncpy( m_callId, (*itr).c_str(), sizeof(m_callId));
        itr++;
    }

    // m_userId
    if (itr != tokenList.end())
    {
        strncpy(m_userId, (*itr).c_str(), sizeof(m_userId));
        itr++;
    }

    // m_ANI
    if (itr != tokenList.end())
    {
        strncpy(m_ANI, (*itr).c_str(), sizeof(m_ANI));
        itr++;
    }

    // m_DTMFCalledNum
    if (itr != tokenList.end())
    {
        strncpy(m_DTMFCalledNum, (*itr).c_str(), sizeof(m_DTMFCalledNum));
        itr++;
    }

    // m_E164CalledNum
    if (itr != tokenList.end())
    {
        strncpy(m_E164CalledNum, (*itr).c_str(), sizeof(m_E164CalledNum));
        itr++;
    }

    // formatted start date field
    if (itr != tokenList.end())
    {
        itr++;  // skip date field
    }

    // formatted start time field

    if (itr != tokenList.end())
    {
        itr++;  // skip time field
    }

    // m_gwStartTime

    if (itr != tokenList.end())
    {
        m_gwStartTime = strtol((*itr).c_str(), (char **)0, 10);
        itr++;
    }

    // m_gwStartTimeMsec
    if (itr != tokenList.end())
    {
        m_gwStartTimeMsec = atoi((*itr).c_str());
        itr++;
    }

    // formatted end date field
    if (itr != tokenList.end())
        itr++;  // skip date field

    // formatted end time field
    if (itr != tokenList.end())
        itr++;  // skip time field

    // m_gwEndTime
    if (itr != tokenList.end())
    {
        m_gwEndTime = strtol((*itr).c_str(), (char **)0, 10);
        itr++;
    }

    // m_gwEndTimeMsec
    if (itr != tokenList.end())
    {
        m_gwEndTimeMsec = atoi((*itr).c_str());
        itr++;
    }

    // m_gwStartRing
    if (itr != tokenList.end())
    {
        m_gwStartRing = strtol((*itr).c_str(), (char **)0, 10);
        itr++;
    }

    // m_gwStartRingMsec
    if (itr != tokenList.end())
    {
        m_gwStartRingMsec = atoi((*itr).c_str());
        itr++;
    }

    // formatted duration field
    if (itr != tokenList.end())
        itr++;  // skip duration field

    // m_billingDuration
    if (itr != tokenList.end())
    {
        m_billingDuration = atoi((*itr).c_str());
        itr++;
    }

    // m_billingDurationMsec
    if (itr != tokenList.end())
    {
        m_billingDurationMsec = atoi((*itr).c_str());
        itr++;
    }

    // m_originatorIp
    if (itr != tokenList.end())
    {
        strncpy(m_originatorIp, (*itr).c_str(), sizeof(m_originatorIp));
        itr++;
    }

    // m_originatorLine
    if (itr != tokenList.end())
    {
        m_originatorLine = atoi((*itr).c_str());
        itr++;
    }

    // m_terminatorIp
    if (itr != tokenList.end())
    {
        strncpy(m_terminatorIp, (*itr).c_str(), sizeof(m_terminatorIp));
        itr++;
    }

    // m_terminatorLine
    if (itr != tokenList.end())
    {
        m_terminatorLine = atoi((*itr).c_str());
        itr++;
    }

    // m_callType
    if (itr != tokenList.end())
    {
        m_callType = (CdrCallType)(*itr)[0];
        itr++;
    }

    // m_callParties
    if (itr != tokenList.end())
    {
        m_callParties = (CdrCallParties)atoi((*itr).c_str());
        itr++;
    }

    // m_callDisconnect
    if (itr != tokenList.end())
    {
        m_callDisconnect = (CdrCallEnd)(*itr)[0];
        itr++;
    }

    // m_callDirection
    if (itr != tokenList.end())
    {
        m_callDirection = (CdrCallDirection)(*itr)[0];
        itr++;
    }
}

int
CdrRadius::dumpFlds( char *buf, int maxLen ) const
{
    if (buf == 0 || maxLen == 0)
    {
        return 0;
    }

    time_t t = m_gwStartTime;
    tm *st = gmtime(&t);

    char datebuf[32];
    sprintf(datebuf, "%02d/%02d/%04d",
            (st->tm_mon + 1),
            st->tm_mday,
            (1900 + st->tm_year));

    char timebuf[32];
    sprintf(timebuf, "%02d:%02d:%02d",
            st->tm_hour,
            st->tm_min,
            st->tm_sec);

    time_t t2 = m_gwEndTime;
    tm *st2 = gmtime(&t2);

    char datebuf2[32];
    sprintf(datebuf2, "%02d/%02d/%04d",
            (st2->tm_mon + 1),
            st2->tm_mday,
            (1900 + st2->tm_year));

    char timebuf2[32];
    sprintf(timebuf2, "%02d:%02d:%02d",
            st2->tm_hour,
            st2->tm_min,
            st2->tm_sec);

    char durationbuf[32];
    sprintf(durationbuf, "%03d:%02d:%02d",
            (m_billingDuration / 60),
            (m_billingDuration % 60),
            m_billingDurationMsec);

    strstream strm(buf, maxLen);

    strm << enumToString(m_callEvent) << ","
    << m_callId << ","
    << m_userId << ","
    << m_ANI << ","
    << m_DTMFCalledNum << ","
    << m_E164CalledNum << ","
    << datebuf << ","
    << timebuf << ","
    << m_gwStartTime << ","
    << m_gwStartTimeMsec << ","
    << datebuf2 << ","
    << timebuf2 << ","
    << m_gwEndTime << ","
    << m_gwEndTimeMsec << ","
    << m_gwStartRing << ","
    << m_gwStartRingMsec << ","
    << durationbuf << ","
    << m_billingDuration << ","
    << m_billingDurationMsec << ","
    << m_originatorIp << ","
    << m_originatorLine << ","
    << m_terminatorIp << ","
    << m_terminatorLine << ","
    << (char) m_callType << ","
    << (int) m_callParties << ","
    << (char) m_callDisconnect << ","
    << (char) m_callDirection
    << ends;

    return strlen(buf);
}

string
enumToString( const MindVsaStatus e )
{
    switch (e)
    {
        case MIND_VSA_OK:
        return "MIND_VSA_OK";
        case MIND_VSA_FAIL:
        return "MIND_VSA_FAIL";
        case MIND_VSA_INVALID_ARG:
        return "MIND_VSA_INVALID_ARG";
        case MIND_VSA_UNKNOWN_USER:
        return "MIND_VSA_UNKNOWN_USER";
        case MIND_VSA_ACCOUNT_USE:
        return "MIND_VSA_ACCOUNT_USE";
        case MIND_VSA_CARD_EXPIRED:
        return "MIND_VSA_CARD_EXPIRED";
        case MIND_VSA_CREDIT_LIMIT:
        return "MIND_VSA_CREDIT_LIMIT";
        case MIND_VSA_USER_BLOCKED:
        return "MIND_VSA_USER_BLOCKED";
        case MIND_VSA_BAD_LINE_NUM:
        return "MIND_VSA_BAD_LINE_NUM";
        case MIND_VSA_INVALID_NUM:
        return "MIND_VSA_INVALID_NUM";
        case MIND_VSA_NO_RATE:
        return "MIND_VSA_NO_RATE";
        case MIND_VSA_NO_AUTH:
        return "MIND_VSA_NO_AUTH";
        case MIND_VSA_NO_MONEY:
        return "MIND_VSA_NO_MONEY";
        case MIND_VSA_ACCT_INACTIVE:
        return "MIND_VSA_ACCT_INACTIVE";
    }
    return "UNKNOWN";
}

string
enumToString( const CdrCallEvent e )
{
    switch (e)
    {
        case CALL_START:
        return "CALL_START";
        case CALL_UPDATE:
        return "CALL_UPDATE";
        case CALL_RING:
        return "CALL_RING";
        case CALL_END:
        return "CALL_END";
        case CALL_COMPLETE:
        return "CALL_COMPLETE";
        case CALL_BILL:
        return "CALL_BILL";
        default:
        return "CALL_UNKNOWN";
    }
}

CdrCallEvent
stringToEnum( const string &s )
{
    if (s == "CALL_START") return CALL_START;
    else if (s == "CALL_UPDATE") return CALL_UPDATE;
    else if (s == "CALL_RING") return CALL_RING;
    else if (s == "CALL_END") return CALL_END;
    else if (s == "CALL_COMPLETE") return CALL_COMPLETE;
    else if (s == "CALL_BILL") return CALL_BILL;

    return CALL_UNKNOWN;
}

ostream&
operator<< ( ostream &strm, const CdrRadius &dat )
{
    char buffer[2048];
    dat.dumpFlds(buffer, 2048);
    strm << buffer;
    return strm;
}
