
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

static const char* const Waiter_cxx_Version = 
    "$Id: Waiter.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include "Waiter.h"
#include <errno.h>
#include "cpLog.h"

Waiter::Waiter ()
{
}

Waiter::~Waiter ()
{
}

/** Wait for next event or timeout */
bool Waiter::Wait (pEventQueue evt, pEventSourcesList list)
{
  unsigned int i;				// iterator
  int rc;				// return code
  int MaxFD = 0;
  fd_set rfds, wfds, efds;
  struct timeval timeout;
  VmTime timeToNext;


  // set time of next event to current time + 10 seconds
  timeToNext = vm_gettimeofday () + 10000;	// TODO: Make timeout configurable

  FD_ZERO (&rfds);
  FD_ZERO (&wfds);
  FD_ZERO (&efds);


  // Find max file handle and closest event time
  for (i = 0; i < list->size (); i++)
  {
    if ((*list)[i]->IsActive ())
    {
      int
	Max = (*list)[i]->FdSet (&rfds, &wfds, &efds);
      if (Max > MaxFD)
	MaxFD = Max;
      (*list)[i]->SetNext (&timeToNext);
    }
  }

  // fill timeval structure for select()
  if (timeToNext - vm_gettimeofday () > 0)
    vm_time2timeval (timeToNext - vm_gettimeofday (), &timeout);
  else
    vm_time2timeval (0, &timeout);

  // wait for an event
  rc = select (MaxFD, &rfds, &wfds, &efds, &timeout);
  if (rc < 0)
  {
    cpLog (LOG_DEBUG, "Waiter:Select error %d", errno);
  }

  // Process event sources
  for (i = 0; i < list->size (); i++)
  {
    if ((*list)[i]->IsActive ())
    {
      (*list)[i]->Process (evt);
    }
  }
  return false;
}
