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


static const char* const SipThread_cxx_Version =
    "$Id: SipThread.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "global.h"
#include "cpLog.h"
#include "SipThread.hxx"
#include "SipVia.hxx"
#include "SipEvent.hxx"
#include "SipMsg.hxx"
#include "SipTransceiver.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;


SipThread::SipThread( const Sptr < SipTransceiver > sipStack, 
                      const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo ,
                      bool callLegHistory)
      : ThreadIf(0, VTHREAD_PRIORITY_DEFAULT, 64 * 1024 /* VTHREAD_STACK_SIZE_DEFAULT*/),
        mySipStack(sipStack), myOutputFifo(outputFifo), myCallLegHistory(callLegHistory)
{
    assert ( mySipStack != 0 );
    assert ( myOutputFifo != 0 );
}



SipThread::~SipThread()
{
}



bool
SipThread::discardMessage( Sptr < SipMsg > &sipMsg) const
{
    int numberOfVia = sipMsg->getNumVia();

    // check required only if > 2 via's in msg
    //
    if ( numberOfVia <= 2 )
    {
        return false;
    }

    string my_local_ip = mySipStack->getLocalIp();
    if (my_local_ip.size() == 0) {
       my_local_ip = theSystem.gethostAddress(); //OK
    }

    SipVia mySipVia("", my_local_ip);
    mySipVia.setHost(my_local_ip);

    for ( int i=0, loopCount = 0; i < numberOfVia; i++ )
    {
        const SipVia& recvSipVia = sipMsg->getVia(i);

        if ( recvSipVia == mySipVia )
        {
            loopCount++;

            if ( loopCount > 2 )
            {
                cpLog(LOG_DEBUG, "Discarding Msg, loop > %d. %s", 
		      loopCount, recvSipVia.encode().logData());

                return ( true );
            }
        }
    }

    return ( false );
}



void
SipThread::thread()
{
    cpLogSetLabelThread (VThread::selfId(), "bcSip");

    while ( true )
    {
        try
        {
            assert ( mySipStack != 0 );
            assert ( myOutputFifo != 0 );

            Sptr < SipMsgQueue > sipRcv( mySipStack->receive(1000) );

            if ( sipRcv != 0 )
            {
                Sptr < SipMsg > sipMsg = sipRcv->back();

                if ( sipMsg != 0 )
                {
                    Sptr < SipEvent > nextEvent = new SipEvent(myOutputFifo);

                    if ( nextEvent != 0 )
                    {
                        nextEvent->setSipReceive(sipRcv);
                        nextEvent->setSipStack(mySipStack);
                        if(myCallLegHistory) nextEvent->setCallLeg();

                        myOutputFifo->add(nextEvent);
                    }
                }
            }
        }

        catch ( VException& v)
        {
            cpLog( LOG_DEBUG, "VException from SIP  stack %s", v.getDescription().c_str() );
        }
        catch ( ... )
        {
            cpLog( LOG_DEBUG, "Exception from SIP stack" );
        }

        if ( isShutdown() == true )
        {
            return;
        }
    }
}
