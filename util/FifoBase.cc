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


static const char* const FifoBase_cc_Version =
    "$Id: FifoBase.cc,v 1.1 2004/05/01 04:15:33 greear Exp $";


template <class Msg>
FifoBase<Msg>::FifoBase()
   : myFifoSize(0),
     myMutex("FifoBase"),
     myTimerSize(0),
     myShutdown(false)
{
}



template <class Msg>
FifoBase<Msg>::~FifoBase()
{
}



template <class Msg>
void
FifoBase<Msg>::add(const Msg & msg)
{
    Lock lock(myMutex); (void)lock;
    if ( myShutdown )
    {
        return;
    }

    myFifo.push_back(new Msg(msg));
    myFifoSize++;
    
    try
    {
        wakeup();
    }
    catch ( VException & )
    {
        assert(0);
    }
}


template <class Msg>
typename FifoBase<Msg>::EventId
FifoBase<Msg>::addDelay(
    const Msg	    &   msg,
    const TimeVal   &   relativeTimeout
)
{
    Lock lock(myMutex); (void)lock;

    if ( myShutdown )
    {
        return ( 0 );
    }

    EventId newId = myTimer.add(new Msg(msg), relativeTimeout);
    myTimerSize++;
    
    postAddTimed(newId);

    return ( newId );
}


template <class Msg>
typename FifoBase<Msg>::EventId
FifoBase<Msg>::addUntil(
    const Msg	    &   msg,
    const TimeVal   &   when
)
{
    Lock lock(myMutex); (void)lock;

    if ( myShutdown )
    {
        return ( 0 );
    }

    Vocal::TimeAndDate::TimerEntryId newId = myTimer.add(new Msg(msg), when, false);
    myTimerSize++;
    
    postAddTimed(newId);

    return ( newId );
}


template <class Msg>
typename FifoBase<Msg>::EventId
FifoBase<Msg>::addDelayMs(
    const Msg	            &   msg,
    const Vocal::TimeAndDate::milliseconds_t        pRelativeTimeout
)
{
    Lock lock(myMutex); (void)lock;

    if ( myShutdown )
    {
        return ( 0 );
    }

    Vocal::TimeAndDate::milliseconds_t relativeTimeout = pRelativeTimeout;

    if ( relativeTimeout < 0 )
    {
        relativeTimeout = 0;
    }

    Vocal::TimeAndDate::TimerEntryId newId = myTimer.add(new Msg(msg), relativeTimeout);
    myTimerSize++;
    
    postAddTimed(newId);

    return ( newId );
}


template <class Msg>
typename FifoBase<Msg> ::EventId
FifoBase<Msg> ::addUntilMs(
    const Msg	            &   msg,
    const Vocal::TimeAndDate::milliseconds_t        when
)
{
    Lock lock(myMutex); (void)lock;

    if ( myShutdown )
    {
        return ( 0 );
    }

    EventId newId = myTimer.add(new Msg(msg), when, false);
    myTimerSize++;

    postAddTimed(newId);

    return ( newId );
}


template <class Msg>
void
FifoBase<Msg>::cancel(EventId id)
{
    if ( id == 0 )
    {
        return ;
    }

    Lock lock(myMutex); (void)lock;

    if ( !myTimer.cancel(id) )
    {
        // If the timer didn't hold the message to cancel, walk through
        // the queue and see if we have it there.
        //
        for (	typename MessageContainer::iterator it = myFifo.begin();
                it != myFifo.end();
                ++it
            )
        {
            // Assertion: The id for a timed event is just the
            // address of the memory for the message. If this isn't
            // true in TimerEntry, then we have problems.
            //
            if ( id == (EventId)((*it).operator->()) )
            {
                myFifo.erase(it);
                myFifoSize--;
                return ;
            }
        }
    }
    else
    {
       myTimerSize--;
    }
}


template <class Msg>
int
FifoBase<Msg>::block(Vocal::TimeAndDate::milliseconds_t relativeTimeout)
throw ( VException )
{
    Lock lock(myMutex); (void)lock;

    if ( messageAvailableNoLock() )
    {
        return ( 1 );
    }

    return ( blockNoLock(relativeTimeout) );
}


template <class Msg>
Msg
FifoBase<Msg> ::getNext()
throw ( VException )
{
    Lock lock(myMutex); (void)lock;

    // Wait while there are no messages available.
    //
    while ( !messageAvailableNoLock() )
    {
        blockNoLock();
    }

    // Move expired timers into fifo.
    //
    while ( myTimer.messageAvailable() )
    {
        try
        {
            myFifo.push_back(myTimer.getMessage());
            myFifoSize++;
            myTimerSize--;
        }
        catch ( ... )
        {
            assert(0);
        }
    }

    // Return the first message on the fifo.
    //
    assert (myFifoSize > 0);
    if(myFifo.empty())
    {
       cerr << "Fifo: " << this << " list is empty, but fifo size is " << myFifoSize << endl;
    }
    assert (!myFifo.empty());
    Msg firstMessage = *(myFifo.front());

    myFifo.pop_front();
    myFifoSize--;
    
    return ( firstMessage );
}


template <class Msg>
unsigned int
FifoBase<Msg> ::size() const
{
    Lock lock(myMutex); (void)lock;

    return ( myFifoSize + myTimerSize );
}


template <class Msg>
unsigned int 
FifoBase<Msg>::sizeAvailable() const
{
    Lock lock(myMutex); (void)lock;
    
    return ( myFifoSize );
}


template <class Msg>
unsigned int 
FifoBase<Msg>::sizePending() const
{
    Lock lock(myMutex); (void)lock;
    
    return ( myTimerSize );
}


template <class Msg>
bool
FifoBase<Msg>::messageAvailable()
{
    Lock lock(myMutex); (void)lock;
    
    return ( messageAvailableNoLock() );
}


template <class Msg>
void
FifoBase<Msg>::shutdown()
{
    Lock lock(myMutex); (void)lock;
    
    myShutdown = true;
}


template<class Msg>
bool
FifoBase<Msg>::operator==(const FifoBase & src) const
{
    // Since each oberver is unique, we can compare addresses.
    //
    return ( this == &src );
}


template <class Msg>
bool
FifoBase<Msg>::operator!=(const FifoBase & src) const
{
    return ( this != &src );
}


template <class Msg>
bool
FifoBase<Msg>::operator<(const FifoBase & src) const
{
    return ( this < &src );
}

template <class Msg>
bool
FifoBase<Msg>::operator<=(const FifoBase & src) const
{
    return ( this <= &src );
}


template <class Msg>
bool
FifoBase<Msg>::operator>(const FifoBase & src) const
{
    return ( this > &src );
}

template <class Msg>
bool
FifoBase<Msg>::operator>=(const FifoBase & src) const
{
    return ( this >= &src );
}


template <class Msg>
bool
FifoBase<Msg>::messageAvailableNoLock()
{
   // can this just call myTimerSize? !jf!
   return ( myFifoSize > 0 || myTimer.messageAvailable() ); 
}

template <class Msg>
int
FifoBase<Msg>::blockNoLock(Vocal::TimeAndDate::milliseconds_t relativeTimeout)
throw ( VException )
{
    // Use the shortest timeout value between the given relativeTimout
    // and the timer container's timeout, remembering that infinite timeout
    // is specified by a negative number.
    //
    Vocal::TimeAndDate::milliseconds_t timerTimeout = myTimer.getTimeout(),
                                  timeout;

    // If timerTimeout is infinite, relativeTimeout can only be the
    // same or shorter.
    //
    if ( timerTimeout < 0 )
    {
        timeout = relativeTimeout;
    }

    // If relativeTimeout is infinite, timerTimeout can only be the
    // same or shorter.
    //
    else if ( relativeTimeout < 0 )
    {
        timeout = timerTimeout;
    }

    // Both are positive timeout values. Take the shortest in duration.
    else
    {
        timeout = relativeTimeout < timerTimeout
                  ? relativeTimeout
                  : timerTimeout;
    }

    // Wait for an event. A timer expiry or signal will return a 0 here.
    //
    int numberActive = wait(timeout);

    if ( numberActive > 0 )
    {
        return ( numberActive );
    }

    // See if a timer has expired.  If it has expired, return 1 to indicate
    // a message is available from the timer.
    //
    if ( messageAvailableNoLock() )
    {
        return ( 1 );
    }

    // To get here, either a signal woke us up, or the we used the
    // relativeTimeout value, meaning that a message isn't available from
    // the timer container.
    //
    return ( 0 );
}


template <class Msg>
void
FifoBase<Msg>::postAddTimed(const EventId & newId)
{ 
    Vocal::TimeAndDate::TimerEntryId firstId = myTimer.getFirstTimerEntryId();

    // If we insert the new message at the front of the timer list,
    // we need to wakeup wait() since we have a timed message that
    // expires sooner than the current, or if we didn't have a timer
    // to begin with.
    //
    if ( firstId == newId )
    {
        try
        {
            wakeup();
        }
        catch ( VException & )
        {
            assert(0);
        }
    }
}
