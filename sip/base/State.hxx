#ifndef STATE_HXX
#define STATE_HXX

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


static const char* const State_hxx_Version = 
    "$Id: State.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include <list>
#include "cpLog.h"
#include "Sptr.hxx"
#include "Operator.hxx"
#include "SipProxyEvent.hxx"

namespace Vocal 
{

/* We have circular dependancies, so forward declare.
 */
class State;
class Operator;


/** Return value for process() routine of proxy operators
 *  indicating the processing of the event is complete and no
 *  subsequent operators need to be called. 
 */
extern Sptr<State> PROXY_DONE_WITH_EVENT;


/** Return value for process() routine of proxy operators
 *  indicating the processing of the events is to continue.
 */
extern Sptr<State> PROXY_CONTINUE;


/** Object State
<pre>
<br> Usage of this Class </br>

     It is a operator container. The State::process() calls Operator::process().
     Application in a State can have various operations ( each handled by 
     an operator) that it can perform on the data received.Once operator is 
     done with data it can either flag completion or tell state to call other
     operator in queue to do more processing.Operators are executed in order
     they were added to state.

FooState.hxx:

     class FooState : public Vocal::State
     {
        public:
        FooState();
        virtual ~FooState();
        const char* const name() const;
     };

FooState.cxx:

    using namespace Vocal;

    FooState::FooState()
    {

      Sptr < FooOpAck > opAck           = new FooOpAck;
      addOperator( opAck );

      Sptr < FooOpBye > opBye           = new FooOpBye;
      addOperator( opBye );

      Sptr < FooOpInvite > opInvite     = new FooOpInvite;
      addOperator( opInvite );

      Sptr < FooOpProxy > opProxy       = new FooOpProxy;
      addOperator( opProxy );
   }

   FooState::~FooState()
   {}

   const char* const
   FooState::name() const
   {
     return "FooState";
   }
</pre>
*/
class State
{
    public:

        /** Default constructor
         */
        State(const char* name);


        /** Virtual destructor
         */
        virtual ~State();


        /** When an event arrives for this state, the typical behavior is
         *  to have each operator associated with this state process the event, 
         *  in the order the operator's were added to this state. If the 
         *  operator returns 0, the routine will check shortCircuitState(). 
         *  If this returns false, which is the default, it will continue 
         *  processing operators, otherwise it will stop processing operators 
         *  and return the next state.
         */
        virtual Sptr < State > process(const Sptr < SipProxyEvent > event);


        virtual const string& name() const
        {
            return _name;
        }


        /** Logging helper method
         */
        virtual void doCpLog() const
        {
            cpLog( LOG_DEBUG, "State: %s", name().c_str() );
        }


    protected:


        /** Add an operator to this state.
         */
        void addOperator(const Sptr < Operator > op);


        /** An operator whose process method will be called when the 
         *  state is entered. The returned state from the operator's process
         *  is ignored.
         */
        void addEntryOperator(const Sptr < Operator > op);


        /** An operator whose process method will be called when the 
         *  state is exited. The returned state from the operator's process
         *  is ignored.
         */
        void addExitOperator(const Sptr < Operator > op);


        /** All operators, including entry and exit, and forgotten.
         */
        void clearAll( );


        /** Called upon entry into the state. All associated entry operators'
         *  process methods are called. They will be called in the order they
         *  have been added to the state. There is not short circuiting
         *  operators.
         */
        virtual void processEntry(const Sptr < SipProxyEvent > event);


        /** Called upon exit from the state. All associated exit operators'
         *  process methods are called. They will be called in the order they
         *  have been added to the state. There is not short circuiting
         *  operators.
         */
        virtual void processExit(const Sptr < SipProxyEvent > event);


        /** List of operators.
         */
        typedef list < Sptr < Operator > > OperatorContainer;


        /** Iterator for the operator list. Yes, it's poorly named.
         */
        typedef OperatorContainer::iterator OperatorIter;


        /** Contains operators to execute on state entry
         */
        OperatorContainer       myEntryOperators;    


        /** Contains operators to handle events
         */
        OperatorContainer       myOperators;


        /** Contains operators to execute on state exit
         */
        OperatorContainer       myExitOperators;   

        string _name;
    private:


        /** Add an operator (op) to the given container, logging the
         *  given logMessage.
         */
        void helperAdd( OperatorContainer       &   container, 
	    	        const Sptr < Operator > &   op, 
			const char* const           logMessage);

        // Gotta give it a name.
        State();


        /** Suppress copying
         */
        State(const State &);


        /** Suppress copying
         */
        const State & operator=(const State &);
};
 
}

#endif // STATE_HXX
