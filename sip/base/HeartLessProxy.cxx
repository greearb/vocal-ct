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


static const char* const HeartLessProxy_cxx_Version =
    "$Id: HeartLessProxy.cxx,v 1.2 2004/05/04 07:31:14 greear Exp $";


#include "global.h"
#include "HeartLessProxy.hxx"
#include "SipTransceiverFilter.hxx"
#include "Builder.hxx"
#include "SipThread.hxx"

using namespace Vocal;

HeartLessProxy::HeartLessProxy( 
    const Sptr < Builder >  builder,
    int hashTableSize,
    const string&           local_ip,
    const string&           local_dev_to_bind_to,
    unsigned short          defaultSipPort,
    Data                    applName,
    bool                    filterOn, 
    bool                    nat,
    SipAppContext           aContext
)
{
    myCallContainer = new CallContainer;

    myBuilder = builder;
    myBuilder->setCallContainer(myCallContainer);
    
    myCallProcessingQueue = new list < Sptr < SipProxyEvent > >;


    //  Filter option controls which transceiver object is created for the
    //  sip stack.
    // NOTE:  The filter transceiver is just a typedef to SipTransceiver...
    mySipStack = new SipTransceiver(hashTableSize, local_ip,
                                    local_dev_to_bind_to, applName,
                                    defaultSipPort, nat, aContext);

    myBuilder->setSipStack(mySipStack);

    assert( myCallContainer != 0 );
    assert( myCallProcessingQueue != 0 );
    assert( myBuilder != 0 );
    assert( mySipStack != 0 );
}


HeartLessProxy::~HeartLessProxy()
{
   delete myCallProcessingQueue;
   myCallProcessingQueue = NULL;
}





