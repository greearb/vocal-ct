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



static const char* const UaStateFactory_cxx_Version = 
"$Id: UaStateFactory.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#pragma warning (disable: 4786)

#include "UaStateFactory.hxx"
#include "UaStateIdle.hxx"
#include "UaStateEnd.hxx"
#include "UaStateFailure.hxx"
#include "UaStateInCall.hxx"
#include "UaStateRinging.hxx"
#include "UasStateTrying.hxx"
#include "UacStateTrying.hxx"
#include "UaStateRedirect.hxx"
#include "Lock.hxx"
#include "UaStateInHold.hxx"

using namespace Vocal;
using namespace UA;


UaStateFactory* UaStateFactory::myInstance = 0;
Mutex UaStateFactory::myMutex;


UaStateFactory& 
UaStateFactory::instance()
{
    if(myInstance == 0)
    {
        myInstance = new UaStateFactory();
    }
    return *myInstance;
}

void
UaStateFactory::destroy()
{
    delete UaStateFactory::myInstance;
    UaStateFactory::myInstance = 0;
    
}

UaState* 
UaStateFactory::getState(UStateType stateType)
{
    UaState* retVal;

    myMutex.lock();
    pthread_t tId = pthread_self();
    UaStateMap& tMap = myUaStateMap[tId];
    UaStateMap::iterator itr  = tMap.find(stateType);

    if(itr != tMap.end())
    {
        myMutex.unlock();
        return itr->second;
    }
    switch(stateType)
    {
        case U_STATE_IDLE:
           retVal = new UaStateIdle();
        break;
        case U_STATE_S_TRYING:
           retVal = new UasStateTrying();
        break;
        case U_STATE_C_TRYING:
           retVal = new UacStateTrying();
        break;
        case U_STATE_RINGING:
           retVal = new UaStateRinging();
        break;
        case U_STATE_FAILURE:
           retVal = new UaStateFailure();
        break;
        case U_STATE_INCALL:
           retVal = new UaStateInCall();
        break;
        case U_STATE_END:
           retVal = new UaStateEnd();
        break;
        case U_STATE_REDIRECT:
           retVal = new UaStateRedirect();
	break;
        case U_STATE_HOLD:
	    retVal = new UaStateInHold();
	break;
        default:
            cpLog(LOG_WARNING, "Unsupported state" );
            assert(0); 
        break;
    }
    tMap[stateType] = retVal;
    myMutex.unlock();
    return retVal;
}

UaStateFactory::~UaStateFactory()
{
    for(ThreadBasedUaStateMap::iterator itr = myUaStateMap.begin();
               itr != myUaStateMap.end(); itr++)
    {
        UaStateMap& tMap = itr->second;
        for(UaStateMap::iterator itr2 = tMap.begin();
               itr2 != tMap.end(); itr2++)
        {
            delete (itr2->second);
        }
        tMap.erase(tMap.begin(), tMap.end());
    }
}

