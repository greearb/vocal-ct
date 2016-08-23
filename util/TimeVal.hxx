#if !defined(VOCAL_TIME_VAL_HXX)
#define VOCAL_TIME_VAL_HXX

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

#include "Writer.hxx"
#include <sys/time.h>
#include <unistd.h>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to manipulate the time.<br><br><br>
 */
namespace TimeAndDate
{


#if !defined(VOCAL_MILLISECONDS_T)
#define VOCAL_MILLISECONDS_T

/** 1/1000th of a second.<br><br>
 */
typedef int milliseconds_t;

#endif // !defined(VOCAL_MILLISECONDS_T)


/** A wrapper for the operating system's timeval structure.<br><br>
 */
class TimeVal : public timeval, public Vocal::IO::Writer
{
    public:

    	/** Default constructor.
	 */
    	TimeVal();


    	/** Millisecond constructor.
	 */
        TimeVal(milliseconds_t);


        /** Seconds, microseconds constructor.
         */
        TimeVal(long sec, long usec);
        
        
    	/** Copy constructor.
	 */
        TimeVal(const timeval &);

    	/** Assignment operator.
	 */
        TimeVal &	operator=(const timeval &);


    	/** Set this timeval with the current time. Wrapper for gettimeofday.
	 */
        const TimeVal & now();


    	/** Return the number of milliseconds.
	 */
        milliseconds_t	milliseconds() const;


    	/** Addition operator.
	 */
        TimeVal operator+(const timeval &) const;


    	/** Addition operator, millisecond version.
	 */
        TimeVal operator+(milliseconds_t) const;


    	/** Addition and assignment operator.
	 */
        TimeVal & operator+=(const timeval &);


    	/** Addition and assignment operator, millisecond version.
	 */
        TimeVal & operator+=(milliseconds_t);


    	/** Subtraction operator.
	 */
        TimeVal operator-(const timeval &) const;


    	/** Subtraction operator, millisecond version.
	 */
        TimeVal operator-(milliseconds_t) const;


    	/** Subtraction and assignment operator.
	 */
        TimeVal & operator-=(const timeval &);


    	/** Subtraction and assignment operator, millisecond version.
	 */
        TimeVal & operator-=(milliseconds_t);


    	/** Equality operator.
	 */
        bool	operator==(const timeval &) const;


    	/** Less than relational operator.
	 */
        bool	operator< (const timeval &) const;


    	/** Less than relational operator.
	 */
        bool	operator< (const TimeVal &) const;

    	/** Clear the timeval struct to 0.
	 */
        void	clear();


    	/** Copy the given timeval struct into this.
	 */
        void	copy(const timeval &);


    	/**  The purpose of normalize is to ensure that the tv_usec field
    	 *  is in [0,100000).
	 */
        void	normalize();


    	/** Write a TimeVal to an ostream.
	 */
        ostream & writeTo(ostream &) const;
};


} // namespace TimeAndDate
} // namespace Vocal


#endif // !defined(VOCAL_TIME_VAL_HXX)
