
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


static const char* const CdrManager_cxx_Version =
    "$Id: CdrManager.cxx,v 1.1 2004/05/01 04:14:55 greear Exp $";


#include <time.h>

#include "CdrManager.hxx"
#include "CdrServer.hxx"
#include "CdrCache.hxx"
#include "EventObj.hxx"
#include "CommandLine.hxx"
#include "VCdrException.hxx"
#include "Condition.hxx"
#include "cpLog.h"

using namespace Vocal::Threads;

// Global constants
//
const unsigned long int MIN_SLEEP_TIME = 75000;
const int CYCLE_COUNT = 1000;

// Static declarations
CdrManager* CdrManager::m_instance = 0;


CdrManager&
CdrManager::instance( const CdrConfig *cdata )
{
    if (!m_instance)
    {
        assert(cdata);

        m_instance = new CdrManager(*cdata);
        m_instance->m_cdrServer = new CdrServer(*cdata);
        m_instance->m_cdrCache = new CdrCache(*cdata);

        // register events
        m_instance->registerEvent(m_instance->m_cdrServer);
        m_instance->registerEvent(m_instance->m_cdrCache);
    }
    return *m_instance;
}

CdrManager::CdrManager( const CdrConfig &cdata ) :
    m_configData(cdata),
    m_cdrServer(0),
    m_cdrCache(0)
{}


CdrManager::~CdrManager()
{
    // Since m_cdrServer and m_cdrCache are items in the list,
    // they will be deleted when the list gets deleted

    list < EventObj* > ::iterator itr = m_eventList.begin();
    while (itr != m_eventList.end())
    {
        EventObj *obj = *itr;
        itr = m_eventList.erase(itr);
        delete obj;
    }
}

void
CdrManager::destroy()
{
    delete m_instance;
    m_instance = 0;
}

void
CdrManager::run()
{
    time_t startTime = time(0);

    // These parameters control the number of data event
    // loop cycles before each sleep
    //
    int cycleCount = 0;
    bool isData = false;

    list < EventObj* > ::iterator itr;

    while (1)
    {
        cycleCount = 0;
        isData = false;

        // cycle through event list checking data events,
        // this loop gets priority so loop through it
        // many times before we sleep
        //
        while (cycleCount++ < CYCLE_COUNT)
        {
            itr = m_eventList.begin();
            while (itr != m_eventList.end())
            {
                EventObj *obj = *itr;

                if (obj->isDataReady(0))
                {
                    isData = true;
                    obj->onData();
                }

                if (obj->eventDone())
                {
                    itr = m_eventList.erase(itr);
                    delete obj;
                }
                else
                    itr++;
            }

            // if no data found, break from loop and sleep
            if (!isData)
            {
                break;
            }
        }

        // sleep here so we don't hog the CPU
        //
        vusleep(MIN_SLEEP_TIME);

        // get current time for time events
        //
        time_t now = time(0);

        unsigned long int timeSec = now - startTime;

        // cycle through event list checking time events,
        // we only need to check time events every second or so
        //
        itr = m_eventList.begin();
        while (itr != m_eventList.end())
        {
            EventObj *obj = *itr;

            if (obj->isTimeReady(timeSec))
            {
                obj->onTimer();
            }

            if (obj->eventDone())
            {
                itr = m_eventList.erase(itr);
                delete obj;
            }
            else
                itr++;
        }
    }
}

void
CdrManager::registerEvent( EventObj *obj )
{
    m_eventList.push_back(obj);
}

void
CdrManager::unregister( const EventObj *obj )
{
    for (list < EventObj* > ::iterator itr = m_eventList.begin();
         itr != m_eventList.end();
         itr++)
    {
        if ((*itr) == obj)
        {
            m_eventList.erase(itr);
            break;
        }
    }
}

void
CdrManager::addCache( const CdrClient &msg )
{
    m_cdrCache->add(msg);
}
