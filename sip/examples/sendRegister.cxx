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
static const char* const SendRegisterVersion =
    "$Id: sendRegister.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include <cstdlib>
#include <fstream>

#include "cpLog.h"
#include "Sptr.hxx"
#include "Data.hxx"
#include "VovidaSipStack.hxx"
#include "RegisterMsg.hxx"
#include "TimeFunc.hxx"

using namespace Vocal;


///
int
main( int argc, char* argv[] )
{
    if ( argc < 3 )
    {
        cerr << "Usage: "
        << argv[0]
        << " <local_SIP_port>"
        << " <msg_file_name>"
        << " [ <#messages> [ <interval(ms)> [ s|d ] ] ]"
        << endl;
        exit( -1 );
    }

    Data logFile = "/tmp/rts";
    // logFile += getpid();
    // logFile += ".log";
    cpLogOpen( logFile.logData() );

    if ( argc >= 6 )
    {
        cpLogSetPriority( (*(argv[5]) == 's' ? LOG_DEBUG_STACK : LOG_DEBUG ) );
    }

    int numOfMessages = 1;
    int interval = 2000000;     // 2 seconds

    SipTransceiver stack( "sendRegister", atoi( argv[1] ) );

    SipTransceiver::reTransOff();


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
    if ( argc == 5 )
    {
        interval = atoi( argv[4] ) * 1000;
    }

    cout << "\t<<< Send Register Message test >>>\n" << endl;
    cout << "     Local SIP Port : " << argv[1] << endl;
    cout << "         Input File : " << argv[2] << endl;
    cout << " Number of Messages : " << numOfMessages << endl;
    cout << "           Interval : " << interval << endl;
    cout << "           Log File : " << logFile.logData() << endl;
    cout << endl << "<<< Message to send begin >>>" << endl << str << endl;
    cout << "<<< Message to send end >>>" << endl << endl;

    RegisterMsg registerMsg( str );

    int s = 0;
    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;
    Sptr < StatusMsg > statusMsg;

//    FunctionTimer tTotal;
//    tTotal.start();

//    FunctionTimer tCheck;
//    tCheck.start();

    for ( int i = 1; i <= numOfMessages; i++ )
    {
        SipCallId callId;
        registerMsg.setCallId( callId );

        // Send REGISTER
        stack.sendAsync( registerMsg );

        // Get response
        while ( 1 )
        {
            sipRcv = stack.receive();
            if ( sipRcv != 0 )
            {
                sipMsg = sipRcv->back();
                if ( sipMsg != 0 )
                {
                    statusMsg.dynamicCast( sipMsg );
                    if ( statusMsg == 0 )
                    {
                        cpLog( LOG_ERR, "Not a status message" );
                    }
                    else
                    {
                        ++s;
                        if(statusMsg->getStatusLine().getStatusCode() == 200)
                        {
                            cpLog(LOG_INFO, "Final response:\n%s", statusMsg->encode().logData());
                            break;
                        }
                    }
                }
            }
            else
            {
                cpLog( LOG_ERR, "Received NULL from stack" );
            }
        }
        if ( (i % 100) == 0 )
        {
            cout << "R:" << i << " S:" << s << " ";
//            tCheck.end();
//            tCheck.print( "tCheck" );
//            tCheck.start();
        }
        if ( interval > 0 )
        {
            usleep( interval );
        }
    }
    cout << endl << "Done ";
//    tTotal.end();
//    tTotal.print( "tTotal" );

    exit( 0 );
}
