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

static const char* const SimpleSendingExample_cxx_Version =
    "$Id: SimpleSendingExample.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "VovidaSipStack.hxx"
#include "InviteMsg.hxx"
#include "SipSdp.hxx"
#include "SipSubject.hxx"

using namespace Vocal;
using namespace Vocal::SDP;


int main(int argc, char** argv)
{
    // here is the SIP stack for this message.  the argument is the
    // SIP port for this program.

    Sptr<SipTransceiverFilter> sipStack = new SipTransceiverFilter(argv[0], 5060);

    // construct a SIP URL for the message.
    Sptr<SipUrl> toUrl = new SipUrl(Data("sip:user@domain.com"));

    // construct a simple SIP INVITE .
    InviteMsg msg( toUrl );

    // here is an example of setting a field on a SIP message

    SipSubject subject;
    subject.set("New call");
    msg.setSubject(subject);

    // here is a more complex example.  In this example, we will set
    // the Display Name and From URL in the From: line of this message.

    SipFrom from = msg.getFrom();
    // now, set the display name
    from.setDisplayName( "Bob Smith" );

    // now, create and set the From URL

    Sptr<SipUrl> fromUrl = new SipUrl(Data("sip:bob@example.com"));
    from.setUrl( fromUrl );

    // now, you must set the from in the message to the contents of from
    msg.setFrom(from);

    // in this example, we will set the SDP body of this INVITE .

    // sdp will point to the SDP body in the message.  if we change
    // sdp, we change the SDP contents of the body.

    Sptr<SipSdp> sdp;
    sdp.dynamicCast( msg.getContentData(0) );

    // when you construct an INVITE by passing a URL, the INVITE
    // constructed should contain an SDP body already.  we have gotten
    // it in sdp -- if it is not there, or it is not an SDP body, this
    // assert would fail, but this should never happen.

    assert (sdp != 0) ;

    // set the RTP port to 6000 .
    sdp->setRtpPort(6500);

    // now, we will send the INVITE message

    sipStack->sendAsync(msg);
    sipStack = 0;
    return 0;
}
