#ifndef EVENTSOURCE_H
#define EVENTSOURCE_H

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



static const char* const EventSource_h_Version = 
"$Id: EventSource.h,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include "Event.h"
#include "VmTime.h"
#include "cpLog.h"

/**
  * Base EventSource class.
  * Objects of this type are used in a Waiter.
  */

class EventSource
{
public:

  EventSource ();

  /** Destructor */
  virtual ~ EventSource ();

  /** Process an event if any. Add it to EventQueue. */
  virtual bool Process (pEventQueue evt);

  /** Add all file descriptors to corresponding fdsets.
      Returns max file handle+1 fro salect(); 
   */
  virtual int FdSet (fd_set * rfds, fd_set * wfds, fd_set * efds);

  /** Is EventSource Active ? */
  virtual bool IsActive ();

  /** Set time to next event if closer the timeToNext */
  virtual bool SetNext (VmTime * timeToNext);

  /** deactivate event source */
  virtual void stop ();

  /** activate event source */
  virtual void start ();

protected:
  /** activity flag */
    bool m_bActive;

/** Public attributes */
public:		
    string m_xType;
};


// Override vector<>.push_back to avoid duplicated sources
class EventSourcesList:public vector < EventSource * >
{
public:
  void push_back (EventSource * Source)
  {
    for (unsigned int i = 0; i < this->size (); i++)
    {
      if (Source == (*this)[i])
      {
	cpLog (LOG_DEBUG, "Event source %p already in the list", Source);
	return;
      }
    }
    vector < EventSource * >::push_back (Source);
  }
};

typedef EventSourcesList *pEventSourcesList;


#endif
