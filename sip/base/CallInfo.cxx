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


static const char* const CallInfo_cxx_Version =
    "$Id: CallInfo.cxx,v 1.2 2004/05/04 07:31:14 greear Exp $";


#include "global.h"
#include "CallInfo.hxx"
#include "TimerEvent.hxx"
#include "SystemInfo.hxx"


using namespace Vocal;

CallInfo::CallInfo()
    :   myFeature(0),
        myState(PROXY_CONTINUE),
        mySipCommand(0),
        myTimer(0),
        myCallData(0)
{
}


CallInfo::CallInfo(const CallInfo & rhs)
{
    copyRhsToThis(rhs);
}


const CallInfo &
CallInfo::operator=(const CallInfo & rhs)
{
    if ( this != &rhs )
    {
        copyRhsToThis(rhs);
    }

    return ( *this );
}


CallInfo::~CallInfo()
{
}


const string CallInfo::getLocalIp() const {
   string li = mySipStack->getLocalIp();
   if (li.size() == 0) {
      li = theSystem.gethostAddress(); //OK
   }
   return li;
}


void
CallInfo::setFeature(const Sptr < Feature > nextFeature)
{
    myFeature = nextFeature;
}



Sptr < Feature >
CallInfo::getFeature() const
{
    return ( myFeature );
}



void
CallInfo::setState( const Sptr < State > nextState )
{
    myState = nextState;
}


Sptr < State >
CallInfo::getState() const
{
    return ( myState );
}


void
CallInfo::setSipCommand( const Sptr < SipCommand > sipCommand )
{
    mySipCommand = sipCommand;
}


Sptr < SipCommand >
CallInfo::getSipCommand() const
{
    return ( mySipCommand );
}


void
CallInfo::setTimer( const Sptr < TimerEvent > timerEvent )
{
    assert ( timerEvent != 0 );

    myTimer = timerEvent;
}



Sptr < TimerEvent >
CallInfo::getTimer() const
{
    return ( myTimer );
}



void
CallInfo::removeTimer()
{
    myTimer = 0;
}



Sptr < SipCallLeg >
CallInfo::getSipCallLeg() const
{
    Sptr < SipCallLeg > retSipCallLeg = new SipCallLeg(getLocalIp());

    assert( mySipCommand != 0 );

    *retSipCallLeg = mySipCommand->computeCallLeg();

    return ( retSipCallLeg );
}



void
CallInfo::setCallStateData(Sptr < StateMachnData > callData)
{
    myCallData = callData;
}



Sptr < StateMachnData >
CallInfo::getCallStateData() const
{
    return ( myCallData );
}


void
CallInfo::setCallTime()
{
  myStartTime = time(NULL);
}

time_t
CallInfo::getCallTime()
{
  return ( myStartTime );
}

void
CallInfo::setInvite(Sptr < InviteMsg > invite)
{
   myInviteMessage = invite;
}

Sptr < InviteMsg >
CallInfo::getInvite ()
{
  return (myInviteMessage);
}

void
CallInfo::setSipStack(Sptr < SipTransceiver > sipStack)
{
   mySipStack = sipStack;
}

Sptr < SipTransceiver >
CallInfo::getSipStack()
{
  return (mySipStack);
}

void
CallInfo::copyRhsToThis(const CallInfo & rhs)
{
    myFeature = rhs.myFeature;
    myState = rhs.myState;
    mySipCommand = rhs.mySipCommand;
    myTimer = rhs.myTimer;
    myCallData = rhs.myCallData;
}

