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

static const char* const SdpAttributeTest_cxx_Version =
    "$Id: Sdp2AttributeTest.cxx,v 1.1 2004/05/01 04:15:24 greear Exp $";

#include "Verify.hxx"
#include "Sdp2Attributes.hxx"

using Vocal::SDP::SdpAttributes;
using Vocal::SDP::SdpRtpMapAttribute;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::MediaAttributes;

int main()
{
    {
	SdpAttributes x;
	
	x.addValueAttribute(new ValueAttribute);
	x.addValueAttribute(new ValueAttribute);
	x.addValueAttribute(new ValueAttribute);
	
	SdpAttributes y;
	y = x;
	SdpAttributes z(x);
	
	vector<ValueAttribute*>::iterator i = x.getValueAttributes()->begin();
	vector<ValueAttribute*>::iterator j = y.getValueAttributes()->begin();
	vector<ValueAttribute*>::iterator k = z.getValueAttributes()->begin();
	
	while((i != x.getValueAttributes()->end()) && 
	      (j != y.getValueAttributes()->end()) &&
	      (k != z.getValueAttributes()->end())
	    )
	{
	    test_verify(*i != *j);
	    test_verify(*i != *k);
	    test_verify(strcmp((*i)->getAttribute(), (*j)->getAttribute()) == 0);
	    test_verify(strcmp((*i)->getValue(), (*j)->getValue()) == 0);
	    ++i;
	    ++j;
	    ++k;
	}
    }

    {
	MediaAttributes x;
	
	x.addmap(new SdpRtpMapAttribute);
	x.addmap(new SdpRtpMapAttribute);
	x.addmap(new SdpRtpMapAttribute);
	
	MediaAttributes y;
	y = x;
	
	vector<SdpRtpMapAttribute*>::iterator i = x.getmap()->begin();
	vector<SdpRtpMapAttribute*>::iterator j = y.getmap()->begin();
	
	while((i != x.getmap()->end()) && 
	      (j != y.getmap()->end())
	    )
	{
	    test_verify(*i != *j);
	    ++i;
	    ++j;
	}
    }

    {
	SdpAttributes x;
	Data y("recvonly");
	x.setAttribute(y);

	test_verify(x.getrecvonly());
    }

    {
	SdpAttributes x;
	Data y("sendonly");
	x.setAttribute(y);

	test_verify(x.getsendonly());
    }

    {
	SdpAttributes x;
	Data y("sendrecv");
	x.setAttribute(y);

	test_verify(x.getsendrecv());
    }

    {
	SdpAttributes x;
	Data y("sendrecv:1");
	x.setAttribute(y);

	vector < ValueAttribute* > * va = x.getValueAttributes();
	test_verify(va->begin() != va->end());
	assert(va->begin() != va->end());

	test_verify(Data((*va->begin())->getAttribute()) == "sendrecv");
	test_verify(Data((*va->begin())->getValue()) == "1");
    }


    return test_return_code(21);
}
