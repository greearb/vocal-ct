
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


static const char* const CdrUserCache_cxx_Version =
    "$Id: CdrUserCache.cxx,v 1.1 2004/05/01 04:14:55 greear Exp $";


#include "CdrUserCache.hxx"
#include "ProvisionInterface.hxx"
#include "cpLog.h"

// Initialize static vars
UserMap CdrUserCache::m_userMap;
VRWLock CdrUserCache::m_lock;
unsigned long int CdrUserCache::m_oldestIdx  = 0;
unsigned long int CdrUserCache::m_currentIdx = 0;

// Global constants
const unsigned long int MAX_CACHE_SIZE = 10000;    // Max # entries in userCache
const unsigned long int MAX_CACHE_DELETES = 5000;  // Max # entries to delete from userCache


CdrUserCache::CdrUserCache()
{
    try
    {
        // register for provisioning updates on "Accounts" and "Aliases"
	//
        ProvisionInterface::instance().registerDirForUpdate(
            CdrUserCache::updateMasterId,
            "Accounts" );
        //
        ProvisionInterface::instance().registerDirForUpdate(
            CdrUserCache::updateAliases,
            "Aliases" );
    }
    catch (VException& e)
    {
        cpLog( LOG_ALERT, "Failed to connect to Provisioning Server, reason %s",
               e.getDescription().c_str());
    }
}


CdrUserCache::~CdrUserCache()
{}


void
CdrUserCache::destroy()
{
    m_lock.WriteLock();
    m_userMap.clear();
    m_oldestIdx = 0;
    m_currentIdx = 0;
    m_lock.Unlock();
}


string
CdrUserCache::getCustomerId( const string& aliasId )
{
    //
    // The map structure is:
    //  map< aliasId, pair< masterId, index > >
    //

    string masterId( aliasId );
    bool found = false;

    m_lock.WriteLock();

    UserMap::iterator itr = m_userMap.find(aliasId.c_str());
    if ( itr != m_userMap.end() )
    {
        found = true;
        masterId = (*itr).second.first;           // return the alias name
        (*itr).second.second = m_currentIdx++;    // update the index
    }

    m_lock.Unlock();

    if ( !found )
    {
        // check if provisioning has the id and load it
        // assume ProvisionInterface has already been initialized
        //
        try
        {
            // if the aliasId is not the master, get the master
            if ( ! ProvisionInterface::instance().isAccount( aliasId ) )
            {
               masterId  = ProvisionInterface::instance().getMasterUser( aliasId );
            }

            found = true;
        }
        catch ( VMissingDataException &e )
        {
            cpLog( LOG_DEBUG, "User %s not found in alias list", aliasId.c_str() );
        }
        catch ( VException &e )
        {
            cpLog( LOG_ALERT, "Failed to connect to Provisioning Server, reason %s",
                   e.getDescription().c_str());
        }
    
        unsigned long int deleteCount = 0;
    
        // Lock map before accessing
        m_lock.WriteLock();

        if ( found )
	{
            // if cache is too large, delete half the oldest entries
            //
            if ( m_userMap.size() >= MAX_CACHE_SIZE )
            {
                deleteCount = cleanCache();
            }
	}
    
        // add user to cache, even if it is not in provisioning
        //
        pair < string, unsigned long int > masterPair = make_pair( masterId, m_currentIdx++ );
        m_userMap[ aliasId ] = masterPair;

        // Always ensure map is unlocked
        m_lock.Unlock();

        // Do cpLogs outside of the lock
        //
        if ( deleteCount > 0 )
        {
            cpLog( LOG_INFO, "User cache cleaned, deleted %d entries from the cache",
                   deleteCount );
        }
    }

    return masterId;
}


int
CdrUserCache::cleanCache()
{
    // Set delete criteria, delete half of the cache
    //
    unsigned long int upperRange = ((m_currentIdx - m_oldestIdx) / 2) + m_oldestIdx;
    
    // update index of the oldest record
    m_oldestIdx = upperRange;

    unsigned long int deleteCount = 0;
    
    UserMap::iterator itr = m_userMap.begin();

    // Delete all entries within the delete range
    //
    while ( itr != m_userMap.end() )
    {
        if ( (*itr).second.second < upperRange )
        {
            UserMap::iterator itr2 = itr++;
            m_userMap.erase(itr2);
    
            if ( ++deleteCount > MAX_CACHE_DELETES )
		break;
        }
        else
        {
            itr++;
        }
    }

    if ( deleteCount == 0 && m_userMap.size() > 0 )
    {
        // recursively call cleanCache() until at least 1 element is deleted
        deleteCount = cleanCache();
    }
    
    return deleteCount;
}


void
CdrUserCache::updateAliases( const string& data,
                             const string& fileName,
                             const bool deletedDir )
{
    // we want to ignore deleting directory, should never happen
    if (deletedDir)
    {
        return ;
    }

    cpLog( LOG_INFO, "Received update for user alias: %s", data.c_str() );

    m_lock.WriteLock();

    // just remove user alias from cache, will be added next time it is used

    UserMap::iterator itr = m_userMap.find( data );

    if ( itr != m_userMap.end() )
    {
        m_userMap.erase(itr);
    }

    m_lock.Unlock();
}


void
CdrUserCache::updateMasterId( const string& data,
                              const string& fileName,
                              const bool deletedDir )
{
    // we want to ignore deleting directory, should never happen
    if (deletedDir)
    {
        return ;
    }

    cpLog( LOG_INFO, "Received update for user: %s", data.c_str() );

    m_lock.WriteLock();

    // just remove user from cache, will be added next time it is used

    UserMap::iterator itr = m_userMap.begin();
    UserMap::iterator itr2;
    
    while ( itr != m_userMap.end() )
    {
        if ( (*itr).second.first == data )
        {
            itr2 = itr++;
            m_userMap.erase(itr2);
        }
        else
        {
            itr++;
        }
    }

    m_lock.Unlock();
}
