#if !defined(TICKCOUNT_DOT_HXX)
#define TICKCOUNT_DOT_HXX

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


static const char* const TickCount_hxx_Version = 
    "$Id: TickCount.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "vtypes.h"
#include "VThread.hxx"
#include <sys/times.h>
#include <unistd.h>


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** Represents the number of clock ticks that have elapsed since the
 *  system has been up.<br> <br>
 *
 *  Should act similar to times(2), with a higher
 *  resolution.
 */
class TickCount
{
    public:

	/** Low order portion of tick count.
	 */
	u_int32_t	 	low;


	/** High order portion of tick count.
	 */
	u_int32_t	 	high;


	/** Once calibrated, the 
	 */
	static  int64_t	    	ticksPerSec;


	/** Initialize the ticks to 0.
	 */
	TickCount();


	/** The tick count will be set to the current number of ticks
	 *	that have elasped since the system has started.
	 */
	void    	    	set();


	/** Return the number of ticks as a 64 bit number.
	 */
	int64_t 	    	get() const;


	/** Return the difference between two TickCounts as a 64 bit int.
	 */
	int64_t 	    	operator-(const TickCount & src) const;


	/** Find the number of ticksPerSecond.
	 */
	static  void    	calibrate();
};


inline TickCount::TickCount()
    :   low(0), high(0)
{
}


inline void TickCount::set()
{

#if defined(__GNUG__)
    #if defined(VOCAL_USING_PENTIUM)
    	#define USE_ASM_TICKS 1
    #endif
#endif

#if ( USE_ASM_TICKS == 1 )

    // This code only works on intel 586 / 686 processors 
    //  (pentium, pentium pro, pentium ii, pentium iii) 
    //
    // This assembly code gets the value of the Intel CPU tick register
    // and deposits it in the TickCount tc.
    //

    #define rdtsc(tc) \
	 __asm__ __volatile__("rdtsc" : "=a" ((tc).low), "=d" ((tc).high))      


    rdtsc(*this);

#elif defined( WIN32 )

    low = GetTickCount();

#elif defined(__APPLE__)
    struct tms	vTp;
    low = times(&vTp);
#else

    low = times(0);

#endif

}
    

inline int64_t TickCount::get() const
{
    return ( (static_cast<int64_t>(high) << 32) + low );
}
    

inline int64_t
TickCount::operator-(const TickCount & src) const
{
    return  ( get() - src.get() );
}


inline void    	TickCount::calibrate()
{
    if ( ticksPerSec == 0 )
    {
    	TickCount start, stop;
	
	start.set();
	
	// There's got to be a better way.
	//
	sleep(1);

	stop.set();

	ticksPerSec = stop - start;
    }
}


} // namespace Statistics
} // namespace Vocal 


#endif // !defined(TICKCOUNT_DOT_HXX)
