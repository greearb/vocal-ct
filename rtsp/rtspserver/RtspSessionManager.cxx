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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspSessionManager_cxx_version =
    "$Id: RtspSessionManager.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "RtspSessionManager.hxx"
#include "RtspRtpProcessor.hxx"


// singelton object definitionn
RtspSessionManager* RtspSessionManager::myInstance = 0;


RtspSessionManager::RtspSessionManager()
    : myMaxSessions( 1 ),
      mySessionIdList( 6000 )
{
#if defined (_linux_)    
    // register destroy function to delete singelton
    if( atexit( RtspSessionManager::destroy ) < 0 )
    {
        cpLog( LOG_ALERT, "Failed to register with atexit()" );
    };
#endif
}


RtspSessionManager&
RtspSessionManager::instance()
{
    if( myInstance == 0 )
    {
        myInstance = new RtspSessionManager();
    }
    return *myInstance;
}


RtspSessionManager::~RtspSessionManager()
{
}


void
RtspSessionManager::destroy()
{
    delete RtspSessionManager::myInstance;
    RtspSessionManager::myInstance = 0;
}


bool
RtspSessionManager::maxSessions( const int max )
{
    bool result = true;

    if( max <= 0 )
    {
        cpLog( LOG_ERR, "Max session invalid: %d, using default value", max );
        assert( ! ( myMaxSessions <= 0 ) );
        result = false;
    }
    else
    {
        myMaxSessions = max;
    }

    cpLog( LOG_DEBUG, "Maximum number of sesions: %d", myMaxSessions );

    return result;
}


Data
RtspSessionManager::addRtspSession( Sptr<RtspSession> session )
{
    Data id( "" );

    if( myMaxSessions == 0 )
    {
        cpLog (LOG_ERR, "Can't add anymore sessions" );
        return id;
    }

    if( session->sessionMode() == RTSP_SESSION_MODE_PLAY)
    {
        if( RtspRtpProcessor::instance().playThreadLoaded() )
        {
            cpLog( LOG_ERR, "Play thread overloaded!!!!!!!!!!!!!!!!!!!!!" );
            return id;
        }
    }
    else if( session->sessionMode() == RTSP_SESSION_MODE_REC )
    {
        if( RtspRtpProcessor::instance().recordThreadLoaded() )
        {
            cpLog( LOG_ERR, "Record thread overloaded!!!!!!!!!!!!!!!!!!!" );
            return id;
        }
    }
    else
    {
        cpLog( LOG_DEBUG, "Unknown session state" );
        return id;
    }

    id = findNewSessionId();
    LocalScopeAllocator lo;

    cpLog( LOG_DEBUG, "Adding %s to myRtspSessionMap, %d total",
           id.getData(lo), myRtspSessionMap.size() + 1 );

    session->sessionId( id );

    myRtspSessionMapMutex.lock();
    myRtspSessionMap[ id ] = session;
    myMaxSessions--;
    myRtspSessionMapMutex.unlock();

    return id;
}


bool
RtspSessionManager::delRtspSession( const Data& sessionId )
{
    Data result = sessionId ;

    myRtspSessionMapMutex.lock();
    if( myRtspSessionMap.find( sessionId ) == myRtspSessionMap.end() )
    {
        result = false;
    }
    else
    {
        myRtspSessionMap.erase( sessionId );
        myMaxSessions++;
        LocalScopeAllocator lo;

        cpLog( LOG_DEBUG, "Deleting %s from myRtspSessionMap, %d remaining",
               sessionId.getData(lo), myRtspSessionMap.size() );

    }
    myRtspSessionMapMutex.unlock();

    return true;
}


Sptr<RtspSession>
RtspSessionManager::getRtspSession( const Data& sessionId )
{
    Sptr<RtspSession> session = 0;

    myRtspSessionMapMutex.lock();
    RtspSessionMap::iterator itr = myRtspSessionMap.find( sessionId );
    if( itr == myRtspSessionMap.end() )
    {
        LocalScopeAllocator lo;

        cpLog( LOG_DEBUG, "Session %s not found", sessionId.getData(lo) );
    }
    else
    {
        session = itr->second;
    }
    myRtspSessionMapMutex.unlock();

    return session;
}


Sptr<RtspSession>
RtspSessionManager::getRtspSession()
{
    Sptr<RtspSession> session = 0;

    myRtspSessionMapMutex.lock();
    RtspSessionMap::iterator itr = myRtspSessionMap.begin();
    if( itr == myRtspSessionMap.end() )
    {
        cpLog( LOG_DEBUG, "No sessions in session manager" );
    }
    else
    {
        session = itr->second;
    }
    myRtspSessionMapMutex.unlock();

    return session;
}


Data
RtspSessionManager::findNewSessionId()
{
#ifdef NONRANDOM_SESSIONID
    Data result( mySessionIdList++ );
    return result;
#else

    Data result;
    while(1)
    {
        result = randomGen.get();
        if( myRtspSessionMap.find( result ) == myRtspSessionMap.end() )
        {
            break;
        }
    }

    return result;
#endif
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
