#ifndef State_HXX
#define State_HXX
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

static const char* const State_hxx_version =
    "$Id: State.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include <list> 

#include "Sptr.hxx"
#include "Operator.hxx"
#include "StateEvent.hxx"

// defined for circular dependancies
class State;
class Operator;

/** Return value for process() routine of operators
 *  indicating the processing of the event is complete and no
 *  subsequent operators need to be called.
 */
extern Sptr<State> DONE_STATE;

/** Return value for process() routine of operators
 *  indicating the processing of the events is to continue.
 */
extern Sptr<State> NEXT_STATE;

/** Return value for process() routine of operators
 *  indicating the processing of the events has fatal error.
 */
extern Sptr<State> FATAL_STATE;


/** Base class for states.  States are container of Operators.
    The real work is done on the Operator::process() function.
 */
class State
{
    public:
        /** constructor */
        State() {};
        /** destructor */
        virtual ~State() {};
        /** returns name of this state */
        virtual const char* const name() const
            { return "State"; }

        /** Interface to process of Operator.  Iterates through process() of
            all the operators it contains.
            @param event event to process on
            @return next state
         */
        virtual const Sptr<State> process( const Sptr<StateEvent> event );

    protected:
        /** add an operator to this state */
        void addOperator( const Sptr<Operator> op );

        /** OperatorContainer define */
        typedef list< Sptr<Operator> > OperatorContainer;
        /** list of operators */
        OperatorContainer myOperators;

    protected:
        /** suppress copy constructor */
        State( const State& );
        /** suppress assignment operator */
        State& operator=( const State& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
