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

// $Id: agent.cxx,v 1.1 2004/05/01 04:15:31 greear Exp $


#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "mgc.hxx"

#if 0
char* getHost()
{
    static char buf[256];

    gethostname(buf, 256);

    return buf;
}

void waitForAck(UdpReceiver& receiver)
{
    UdpPacket* data = 0;
    MgcpCommand* test_item;

    bool gotResponse(false);

    while (!gotResponse)
    {
        data = receiver.receive();

        test_item = parseMessage(data->getMessageLocation(), 0);

        if (dynamic_cast < MgcpResponse* > (test_item) != NULL)
        {
            // this is not a proper response
            gotResponse = true;
        }
        free (test_item);
        free (data);
    }
}

#endif

int main()
{
#if 0
    UdpReceiver server(5010);
    CreateConnection test;
#endif

    MgcEndpointConfiguration config ("testID", MgcpBearerInformation());

#if 0
    EndpointId id("testID");
    BearerInformation bearer;
    Entity myself("callagent@foo.bar.com");
    EndpointConfiguration msg0(id, bearer);
    EndpointConfiguration msg1(id, bearer);
    EndpointConfiguration msg2(id, bearer);
    EndpointConfiguration msg3(id, bearer);
    EndpointConfiguration msg4(id, bearer);
    EndpointConfiguration msg5(id, bearer);
#endif

    MgcNotificationRequest request ("testID", "215781728");

    request.addRequestedEvents(MgcRequestEvent());
    request.addRequestedEvents(MgcRequestEvent());
    request.addRequestedEvents(MgcRequestEvent());
    request.addRequestedEvents(MgcRequestEvent());
    request.addRequestedEvents(MgcRequestEvent());

#if 0
    NotificationRequest msg6(id);

    LineEventOffHook offHook;
    LineEventOnHook onHook;
    LineEventHookFlash hookFlash;

    RequestedEvents events6;

    DigitMap temp("[0-9]");

    events6.insert(offHook);


    SignalRequests signals_6;

    SignalRingback ringback;

    SignalRing ring;

    signals_6.insert(ring);
    signals_6.insert(ringback);



    msg6.insert(&myself);
    msg6.insert(&temp);
    msg6.insert(&events6);
    msg6.insert(&signals_6);

    cout << msg6;

    NotificationRequest msg7(id);
    msg7.insert(&myself);
    msg7.insert(&temp);
    RequestedEvents events7;

    events7.insert(offHook);
    events7.insert(onHook);
    events7.insert(hookFlash);
    msg7.insert(&events7);

    // message 8

    Notify msg8(id);
    msg8.insert(&myself);
    //    msg8.insert(&temp);
    ObservedEvents events8;

    events8.insert(offHook);
    events8.insert(onHook);
    events8.insert(hookFlash);
    msg8.insert(&events8);



    //    cerr << events;

    msg6.printData(cout);

    MgcpTransmitter gateway("localhost");

    EndpointConfiguration* test2;

    test2 = new EndpointConfiguration;
    delete (test2);


    msg0.send(gateway);
    waitForAck(server);
    msg1.send(gateway);
    waitForAck(server);
    msg1.send(gateway);
    waitForAck(server);
    msg1.send(gateway);
    waitForAck(server);
    msg1.send(gateway);
    waitForAck(server);
    msg1.send(gateway);
    waitForAck(server);
    msg2.send(gateway);
    waitForAck(server);
    msg3.send(gateway);
    waitForAck(server);
    msg4.send(gateway);
    waitForAck(server);
    msg5.send(gateway);
    waitForAck(server);
    msg1.send(gateway);
    waitForAck(server);
    msg6.send(gateway);
    waitForAck(server);
    msg7.send(gateway);
    waitForAck(server);
    msg8.send(gateway);
    waitForAck(server);


    CreateConnection msg9;
    CallId msg9_id("0123456789ABCDEF");
    ConnectionId msg9_connection("FEDCBA9876543210");
    //    Entity msg9_entity("foo@bar.com");
    //    Options msg9_options;

    cout << "callid: " << msg9_id << "\n";

    cout << "connectionid: " << msg9_connection << "\n";

    ConnectionMode msg9_mode(ConnectionSendRecv);
    RemoteConnectionDescriptor msg9_rcd;

    msg9.insert(&msg9_id);
    msg9.insert(&msg9_connection);
    msg9.insert(&msg9_mode);
    msg9.insert(&myself);
    msg9.insert(&id);

    SDPDescriptor foo;

    foo.setSessionName("test session");
    foo.setUserName("bogawa");
    foo.setAddress(getHost());
    foo.setSessionId(128538);
    foo.setVersion(1000);
    foo.setPort(6502);
    foo.addFormat(1);

    RemoteConnectionDescriptor msg9_rd(foo);

    msg9.insert(&msg9_rd);

    cout << msg9 << "\n";

    msg9.send(gateway);
    waitForAck(server);


    ModifyConnection msg10;

    msg10.insert(&msg9_id);
    msg10.insert(&msg9_connection);
    msg10.insert(&msg9_mode);
    msg10.insert(&myself);
    msg10.insert(&id);

    cout << msg10 << "\n";

    msg10.send(gateway);
    waitForAck(server);

    cout << "--------------------- local test ------------------\n";

    SDPDescriptor bar;

    bar.setSessionName("test session");
    bar.setUserName("bogawa");
    bar.setAddress(getHost());
    bar.setSessionId(128538);
    bar.setVersion(1000);
    bar.setPort(6502);
    bar.addFormat(1);

    LocalConnectionDescriptor local(bar);

    cout << "local: " << local << "\n";

    MgcpResponse response00(200, 500, "OK");

    response00.insert(&events8);
    response00.insert(&local);

    cout << response00 << "\n";
    response00.send(gateway);


    DeleteConnection delconn;

#if 0
    struct rusage usage;

    getrusage (RUSAGE_SELF, &usage);
    cout << "ru_maxrss: " << usage.ru_maxrss << "\n";
    cout << "ru_ixrss: " << usage.ru_ixrss << "\n";
    cout << "ru_idrss: " << usage.ru_idrss << "\n";
    cout << "ru_isrss: " << usage.ru_isrss << "\n";
#endif

#endif 
    return 0;
}
