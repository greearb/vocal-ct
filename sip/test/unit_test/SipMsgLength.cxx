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

static const char* const SipMsgLength_cxx_Version =
    "$Id: SipMsgLength.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "global.h"
#include <stdio.h>
#include <string>
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


int main()
{
    char* msg_invite = "INVITE sip:vivekg@chair.dnrc.bell-labs.com SIP/2.0
                       TO :
                       sip:vivekg@chair.dnrc.bell-labs.com ;   tag    = 1a1b1f1H33n
                       From   : \"J Rosenberg \\\" <sip:jdrosen@lucent.com>
                       ;
                       tag = 98asjd8
                       CaLl-Id
                       : 0ha0isndaksdj@10.1.1.1
                       cseq: 8
                       INVITE
                       Via  : SIP  /   2.0
                       /UDP
                       135.180.130.133
                       Subject :
                       NewFangledHeader:   newfangled value
                       more newfangled value
                       Content-Type: application/sdp
                       v:  SIP  / 2.0  / TCP     12.3.4.5   ;
                       branch  =   9ikj8  ,
                       SIP  /    2.0   / UDP  1.2.3.4   ; hidden
                       m:\"Quoted string \"\" <sip:jdrosen@bell-labs.com> ; newparam = newvalue ;
                       secondparam = secondvalue  ; q = 0.33
                       (((nested comments) and (more)))   ,
                       tel:4443322

                       v=0
                       o=mhandley 29739 7272939 IN IP4 126.5.4.3
                       c=IN IP4 135.180.130.88
                       m=audio 492170 RTP/AVP 0 12
                       m=video 3227 RTP/AVP 31
                       a=rtpmap:31 LPC";

    int len = strlen(msg_invite);
    cout << len << endl;
    test_verify(1437);
    return test_return_code(1);
}
