/*
 * VOCAL is licensed under the Vovida Software License, Version 1.0
 * See the LICENSE AND COPYRIGHT section at the end of this file.
 */

static const char* const Ack200UAS_cxx_Version =
    "$Id: 200AckUAS.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipTransceiverFilter.hxx"
#include "InviteMsg.hxx"
#include "AckMsg.hxx"
#include "StatusMsg.hxx"
#include "SipSdp.hxx"
#include "Sptr.hxx"

using namespace Vocal;
using namespace Vocal::SDP;

/*
 *  This is the UAS side of a INVITE-200-ACK test.
 *  It receives an INVITE and then sends a 200 OK response.
 *  When it receives the ACK, it stops the retransmission
 *  of 200.
 *
 *  The UAC side is in 200AckUAC.cxx.
 */

int
main( int argc, char** argv )
{
    if( argc <= 1 )
    {
        cerr << endl << "Error: Missing local SIP port argument" << endl;
        cerr << "Usage: " << argv[0] << " <UAS_SIP_port>" << endl << endl;
        return -1;
    }
    cpLogSetPriority( LOG_DEBUG_STACK );
    // cpLogSetPriority( LOG_NOTICE );

    SipTransceiverFilter sipStack( argv[0], strtol( argv[1], NULL, 10 ) );

    Sptr < SipMsgQueue > mq = sipStack.receive();
    assert( mq != 0 );

    Sptr < InviteMsg > invite;
    invite.dynamicCast( mq->back() );

    if (invite == 0)
    {
        cerr << "Not an INVITE!" << endl;
        return -1;
    }

    cout << "Received INVITE" << endl;

    // Respond with the 200
    StatusMsg inviteOk( *invite, 200 );

    inviteOk.setNumContact( 0 );   // Clear UAC contact

    Sptr <SipUrl> myUrl = new SipUrl();
    myUrl->setUserValue( Data("UAS") );
    myUrl->setHost( Data("127.0.0.1") );
    myUrl->setPort( argv[1] );

    SipContact me;
    me.setUrl( myUrl );
    inviteOk.setContact( me );

    Sptr<SipContentData> contData = invite->getContentData( 0 );
    Sptr<SipSdp> remoteSdp;
    remoteSdp.dynamicCast( contData );
    if( remoteSdp != 0 )
    {
        cout << "Change RTP port" << endl;
        SipSdp localSdp;
        localSdp.setSdpDescriptor( remoteSdp->getSdpDescriptor() );
        localSdp.setRtpPort( 23456 );
        inviteOk.setContentData( &localSdp, 0 );
    }

    cout << "Send 200" << endl;
    sipStack.sendReply( inviteOk );

    mq = sipStack.receive();

    assert( mq != 0 );

    if( mq->back()->getType() != SIP_ACK )
    {
        cerr << "Not an ACK!" << endl;
        return -1;
    }

    cout << "Received ACK" << endl;

    // TODO: Stop retransmission of 200

    cout << "Test Completed, sleep for 5 seconds" << endl;
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
