
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

static const char* const FullCallSend_cxx_Version =
    "$Id: FullCallSend.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "SipTransceiverFilter.hxx"
#include "InviteMsg.hxx"
#include "ByeMsg.hxx"
#include "AckMsg.hxx"
#include "StatusMsg.hxx"
#include "SipVia.hxx"

using namespace Vocal;

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        cpLogSetPriority(LOG_DEBUG_STACK);
    }
    SipTransceiverFilter trans;

    Sptr <BaseUrl> tourl;
    tourl = BaseUrl::decode(Data("sip:destination@127.0.0.1:5070"));

    int listenPort = 5060;
    int rtpPort = 3456;
    cpLog(LOG_DEBUG, "Completed the Url decoding");

    InviteMsg invite(tourl, listenPort, rtpPort);
    trans.sendAsync(invite);

    Sptr < SipMsgQueue > m;
    Sptr < StatusMsg > status;
    int statusCode = 0;
    while (m == 0)
    {
        m = trans.receive();

        status.dynamicCast(m->back());

        if (status != 0)
        {
            statusCode = status->getStatusLine().getStatusCode();
            cout << "got reply to invite: " << statusCode << endl;
            if (statusCode >= 200)
                break;
            m = 0;
        }
    }
    if (statusCode != 200)
    {
        // this is an error, exit
        cout << "call failed!" << endl;
        return -1;
    }

    // send my ack
    AckMsg ack(*status);
    trans.sendAsync(ack);

    // the call should now be up
    sleep(1);

    ByeMsg bye(*status);
    cout << "sending bye!" << endl;


    SipVia sipVia;
    sipVia.setprotoVersion( "2.0" );
    sipVia.setHost( Data( "127.0.0.1" ) );
    sipVia.setPort( 5060 );
    sipVia.setTransport( Data("UDP") );

    bye.flushViaList();
    bye.setVia( sipVia, 0 );


    trans.sendAsync(bye);

    m = trans.receive();
    status.dynamicCast(m->back());

    if (status != 0)
    {
        statusCode = status->getStatusLine().getStatusCode();
        cout << "got reply to invite: " << statusCode << endl;
        if (statusCode != 200)
        {
            cout << "bye failed!" << endl;
            return -1;
        }
    }
    else
    {
        cout << "nothing for bye!" << endl;
        return -1;
    }
    cout << "call succeeded!" << endl;

    return 0;
}
