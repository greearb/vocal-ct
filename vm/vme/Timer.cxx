
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

static const char* const Timer_cpp_Version = 
"$Id: Timer.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Timer.h"


/** Default Constructor */
Timer::Timer ()
{
  m_xType = "Timer";
}

Timer::~Timer ()
{
}

/** activate timer */
void
Timer::start ()
{
    // Save current time
    m_xStart = vm_gettimeofday ();

    // Calculate ending time
    m_xEnd = m_xStart + m_xTimeout;

    // Activate timer
    m_bActive = true;
}

/** deactivate timer */
void
Timer::stop ()
{
    m_bActive = false;
}

/** set timer */
void
Timer::set (VmTime timeout, int Id)
{
    // Save timeout an ID
    m_xTimeout = timeout;
    m_iId = Id;
}

/** Process */
bool Timer::Process (pEventQueue pevt)
{
    // Active?
    if (!m_bActive)
        return false;
  
     Event evt; 

    // Timer expired?
    if (vm_gettimeofday () < m_xEnd)
         return false;

    // Yes, generate timer event
    evt.setSource (this);
    evt.setSourceType (sourceTimer);
    evt.setType (eventTimer);
 
    // Save ID in the first parameter
    evt.IParm ()->push_back (m_iId);
 
    // add an event to the queue
    pevt->push (evt);
   
    return true;
}

/** set time to next event */
bool Timer::SetNext (VmTime * timeToNext)
{
    // return an ending time if closer then "timeToNext"
    if (m_xEnd < *timeToNext)
       *timeToNext = m_xEnd;
     return true;
}
