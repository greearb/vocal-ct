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

static const char* const EmbeddedObjTest_cxx_Version ="";

#include "global.h"
#include "cpLog.h"
#include "symbols.hxx"
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <iostream.h>
#include "SipUrl.hxx"
#include "SipTo.hxx"
#include "SipFrom.hxx"
#include "SipCallId.hxx"
#include "SipCSeq.hxx"
#include "SipContentLength.hxx"
#include "SipContentType.hxx"
#include "SipContentEncoding.hxx"
#include "SipRoute.hxx"
#include "SipDate.hxx"
#include "SipContact.hxx"
#include "SipExpires.hxx"
#include "SipProxyAuthorization.hxx"
#include "SipMaxForwards.hxx"
#include "SipRequire.hxx"
#include "SipOrganization.hxx"
#include "SipTimestamp.hxx"
#include "SipAllow.hxx"
#include "SipSubject.hxx"
#include "SipAccept.hxx"
#include "SipRequestLine.hxx"
#include "SipStatusLine.hxx"
#include "InviteMsg.hxx"
#include "Verify.hxx"
#include "SipRawHeaderContainer.hxx"
#include "Sptr.hxx"
#include <vector>
using namespace Vocal;

int main(int argc, char *argv[])
{
    Data emb("sip:user@B?Route%3D%20%3Csip%3Auser%40B%3E%2C%3Csip%3Auser%40C%3E%2C%3Csip%3Auser%40v%3E");
    Data res("Route%3A%20%3Csip%3Auser%40B%3E%2C%3Csip%3Auser%40C%3E%2C%3Csip%3Auser%40v%3E");
    const string em("Route: <sip:user@B>,<sip:user@C>,<sip:user@v>");
    string ro = "Route: ";
    typedef SipHeaderList <SipRoute> SipRouteList;
/* ReverseEscape Test case 
   situation will happen on the Proxy which gets contact with embedded routes 
   form RS it need to construct the route header*/

    SipUrl sipUrl7(emb);
    int count = 0;
    SipRawHeaderContainer cont = sipUrl7.getEscObj()->getHeaderList();
    // SipRouteList tmp;
    //cont.getHeaderList(&tmp, SIP_ROUTE_HDR);
    vector < Sptr<SipRoute> > rot = sipUrl7.getEscObj()->getRouteList();
    int sizeV = rot.size()-1;
    for (vector < Sptr<SipRoute> >::iterator i = rot.begin(); i!=
	     rot.end(); ++i){
	Sptr<SipRoute> E = *i;
	    ro += E->encode().convertString();
	    
	    if(count < sizeV)
		ro += ",";
	    count++;
    }
    //cout<<"Final Route is "<<ro<<endl;
    

/* Test Case when RS need to generate the embedded route header to 
   embed it in contact */

    EmbeddedObj en(em);
    Data v = en.getforwardEscapeheader();
    //cout<<v.convertString()<<endl;
    
    
    test_verify(res == v); /* Proxy side Use  Test */
 
    test_verify(em == ro);/* RS side Use Test */

    
    return test_return_code(2);
}
