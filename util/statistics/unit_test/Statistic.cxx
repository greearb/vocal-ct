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

#include "DurationStatistic.hxx"
#include "TotalStatistic.hxx"
#include "Data.hxx"
#include "VLog.hxx"
#include <unistd.h>

using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::TotalStatistic;
using Vocal::Statistics::Statistic;
using Vocal::Statistics::StatisticsDb;
using Vocal::Logging::VLog;

int main(int argc, char** argv)
{
    VLog::init(LOG_DEBUG);
    VLog::on(LOG_DEBUG);
    
    VLog    log;
    
    StatisticsDb   	    statDb;
    
    TotalStatistic  	    total(statDb, "Total"),
    	    	    	    sleep0_ticks(statDb, "sleep(0) ticks total"),
			    sleep0_count(statDb, "sleep(0) calls total");

    DurationStatistic	    sleep0_duration_a(statDb, "sleep(0) duration", 30),
    	    	    	    sleep0_duration_b(statDb, "sleep(0) duration");
    
    total.set(4);
    total++;
    ++total;
    total += 4;

    for ( int i = 0; i < 7; i++ )
    {
	sleep0_duration_a.start();
	sleep(0);
	sleep0_duration_a.stop();
	
	VDEBUG(log) << sleep0_duration_a << VDEBUG_END(log);

	sleep0_duration_b.start();
	sleep(0);
	sleep0_duration_b.stop();

    	sleep0_duration_a.combine(sleep0_duration_b);
	
	sleep0_ticks += sleep0_duration_a.length();
	sleep0_count++;

	sleep0_duration_a.record();
    }

    VDEBUG(log) << statDb << VDEBUG_END(log);

    Statistic * sleep0_find = statDb.find(sleep0_duration_a);
    
    VDEBUG(log) << '\n' << *sleep0_find << VDEBUG_END(log);
    
    sleep0_find->record();

    Statistic * new_sleep0 = sleep0_find->copy();
    
    new_sleep0->record();
    
    delete new_sleep0;

    DurationStatistic	fooStat(statDb, "sleep(0) duration");
    
    fooStat = sleep0_duration_a;
    
    VDEBUG(log) << "fooStat: " << fooStat << VDEBUG_END(log);
    
    fooStat.record();

    VDEBUG(log) << statDb << VDEBUG_END(log);

    VLog::uninit();
        
    return ( 0 );
}
