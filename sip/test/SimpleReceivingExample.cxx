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

static const char* const SimpleReceivingExample_cxx_Version =
    "$Id: SimpleReceivingExample.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "VovidaSipStack.hxx"
#include "InviteMsg.hxx"
#include "SipSdp.hxx"
#include <iostream>

using namespace Vocal;
using namespace Vocal::SDP;


int main(int argc, char** argv)
{
    // here is the SIP stack for this message.  the argument is the
    // SIP port for this program.

    Sptr<SipTransceiverFilter> sipStack = new SipTransceiverFilter(argv[0], 5060);


    // get the message -- this comes in a queue of messages
    Sptr<SipMsgQueue> queue = sipStack->receive();

    Sptr<SipMsg> msg = queue->back();

    // Convert the object to one of class SipInvite, so that we can
    // access the headers for it particularly

    Sptr<InviteMsg> invite;
    invite.dynamicCast(msg);

    if(invite != 0)
    {
	// if invite != 0 (e.g. if invite is pointing to a message)
	// then it is a SIP INVITE.  Examine the From: field

	SipFrom from = invite->getFrom();

	// print the Display Name of the From:
	cout << "Display Name: " << from.getDisplayName() << endl;

	// now, get the From: URL .

	Sptr<BaseUrl> url = from.getUrl();

	// verify that this URL is a SIP URL

	if(url->getType() == SIP_URL)
	{
	    // convert to a Sptr<SipUrl> so we can access SIP-specific fields
	    Sptr<SipUrl> sipUrl;
	    sipUrl.dynamicCast(url);
	    // this assertion should always be true
	    assert(sipUrl != 0);

	    cout << "Domain name: " << sipUrl->getHost() << endl;
	}

	// now, retrieve the RTP port from the SDP body of the message

	Sptr<SipSdp> sdp;
	sdp.dynamicCast(invite->getContentData(0));
	if(sdp != 0)
	{
	    int port = sdp->getRtpPort();
	    cout << "port: " << port << endl;
	}

	// now, construct a 200 OK response .  the first argument here
	// is the message to respond to, the second is the response type.

	StatusMsg status(*invite, 200);

	// you can (and probably should) add an SDP body containing
	// the information for your RTP streams here.  However, we
	// will send the reply without filling out the status message

	sipStack->sendReply(status);
    }
    

    sipStack = 0;
    return 0;
}
