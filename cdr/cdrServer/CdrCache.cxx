
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


static const char* const CdrCache_cxx_Version =
    "$Id: CdrCache.cxx,v 1.2 2004/06/14 00:33:53 greear Exp $";


#include <strstream>

#include "CdrCache.hxx"
#include "VCdrException.hxx"
#include "CdrFileHandler.hxx"
#include "cpLog.h"


// Global constants
//
const uint64 CDR_MAX_LIFE = 86400 * 1000;  // Max life of a call record
const uint64 CDR_BILL_LIFE = 10 * 1000;    // Max life of billed call in cache
const uint64 CDR_PURGE_FREQ = 10 * 1000;   // purge every 10 secs


CdrCache::CdrCache( const CdrConfig &cdata ) :
        EventObj(CDR_PURGE_FREQ, true),
        m_fileHandle(cdata.m_billingDirectory, cdata.m_billingFileName),
        m_data(cdata),
        m_lastFileCheck(0)
{
    m_fileHandle.open(O_WRONLY | O_CREAT | O_APPEND);
    m_fileHandle.setRolloverSize(cdata.m_rolloverSize);
    m_fileHandle.setRolloverPeriod(cdata.m_rolloverPeriod);
    m_fileHandle.setFileSuffix(cdata.m_unsentFileExt);
    m_fileHandle.setCheckFileSizeOn();
}

CdrCache::~CdrCache()
{
}

void
CdrCache::onTimer()
{
    purgeCache();

    // Since the timer goes off every CDR_PURGE_FREQ seconds, we keep
    // a count to figure out the fileCheckFreq
    //
    m_lastFileCheck += CDR_PURGE_FREQ;

    if ( m_lastFileCheck >= m_data.m_fileCheckFreq )
    {
        m_lastFileCheck = 0;
        try
        {
            m_fileHandle.checkFileSize();
        }
        catch ( VCdrException&e )
        {
            cpLog(LOG_ERR,
                  "WARNING:Billing data file %s is not accessible, trying to reopen",
                  m_fileHandle.getFullFileName().c_str());

            m_fileHandle.open(O_WRONLY | O_CREAT | O_APPEND);

            cpLog(LOG_ERR,
                  "WARNING:Reopened billing data file %s, Records may have been lost",
                  m_fileHandle.getFullFileName().c_str());
        }
    }
}

void
CdrCache::add( const CdrClient &dat ) throw (VCdrException&)
{
    CdrClient datCopy(dat);

    string evtStr(enumToString(datCopy.m_callEvent));
    string callId(datCopy.m_callId);

    cpLog(LOG_DEBUG, "Received %s from marshal, CallId:%s, From:%s, To:%s, StartTime:%lu",
          evtStr.c_str(),
          datCopy.m_callId,
          datCopy.m_userId,
          datCopy.m_recvId,
          datCopy.m_gwStartTime);

    // dump the record into the billing file
    //
    CdrRadius tmp;
    datCopy.m_callEvent = CALL_UNKNOWN;
    tmp.update(datCopy);  // passing unknown will force copying of all fields
    tmp.m_callEvent = dat.m_callEvent;
    datCopy.m_callEvent = dat.m_callEvent;

    m_fileHandle.writeCdr(tmp);

    CdrMap::iterator itr = m_cdrMap.find(callId.c_str());

    // Check if call record exists in cache, callId is unique
    //
    if ( itr != m_cdrMap.end() )
    {
        cpLog(LOG_DEBUG, "Found call record %s in map", evtStr.c_str());

        CdrRadius &ref = (*itr).second;

        //
        // update the record in cache
        //
        if (ref.m_callEvent == CALL_BILL)
        {
            cpLog(LOG_DEBUG,
                  "Received record out of order, call already billed, state:%s call ID:%s",
                  evtStr.c_str(), ref.m_callId);
        }
        else if ( (datCopy.m_callEvent == CALL_RING && m_data.m_billRingTime) ||
                  datCopy.m_callEvent == CALL_START )
        {
            ref.update(datCopy);
        }
        else if ( datCopy.m_callEvent == CALL_END )
        {
            ref.update(datCopy);

            // write out the completed billing record
            ref.m_callEvent = CALL_BILL;
            m_fileHandle.writeCdr(ref);

            cpLog(LOG_DEBUG, "Completed call Id: %s, wrote CALL_BILL to file", ref.m_callId);
        }

        if ( cpLogGetPriority() >= LOG_DEBUG_STACK )
        {
            // log the record
            char buffer[1024];
            ref.dumpFlds(buffer, sizeof(buffer));
            cpLog(LOG_DEBUG_STACK, buffer);
        }
    }
    else
    {
        if ( (datCopy.m_callEvent == CALL_START && datCopy.m_gwStartTime > 0) ||
             (datCopy.m_callEvent == CALL_RING && m_data.m_billRingTime && datCopy.m_gwStartRing > 0) )
        {
            CdrRadius rad;
            rad.update(datCopy);

            // add new record to cache
            m_cdrMap[callId] = rad;

            cpLog(LOG_DEBUG, "Received new record, state:%s call ID:%s",
                  evtStr.c_str(), callId.c_str());
        }
        else
        {
            cpLog(LOG_DEBUG, "Received uncorrelated record, state:%s call ID:%s",
                  evtStr.c_str(), callId.c_str());
        }
    }
}

void
CdrCache::purgeCache()
{
    unsigned long int now = time(0);
    CdrMap::iterator itr = m_cdrMap.begin();
    CdrMap::iterator itr2;

    while ( itr != m_cdrMap.end() )
    {
        CdrRadius &ref = (*itr).second;

        // purge old, dropped calls and billed calls
        if ( (ref.m_callEvent == CALL_BILL && (now - ref.m_gwEndTime) > CDR_BILL_LIFE) ||
             (ref.m_gwEndTime != 0 && (now - ref.m_gwEndTime) > CDR_MAX_LIFE ) ||
             (ref.m_gwEndTime == 0 && (now - ref.m_gwStartTime) > CDR_MAX_LIFE) )
        {
            cpLog(LOG_DEBUG_STACK, "Purging cache, deleting call Id:%s",ref.m_callId);

            itr2 = itr++;
            m_cdrMap.erase(itr2);
        }
        else
        {
            itr++;
        }
    }
}
