
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

static const char* const Event_cpp_Version = 
"$Id: Event.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Event.h"
#include "cpLog.h"
#include "EventSource.h"

Event::Event ()
{
  // Clear an event
  m_xType = 0;
  m_xSource = NULL;
  m_xIParm.clear ();
  m_xSParm.clear ();
}

Event::~Event ()
{
  // Clear an event
  m_xType = 0;
  m_xSource = NULL;
  m_xIParm.clear ();
  m_xSParm.clear ();
}

/** Get an access to an Integer parameters list */
EventIParameter *
Event::IParm ()
{
  return &m_xIParm;
}

/** Get a pointer to a String parameters list */
EventSParameter *
Event::SParm ()
{
  return &m_xSParm;
}

/** set event type */
void
Event::setType (EventType type)
{
  m_xType = type;
}

/** get event type */
EventType Event::getType ()
{
  return m_xType;
}

/** set event source */
void
Event::setSource (EventSource * source)
{
  m_xSource = source;
}

/** get event source */
EventSource *
Event::getSource ()
{
  return m_xSource;
}

/** set source type */
void
Event::setSourceType (int sourceType)
{
  m_iSourceType = sourceType;
}

/** get source type */
int
Event::getSourceType ()
{
  return m_iSourceType;
}

/** print current event to log */
void
Event::print ()
{
  cpLog (LOG_DEBUG, "-- Event -- ");
  cpLog (LOG_DEBUG, "\tSourceType	:%d", m_iSourceType);
  if (m_xSource != NULL)
  {
    cpLog (LOG_DEBUG, "\tSource		:%s", m_xSource->m_xType.c_str ());
  }
  else
  {
    cpLog (LOG_ERR, "Ivalid event. Source not specified. (%d)",
	   m_iSourceType);
  }
  cpLog (LOG_DEBUG, "\tType		:%d", m_xType);
  cpLog (LOG_DEBUG, "");
  for (unsigned int i = 0; i < m_xIParm.size (); i++)
  {
    cpLog (LOG_DEBUG, "\tIParm[%d]	:%d", i, m_xIParm[i]);
  }
  for (unsigned int i = 0; i < m_xSParm.size (); i++)
  {
    cpLog (LOG_DEBUG, "\tSParm[%d]	:%s", i, m_xSParm[i].c_str ());
  }
  cpLog (LOG_DEBUG, "");
}
