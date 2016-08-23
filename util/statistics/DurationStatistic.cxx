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


#include "global.h"
#include "DurationStatistic.hxx"
#include <cassert>


using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::Statistic;
using Vocal::Statistics::StatisticsDb;
using Vocal::Statistics::Duration;


DurationStatistic::DurationStatistic(
    StatisticsDb    	&   p_db, 
    const Data      	&   p_name,
    u_int32_t	    	    p_window
)
    :	Statistic(p_db, p_name),
    	m_duration(),
    	m_next(0),
	m_last(0),
	m_window(p_window),
	m_size(1),
	m_length(0)
{
}


DurationStatistic::DurationStatistic(const DurationStatistic & src)
    :	Statistic(src),
    	m_duration(src.m_duration),
	m_next(0),
	m_last(0),
	m_window(src.m_window),
	m_size(1),
	m_length(src.m_duration.length())	
{
    // If the given statistic is a combination, just append the entire
    // list to the end, which will give us two copies of the first
    // element. Then just pop the first one off.
    //
    if ( src.m_next != 0 )
    {
    	append(src);
	pop();
    }
}


const DurationStatistic & 
DurationStatistic::operator=(const DurationStatistic & src)
{
    if ( this != &src )
    {
    	clear();
	
    	Statistic::operator=(src);
	
	m_duration = src.m_duration;
	m_next = 0;
	m_last = 0;
	m_window = src.m_window;
	m_size = 1;
	m_length = src.m_duration.length();    

	// See copy ctor.
	//
    	if ( src.m_next != 0 )
	{
    	    append(src);
	    pop();
	}
    }
    return ( *this );
}


DurationStatistic::~DurationStatistic() throw (Vocal::SystemException)
{
    clear();
}

    	
void
DurationStatistic::combine(const Statistic & src)
{
    const DurationStatistic * srcD 
    	= dynamic_cast<const DurationStatistic *>(&src);
    
    if ( srcD == 0 )
    {
    	assert( srcD != 0 );
	return;
    }

    append(*srcD);
    
    if ( m_window > 0 )
    {
	while ( m_size > m_window )
	{
    	    pop();
	}
    }
}


Statistic *
DurationStatistic::copy() const
{
    return ( new DurationStatistic(*this) );
}


DurationStatistic *
DurationStatistic::next() const
{
    return ( m_next );
}


u_int32_t   
DurationStatistic::size() const
{
    return ( m_size );
}
	
	
u_int32_t   
DurationStatistic::window() const
{
    return ( m_window );
}
	
	
int64_t 
DurationStatistic::length() const
{
    return ( m_length );
}
	

double	
DurationStatistic::average() const
{
    int64_t len = length();
    
    return ( static_cast<double>(len) / size() );
}


ostream &   
DurationStatistic::writeTo(ostream & out) const
{
    out << name().logData() << ":\n{\n  data: (" << 
	#ifdef WIN32 
	(int) // I don't want to deal with it.
	#endif
	m_duration.length();
    
    for (   DurationStatistic * current = m_next; 
    	    current != 0; 
	    current = current->m_next 
	)
    {
    	out << "," << 
	#ifdef WIN32 
	(int) // I don't want to deal with it.
	#endif
	current->m_duration.length();
    }

    out << "),\n  window: " << window()
    	<< ",\n  size: " << size()
	<< ",\n  length: " << 
	#ifdef WIN32 
	(int) // I don't want to deal with it.
	#endif
	length()
	<< ",\n  average: " << average() << "\n}";

    return ( out );
}
	 

DurationStatistic::DurationStatistic(
    const Statistic    	&   p_stat, 
    const Duration     	&   p_duration,
    u_int32_t	    	    p_window
)
    :	Statistic(p_stat),
    	m_duration(p_duration),
    	m_next(0),
	m_last(0),
	m_window(p_window),
	m_size(1),
	m_length(p_duration.length())
{
}

void
DurationStatistic::append(const DurationStatistic & src)
{
    // Create a copy of the list first, then glue them together. 
    // This avoids an infinite loop when you append a list to itself.
    //
    DurationStatistic 	    	*   newList = 0,
    	    	    	    	*   newLast = 0;

    u_int32_t   	    	    newSize = 0;
    int64_t     	    	    newLength = 0;

    const DurationStatistic 	*   nextStat = 0,
			    	*   currentStat = 0;

    for ( currentStat = &src; currentStat != 0; currentStat = nextStat )
    {
	if ( key() != currentStat->key() )
	{
    	    assert( key() == currentStat->key() );
	    return;
	}

    	nextStat = currentStat->m_next;
	
    	if ( m_last )
	{
	    assert( newLast != m_last );
	}

    	DurationStatistic * newStat = new DurationStatistic(
    	    	*currentStat, currentStat->m_duration, currentStat->m_window);

    	if ( newList == 0 )
	{
	    newList = newStat;
	    newLast = newStat;
	}
	else
	{
	    newLast->m_next = newStat;
	    newLast = newStat;
	}

	++newSize;
	
	newLength += currentStat->m_duration.length();
    }
    
    if ( m_next == 0 )
    {
    	assert( m_last == 0 );
	
	m_next = newList;
	m_last = newLast;
    }
    else
    {
    	assert( m_last != 0 );
	assert( m_last->m_next == 0 );
	assert( m_last->m_last == 0 );

    	m_last->m_next = newList;
	m_last = newLast;
    }

    m_size += newSize;

    if ( m_length == 0 )
    {
    	m_length = m_duration.length();
    }
    m_length += newLength;

    assert( m_size > 1 );
    assert( m_next != 0 );
    assert( m_last != 0 );
    assert( m_last->m_next == 0 );
    assert( m_last->m_last == 0 );
}


void
DurationStatistic::pop()
{
    if ( m_size <= 1 )
    {
    	assert( m_size > 2 );
	return;
    }

    if ( m_next == 0 )
    {
    	assert( m_next != 0 );
	return;
    }

    m_length -= m_duration.length();

    m_size--;

    m_duration = m_next->m_duration;

    if ( m_next == m_last )
    {
    	m_last = 0;
    }    

    DurationStatistic * nuke = m_next;

    m_next = m_next->m_next;
        
    nuke->m_next = 0;
    delete nuke;

    assert( m_size >= 1 );

    if ( m_next == 0 )
    {
    	assert( m_last == 0 );
    }
    else
    {
    	assert( m_last != 0 );
	assert( m_last->m_next == 0 );
	assert( m_last->m_last == 0 );
    }
}


void	
DurationStatistic::clear()
{
    if ( m_next == 0 )
    {
    	assert( m_last == 0 );
    	return;
    }
    
    DurationStatistic * nextStat = 0;
    
    for (   DurationStatistic * currentStat = m_next;
    	    currentStat != 0;
	    currentStat = nextStat
	)
    {
    	nextStat = currentStat->m_next;

    	currentStat->m_next = 0;
	
    	delete currentStat;
    }
    
    m_next = m_last = 0;

    m_size = 1;
    
    m_length = 0;
}

