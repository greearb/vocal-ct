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


static const char* const StatisticsDb_cxx_Version = 
    "$Id: StatisticsDb.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include "StatisticsDb.hxx"
#include "Statistic.hxx"
#include "Lock.hxx"
#include "StackLock.hxx"
#include <cassert>

using namespace Vocal::Threads;

using Vocal::Statistics::StatisticsDb;
using Vocal::Statistics::StatisticsDbKey;
using Vocal::Statistics::Statistic;
//using Vocal::Threads::WriteLock;
//using Vocal::Threads::ReadLock;


const StatisticsDbKey  Vocal::Statistics::INVALID_STAT_DB_KEY = 0;


StatisticsDb::StatisticsDb(const char * name)
    :	myName(name ? name : "StatisticsDb"),
    	myNextKey(INVALID_STAT_DB_KEY+1)
{
    myStatNames.push_back(Data("INVALID_STAT_DB_KEY"));
}


StatisticsDb::~StatisticsDb()
{
    clear();
}


StatisticsDbKey
StatisticsDb::key(const Data & key)
{
    StackLock    lock(myLock);
    
    StatisticsDbKey   result = INVALID_STAT_DB_KEY;

    StatisticRegistrations::iterator	regIter;

    regIter = myStatRegistrations.find(key);
    
    if ( regIter == myStatRegistrations.end() )
    {
    	result = myStatRegistrations[key] = myNextKey++;
	myStatNames.push_back(key);
	
	assert( myStatNames.size() == myNextKey );
    }
    else
    {
    	result = regIter->second;
    }
    
    return ( result );
}


const Data &	    	
StatisticsDb::keyName(StatisticsDbKey index) const
{
    StackLock  lock(myLock);
    
    if ( index > myStatNames.size() )
    {
    	assert( index <= myStatNames.size() );

	index = INVALID_STAT_DB_KEY;
    }
    
    return ( myStatNames[index] );
}


StatisticsDbKey
StatisticsDb::nextKey() const
{
    return ( myNextKey );
}

void	
StatisticsDb::record(const Statistic & statistic)
{
    if ( !(statistic.db() == *this) )
    {
    	assert( statistic.db() == *this );
	return;
    }

    StackLock    lock(myLock);
    
    StatisticsDbKey key = statistic.key();
    
    Database::iterator	dbIter = m_db.find(key);
    
    if ( dbIter == m_db.end() )
    {
    	// Memory allocation
	//
    	m_db[key] = statistic.copy();	
    }
    else
    {
    	dbIter->second->combine(statistic);
    }
}


Statistic *
StatisticsDb::find(const Statistic & statistic) const
{
    Statistic * result = 0;
    
    if ( !(statistic.db() == *this) )
    {
    	assert( statistic.db() == *this );
	return ( result );
    }

    StackLock    lock(myLock);

    Database::const_iterator	dbIter = m_db.find(statistic.key());
    
    if ( dbIter != m_db.end() )
    {
    	result = dbIter->second;
    }
    
    return ( result );
}


void	
StatisticsDb::erase(const Statistic & statistic)
{
    if ( !(statistic.db() == *this) )
    {
    	assert( statistic.db() == *this );
	return;
    }

    StackLock    lock(myLock);

    Database::iterator	dbIter = m_db.find(statistic.key());
    
    if ( dbIter != m_db.end() )
    {
    	// Memory freed
	//
    	delete dbIter->second;
    	m_db.erase(dbIter);
    }
    
    return;
}


void	
StatisticsDb::clear()
{
    StackLock    lock(myLock);

    for (   Database::iterator dbIter = m_db.begin();
    	    dbIter != m_db.end();
	    dbIter++
	)
    {
    	// Memory freed
	//
    	delete dbIter->second;
    }

    m_db.clear();
}


ostream &   
StatisticsDb::writeTo(ostream & out) const
{
    StackLock    lock(myLock);

    out << '\n' << myName.logData() << '\n';
    
    for (   Database::const_iterator dbIter = m_db.begin();
    	    dbIter != m_db.end();
	    dbIter++
	)
    {
    	out << *(dbIter->second) << '\n';
    }
    
    return ( out );
}


const Data &	
StatisticsDb::name() const
{
    return ( myName );
}


bool 
StatisticsDb::operator==(const StatisticsDb & src) const
{
    return ( this == &src );
}


bool 
StatisticsDb::operator<(const StatisticsDb & src) const
{
    return ( this < &src );
}
