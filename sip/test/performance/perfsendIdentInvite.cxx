
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

#include <cstdlib>
#include <fstream>

#include "Sptr.hxx"
#include "Data.hxx"
#include "VovidaSipStack.hxx"
#include "InviteMsg.hxx"
#include "AckMsg.hxx"
#include "SipTransceiver.hxx"

using namespace Vocal;


///
int
main( int argc, char* argv[] )
{
    if ( argc < 3)
    {
        cerr << "Usage: "
        << argv[0]
        << " <local_SIP_port>"
        << " <msg_file_name>"
        << " [ <#messages> ]"
        << endl;
        exit( -1 );
    }

 


  

   

    int numOfMessages = 1;
    
    SipTransceiver stack(argv[0], atoi( argv[1] ) );
    

    stack.setRandomLosePercent(0);

    std::ifstream inputFile( argv[2] );
    if ( !inputFile )
    {
        cerr << "Error: Cannot open input file " << argv[2] << endl;
        exit( -1 );
    }
    string str;
    char ch;
    while ( inputFile.get( ch ) )
    {
        str += ch;
    }

    // Optional command line arguments
    if ( argc >= 4 )
    {
        numOfMessages = atoi( argv[3] );
    }

 
#if 0

    cout << "\t<<< Send Invite Message test >>>\n" << endl;
    cout << "     Local SIP Port : " << argv[1] << endl;
    cout << "         Input File : " << argv[2] << endl;
    cout << " Number of Messages : " << numOfMessages << endl;
    cout << "           Interval : " << interval << endl;
    cout << "           Log File : " << logFile.logData() << endl;
    cout << endl << "<<< Message to send begin >>>" << endl << str << endl;
    cout << "<<< Message to send end >>>" << endl << endl;
#endif
    InviteMsg inviteMsg( str );

    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;
    Sptr < StatusMsg > statusMsg;

   

    // Note: this is needed as the stack won't code the from user
    // field otherwise
    SipFrom from = inviteMsg.getFrom();
    from.setUser(Data("-"));
    inviteMsg.setFrom(from);
    
    
    int numIdentical = 2;
    
    for ( int i = 1; i <= numOfMessages; i++ )
    { 
	for (int j = 0; j < numIdentical; j++)  //for test 1.2.3, send 2 identical INVITES.
	{
	    // Send INVITE
	    stack.sendAsync( inviteMsg );
	    cout << "sent invite out:" << i << "times" << endl;
	}
	//changing the INVITE.
	SipCallId callId;
	inviteMsg.setCallId( callId );
	
    }
    return 0;
}
