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

static const char* const State_cxx_version =
    "$Id: State.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "State.hxx"
#include "Operator.hxx"


// Create a unique instance for comparison
Sptr<State> DONE_STATE = 0;
Sptr<State> NEXT_STATE = new State;
Sptr<State> FATAL_STATE = new State;


const Sptr<State>
State::process( const Sptr<StateEvent> event )
{
    Sptr<State> nextState = NEXT_STATE;

    // iterate through all operators of the given state
    for( OperatorContainer::iterator itr = myOperators.begin();
         itr != myOperators.end(); itr++ )
    {
        //cpLog( LOG_DEBUG, "Trying %s", (*itr)->name() );
        Sptr<State> newState = (*itr)->process( event );
        if( newState == DONE_STATE )
        {
            // done processing
            cpLog( LOG_DEBUG, "Operator done processing\n" );
            nextState = newState;
            break;
        }
        else if( newState == NEXT_STATE )
        {
            // go to next operator
            continue;
        }
        else
        {
            // change state
            cpLog( LOG_DEBUG, "Operator done processing with state change" );
            nextState = newState;
            break;
        }
    }

    return nextState;
}


void
State::addOperator( const Sptr<Operator> op )
{
    //cpLog( LOG_DEBUG, "Insert %s operator", op->name() );
    myOperators.push_back( op );
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
