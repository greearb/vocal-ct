
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
static const char* const udpMsgTest_cxx_Version =
    "$Id: udpMsgTest.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include <cstdlib>

#include <UdpStack.hxx>
#include <NetworkAddress.h>
#include <Sptr.hxx>
#include <support.hxx>

int
main( int argc, char* argv [] )
{
    if ( argc < 5 )
    {
        cerr << "Usage: "
        << argv[0]
        << " <remote_host>"
        << " <remote_port> "
        << " <local_port> "
        << " <msg_file_name>"
        << " [<#messages>]"
        << " [<interval(ms)>]"
        << endl;
        exit( -1 );
    }

    Sptr < UdpStack > udpStack = new UdpStack;
    int numOfMessages = 1;
    int interval = 2000000;

    // Parse all command line arguments
    //    udpStack->setDestination( argv[1], atoi( argv[2] ) );
    udpStack->setLocal( atoi( argv[3] ) );
    NetworkAddress dest( argv[1], atoi( argv[2] ) );

    std::ifstream inputFile( argv[4] );
    if ( !inputFile )
    {
        cerr << "Error: Cannot open input file " << argv[4] << endl;
        exit( -1 );
    }
    string s;
    char ch;
    while ( inputFile.get( ch ) )
    {
        s += ch;
    }
    if ( argc >= 6 )
    {
        numOfMessages = atoi( argv[5] );
    }
    if ( argc == 7 )
    {
        interval = atoi( argv[6] ) * 1000;
    }
    cout << "\t<<< UDP message test >>>\n" << endl;
    cout << "        Remote Host : " << argv[1] << endl;
    cout << "        Remote Port : " << atoi( argv[2] ) << endl;
    cout << "         Local Port : " << atoi( argv[3] ) << endl;
    cout << "         Input File : " << argv[4] << endl;
    cout << " Number of Messages : " << numOfMessages << endl;
    cout << "           Interval : " << interval << endl;
    cout << endl << "<<< Message to send begin >>>" << endl << s << endl;
    cout << "<<< Message to send end >>>" << endl << endl;

    //TODO Spawn the receiving thread that does something useful

    // Find the location of the Call-ID:
    string::size_type start = s.find( "Call-ID: " ) + 9;
    string::size_type end = s.find( "@", start );
    srandom((unsigned int)time(NULL));
    int tmpCallId;
    string t;

    // Send messages
    for ( int i = 0; i < numOfMessages; i++ )
    {
        cout << "\rSending " << i + 1;
        cout.flush();
        //TODO Modify the message
        tmpCallId = random();
        t = s;
        t.replace( start, end - start, itos( tmpCallId ) );
        udpStack->transmitTo( t.c_str(), t.length(), &dest );
        usleep( interval );
    }
    cout << endl << endl;
    //TODO Wait for the receiving thread to terminate
    exit( 0 );
}
