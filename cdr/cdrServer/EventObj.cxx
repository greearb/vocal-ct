
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


static const char* const EventObj_cxx_Version =
    "$Id: EventObj.cxx,v 1.1 2004/05/01 04:14:55 greear Exp $";


#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "EventObj.hxx"


bool
EventObj::isDataReady( const unsigned long int usec )
{
    if (m_fileDesc == 0)
    {
        return false;
    }

    fd_set rfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = usec;

    FD_ZERO(&rfds);
    FD_SET(m_fileDesc, &rfds);

    int retval = select(m_fileDesc + 1, &rfds, NULL, NULL, &tv);

    if (retval > 0 && FD_ISSET(m_fileDesc, &rfds))
    {
        if (!m_reoccuring)
        {
            m_done = true;
        }

        return true;
    }
    return false;
}

bool
EventObj::isTimeReady( const unsigned long int sec )
{
    if (m_seconds == 0)
    {
        return false;
    }

    if ( (sec - m_lastTime) >= m_seconds )
    {
        m_lastTime = sec;

        if (!m_reoccuring)
        {
            m_done = true;
        }

        return true;
    }
    return false;
}

bool
EventObj::eventDone()
{
    return m_done;
}
