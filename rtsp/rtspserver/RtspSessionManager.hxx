#ifndef RtspSessionManager_hxx
#define RtspSessionManager_hxx
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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspSessionManager_hxx_version =
    "$Id: RtspSessionManager.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "RtspSession.hxx"

#include "Sptr.hxx"
#include "Random.hxx"
#include <map>


/** Mananges all the rtsp sessions for rtsp server.
    <ul>
    <li>add and delete sessions into manager
    <li>ensures unique sessionIds
    <li>enforces maximum number of sessions a server can handle
    </ul>
 */
class RtspSessionManager
{
    public:
        /** create a singelton object */
        static RtspSessionManager& instance();
        /** destructor */
        ~RtspSessionManager();
        /**  frees the memory associated with singelton instance */
        static void destroy();

        /** sets the max number of sessions */
        bool maxSessions( const int max );

        /** returns the number of current sessions  */
        int getSessions() const
            { return myRtspSessionMap.size(); }

        /** adds a new session into rtsp session map
            @return "" if failed to add session (ie server overloaded), else return its new sessionID
         */
        Data addRtspSession( Sptr<RtspSession> session );

        /** delete an existing session from rtsp session map
            @return false if session not found
         */
        bool delRtspSession( const Data& sessionId );

        /** return first session from rtsp session map
            @return 0 if none remaining
         */
        Sptr<RtspSession> getRtspSession();

        /** finds an existing session from rtsp session map
            @return 0 if not found
         */
        Sptr<RtspSession> getRtspSession( const Data& sessionId );

    private:
        /** constructor */
        RtspSessionManager();

        /** singelton object */
        static RtspSessionManager* myInstance;

        /** return a free session ID */
        Data findNewSessionId();

        /** max number of sessions */
        int myMaxSessions;

        /** RtspSession map type.  Map indexed on sessionID and RTSP Session
         */
        typedef map< Data, Sptr<RtspSession> > RtspSessionMap;
        /** map of active RTSP sessions */
        RtspSessionMap myRtspSessionMap;
        /** */
        VMutex myRtspSessionMapMutex;

        /** for testing to generate predictable sessionIds */
        int mySessionIdList;

        /** generate random numbers */
        Random randomGen;

    protected:
        /** suppress copy constructor */
        RtspSessionManager( const RtspSessionManager& );
        /** suppress assignment operator */
        RtspSessionManager& operator=( const RtspSessionManager& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
