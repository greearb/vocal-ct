#ifndef TIMER_ENTRY_DOT_H
#define TIMER_ENTRY_DOT_H

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


static const char* const TimerEntryHeaderVersion =
    "$Id: TimerEntry.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "Writer.hxx"
#include "Sptr.hxx"
#include "TimeVal.hxx"
#include <cassert>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to manipulate the time.
 */
namespace TimeAndDate
{


#if !defined(VOCAL_MILLISECONDS_T)
#define VOCAL_MILLISECONDS_T

/* 1/1000th of a seconds.<br><br>
 */
typedef int milliseconds_t;

#endif // !defined(VOCAL_MILLISECONDS_T)


/** The key for a timer entry.<br><br>
 */
typedef void * TimerEntryId;


/** An entry in a TimerContainer.<br><br>
 *
 *  A TimerEntry holds some type of event and the time the event is 
 *  supposed to take place. The TimerContainer is an ordered list 
 *  (by time) of TimerEntries.
 *
 *  @see    Vocal::TimeAndDate::TimerContainer
 *  @see    Vocal::TimeAndDate::TimeVal
 */
template < class Msg >
class TimerEntry : public Vocal::IO::Writer
{
    public:

	/** Construct with an event and the time of expiration (in ms).
	 */
        TimerEntry(Sptr < Msg > , milliseconds_t time, bool relative = true);


	/** Construct with an event and the time of expiration.
	 */
        TimerEntry(Sptr < Msg > , const TimeVal & time, bool relative = true);


	/** Copy constructor.
	 */
        TimerEntry(const TimerEntry &);

        TimerEntry();

	/** Virtual destructor.
	 */
        virtual ~TimerEntry();


	/** Assignment operator.
	 */
        TimerEntry &	operator=(const TimerEntry &);


	/** Return the time of expiration for the event.
	 */
        milliseconds_t	getTimeout() const;


	/** Return the opaque id for this timer entry.
	 */
        TimerEntryId	getId() const;


	/** Return the associated event.
	 */
        Sptr < Msg > getMessage() const;


	/** Returns true if the expiration time for the associcated 
	 *  event has passed.
	 */
        bool	hasExpired() const;


	/** Equality relational operator.
	 */
        bool	operator==(const TimerEntry &) const;


	/** Inequality relational operator.
	 */
        bool	operator!=(const TimerEntry &) const;


	/** Less than relational operator.
	 */
        bool	operator< (const TimerEntry &) const;


	/** Less than or equal to relational operator.
	 */
        bool	operator<=(const TimerEntry &) const;


	/** Greater than relational operator.
	 */
        bool	operator> (const TimerEntry &) const;


	/** Greater than or equal to relational operator.
	 */
        bool	operator>=(const TimerEntry &) const;


	/** Write the TimerEntry to an ostream.
	 */
        virtual ostream & writeTo(ostream &) const;


    private:

        TimeVal     	absExpires_;
        Sptr < Msg > 	msg_;
};


#include "TimerEntry.cc"


} // namespace Time
} // namespace Vocal




#endif // !defined(TIMER_ENTRY_DOT_H)
