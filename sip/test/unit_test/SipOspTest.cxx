
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

static const char* const SipOspTest_cxx_Version =
    "$Id: SipOspTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "global.h"
#include "SipAuthorization.hxx"
#include "SipOsp.hxx"
#include "symbols.hxx"
static const char* version UNUSED_VARIABLE =
    "$";




#include <cassert>
#include <sys/time.h>
#include <cpLog.h>

#include "InviteMsg.hxx"
#include "Verify.hxx"
#include "BaseUrl.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


int testEncodeDecode()
{
    Data url;
    url = "sip:destination@192.168.5.12";
    Sptr <BaseUrl> tourl;
    tourl = BaseUrl::decode(url, host_addr);
    int listenPort = 5060;
    int rtpPort = 3456;
    cpLog(LOG_DEBUG, "Completed the Url decoding");

    InviteMsg invite(tourl, host_addr, listenPort, rtpPort);

    SipOsp osp("", host_addr);
    osp.setData("osp data here");

    invite.setContentData(&osp);

#if 0
    invite.setAuthBasic("cookie");
    invite.setAuthDigest("012345", "sunitha", "pwd", "INVITE", "voivda");
#endif



#if 0
    SipProxyAuthorization auth;
    auth.setBasicCookie("abcd");
    auth.setAuthScheme(AUTH_BASIC);
    invite.setAuthorization(auth);
#endif

    Data data = invite.encode();
    cout << " before decoding " << data.logData() << endl;
    InviteMsg newInvite(data, host_addr);


#if 0
    bool auth = newInvite.checkAuthBasic("cookie");
    bool auth = newInvite.checkAuthDigest("012345", "sunitha", "pwd", "INVITE");
    if (auth)
    {
        cpLog(LOG_DEBUG_STACK, " digest auth success ");
    }
    else
    {
        cpLog(LOG_DEBUG_STACK, " digest auth failure ");
    }
#endif

    Data newdata = newInvite.encode();

    cout << "after encoding " << newdata.logData() << endl;

    if (data == newdata)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}


int main()
{
    cpLogSetPriority(LOG_DEBUG_STACK);
#if 0
    Data url;
    url = "sip:destination@192.168.5.12";
    SipUrl tourl(url);
#endif

    test_verify(testEncodeDecode() != -1);

    return test_return_code(1);
}
