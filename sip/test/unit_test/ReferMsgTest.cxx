/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2001 Vovida Networks, Inc.  All rights reserved.
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

static const char* const ReferMsgTest_cxx_Version =
    "$Id: ReferMsgTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "cpLog.h"
#include "Verify.hxx"
#include "ReferMsg.hxx"
#include "InviteMsg.hxx"
#include "SipReferredBy.hxx"
#include "SipVia.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;


int
testRequestEncode()
{
    string host_addr = theSystem.gethostAddress();

    int rc = 0;

    InviteMsg inviteMsg(
"INVITE sip:b@agentb.agentland SIP/2.0\r\nVia: SIP/2.0/UDP agenta.agentland\r\nTo: <sip:b@agentland>;tag=193402342\r\nFrom: <sip:a@agentland>;tag=4992881234\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809823 INVITE\r\nContact: sip:a@agenta.agentland\r\nContent-Length: 0\r\n\r\n"
                       );

    Sptr< SipUrl > referToUri = new SipUrl("", host_addr);
    referToUri->setUserValue( "whatever" );
    referToUri->setHost( "wherever" );

    // Create a REFER request with an INVITE message
    ReferMsg request1( inviteMsg, referToUri );

    Sptr< SipUrl > myUri = new SipUrl("", host_addr);
    myUri->setUserValue( "b" );
    myUri->setHost( "agentb.agentland" );

    SipReferredBy referredBy("", host_addr);
    referredBy.setReferrerUrl( myUri );
    request1.setReferredBy( referredBy );

    SipContact contact("", host_addr);
    contact.setUrl( myUri );
    request1.setContact( contact );

    // Over-write Via
    request1.flushViaList();
    SipVia sipVia("", host_addr);
    sipVia.setprotoVersion( "2.0" );
    sipVia.setHost( myUri->getHost() );
    sipVia.setTransport( "UDP" );
    request1.setVia( sipVia, 0 );


    cpLog( LOG_DEBUG_STACK, "Refer Message:\n%s", request1.encode().logData() );

    Data rawMsg1(
"REFER sip:a@agenta.agentland SIP/2.0\r\nVia: SIP/2.0/UDP agentb.agentland:5060\r\nTo: <sip:a@agentland>;tag=4992881234\r\nFrom: <sip:b@agentland>;tag=193402342\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809824 REFER\r\nRefer-To: <sip:whatever@wherever>\r\nReferred-By: <sip:b@agentb.agentland>\r\nRequire: cc Refer\r\nContact: <sip:b@agentb.agentland>\r\nContent-Length: 0\r\n\r\n"
                );
    cpLog( LOG_DEBUG_STACK, "Raw Message:\n%s", rawMsg1.logData() );

    test_verify( request1.encode() == rawMsg1 );
    rc++;

#if 0

    // Create a REFER request with a status message

    StatusMsg statusMsg(
"SIP/2.0 202 Accepted\r\nVia: SIP/2.0/UDP agenta.agentland\r\nTo: <sip:b@agentland>;tag=4992881234\r\nFrom: <sip:a@agentland>;tag=193402342\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809824 REFER\r\nContact: <sip:b@agentland>\r\nContent-Length: 0\r\n\r\n"
                  );

    test_verify( request2.encode() == rawMsg2 );
    rc++;
#endif

    return rc;
}


int
testRequestDecode()
{
    string host_addr = theSystem.gethostAddress();

    int rc = 0;

    Data rawMsg1(
// Example Message One (F1) from draft-ietf-sip-refer-02.txt
// Slightly modified for automated testing
// - add 5060 in request uri
	"REFER sip:b@agentland:5060 SIP/2.0\r\nVia: SIP/2.0/UDP agenta.agentland\r\nTo: <sip:b@agentland>\r\nFrom: <sip:a@agentland>;tag=193402342\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809823 REFER\r\nRefer-To: (whatever URL)\r\nReferred-By: <sip:a@agentland>\r\nContact: sip:a@agentland\r\nContent-Length: 0\r\n\r\n"
	);
    cpLog( LOG_DEBUG_STACK, "Raw Message:\n%s", rawMsg1.logData() );

    ReferMsg request1( rawMsg1, host_addr );
    cpLog( LOG_DEBUG_STACK, "Refer Message:\n%s", request1.encode().logData() );

    test_verify( rawMsg1 == request1.encode() );
    rc++;

    Data rawMsg2(
// Example Message Five (F5) from draft-ietf-sip-refer-02.txt
// Slightly modified for automated testing
// - add 5060 in request uri
	"REFER sip:b@agentland:5060 SIP/2.0\r\nVia: SIP/2.0/UDP agenta.agentland\r\nTo: <sip:b@agentland>;tag=4992881234\r\nFrom: <sip:a@agentland>;tag=193402342\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809824 REFER\r\nRefer-To: (some different URL)\r\nReferred-By: <sip:a@agentland>\r\nContact: sip:a@agentland\r\nContent-Length: 0\r\n\r\n" 
	);
    cpLog( LOG_DEBUG_STACK, "Raw Message:\n%s", rawMsg2.logData() );

    ReferMsg request2( rawMsg2, host_addr );
    cpLog( LOG_DEBUG_STACK, "Refer Message:\n%s", request2.encode().logData() );

    test_verify( rawMsg2 == request2.encode() );
    rc++;

    return rc;
}


int
testResponseEncode()
{
    string host_addr = theSystem.gethostAddress();
    int rc = 0;

    Data rawMsg1(
// Example Message Five (F5) from draft-ietf-sip-refer-02.txt
// Slightly modified for automated testing
// - add 5060 in request uri
	"REFER sip:b@agentland:5060 SIP/2.0\r\nVia: SIP/2.0/UDP agenta.agentland\r\nTo: <sip:b@agentland>;tag=4992881234\r\nFrom: <sip:a@agentland>;tag=193402342\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809824 REFER\r\nRefer-To: (some different URL)\r\nReferred-By: <sip:a@agentland>\r\nContact: sip:a@agentland\r\nContent-Length: 0\r\n\r\n"
	);
    ReferMsg request( rawMsg1, 	host_addr );

    // Create a 202 Accepted response with a REFER request
    StatusMsg response( request, 202 /* Accepted */ );
    Sptr< SipUrl > uri = new SipUrl("", host_addr);
    uri->setUserValue( "b" );
    uri->setHost( "agentland" );
    SipContact contact("", host_addr);
    contact.setUrl( uri );
    response.setContact( contact );
    cpLog( LOG_DEBUG_STACK, "202 Response:\n%s", response.encode().logData() );

    Data rawMsg2(
// Example Message Six (F6) from draft-ietf-sip-refer-02.txt
// Slightly modified for automated testing
// - add '<' '>' around Contact URI
"SIP/2.0 202 Accepted\r\nVia: SIP/2.0/UDP agenta.agentland\r\nTo: <sip:b@agentland>;tag=4992881234\r\nFrom: <sip:a@agentland>;tag=193402342\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809824 REFER\r\nContact: <sip:b@agentland>\r\nContent-Length: 0\r\n\r\n"
                );
    cpLog( LOG_DEBUG_STACK, "Raw Message:\n%s", rawMsg2.logData() );

    test_verify( response.encode() == rawMsg2 );
    rc++;

    return rc;
}


int
testResponseDecode()
{
    string host_addr = theSystem.gethostAddress();
    int rc = 0;

    Data rawMsg1(
// Example Message Two (F2) from draft-ietf-sip-refer-02.txt
"SIP/2.0 202 Accepted\r\nVia: SIP/2.0/UDP agenta.agentland\r\nTo: <sip:b@agentland>;tag=4992881234\r\nFrom: <sip:a@agentland>;tag=193402342\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809823 REFER\r\nContact: sip:b@agentland\r\nContent-Length: 0\r\n\r\n"
                );
    cpLog( LOG_DEBUG_STACK, "Raw Message:\n%s", rawMsg1.logData() );

    StatusMsg response1( rawMsg1, host_addr );
    cpLog( LOG_DEBUG_STACK, "202 Response:\n%s", response1.encode().logData() );

    test_verify( rawMsg1 == response1.encode() );
    rc++;

    Data rawMsg2(
// Example Message Six (F6) from draft-ietf-sip-refer-02.txt
"SIP/2.0 202 Accepted\r\nVia: SIP/2.0/UDP agenta.agentland\r\nTo: <sip:b@agentland>;tag=4992881234\r\nFrom: <sip:a@agentland>;tag=193402342\r\nCall-ID: 898234234@agenta.agentland\r\nCSeq: 93809824 REFER\r\nContact: sip:b@agentland\r\nContent-Length: 0\r\n\r\n"
                );
    cpLog( LOG_DEBUG_STACK, "Raw Message:\n%s", rawMsg2.logData() );

    StatusMsg response2( rawMsg2, host_addr );
    cpLog( LOG_DEBUG_STACK, "202 Response:\n%s", response1.encode().logData() );

    test_verify( rawMsg2 == response2.encode() );
    rc++;

    return rc;
}


int
main()
{
    cpLogSetPriority( LOG_DEBUG_STACK );

    return test_return_code( testRequestEncode() +
                             testRequestDecode() +
                             testResponseEncode() +
                             testResponseDecode() );
}
