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
#include "TotalStatistic.hxx"
#include <cassert>


using Vocal::Statistics::TotalStatistic;
using Vocal::Statistics::Statistic;
using Vocal::Statistics::StatisticsDb;


TotalStatistic::TotalStatistic(StatisticsDb & p_db, const Data & p_name)
    :	Statistic(p_db, p_name),
    	m_total(0)
{
}


TotalStatistic::TotalStatistic(const TotalStatistic & src)
    :	Statistic(src),
    	m_total(src.m_total)
{
}


const TotalStatistic & 
TotalStatistic::operator=(const TotalStatistic & src)
{
    if ( this != &src )
    {
    	Statistic::operator=(src);
	
	m_total = src.m_total;
    }
    return ( *this );
}


void
TotalStatistic::combine(const Statistic & src)
{
    if ( key() != src.key() )
    {
    	assert( key() == src.key() );
	return;
    }

    const TotalStatistic *  srcStat 
    	= dynamic_cast<const TotalStatistic *>(&src);

    if ( srcStat == 0 )
    {
    	assert( srcStat != 0 );
	return;
    }
    
    m_total += srcStat->m_total;
    
    return;
}


Statistic *
TotalStatistic::copy() const
{
    return ( new TotalStatistic(*this) );
}


void
TotalStatistic::set(uint32 total)
{
    m_total = total;
    record();
}


const TotalStatistic &    
TotalStatistic::operator++()
{
    m_total = 1;
    record();
    
    return ( *this );
}


const TotalStatistic &    
TotalStatistic::operator++(int)
{
    m_total = 1;
    record();

    return ( *this );
}


const TotalStatistic &    
TotalStatistic::operator+=(uint32 inc)
{
    m_total = inc;
    record();

    return ( *this );
}


uint32   
TotalStatistic::get() const
{
    return ( m_total );
}


ostream & 
TotalStatistic::writeTo(ostream & out) const
{
    return ( out << name().logData() << ": " << m_total );
}

