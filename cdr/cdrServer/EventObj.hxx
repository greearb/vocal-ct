#ifndef  EventObj_hxx
#define EventObj_hxx

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


static const char* const EventObj_hxx_Version =
    "$Id: EventObj.hxx,v 1.2 2004/06/09 07:19:34 greear Exp $";

#include <BugCatcher.hxx>

/**
 **   EventObj, base class which manages time and data events
 */

class EventObj : public BugCatcher {
public:

   /**
    * Constructor
    * @param int fileDesc of the data event
    * @param bool reoccuring false if a one off
    */
   EventObj( int fileDesc, bool reoccuring = true )
         : m_fileDesc( fileDesc ),
           m_seconds( 0 ),
           m_lastTime( 0 ),
           m_reoccuring( reoccuring ),
           m_done( false ) {}
   
   /**
    * Constructor
    * @param int seconds time interval for events
    * @param bool reoccuring false if a one off
    */
   EventObj( unsigned long int seconds, bool reoccuring = true )
         : m_fileDesc( -1 ),
           m_seconds( seconds ),
           m_lastTime( 0 ),
           m_reoccuring( reoccuring ),
           m_done( false ) {}
   
   ///
   virtual ~EventObj() {}

   ///
   virtual void onData() {}

   ///
   virtual void onTimer() {}


   int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
              int& maxdesc, uint64& timeout, uint64 now);
   
   void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
             uint64 now);

   ///
   bool eventDone() { return m_done; }

protected:

   ///
   int m_fileDesc;
   ///
   uint64 timerMs;
   ///
   uint64 lastTime;
   ///
   bool m_reoccuring;
   ///
   bool m_done;

private:
   // Don't allow these, copying file descriptors is a very bad idea
   EventObj( const EventObj &rhs );
   EventObj& operator=( const EventObj &rhs );
};
#endif
