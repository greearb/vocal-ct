#ifndef TIMER_CONTAINER_DOT_H
#define TIMER_CONTAINER_DOT_H

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


static const char* const TimerContainerHeaderVersion =
    "$Id: TimerContainer.hxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#ifdef _MSC_VER
#pragma warning(disable:4290)
#endif

#include <set>
#include <map>
#include <algorithm>

#include "TimerEntry.hxx"
#include "Writer.hxx"
#include "Sptr.hxx"
#include "VMissingDataException.hxx"
#include "TimeVal.hxx"

using std::map;

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to manipulate the time.<br><br>
 */
namespace TimeAndDate
{

class TimeVal;


/** An ordered list of TimerEntries. This list is a list of events and the
 *  time those events expire.<br><br>
 *
 *  @see    Vocal::TimeAndDate::TimerEntry
 */
template < class Msg >
class TimerContainer : public Vocal::IO::Writer
{
    public:


    	/** Default constructor
	 */
        TimerContainer();


    	/** Virtual destructor
	 */
        virtual	~TimerContainer();


        /** Add a new message to the timer.
         */
        TimerEntryId	add(Sptr < Msg > ,
                         milliseconds_t when,
                         bool           relative = true);


        /** Add a new message to the timer.
         */
        TimerEntryId	add(Sptr < Msg > ,
                         const TimeVal &    when,
                         bool               relative = true);


        /** Cancel a delayed event. Returns true is id was found, and
         *  false otherwise.
         */
        bool cancel(TimerEntryId);


        /** Get the timeout value for the first available
         *  message. Returns -1 if no messages available
         *  (conveying infinite timeout).
         */
        int getTimeout();


        /** Returns the identifier of the first entry.
         */
        TimerEntryId	getFirstTimerEntryId();


        /** Returns true if a message is available.
         */
        bool	messageAvailable();


        /** Returns the first message available. Throws a
         * status exception if no messages are available.
         */
        Sptr < Msg > getMessage()
        throw ( VMissingDataException );


    	/** Write a TimerContainer to an ostream.
	 */
        ostream & writeTo(ostream & out) const;


        /** Return the number of all the pending events in the TimerContainer
	 */
        unsigned int size() const;

    private:

      void postAdd(const TimerEntry<Msg> &);

		// 24/11/03 fpi
		// WorkAround Win32
		// add "::" in front of "std"
      typedef ::std::multiset< TimerEntry<Msg> > TimerSet;
      TimerSet _timerSet;

      ::std::map<TimerEntryId, TimerEntry<Msg> > _idToTimer;
};


#include "TimerContainer.cc"


} // namespace Time
} // namespace Vocal


#endif // !defined(TIMER_CONTAINER_DOT_H)
