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

static const char* const SipTransactionIdTest_cxx_Version =
    "$Id: SipTransactionIdTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipTransactionId.hxx"
#include <cassert>
#include "SipMsg.hxx"
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;



char* msgtext[] = 
{ 
  "ACK sip:7013000@172.19.175.70:5060;user=phone SIP/2.0\r
Via: SIP/2.0/UDP 172.19.175.71:5060;branch=b000f019d04ffcee7b4a80ba90009df3.1\r
From: \"7010300\"<sip:7010300@172.19.175.226:5060>\r
To: <sip:7013000@172.19.175.71:5060;user=phone>;tag=9cacee78\r
Call-ID: 5CF43908-1A820AE0-0-5163F880@172.19.175.226\r
CSeq: 101 ACK\r
",

  "SIP/2.0 302 Moved Temporarily\r
Via: SIP/2.0/UDP 172.19.175.71:5060;branch=b000f019d04ffcee7b4a80ba90009df3.1\r
Via: SIP/2.0/UDP 172.19.175.226:54796\r
From: \"7010300\"<sip:7010300@172.19.175.226:5060>\r
To: <sip:7013000@172.19.175.71:5060;user=phone>;tag=9cacee78\r
Call-ID: 5CF43908-1A820AE0-0-5163F880@172.19.175.226\r
CSeq: 101 INVITE\r
Contact: <sip:7013000@172.19.175.72:5060;user=phone>\r
",
  0
};
//"

void testEncodeDecode()
{
  SipMsg* status;
  SipMsg* ack;
  
  ack = SipMsg::decode(msgtext[0], host_addr);
  status = SipMsg::decode(msgtext[1], host_addr);

  SipTransactionId ackId(*ack);
  SipTransactionId statusId(*status);

  test_verify(!(statusId < ackId));
  test_verify((ackId < statusId));

  Data lLeft(statusId.getLevel1());
  lLeft += statusId.getLevel2();

  Data rRight(ackId.getLevel1());
  rRight += ackId.getLevel2();
  
  test_verify(lLeft == rRight);
}


int main()
{
    testEncodeDecode();

    return test_return_code(3);
}
