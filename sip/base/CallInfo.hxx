#ifndef CALL_INFO_HXX
#define CALL_INFO_HXX

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


static const char* const CallInfo_hxx_Version = 
    "$Id: CallInfo.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "Mutex.hxx"
#include "Sptr.hxx"
#include "SipCallLeg.hxx"
#include "SipCommand.hxx"
#include "SipTransceiver.hxx"
#include "InviteMsg.hxx"
#include "Feature.hxx"
#include "StateMachnData.hxx"
#include <time.h>


using namespace Vocal::Threads;

namespace Vocal
{

class TimerEvent;
class Feature;
class State;

/** Object CallInfo
<pre>
 <br> Usage of this Class </br>

      CallInfo is a base class. It stores information about a call.
      A SipProxyEvent object has CallInfo. CallInfo data members are
      thread safe.

</pre>
*/
class CallInfo
{
    public:


        /** Normal constructor initializes its data members.
         */
        CallInfo();


        /**
         * Copy constructor
         * @param rhs the entry to copy
         */
        CallInfo(const CallInfo& rhs);


        /** Destructor method
         */
        virtual ~CallInfo();


        /**
         * Assignment Operator
         * @param rhs the entry to assign
         * @return *this
         */
        const CallInfo & operator=(const CallInfo& rhs);


        /** Feature mutator
         */
        void setFeature(const Sptr < Feature > nextFeature);
        
        
        /** Feature accessor
         */
        Sptr < Feature > getFeature() const;


        /** State mutator
         */
        void setState(const Sptr < State > nextState);


        /** State accessor
         */
        Sptr < State > getState() const;


        /** SipCommand mutator
         */
        void setSipCommand(const Sptr < SipCommand > sipCommand);


        /** SipCommand accessor
         */
        Sptr < SipCommand > getSipCommand() const;


        /** Timer mutator
         */
        void setTimer(const Sptr < TimerEvent > timerEvent);


        /** Timer accessor
         */
        Sptr < TimerEvent > getTimer() const;


        /** CallData mutator
         */
        void setCallStateData(Sptr < StateMachnData > callData);


        /** CallData accessor
         */
        Sptr < StateMachnData > getCallStateData() const;


        /** CallLeg accessor
         */
        Sptr < SipCallLeg > getSipCallLeg() const;


        /** Sets itsTimer member to 0.
         */
        void removeTimer();


        /** Call mutex lock on CallInfo
         */
        void lock();


        /** Call mutex unlock on CallInfo
         */
        void unlock();
 
        /** Set the start time of the call
	     */
        void setCallTime();
  
        /** start_time accessor
	     */
        time_t getCallTime();  

        /** InviteMsg mutator
         */
        void setInvite( Sptr < InviteMsg > invite);

        /** InviteMsg accessor
         */
        Sptr < InviteMsg > getInvite();

        /** SipStack mutator
         */
        void setSipStack (Sptr < SipTransceiver > sipStack);

        /** SipStack accessor
         */
        Sptr < SipTransceiver > getSipStack();

        const string getLocalIp() const;

    private:


        /** Copy method
         *  @param rhs builder to copy
         *  @return void
         */
        void copyRhsToThis(const CallInfo& rhs);


        /** 
         */
        Mutex myMutex;


        /** Pointer to the call's Feature
         */
        Sptr < Feature >    myFeature;


        /** Pointer to the call's State
         */
        Sptr < State >      myState;


        /** Pointer to the call's SipCommand
         */
        Sptr < SipCommand > mySipCommand;


        /** Pointer to the call's TimerEvent
         */
        Sptr < TimerEvent > myTimer;


        /** Pointer to the call's StateMachData
         */
        Sptr < StateMachnData > myCallData;

        /** Start time of the call
         */
        time_t myStartTime;

	   /** Pointer to the call's InviteMsg
        */
        Sptr < InviteMsg > myInviteMessage;

       /** Pointer to the call's SipStack
        *
        */
        Sptr < SipTransceiver > mySipStack;
};
 
}

#endif // CALL_INFO_HXX
