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

/**********************************************************************
 
$Id: gw.cxx,v 1.1 2004/05/01 04:15:31 greear Exp $
 
**********************************************************************/


// sample gateway code.

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

//#include "mgcpCoding.hxx"

#include "mg.hxx"

#include <queue>
#include "nullHw.hxx"
#include "tpjackHw.hxx"

#include <typeinfo>
#include <map>

/**********************************************************************
 
Data structures
 
**********************************************************************/


class StateMachine;
class StateEvent;


typedef void (*StateHandler)(StateEvent* event, StateMachine* state);

void stateUninitialized(StateEvent* event, StateMachine* state);
void waitForResponse(StateEvent* event, StateMachine* state);
void stateIdle(StateEvent* event, StateMachine* state);
void stateRingingIn(StateEvent* event, StateMachine* state);
void stateRingingOut(StateEvent* event, StateMachine* state);
void stateHalfOpenIn(StateEvent* event, StateMachine* state);
void stateHalfOpen(StateEvent* event, StateMachine* state);
void stateActiveNoNotifications(StateEvent* event, StateMachine* state);
void stateActive(StateEvent* event, StateMachine* state);
void stateWaitForClose(StateEvent* event, StateMachine* state);
void stateWaitOnHook(StateEvent* event, StateMachine* state);


enum StateEventStimulus
{
    StimulusNULL,
    StimulusNewMessage,
    StimulusResponse,
    StimulusOffHook,
    StimulusOnHook,
};


class StateMachine
{
    public:
        StateMachine()
        {}
        ;

        // yes, this is gross
        RequestId CurrentRequestId;
        CallId callID;
        ConnectionMode callMode;
        MgcpTransmitter* callAgent;
        UdpReceiver* server;
        LocalConnectionDescriptor localDescriptor;
        RemoteConnectionDescriptor RemoteDescriptor;

        HardwareObject* hardware;

        map < StateEventStimulus, Event* > activeEventList;

        setState(StateHandler current, StateHandler pending)
        {
            char* string = "";
            if (current == stateUninitialized)
                string = "stateUninitialized";
            if (current == waitForResponse)
                string = "waitForResponse";
            if (current == stateIdle)
                string = "stateIdle";
            if (current == stateRingingOut)
                string = "stateRingingOut";
            if (current == stateHalfOpen)
                string = "stateHalfOpen";
            if (current == stateActiveNoNotifications)
                string = "stateActiveNoNotifications";
            if (current == stateActive)
                string = "stateActive";


            cout << "State changed to: " << string << "\n";



            currentState = current;
            pendingState = pending;
        };

        StateHandler currentState;
        StateHandler pendingState;


};

class StateEvent
{
    public:
        StateEvent() : stimulus(StimulusNULL)
        {}



        StateEventStimulus getStimulusType()
        {
            return stimulus;
        }
        setStimulusType(StateEventStimulus x)
        {
            stimulus = x;
        }

        setMessage(MgcpCommand* msg)
        {
            newMessage = msg;
            if (dynamic_cast < MgcpResponse* > (msg))
            {
                stimulus = StimulusResponse;
            }
            else
            {
                stimulus = StimulusNewMessage;
            }
        }
        MgcpCommand* getMessage()
        {
            return newMessage;
        }

        void setStimulus(StateEventStimulus stim)
        {
            stimulus = stim;
        }
        StateEventStimulus getStimulus()
        {
            return stimulus;
        }

    private:
        StateEventStimulus stimulus;
        MgcpCommand* newMessage;
};




char* getHost()
{
    static char buf[256];

    gethostname(buf, 256);

    return buf;
}

void wrongMessage()
{
    cerr << "received invalid message\n";

    int parentpid = getpid();
    // parent
    kill(parentpid, SIGSTOP);

    exit(1);
}

void fatalError(char* errorMsg)
{
    cerr << errorMsg << "\n";

    exit(1);
}

/**********************************************************************
 
Globals
 
**********************************************************************/

EndpointId set("testID@localhost:5050");





void stateUninitialized(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusNewMessage)
    {
        MgcpCommand* test_item = event->getMessage();
        //    MgcpCommand* test_item = parseMessage(data->getMessageLocation(), 0);

        // state 3 -- receive notification request


        NotificationRequest* nrPtr;

        nrPtr = dynamic_cast < NotificationRequest* > (test_item);

        if (nrPtr == NULL)
            wrongMessage();

        // check for the right parameters

        state->CurrentRequestId = *(nrPtr->getRequestId());

        vector < Event* > * eventList((nrPtr->getRequestedEvents())->getVector());

        if (!eventList)
            wrongMessage();
        // get the appropriate events --


        vector < Event* > ::iterator eventIter;

        eventIter = eventList->begin();

        while (eventIter != eventList->end())
        {
            if (dynamic_cast < LineEventOffHook* > (*eventIter) == NULL)
                wrongMessage();  // only accept this event sir

            ++eventIter;
        }

        // reply OK.

        MgcpResponse response00(200, nrPtr->getTransactionId(), "OK");
        response00.send(*(state->callAgent));

        state->setState(stateIdle, NULL);
    }

    // ignore other messages
}



void waitForResponse(StateEvent* event, StateMachine* state)
{
    if (state->pendingState == NULL)
    {
        fatalError("illegal state machine");
    }
    if (event->getStimulusType() == StimulusResponse)
    {
        //
        state->setState(state->pendingState, NULL);
    }
    else
    {
        // this is an event which this state must handle somehow or another.
        state->pendingState(event, state);
    }
}

void stateIdle(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusOffHook)
    {
        Notify msg8(set);
        ObservedEvents events8;
        LineEventOffHook offHook;

        events8.insert(offHook);

        msg8.insert(&events8);
        msg8.insert(&(state->CurrentRequestId));
        msg8.send(*(state->callAgent));


        state->setState(waitForResponse, stateRingingOut);

    }
    else if (event->getStimulusType() == StimulusNewMessage)
    {
        MgcpCommand* test_item = event->getMessage();

        if (typeid(*test_item) == typeid(NotificationRequest))
        {
            NotificationRequest* nrPtr;

            nrPtr = dynamic_cast < NotificationRequest* > (test_item);

            if (nrPtr == NULL)
                wrongMessage();

            // check for the right parameters

            state->CurrentRequestId = *(nrPtr->getRequestId());

            vector < Event* > * eventList((nrPtr->getRequestedEvents())->getVector());

            if (!eventList)
                wrongMessage();
            // get the appropriate events --

            vector < Event* > ::iterator eventIter;

            eventIter = eventList->begin();

            bool lookForOffHook(false);

            while (eventIter != eventList->end())
            {
                if (dynamic_cast < LineEventOffHook* > (*eventIter) != NULL)
                    lookForOffHook = true;

                ++eventIter;
            }

            if (!lookForOffHook)
                wrongMessage();

            SignalRequests* signals;

            signals = nrPtr->getSignalRequests();

            vector < Signal* > * signalList(signals->getList());

            if (!signalList)
                wrongMessage();
            // get the appropriate signals --

            vector < Signal* > ::iterator signalIter;

            signalIter = signalList->begin();

            bool sendRing(false);
            bool sendRingback(false);

            while (signalIter != signalList->end())
            {
                if (dynamic_cast < SignalRing* > (*signalIter) != NULL)
                {
                    sendRing = true;
                }
                if (dynamic_cast < SignalRingback* > (*signalIter) != NULL)
                {
                    sendRingback = true;
                }

                ++signalIter;
            }

            if (!(sendRing && sendRingback))
                wrongMessage();

            state->hardware->sendSignal(SignalRingStart);
            state->hardware->sendSignal(SignalRemoteRingbackStart);

            // reply OK.

            MgcpResponse response00(200, nrPtr->getTransactionId(), "OK");
            response00.send(*(state->callAgent));

            state->CurrentRequestId = (*(nrPtr->getRequestId()));

            state->setState(stateRingingIn, NULL);

        }
    }
    else
    {
        wrongMessage();
    }
}


// the set is ringing and you may get the message offhook/onhook

void stateRingingIn(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusOffHook)
    {
        // send a notify

        Notify msg8(set);
        ObservedEvents events8;
        LineEventOffHook offHook;

        events8.insert(offHook);
        //    msg8.insert(&set);

        msg8.insert(&events8);
        msg8.insert(&state->CurrentRequestId);
        msg8.send(*(state->callAgent));

        // wait for the create connection

        state->setState(waitForResponse, stateHalfOpenIn);
    }
}

void stateRingingOut(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusNewMessage)
    {
        MgcpCommand* test_item = event->getMessage();

        // state 5 -- receive create connection

        CreateConnection* ccPtr;

        ccPtr = dynamic_cast < CreateConnection* > (test_item);

        if (ccPtr == NULL)
            wrongMessage();

        // now calculate the return address information

        // don't forget to save the remote side's data

        // now you need to reply with some useful info.

        state->callID = (*(ccPtr->getCallId()));
        ConnectionMode callMode(*(ccPtr->getConnectionMode()));

        if (callMode.getMode() != ConnectionSendRecv)
            wrongMessage();

        state->RemoteDescriptor = *(ccPtr->getRemoteConnection());

        //    cout << "remote: " << *RemoteDescriptor << "\n";

        // send a reply with the correct SDP information

        SDPDescriptor local;

        local.setSessionName("test session");
        local.setUserName("testID");
        string host;
        host = getHost();
        local.setAddress(host);
        local.setSessionId(128538);
        local.setVersion(1000);
        local.setPort(6050);
        //    local.addFormat(0);

        //    cout << *(local.encode()) << "\n";

        LocalConnectionDescriptor msg9_rd(local);

        state->localDescriptor = msg9_rd;

        state->setState(stateHalfOpen, NULL);

        MgcpResponse response01(200, ccPtr->getTransactionId(), "OK");
        response01.insert(&msg9_rd);
        response01.send(*(state->callAgent));
        //    free(data);
        //    data = NULL;
    }

}

void stateHalfOpenIn(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusNewMessage)
    {
        MgcpCommand* test_item = event->getMessage();
        CreateConnection* ccPtr;

        ccPtr = dynamic_cast < CreateConnection* > (test_item);

        if (ccPtr == NULL)
            wrongMessage();

        state->callID = (*(ccPtr->getCallId()));
        state->callMode = (*(ccPtr->getConnectionMode()));

        if (state->callMode.getMode() != ConnectionSendRecv)
            wrongMessage();

        state->RemoteDescriptor = *(ccPtr->getRemoteConnection());

        SDPDescriptor local;

        local.setSessionName("test session");
        local.setUserName("testID");
        string host;
        host = getHost();
        local.setAddress(host);
        local.setSessionId(128538);
        local.setVersion(1000);

        local.setPort(6060 + (rand() % 1024));

        LocalConnectionDescriptor msg9_rd(local);

        MgcpResponse response01(200, ccPtr->getTransactionId(), "OK");
        response01.insert(&msg9_rd);
        response01.send(*(state->callAgent));

        state->hardware->audioStart(
            local.getPort(),
            state->RemoteDescriptor.getDescriptor()->getAddress()->c_str(),
            state->RemoteDescriptor.getDescriptor()->getPort()
        );

        state->setState(stateActiveNoNotifications, NULL);
    }
}
void stateHalfOpen(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusNewMessage)
    {
        MgcpCommand* test_item = event->getMessage();

        // state 7 -- receive create connection

#ifdef DEBUG
        cout << "\t\tdata: " << (data->getMessageLocation()) << "\n";
        cout << "\t\tparsed: " << (*test_item) << "\n";
#endif

        ModifyConnection* mdPtr;

        mdPtr = dynamic_cast < ModifyConnection* > (test_item);

        if (mdPtr == NULL)
            wrongMessage();

        CallId newCallID(*(mdPtr->getCallId()));

        if (newCallID != state->callID)
            wrongMessage();

        ConnectionMode newCallMode(*(mdPtr->getConnectionMode()));

        if (newCallMode.getMode() != ConnectionSendRecv)
            wrongMessage();


        state->RemoteDescriptor = *(mdPtr->getRemoteConnection());


        // this needs to be fixed
        state->hardware->audioStart(
            state->localDescriptor.getDescriptor()->getPort(),
            state->RemoteDescriptor.getDescriptor()->getAddress()->c_str(),
            state->RemoteDescriptor.getDescriptor()->getPort()
        );


        MgcpResponse response02(200, mdPtr->getTransactionId(), "OK");
        response02.send(*(state->callAgent));

        state->setState(stateActiveNoNotifications, NULL);

    }
}

void stateActiveNoNotifications(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusNewMessage)
    {
        MgcpCommand* test_item = event->getMessage();

        NotificationRequest* nrPtr;

        nrPtr = dynamic_cast < NotificationRequest* > (test_item);

        if (nrPtr == NULL)
            wrongMessage();

        // check for the right parameters

        state->CurrentRequestId = (*(nrPtr->getRequestId()));

        vector < Event* > * eventList((nrPtr->getRequestedEvents())->getVector());

        if (!eventList)
            wrongMessage();

        // get the appropriate events --

        vector < Event* > ::iterator eventIter;

        eventIter = eventList->begin();

        while (eventIter != eventList->end())
        {
            if (dynamic_cast < LineEventOnHook* > (*eventIter) == NULL)
                wrongMessage();  // only accept this event sir

            ++eventIter;
        }

        // reply OK.


        MgcpResponse response00(200, nrPtr->getTransactionId(), "OK");
        response00.send(*(state->callAgent));

        state->setState(stateActive, NULL);
    }
    //    else if(event->getStimulusType() == StimulusNewMessage)

    //    cout << "remote: " << *RemoteDescriptor << "\n";
}


void stateActive(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusNewMessage)
    {
        MgcpCommand* test_item = event->getMessage();

        DeleteConnection* p;

        p = dynamic_cast < DeleteConnection* > (test_item);

        if (p == NULL)
            wrongMessage();

        state->callID = (*(p->getCallId()));
        ConnectionId connectId(*(p->getConnectionId()));

        // ack this message

        MgcpResponse response00(200, p->getTransactionId(), "OK");
        response00.send(*(state->callAgent));
        state->setState(stateWaitOnHook, NULL);

        // close the audio channel

        state->hardware->sendSignal(SignalAudioStop);
    }
    else if (event->getStimulusType() == StimulusOnHook)
    {
        Notify msg8(set);
        ObservedEvents events8;
        LineEventOnHook onHook;

        events8.insert(onHook);

        msg8.insert(&events8);
        msg8.insert(&(state->CurrentRequestId));
        msg8.send(*(state->callAgent));

        state->setState(waitForResponse, stateWaitForClose);
    }

}


void stateWaitForClose(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusNewMessage)
    {
        MgcpCommand* test_item = event->getMessage();

        DeleteConnection* p;

        p = dynamic_cast < DeleteConnection* > (test_item);

        if (p == NULL)
            wrongMessage();

        state->callID = (*(p->getCallId()));
        ConnectionId connectId(*(p->getConnectionId()));

        // ack this message

        MgcpResponse response00(200, p->getTransactionId(), "OK");
        response00.send(*(state->callAgent));
        state->setState(stateUninitialized, NULL);

        // close the audio channel

        state->hardware->sendSignal(SignalAudioStop);
    }
}


void stateWaitOnHook(StateEvent* event, StateMachine* state)
{
    if (event->getStimulusType() == StimulusOnHook)
    {
        Notify msg8(set);
        ObservedEvents events8;
        LineEventOnHook onHook;

        events8.insert(onHook);

        msg8.insert(&events8);
        msg8.insert(&(state->CurrentRequestId));
        msg8.send(*(state->callAgent));

        state->setState(waitForResponse, stateUninitialized);
    }
}


void EventWatcher(StateEvent* event, StateMachine* state)
{

    // this watches for offhook/onhook events and handles them as
    // appropriate due to the bit flag that has been set.

    if (state->activeEventList[event->getStimulus()])
    {
        // now you need to send a copy of this one
        Notify msg8(set);
        ObservedEvents events8;

        events8.insert(*(state->activeEventList[event->getStimulusType()]));

        msg8.insert(&events8);
        msg8.insert(&state->CurrentRequestId);
        msg8.send(*(state->callAgent));
    }
    // now you need to go on to the next state, or just wait here...
}





int main(int argc, char* argv[])
{
    char* agent_host = "localhost";
    string device("null");
    StateMachine state;
    int port = 5050;

    if (argc != 4)
    {
        // this is an error

        cerr << "usage: gw1 device agentHostname portnumber\n";

        cerr << "defaulting to null / localhost\n";
    }
    else
    {
        device = argv[1];
        agent_host = argv[2];
        port = atoi(argv[3]);
    }


    if (device == "tpjack")
    {
        state.hardware = new TpjackHardwareObject("/dev/ixj0");
    }
    else
    {
        state.hardware = new NullHardwareObject("/dev/ixj0");
    }

    MgcpTransmitter callAgent(agent_host, 5010);

    UdpReceiver server(port);   // hardcoded...
    UdpPacket* data = 0;

    state.callAgent = &callAgent;
    state.server = &server;
    state.setState(stateUninitialized, NULL);

    MgcpCommand* test_item;


    fd_set read_fds;

    struct timeval time_val;

    srand(time(NULL));

    for (; ; )
    {
        FD_ZERO(&read_fds);
        state.hardware->addToFdSet(&read_fds);
        state.server->addToFdSet(&read_fds);

        time_val.tv_sec = 0;
        time_val.tv_usec = 300;

        // instead of data, it should be stateevent

        int retval = select(128, &read_fds, NULL, NULL, &time_val);

        if (state.hardware->process(&read_fds))
        {
            //	    hardware.processData(&read_fds);
            // incoming/outgoing hardware interface
            HardwareEvent hwEvent;
            while ((hwEvent = state.hardware->getEvent()) != EventNULL)
            {

                StateEvent event;

                switch (hwEvent)
                {
                    case EventOffHook:
                    event.setStimulus(StimulusOffHook);
                    break;
                    case EventOnHook:
                    event.setStimulus(StimulusOnHook);
                    break;
                    case EventAudioFailed:
                    case EventAudioClosed:
                    default:
                    break;
                };

                state.currentState(&event, &state);
            }
        }

        if (state.server->checkIfSet(&read_fds))
        {
            // incoming control packet
            data = state.server->receive();
            MgcpCommand* test_item =
                parseMessage(data->getMessageLocation(), 0);

            StateEvent event;

            event.setMessage(test_item);

            state.currentState(&event, &state);

        }

        if (0)
        {
            // incoming audio

        }


    }

    return 0;
}
