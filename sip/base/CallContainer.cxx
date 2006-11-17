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


#include "global.h"
#include "CallContainer.hxx"
#include "CancelMsg.hxx"
#include "StatusMsg.hxx"
#include "SipCommand.hxx"
#include "SipTransceiver.hxx"

using namespace Vocal;

CallContainer::CallContainer()
{
}



CallContainer::~CallContainer()
{
    myCallInfo.clear();
}



Sptr < CallInfo >
CallContainer::getCall(const Sptr < SipCallLeg > newOrExistingCallLeg)
{
    assert ( newOrExistingCallLeg != 0 );

    Sptr < CallInfo > newCall = 0;

    TableIter iter = myCallInfo.find(newOrExistingCallLeg);
    
    if ( iter != myCallInfo.end() )
    {
	cpLog(LOG_DEBUG, "getCall(): CallLeg (%s) already exists. ",newOrExistingCallLeg->encode().logData());
        newCall = iter->second;
    }
    else
    {
	newCall = new CallInfo;
	
	cpLog(LOG_DEBUG, "getCall(): add a new entry in CallContainer\n%s",
	      newOrExistingCallLeg->encode().logData());
	
	myCallInfo[newOrExistingCallLeg] = newCall;
    }

    assert ( newCall != 0 );

    return ( newCall );
}

Sptr < CallInfo >
CallContainer::getCall(const Sptr < SipCallLeg > newOrExistingCallLeg, Sptr < InviteMsg > invite,
                       Sptr < SipTransceiver > sipStack)
{
    assert ( newOrExistingCallLeg != 0 );

    Sptr < CallInfo > newCall = 0;

    TableIter iter = myCallInfo.find(newOrExistingCallLeg);
    
    if ( iter != myCallInfo.end() )
    {
       cpLog(LOG_DEBUG, "getCall(): CallLeg (%s) already exists. ",
             newOrExistingCallLeg->encode().logData());
        newCall = iter->second;
    }
    else
    {
	newCall = new CallInfo;
 
        newCall->setInvite(invite);
        newCall->setSipStack(sipStack);
	newCall->setCallTime();

	cpLog(LOG_DEBUG, "getCall(): add a new entry in CallContainer\n%s",
	      newOrExistingCallLeg->encode().logData());

        myCallInfo[newOrExistingCallLeg] = newCall;

    }

    assert ( newCall != 0 );

    return ( newCall );
}

Sptr < CallInfo >
CallContainer::findCall(const Sptr < SipCallLeg > newOrExistingCallLeg)
{
    assert ( newOrExistingCallLeg != 0 );

    Sptr < CallInfo > newCall = 0;

    cpLog(LOG_DEBUG, "To find an entry in CallContainer\n%s",
          newOrExistingCallLeg->encode().logData());

    cpLog( LOG_DEBUG, "there are %d entries", myCallInfo.size() );

    const TableIter iter = myCallInfo.find(newOrExistingCallLeg);
    
    if ( iter != myCallInfo.end() )
    {
	cpLog( LOG_DEBUG, "found" );
	newCall = iter->second;
    }
    else
    {
	cpLog( LOG_ERR, "not found" );
    }

    return ( newCall );
}


Sptr < CallInfo >
CallContainer::addCall(const Sptr < SipCallLeg > newOrExistingCallLeg,
                       const Sptr < CallInfo > theCallInfo)
{
    assert ( newOrExistingCallLeg != 0 );
    assert ( theCallInfo != 0 );

    cpLog(LOG_DEBUG, "addCall(): add a new entry in CallContainer\n%s",
          newOrExistingCallLeg->encode().logData());

    myCallInfo[newOrExistingCallLeg] = theCallInfo;

    return ( theCallInfo );
}



void
CallContainer::removeCall(const Sptr < SipCallLeg > existingCall)
{
    assert ( existingCall != 0 );

    const TableIter iter = myCallInfo.find( existingCall );

    cpLog(LOG_DEBUG, "remove entry in CallContainer\n%s",
          existingCall->encode().logData());

    if ( iter != myCallInfo.end() )
    {
        myCallInfo.erase(iter);
    }
    else
    {
	    cpLog(LOG_ERR, "**** NO CALLLEG found");
    }
}

void
CallContainer::checkCalls()
{
   double call_duration;

   Sptr <CallInfo> existingCall = 0;

   if (myCallInfo.size() != 0){
      for( TableIter iter = myCallInfo.begin(); iter != myCallInfo.end(); iter++){
          existingCall = iter->second;
          call_duration = difftime(time(NULL), existingCall->getCallTime());

          // Set the 'timeout' value in here
          if( call_duration > 30.0 ){
             cancelCall(existingCall);
             
             cpLog(LOG_DEBUG, "cancelling call, duration: %lf", call_duration);

             myCallInfo.erase(iter);
          }
      }
    
      cpLog(LOG_DEBUG, "Number of pending calls: %d", myCallInfo.size());

   }
}

void
CallContainer::cancelCall(const Sptr < CallInfo > callInfo)
{
   Sptr < InviteMsg > savedInvite = callInfo->getInvite();
   Sptr < SipTransceiver > sipStack = callInfo->getSipStack();

   Sptr<StatusMsg> errorMsg = new StatusMsg( *savedInvite, 480 );
   //errorMsg->removeVia( 0 );
   sipStack->sendReply( errorMsg, "CallContainer cancelCall, Reply" );

   Sptr<CancelMsg> cancelMsg = new CancelMsg( *savedInvite );
   sipStack->sendAsync(cancelMsg.getPtr(), "CallContainer cancelCall, Async");

   cpLog(LOG_INFO, "Call cancelled");
}

