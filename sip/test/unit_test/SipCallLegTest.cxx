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

static const char* const SipCallLegTest_cxx_Version =
    "$Id: SipCallLegTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipFrom.hxx"
#include "SipTo.hxx"
#include "SipCallId.hxx"
#include "SipCallLeg.hxx"
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


bool test_00()
{
    Sptr <BaseUrl> fromBaseUrl;
    fromBaseUrl = BaseUrl::decode(Data("sip:test1@test.com"), host_addr );
    
    SipFrom from1(fromBaseUrl, host_addr);
    
    Sptr <BaseUrl> toBaseUrl;
    toBaseUrl = BaseUrl::decode(Data("sip:test2@test2.com"), host_addr);
    SipTo to1(toBaseUrl, host_addr);
    SipCallId id1("1000@a.com", host_addr);

    id1.setData("f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

    Sptr <BaseUrl> from2BaseUrl;
    from2BaseUrl  = BaseUrl::decode(Data("sip:test1@test.com"), host_addr );
    SipFrom from2(from2BaseUrl, host_addr);
    
    Sptr <BaseUrl> to2BaseUrl;
    to2BaseUrl = BaseUrl::decode(Data("sip:test2@test2.com"), host_addr );
    SipTo to2(to2BaseUrl, host_addr);
    SipCallId id2("1000@yahoo.com", host_addr);

    id2.setData("e81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

    SipCallLeg leg1(from1, to1, id1, host_addr);
    SipCallLeg leg2(from2, to2, id2, host_addr);

    return leg2 < leg1;
}

bool test_01()
{
    Sptr <BaseUrl> from1BaseUrl;
    from1BaseUrl = BaseUrl::decode(Data("sip:test1@test.com"), host_addr );
    SipFrom from1(from1BaseUrl, host_addr);
    
    Sptr <BaseUrl> to1BaseUrl;
    to1BaseUrl = BaseUrl::decode(Data("sip:test2@test2.com"), host_addr );
    SipTo to1(to1BaseUrl, host_addr);
    
    SipCallId id1("", host_addr);

    id1.setData("f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

    
    Sptr <BaseUrl> from2BaseUrl;
    from2BaseUrl = BaseUrl::decode(Data("sip:test1@test.com"), host_addr );
    SipFrom from2(from2BaseUrl, host_addr);
    
    Sptr <BaseUrl> to2BaseUrl;
    to2BaseUrl = BaseUrl::decode(Data("sip:test2@test2.com"), host_addr );
    SipTo to2(to2BaseUrl, host_addr);
    SipCallId id2("", host_addr);

    id2.setData("f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

    SipCallLeg leg1(from1, to1, id1, host_addr);
    SipCallLeg leg2(from2, to2, id2, host_addr);

    return !(leg2 < leg1);
}

bool test_02()
{
    
    
    
    Sptr <BaseUrl> from1BaseUrl;
    from1BaseUrl = BaseUrl::decode(Data("sip:test1@test.com"), host_addr );
    SipFrom from1(from1BaseUrl, host_addr);
    
    Sptr <BaseUrl> to1BaseUrl;
    to1BaseUrl = BaseUrl::decode(Data("sip:test2@test2.com"), host_addr );
    SipTo to1(to1BaseUrl, host_addr);
    
    SipCallId id1("", host_addr);

    id1.setData("e81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

    
    
    Sptr <BaseUrl> from2BaseUrl;
    from2BaseUrl = BaseUrl::decode(Data("sip:test1@test.com"), host_addr );
    SipFrom from2(from2BaseUrl, host_addr);
    
    Sptr <BaseUrl> to2BaseUrl;
    to2BaseUrl = BaseUrl::decode(Data("sip:test2@test2.com"), host_addr );
    SipTo to2(to2BaseUrl, host_addr);
    
    SipCallId id2("", host_addr);

    id2.setData("f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

    SipCallLeg leg1(from1, to1, id1, host_addr);
    SipCallLeg leg2(from2, to2, id2, host_addr);

    return !(leg2 < leg1);
}

void test()
{
    Sptr <BaseUrl> from1BaseUrl;
    from1BaseUrl = BaseUrl::decode(Data("sip:test1@test.com"), host_addr );
    SipFrom from1(from1BaseUrl, host_addr);
    
    Sptr <BaseUrl> to1BaseUrl;
    to1BaseUrl = BaseUrl::decode(Data("sip:test2@test2.com"), host_addr );
    SipTo to1(to1BaseUrl, host_addr);
    
    SipCallId id1("", host_addr);

    id1.setData("e81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

    
    
    Sptr <BaseUrl> from2BaseUrl;
    from2BaseUrl = BaseUrl::decode(Data("sip:test1@test.com"), host_addr );
    SipFrom from2(from2BaseUrl, host_addr);
    
    Sptr <BaseUrl> to2BaseUrl;
    to2BaseUrl = BaseUrl::decode(Data("sip:test2@test2.com"), host_addr );
    SipTo to2(to2BaseUrl, host_addr);
    
    SipCallId id2("", host_addr);

    id2.setData("f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

    {
      SipCallLeg leg1(from1, to1, id1, host_addr);
      SipCallLeg leg2(leg1);
      
      test_verify(leg1 == leg2);
    }

    {
      SipCallLeg leg1(from1, to1, id1, host_addr);
      SipCallLeg leg2(leg1);
      
      test_verify(leg1.getFrom().getUser() == "test1");
    }


    {
      SipCallLeg leg1(from1, to1, id1, host_addr);
      SipCallLeg leg2(leg1);

      test_verify(leg1.encode() == leg2.encode());
    }

    {
      SipCallLeg leg1(from1, to1, id1, host_addr);
      SipCallLeg leg2(leg1);

      test_verify(leg2.getFrom().getUser() == "test1");
    }
}


int main()
{
    test_verify(test_00());
    test_verify(test_01());
    test_verify(test_02());

    test();

    return test_return_code(7);
}
