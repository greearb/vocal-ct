#ifndef TIMER_H_
#define TIMER_H_

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





static const char* const TimerHeaderVersion =
    "$Id: Timer.h,v 1.3 2004/11/19 01:54:38 greear Exp $";




#include "debug.h"
#include "VTime.hxx"
#include <unistd.h>

#include "global.h"
#include <list>

bool operator < ( const timeval& lhs, const timeval& rhs);
void addMs ( timeval* lhs, int timeMs);
void subMs ( timeval* lhs, int timeMs);

long tv_diff ( const timeval& lhs, const timeval& rhs);


typedef unsigned int TimerEventId ;
template < class X >
class TimerObj
{
    public:
        TimerObj()
        {}


        TimerObj(const TimerObj& x) :
        myTime(x.myTime),
        myObject(x.myObject),
        eventId(x.eventId)
        {}


        TimerObj(const timeval& time, X object) :
        myTime(time),
        myObject(object),
        eventId(0)
        {}

        // assignment operator

        void operator = (const TimerObj& x)
        {
            myTime = x.myTime;
            myObject = x.myObject;
            eventId = x.eventId;
        }

        // equality check operator
        bool operator == (const TimerObj& x)
        {
            if ((myTime.tv_sec == x.myTime.tv_sec) && (myTime.tv_usec == x.myTime.tv_usec) && (myObject == x.myObject) && (eventId == x.eventId))
                return ( true );
            return ( false );
        }

        timeval myTime;
        X myObject;
        TimerEventId eventId;
};

template < class X > bool operator < (const TimerObj < X > & x, const TimerObj < X > & y)
{
    return x.myTime < y.myTime;
}

template < class T >
class Timer
{
    public:
        Timer();
        Timer(long usecWait, T bad);
        Timer(const timeval& minWait, T bad);
        void insert(T obj, int msDelay);
        void remove(const TimerObj < T > & obj);
        TimerObj < T > getlastInserted()
        {
            return lastInserted;
        }
        bool sleepFor(timeval* t);

        bool ready();
        T top();
        void pop();
    private:
        TimerEventId nextEventId;
        list < TimerObj < T > > events;
        //    priority_queue<TimerObj<T> > events;
        timeval myMinWait;
        T bogus;

        static TimerObj < T > lastInserted;
};

template < class T > TimerObj < T > Timer < T > ::lastInserted = TimerObj < T > ();

template < class T >
Timer < T > ::Timer()
        : nextEventId(0)
{
    myMinWait.tv_sec = 0;
    myMinWait.tv_usec = 0;

    TimerObj < T > foo;

    events.push_back(foo);
    pop();
}

template < class T >
Timer < T > ::Timer(long usecWait, T bad)
        : nextEventId(0),
        bogus(bad)
{

    myMinWait.tv_sec = 0;
    myMinWait.tv_usec = usecWait;

    while(myMinWait.tv_usec > 1000000)
    {
        myMinWait.tv_sec++;
        myMinWait.tv_usec -= 1000000;
    }

    TimerObj < T > foo;

    events.push_back(foo);
    pop();
}

template < class T >
Timer < T > ::Timer(const timeval& minWait, T bad)
        : nextEventId(0),
        myMinWait(minWait),
        bogus(bad)
{

    TimerObj < T > foo;

    events.push_back(foo);
    pop();
}



template < class T >
bool Timer < T > ::sleepFor(timeval* t)
{
    timeval now;
    bool retval;

    int err = gettimeofday(&now, NULL);
    assert( !err );

    if (!events.empty())
    {
        if (events.front().myTime < now)
        {
            // indicate almost no time to wait!
            t->tv_sec = 0;
            t->tv_usec = 1;
            //XDEBUG(3, cout << "diff: " << tv_diff(events.front().myTime, now) << endl; ) ;
            retval = false;
        }
        else
        {
            t->tv_sec = events.front().myTime.tv_sec - now.tv_sec;
            t->tv_usec = events.front().myTime.tv_usec - now.tv_usec;
            while (t->tv_usec < 0)
            {
                t->tv_sec--;
                t->tv_usec += 1000000;
            }
            retval = true;
        }
    }
    else
    {
        *t = myMinWait;
        retval = true;
    }

    return retval;
}


template < class T >
void Timer < T > ::insert(T obj, int msDelay)
{
    assert( INT_MAX > 2000000000 );
    assert( INT_MIN < -2000000000 );

    timeval now;
    int err = gettimeofday(&now, NULL);
    assert( !err );

    //XDEBUG(3, cout << "DELAY: " << msDelay << endl);
    addMs(&now, msDelay);

    TimerObj < T > x(now, obj);

    // add the event to the priority queue

    // get a new ID
    TimerEventId id = nextEventId++;
    x.eventId = id;

    // put the message on the FIFO
    events.push_back( x );

    // save the last inserted item
    lastInserted = x;

    // get the list into priority order
    events.sort();

}


template < class T >
void Timer < T > ::remove(const TimerObj < T > & obj )
{
    if(!events.empty())
    {
        events.remove( obj );
    }
}

template < class T >
void Timer < T > ::pop()
{
    if(!events.empty())
    {
        events.pop_front();
    }
}

template < class T >
bool Timer < T > ::ready()
{
    timeval now;
    int err = gettimeofday(&now, NULL);
    assert( !err );

    if (!events.empty())
    {
        if (events.front().myTime < now)
        {
            return true;
        }
    }

    return false;
}

template < class T >
T Timer < T > ::top()
{
    if(!events.empty())
    {
        return events.front().myObject;
    }
    return bogus;
}
#endif
