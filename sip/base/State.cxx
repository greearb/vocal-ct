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
#include "State.hxx"
#include "CallInfo.hxx"

using namespace Vocal;


// Create a unique instance for comparison.
//
Sptr<State> Vocal::PROXY_DONE_WITH_EVENT = new State("Done");


// Continue is just 0
//
Sptr<State> Vocal::PROXY_CONTINUE = new State("Continue");


State::State(const char* event_name)
      : _name(event_name)
{
}


State::~State()
{
    myOperators.clear();
}


Sptr < State >
State::process(const Sptr < SipProxyEvent > event)
{
    Sptr<State> rv;
    Sptr < State > currentState;
    Sptr < State > nextState;
	 OperatorIter iter;

    if ( event == 0 )
    {
        assert ( event != 0 );
        goto out;
    }

    //  Get the next state from the event's call info. This will be the
    //  next state if a state isn't returned from one of the operators.
    //
    currentState = event->getCallInfo()->getState();


    cpLog(LOG_DEBUG_STACK, "State::process, event: %s, currentState: %s  thisState: %s\n",
          event->toString().c_str(), currentState->name().c_str(),
          name().c_str());
    
    // To process this state, the currentState must either be PROXY_CONTINUE
    // or this. If it is PROXY_CONTINUE, that means that this is really
    // a stateless proxy and they didn't bother to set the currentState.
    //
    if  ( (currentState != PROXY_CONTINUE)
          &&  (currentState != this) )
    {
        cpLog(LOG_DEBUG, "******************************PROXY CONTINUE");
        rv = PROXY_CONTINUE;
        goto out;
    }

    nextState = PROXY_CONTINUE;

    // Iterating through all operators of the given state.
    //
    for (   iter = myOperators.begin(); 
            iter != myOperators.end(); 
            iter++ 
        )
    {
        cpLog(LOG_DEBUG, "State::process: Processing operator (%s)", (*iter)->name());
        Sptr < State > newState = (*iter)->process(event);

	if( newState == PROXY_DONE_WITH_EVENT )
	{
	    cpLog(LOG_DEBUG, "***** Done with Event ***");
	    nextState = newState;
	    break;
	}
	else if( newState != PROXY_CONTINUE )
	{
	    // We have to assert, because two operators returning new
	    // states is a major bug.
	    assert( nextState == PROXY_CONTINUE );
	    nextState = newState;
	}
    }

    if( nextState != PROXY_CONTINUE )
    {                
	// Leaving this state, so invoke the exit operators.
	//
	processExit(event);
    
	// Update the nextState.
	//
	event->getCallInfo()->setState(nextState);
    
	// Entering the new state, so invoke the entry operator.
	//
	nextState->processEntry(event);
    }

    rv = nextState;

  out:
    cpLog(LOG_DEBUG, "DOne with state (%s), nextState: %s",
          name().c_str(), rv->name().c_str());
    return rv;
}


void
State::addOperator(const Sptr < Operator > op)
{
    helperAdd(myOperators, op, "Insert %s operator");
}



void
State::addEntryOperator(const Sptr < Operator > op)
{
    helperAdd(myEntryOperators, op, "Insert %s entry operator");
}



void
State::addExitOperator(const Sptr < Operator > op)
{
    helperAdd(myExitOperators, op, "Insert %s exit operator");
}



void
State::clearAll( )
{
    while ( myEntryOperators.empty() == false )
    {
        myEntryOperators.pop_back();
    }

    while ( myExitOperators.empty() == false )
    {
        myExitOperators.pop_back();
    }

    while ( myOperators.empty() == false )
    {
        myOperators.pop_back();
    }
}


void
State::processEntry(const Sptr < SipProxyEvent > event)
{
    cpLog(LOG_DEBUG_STACK, "State::processEntry, thiState: %s\n", name().c_str());

    for (   OperatorIter iter = myEntryOperators.begin(); 
            iter != myEntryOperators.end(); 
            iter++ 
        )
    {
        (*iter)->process(event);
    }
}



void
State::processExit(const Sptr < SipProxyEvent > event)
{
    cpLog(LOG_DEBUG_STACK, "State::processExit, thiState: %s\n", name().c_str());

    for (   OperatorIter iter = myExitOperators.begin(); 
            iter != myExitOperators.end(); 
            iter++ 
        )
    {
        (*iter)->process(event);
    }
}




void
State::helperAdd( 
    OperatorContainer	    &   container, 
    const Sptr < Operator > & 	op, 
    const char* const 	    	cpLogStr 
)
{
    assert( op != 0 );

    cpLog(LOG_DEBUG, cpLogStr , op->name());

    container.push_back(op);

    assert( container.begin() != container.end() );
}


