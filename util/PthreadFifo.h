#ifndef VOCAL_PTHREAD_FIFO_H
#define VOCAL_PTHREAD_FIFO_H

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

// Written by Ben Greear, as the other Fifo seems to have deadlocks due to
// misuse of conditionals.

static const char* const PthreadFifo_h_Version =
"$Id: PthreadFifo.h,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "TimerContainer.hxx"
#include "Sptr.hxx"
#include "TimeVal.hxx"
#include <list>
#include <pthread.h>
#include "NonCopyable.hxx"
#include <cpLog.h>

using namespace Vocal;
using namespace Vocal::TimeAndDate;

using std::list;

namespace Vocal {
/** 
    First in first out queue interface, with the added functionality of 
    being able to handle timed entries.
    <P>
    <B>Example 1:</b>
    <p>
    In a shared context:
    <pre>
      Fifo&lt;int> f;
    </pre>
    <p>
    In thread 1:
    <p>
    <pre>
      f.add(1); // add the number 1 to the fifo f
    </pre>
    <p>
    In thread 2:
    <p>
    <pre>
      int x = f.getNext();
      // now, x == 1
    </pre>

    <b>Example 2:</b>
    <p>
    In a shared context:
    <pre>
      Fifo&lt;int> f;
    </pre>

    <p>
    In thread 1:
    <p>
    <pre>
      // add the number 1 to the fifo f after 1000 ms delay
      f.addDelayMs(1, 1000); 
    </pre>
    <p>
    In thread 2:
    <p>
    <pre>
      int x = f.getNext();
      // now, x == 1
      
      // note that this will block for 1000ms for the number to be put
      // in the fifo.
    </pre>

 */

#define LOCK_CLASS cpLog(LOG_DEBUG, "lock class, tid: %d\n", pthread_self()); pthread_mutex_lock(&class_lock); cpLog(LOG_DEBUG, "locked class, tid: %d\n", pthread_self())
#define UNLOCK_CLASS cpLog(LOG_DEBUG, "unlock class, tid: %d\n", pthread_self()); pthread_mutex_unlock(&class_lock); cpLog(LOG_DEBUG, "unlocked class, tid: %d\n", pthread_self())

#define LOCK_COND cpLog(LOG_DEBUG, "lock cond, tid: %d\n", pthread_self()); pthread_mutex_unlock(&cond_lock); cpLog(LOG_DEBUG, "locked cond, tid: %d\n", pthread_self())
#define UNLOCK_COND cpLog(LOG_DEBUG, "unlock class, tid: %d\n", pthread_self()); pthread_mutex_unlock(&cond_lock); cpLog(LOG_DEBUG, "unlocked cond, tid: %d\n", pthread_self())



template < class Msg >
class FifoBase : public NonCopyable {
public:


   /** Id for delayed events. Needed to cancel an event.
    */
   typedef Vocal::TimeAndDate::TimerEntryId EventId;


   /** Create an empty fifo.
    */
   FifoBase() : myFifoSize(0), myTimerSize(0), myShutdown(0) {
      pthread_mutex_init(&cond_lock, NULL);
      pthread_mutex_init(&class_lock, NULL);

      pthread_cond_init(&cond, NULL);
   }

   /** Delete the fifo.
    */
   virtual ~FifoBase() {
      pthread_mutex_destroy(&cond_lock);
      pthread_mutex_destroy(&class_lock);

      pthread_cond_destroy(&cond);
   }


   // This does not mean much, as another thread can race in and grab the event
   // after this method returns, but before we are able to getMsg().  Should probably
   // migrate to a getNext() with timeout...
   int block(milliseconds_t relativeTimeout) {
      int rv;
      if ( messageAvailable() ) {
         return 1;
      }

      LOCK_CLASS;
      // Use the shortest timeout value between the given relativeTimout
      // and the timer container's timeout, remembering that infinite timeout
      // is specified by a negative number.
      //
      milliseconds_t timerTimeout = myTimer.getTimeout();
      milliseconds_t timeout;

      // If timerTimeout is infinite, relativeTimeout can only be the
      // same or shorter.
      //
      if ( timerTimeout < 0 ) {
         timeout = relativeTimeout;
      }
      
      // If relativeTimeout is infinite, timerTimeout can only be the
      // same or shorter.
      //
      else if ( relativeTimeout < 0 ) {
         timeout = timerTimeout;
      }

      // Both are positive timeout values. Take the shortest in duration.
      else {
         timeout = relativeTimeout < timerTimeout
            ? relativeTimeout
            : timerTimeout;
      }

      struct timeval now;
      gettimeofday(&now, NULL);
      struct timespec abs_timeout;
      memset(&abs_timeout, 0, sizeof(abs_timeout));

      TimeVal tv_timeout(now.tv_sec, now.tv_sec + timeout);

      abs_timeout.tv_nsec = tv_timeout.tv_usec * 1000;
      abs_timeout.tv_sec = tv_timeout.tv_sec;

      UNLOCK_CLASS;

      // Wait for an event. A timer expiry or signal will return a 0 here.
      //
      LOCK_COND;
      while (!messageAvailable() && (rv != ETIMEDOUT)) {
         rv = pthread_cond_timedwait(&cond, &cond_lock, &abs_timeout);
      }
      UNLOCK_COND;

      // See if a timer has expired.  If it has expired, return 1 to indicate
      // a message is available from the timer.
      //
      if (messageAvailable() ) {
         return 1;
      }

      // To get here, either a signal woke us up, or the we used the
      // relativeTimeout value, meaning that a message isn't available from
      // the timer container.
      //
      return 0;
   }


   /** Add a message to the fifo.
    */
   void add(const Msg &msg) {

      LOCK_COND;
      
      LOCK_CLASS;
      myFifo.push_back(new Msg(msg));
      myFifoSize++;

      UNLOCK_CLASS;

      // Signal anyone waiting...
      if (myFifoSize) {
         pthread_cond_broadcast(&cond);
      }
      UNLOCK_COND;
   }

   /** If the relative timeout is greater than 0, the message will 
    *  be added to the fifo after the number of milliseconds have passed.
    *          
    *  The returned value is an opaque id that can be used
    *  to cancel the event before the timer expires. If the
    *  time is specified as 0, this defaults the to the
    *  regular add(), returning an id of 0.
    */
   EventId addDelay(const Msg &msg, const TimeVal & relativeTimeout) {
      return addDelayMs(msg, relativeTimeout.milliseconds());
   }


   /** If the expiry time is later than the time now, the message will 
    *  be added to the fifo after the time has passed.
    *          
    *  The returned value is an opaque id that can be used
    *  to cancel the event before the timer expires. If the
    *  time is specified as 0, this defaults the to the
    *  regular add(), returning an id of 0.
    */
   EventId addUntil(const Msg &msg, const TimeVal & when) {
      return addUntilMs(msg, when.miliseconds());
   }

   /** If the relative timeout (in milliseconds) is greater than 0, 
    *  the message will be added to the fifo after the number of 
    *  milliseconds have passed.
    *          
    *  The returned value is an opaque id that can be used
    *  to cancel the event before the timer expires. If the
    *  time is specified as 0, this defaults the to the
    *  regular add(), returning an id of 0.
    */
   EventId addDelayMs(const Msg &msg, const milliseconds_t pRelativeTimeout) {

      milliseconds_t relativeTimeout = pRelativeTimeout;

      if ( relativeTimeout < 0 ) {
         relativeTimeout = 0;
      }

      LOCK_COND;

      LOCK_CLASS;

      TimerEntryId newId = myTimer.add(new Msg(msg), relativeTimeout);
      myTimerSize++;

      bool do_notify = (myTimer.getFirstTimerEntryId() == newId);
      UNLOCK_CLASS;

      if (do_notify) {
         pthread_cond_broadcast(&cond);
      }

      UNLOCK_COND;

      return ( newId );
   }

   /** If the expiry time (in milliseconds) is later than the
    *  time now, the message will be added to the fifo after the
    *  time has passed.
    *          
    *  The returned value is an opaque id that can be used
    *  to cancel the event before the timer expires. If the
    *  time is specified as 0, this defaults the to the
    *  regular add(), returning an id of 0.
    */
   EventId addUntilMs(const Msg &, const milliseconds_t when) {
      LOCK_COND;

      LOCK_CLASS;
      EventId newId = myTimer.add(new Msg(msg), when, false);
      myTimerSize++;

      bool do_notify = (myTimer.getFirstTimerEntryId() == newId);
      UNLOCK_CLASS;

      if (do_notify) {
         pthread_cond_broadcast(&cond);
      }

      UNLOCK_COND;

      return newId;
   }


   /** Cancel a delayed event.
    */
   void cancel(const EventId& id) {
      if ( id == 0 ) {
         return ;
      }

      LOCK_CLASS;
      if ( !myTimer.cancel(id) ) {
         // If the timer didn't hold the message to cancel, walk through
         // the queue and see if we have it there.
         //
         for (typename MessageContainer::iterator it = myFifo.begin(); it != myFifo.end(); ++it) {
            // Assertion: The id for a timed event is just the
            // address of the memory for the message. If this isn't
            // true in TimerEntry, then we have problems.
            //
            if ( id == (EventId)((*it).operator->()) ) {
               myFifo.erase(it);
               myFifoSize--;
               goto out;
            }
         }
      }
      else {
         myTimerSize--;
      }

     out:
      UNLOCK_CLASS;
   }//cancel


   /** Returns the first message available. It will wait if no
    *  messages are available. If a signal interrupts the wait,
    *  it will retry the wait. Signals can therefore not be caught
    *  via getNext.
    */
   Msg getNext() {
      LOCK_COND;
      while (!messageAvailable()) {
         pthread_cond_wait(&cond, &cond_lock);
      }

      // When here, something is available, and we hold the conditional lock
      // so no one else can sneak in and steal our message.

      // Grab the class lock
      LOCK_CLASS;

      // Move expired timers into fifo.
      //
      while ( myTimer.messageAvailable() ) {
        try {
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
      if(myFifo.empty()) {
         cerr << "Fifo: " << this << " list is empty, but fifo size is " << myFifoSize << endl;
      }
      assert (!myFifo.empty());
      Msg firstMessage = *(myFifo.front());

      myFifo.pop_front();
      myFifoSize--;

      // Unlock class data
      UNLOCK_CLASS;

      // Unlock conditional
      UNLOCK_COND;
    
      return firstMessage;
   }


   /** Get the current size of the fifo. Note that the current
    *  size includes all of the pending events, even those which
    *  have not yet activated so you should not use this function
    *  to determine whether a call to getNext() will block or not.
    *  Use messageAvailable() instead.
    */
   unsigned int size() const {
      return myFifoSize + myTimerSize;
   }


   /** Get the current number of messages available. Note that 
    *  the size does not include all of the pending events.
    *  You may not want to this function to determine the number of times
    *  you could call getNext() before it would block, since messages
    *  may become available during message processing. Consider using 
    *  messageAvailable() instead.
    */
   unsigned int sizeAvailable() const {
      return myFifoSize;
   }

   /** Get the current number of messages pending. Note that the current
    *  size does not include the available messages. You should not use 
    *  this function to determine whether a call to getNext() will block 
    *  or not. Use messageAvailable() instead.
    */
   unsigned int sizePending() const {
      return myTimerSize;
   }


   /** Returns true if a message is available.
    */
   bool	messageAvailable() {
      bool rv;
      LOCK_CLASS;
      rv = messageAvailableNoLock();
      UNLOCK_CLASS;
      return rv;
   }


   /** Indicate that the fifo no longer accepts messages.
    */
   void	shutdown() { myShutdown = true; }

   /** Returns true if a message is available. The default implementation
    *  looks at the size of the fifo.
    */
   virtual bool	messageAvailableNoLock() {
      return ( myFifoSize > 0 || myTimer.messageAvailable() ); 
   }

protected:

   typedef list < Sptr < Msg > > MessageContainer;

   /**
    */
   MessageContainer    myFifo;
   unsigned long myFifoSize;

   // Protects conditional.
   pthread_cond_t cond;
   pthread_mutex_t cond_lock;

   // Protects class data (ie, the fifo)
   pthread_mutex_t class_lock;

   Vocal::TimeAndDate::TimerContainer < Msg > myTimer;
   unsigned long myTimerSize;

   bool	myShutdown;
};


/** Opaque id so that time delayed fifo events may be cancelled.
 *  For backwards compatibility. Use FifoBase::EventId instead.
 */
   typedef     Vocal::TimeAndDate::TimerEntryId 	FifoEventId;


template <class Msg> class Fifo : public FifoBase<Msg> {
public:

   Fifo() : FifoBase<Msg>() { }
   virtual ~Fifo() { }
};

} // namespace Vocal

#endif
