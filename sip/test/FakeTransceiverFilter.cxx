
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
#include "Fifo.h"
#include "VMutex.h"
#include "SipTransceiver.hxx"
#include <Sptr.hxx>
#include "SipMsgQueue.hxx"
#include "SipFilterReceiveDBase.hxx"
#include <list>
#include <hash_set>

typedef vector < Sptr < SipMsg > > CallLegVector;

class SipMsg;
class SipCommand;
class StatusMsg;


Fifo < Sptr < SipMsg > > fakeTranceiver;

///
class FakeTransceiverFilter
{
    public:
        ///
        FakeTransceiverFilter(int sipPort = 5060);
        ///
        ~FakeTransceiverFilter()
        {
            timerPollShutdownMutex.lock();
            timerPollShutdown = true;
            timerPollShutdownMutex.unlock();
            timerFifo.add(0);
            timerPollThread.join();
        }


        /// This is a blocking call.
        /// A timeOut out of -1 means infinite timeout.
        ///
        void receive(int timeOutMs = -1);

        /// this is NOT blocking - msg is on the heap
        //    Sptr<SipReceive> receiveAsync();
        /// XXX -- get rid of these interfaces! -- only base depends on them

        ///
        void printSize();

        ///
        void clear();

    protected:
        /// -- technically this belongs to the internal structure of the stack
        /// -- it is only exposed for testing purposes

    private:

        ///
        //    SipTransceiver tranceiver;
        ///
        VThread timerPollThread;
        ///
        static void* timerPollThreadWrapper(void* session);
        ///
        void timerPollMain();
        ///
        bool timerPollShutdown;
        ///
        VMutex timerPollShutdownMutex;
        ///
        void addToList(Sptr < SipMsg > sipMsg);
        ///
        Fifo < Sptr < SipMsg > > timerFifo;
        ///
        // cue - commented out, use SipTransceiver directly to avoid Sptr problems
        // CallLegVector callLegVector;

        ///
        SipFilterReceiveDBase receivedResponse;

        ///
        SipFilterReceiveDBase receivedRequest;

        ///
        int timerInserts;
        ///
        int timerDeletes;

};

static const char* version UNUSED_VARIABLE =
    "$Id: FakeTransceiverFilter.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include <cassert>
#include <sys/time.h>
#include <unistd.h>
#include <cpLog.h>

#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "InviteMsg.hxx"
#include "CancelMsg.hxx"
#include "AckMsg.hxx"
#include "ByeMsg.hxx"
#include "RegisterMsg.hxx"
#include "OptionsMsg.hxx"
#include "InfoMsg.hxx"
#include <list>
#include <algorithm>

#define DEBUG_FILTER 1

#ifdef DEBUG_FILTER
//static unsigned int timerFifoMaxSize = 0;
#endif

FakeTransceiverFilter::FakeTransceiverFilter(int sipPort):
        //    tranceiver(sipPort),
        //    controlList(1600),
        timerPollShutdown(false),
        timerInserts(0),
        timerDeletes(0)
{
    //start the timerPoll thread
    timerPollThread.spawn(timerPollThreadWrapper, this);
    cpLog( LOG_DEBUG_STACK, "Spawning the timer thread");
}


void
FakeTransceiverFilter::receive(int timeOut)
{
    Sptr < SipMsg > tmpMsg = fakeTranceiver.getNext();

    Sptr < SipMsg > sipMsg = copyPtrtoSptr(&(*tmpMsg));
    //    cpLog(LOG_DEBUG_STACK, "%s", (sipMsg->encode().logData()) );

    Sptr < SipCommand > sipCommand;
    sipCommand.dynamicCast(sipMsg);

    Sptr < SipMsg > statusResponse;

    //bool isDuplicate = receivedRequest.findOrInsert(sipMsg, &statusResponse);

    timerFifo.addDelayMs(sipMsg, 1000);
    timerInserts++;
    //    isduplicateReceived( sipMsg );

    //    cpLog(LOG_DEBUG, "***** end receive *****");
}



void*
FakeTransceiverFilter::timerPollThreadWrapper(void* trans)
{
    assert( trans );

    FakeTransceiverFilter* s = static_cast < FakeTransceiverFilter* > (trans);
    s->timerPollMain();

    return 0;
}


void
FakeTransceiverFilter::timerPollMain()
{
    cpLog(LOG_DEBUG_STACK, "starting timerPollMain");

    while (1)
    {
        Sptr < SipMsg > sipMsg = timerFifo.getNext();
        timerDeletes++;
        if (sipMsg != 0)
        {
            Sptr < SipCommand > sipCommand;
            //	    Sptr<StatusMsg> statusMsg;

            //	    statusMsg.dynamicCast(sipMsg);
            sipCommand.dynamicCast(sipMsg);
            if (sipCommand != 0)
            {
                cpLog(LOG_DEBUG_STACK, "erasing request");
                cpLog(LOG_DEBUG_STACK, "erasing: \n%s",
                      sipCommand->encode().logData());
                receivedRequest.erase(sipMsg);
            }
        }
        timerPollShutdownMutex.lock();
        bool shouldShutdown = timerPollShutdown;
        timerPollShutdownMutex.unlock();
        if (shouldShutdown)
        {
            return ;
        }
    }//while 1
}



void
FakeTransceiverFilter::printSize()
{
    //    tranceiver.printSize();

    cpLog(LOG_INFO, "** sizes for TransceiverFilter objects:");
    receivedResponse.printSize();
    receivedRequest.printSize();
    cpLog(LOG_INFO, "timerFifo: %d %d %d", timerFifo.size(), timerInserts,
          timerDeletes);
}


void
FakeTransceiverFilter::clear()
{
    receivedResponse.clear();
    receivedRequest.clear();
}









///
int
main( int argc, char* argv[] )
{
    cpLogSetPriority(LOG_INFO);

    char* filename = argv[1];


    int decodeCount = 10000;

    if (argc > 2)
    {
        decodeCount = atoi(argv[2]);
    }


    std::ifstream inputFile( filename );
    if ( !inputFile )
    {
        cerr << "Error: Cannot open input file " << argv[1] << endl;
        exit( -1 );
    }
    string str;
    char ch;
    while ( inputFile.get( ch ) )
    {
        str += ch;
    }

    Data data;
    data = str;

    InviteMsg inviteMsg( str);

    FakeTransceiverFilter fake(5060);

    for (int i = 0; i < decodeCount; i++)
    {
        SipCallId callId;
        inviteMsg.setCallId( callId );

	Data encData = inviteMsg.encode();
    
        fakeTranceiver.add(SipMsg::decode(encData));
        fake.receive();
        if ((i % 100) == 0)
        {
            cout << "messages: " << i << endl;
        }
    }

    fake.printSize();
    sleep(5);
    fake.printSize();

    exit( 0 );
}
