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



static const char* const ControlStateFactory_cxx_Version = 
"$Id: ControlStateFactory.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include "ControlStateFactory.hxx"
#include "StateTrying.hxx"
#include "StateUacRinging.hxx"
#include "StateUasRinging.hxx"
#include "StateInCall.hxx"
#include "StateInHold.hxx"
#include "StateInit.hxx"
#include "StateTearDown.hxx"

using namespace Vocal;
using namespace Vocal::UA;

ControlStateFactory* ControlStateFactory::myInstance = 0;
Mutex ControlStateFactory::myMutex;


ControlStateFactory& 
ControlStateFactory::instance()
{
    if(myInstance == 0)
    {
        myInstance = new ControlStateFactory();
    }
    return *myInstance;
}

void
ControlStateFactory::destroy()
{
    delete ControlStateFactory::myInstance;
    ControlStateFactory::myInstance = 0;
    
}

ControlState* 
ControlStateFactory::getState(CStateType stateType)
{
    ControlState* retVal;

    myMutex.lock();
    pthread_t tId = pthread_self();
    ControlStateMap& tMap = myControlStateMap[tId];

    ControlStateMap::iterator itr  = tMap.find(stateType);
    if(itr != tMap.end())
    {
        myMutex.unlock();
        return itr->second;
    }
    switch(stateType)
    {
        case INIT:
           retVal = new StateInit();
        break;
        case TRYING:
           retVal = new StateTrying();
        break;
        case UAS_RINGING:
           retVal = new StateUasRinging();
        break;
        case UAC_RINGING:
           retVal = new StateUacRinging();
        break;
        case IN_CALL:
           retVal = new StateInCall();
        break;
        case TEAR_DOWN:
           retVal = new StateTearDown();
        break;
        case HOLD:
	    retVal = new StateInHold();
        break;
        default:
            assert(0); 
        break;
    }
    tMap[stateType] = retVal;
    myMutex.unlock();
    return retVal;
}

ControlStateFactory::~ControlStateFactory()
{
    for(ThreadBasedControlStateMap::iterator itr = myControlStateMap.begin();
               itr != myControlStateMap.end(); itr++)
    {
        ControlStateMap& tMap = itr->second;
        for(ControlStateMap::iterator itr2 = tMap.begin();
               itr2 != tMap.end(); itr2++)
        {
            delete (itr2->second);
        }
        tMap.erase(tMap.begin(), tMap.end());
    }
}

