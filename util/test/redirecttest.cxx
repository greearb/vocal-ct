
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
static const char* const redirecttest_cxx_Version =
    "$Id: redirecttest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include "SipRedirect.hxx"
#include "InviteMsg.hxx"
#include "Sptr.hxx"

main()
{
    SipRedirect x;
    SipRedirect y;

    Sptr < InviteMsg > invite = new InviteMsg();

    // add the sipredirect message
    // after adding the  counter, limit, and token
    //test scenarios
    // encode invite
    // encode status
    // make the status from the invite
    x.setLimit(1);
    x.setCounter(2);
    Data xa = "sssss";
    Data ya = "yyyyy";
    Data xa1 = "sssssaaaaa";
    Data ya1 = "yyyyymmmmm";
    y.setLimit(2);
    y.setReason(1);
    y.setCounter(50);
    x.setTokenDetails(xa, ya);
    x.setTokenDetails(xa1, ya1);
    invite->setRedirect(x);
    invite->setRedirect(y);


    cout << invite->encode().getData() << endl;
    Data data = invite->encode();
    Sptr < InviteMsg > invite1 = new InviteMsg( data );

    cout << invite1->encode().getData();

    //InviteMsg inmsg = *invite1;

    Sptr < StatusMsg > status = new StatusMsg(*invite1, 302);

    cout << status->encode().getData();


    status = new StatusMsg(*invite1, 302);

    cout << status->encode().getData();

    Sptr < StatusMsg > status1 = status;

    cout << status1->encode().getData();
    //  invite = new InviteMsg(data);

    //cout << invite->encode().getData();







}
