
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
static const char* const tIPAddress_cxx_Version = 
"$Id: tIPAddress.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Application.hxx"
#include "IPAddress.hxx"

using namespace Vocal;
using namespace Vocal::Transport;


class tIPAddress : public Application
{
    public:
    	tIPAddress() {}
	~tIPAddress() {}

    	ReturnCode  	    run();	
};


class tIPAddressCreator : public ApplicationCreator
{
    Application * create() { return ( new tIPAddress ); }
};

ReturnCode
tIPAddress::run()
{
    u_int8_t	    ip_as_octets[4] = { 192, 168, 7, 10 };
    
    u_int32_t	    ip_as_long = ((192<<24) + (168<<16) + (7<<8) + 14);
    
    sockaddr_in     ip_as_sa_in;
    sockaddr *	    ip_as_sa = (sockaddr *)&ip_as_sa_in;
    ip_as_sa->sa_family = AF_INET;
    ip_as_sa_in.sin_port = 8000;
    ip_as_sa_in.sin_addr.s_addr = htonl(ip_as_long+1);
    
    IPAddress	    ip0;
    IPAddress	    ip1(1719);
    IPAddress	    ip2("192.168.7.26", 7);
    IPAddress	    ip3(ip_as_octets, 1720);
    IPAddress	    ip4(ip_as_long, 4201);
    IPAddress	    ip5(ip_as_sa_in);
    IPAddress	    ip6(ip5);
    IPAddress	    ip7;

    IPAddress       ip8;
    
    ip8.setIP("127.0.0.1/6005");
    cout << ip8 << endl;
    
    ip8.setIP("127/0");
    cout << ip8 << endl;

    ip8.setIP("127.0.0.1:0");
    cout << ip8 << endl;
    
    ip8.setIP("6005");
    cout << ip8 << endl;
    
    ip7 = ip2;
            
    cout    << ip0  << endl
    	    << ip1  << endl
	    << ip2  << endl
	    << ip3  << endl
	    << ip4  << endl
	    << ip5  << endl
	    << ip6  << endl
	    << ip7  << endl;

    cout    <<	( ip0 == ip1 ) << endl
    	    <<	( ip1 == ip2 ) << endl    
    	    <<	( ip2 == ip3 ) << endl    
    	    <<	( ip3 == ip4 ) << endl    
    	    <<	( ip4 == ip5 ) << endl    
    	    <<	( ip5 == ip6 ) << endl;

    cout << ip2.getIPAndPortAsString() << endl;
    	    
    return ( 0 );
}

int main(int argc, char ** argv, char ** arge)
{
    tIPAddressCreator creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
