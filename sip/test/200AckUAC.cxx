/*
 * VOCAL is licensed under the Vovida Software License, Version 1.0
 * See the LICENSE AND COPYRIGHT section at the end of this file.
 */

static const char* const Ack200UAS_cxx_Version =
    "$Id: 200AckUAC.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "SipTransceiverFilter.hxx"
#include "InviteMsg.hxx"
#include "AckMsg.hxx"
#include "StatusMsg.hxx"
#include "SipSdp.hxx"
#include "Sptr.hxx"
#include "SipVia.hxx"

using namespace Vocal;
using namespace Vocal::SDP;

/*
 *  This is the UAC side of a INVITE-200-ACK test.
 *  It sends an INVITE and then receives a 200 OK response.
 *  When it receives the 200 OK, it sends an ACK.
 *
 *  The UAS side is in 200AckUAS.cxx.
 */

int
main( int argc, char** argv )
{
    if( argc <= 2 )
    {
        cerr << endl << "Error: Missing arguments" << endl;
        cerr << "Usage: " << argv[0] << " <UAC_SIP_port> <UAS_SIP_port>"
             << endl << endl;
        return -1;
    }
    cpLogSetPriority( LOG_DEBUG_STACK );
    // cpLogSetPriority( LOG_NOTICE );

    int sipPort = strtol( argv[1], NULL, 10 );

    SipTransceiverFilter sipStack( argv[0], sipPort );

    Sptr <BaseUrl> toUrl;
    toUrl = BaseUrl::decode( Data("sip:UAS@127.0.0.1:") + Data(argv[2]) );

    int rtpPort = 23450;

    InviteMsg invite( toUrl, sipPort, rtpPort );

    Data branch = invite.computeBranch();
    cout << "INVITE Via branch: " << branch << endl;

    sipStack.sendAsync( invite );

    cout << "Send INVITE" << endl;

    Sptr < SipMsgQueue > mq = sipStack.receive();
    assert( mq != 0 );

    Sptr < StatusMsg > inviteOK;
    inviteOK.dynamicCast( mq->back() );

    if( inviteOK == 0 )
    {
        cerr << "Not a Status Response!" << endl;
        return -1;
    }

    int statusCode = inviteOK->getStatusLine().getStatusCode();

    if( statusCode != 200 )
    {
        cerr << "Not a 200 OK!" << endl;
        return -1;
    }

    cout << "Received 200 OK" << endl;

    // Send ACK
    AckMsg ack( *inviteOK );

    // Change branch in Via: header to make it a different transcation
    branch = ack.computeBranch();
    cout << "ACK Via branch: " << branch << endl;

    SipVia via = ack.getVia();
    via.setBranch( branch );
    ack.flushViaList();
    ack.setVia( via, 0 );

    sipStack.sendAsync( ack );

    cout << "Send ACK" << endl;

    cout << "Test Complete, sleep for 5 seconds" << endl;
    sleep( 5 );

    return 0;
}

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2002 Vovida Networks, Inc.  All rights reserved.
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
