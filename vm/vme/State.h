#ifndef STATE_H
#define STATE_H

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



static const char* const State_h_Version = 
"$Id: State.h,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include "Waiter.h"
#include "Session.h"
#include <string>

/**
  * Generic state <br><br>
  */

/** Common State Return Codes
 */
const int StateStay = 1;	/// Signal to continue loop in current state
const int StateReturn = 2;	/// Return to the calling state
const int StatePanic = 3;	/// Fatal error occured
const int StateEndOfSession = 4;	/// End of session
const int StateUserCode = 1000;	/// User return code

class State
{

public:
    /** Constructor 
     */  
    State (string Name);

    /** Destructor 
     */
    virtual ~ State ();

    /** set state name 
    */
    virtual void Set (string Name);
    
    /** Main state loop
     */
     virtual int Process (Session * session);
  
    /** Main message loop
     */
    virtual int ProcessLoop (Session * session);

    /** Event handlers called from ProcessLoop.
     */
    /** Process DTMF digits 
     */
    virtual int ProcessDTMF (pEvent evt);

    /** Process messages from recorder 
     */
    virtual int ProcessRecorder (pEvent evt);

    /** Process events from Line
     */
    virtual int ProcessLine (pEvent evt);

    /** Process events from timers 
    */
    virtual int ProcessTimer (pEvent evt);

    /** Process events from player 
    */
    virtual int ProcessPlayer (pEvent evt);

    /** Process events from generic source 
    */
    virtual int ProcessGeneric (pEvent evt);

    virtual void setSession(Session *session);

    virtual Session *getSession();

/** Protected attributes 
*/
protected:		
    /** State Name
     */
    string m_xName;
 
    /** State event sources list
     */
    EventSourcesList m_xEventSources;

    /** guard timer to avoid state hangups 
    */
    Timer m_xGuardTimer;

private:
    /** session pointer holder
    */
    Session *m_xSession;
};

//typedef State *pState;

#endif
