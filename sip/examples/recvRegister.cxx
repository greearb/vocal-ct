/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2003 Vovida Networks, Inc.  All rights reserved.
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
static const char* const recvRegisterVersion =
    "$Id: recvRegister.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include <cstdlib>
#include <fstream>

#include "cpLog.h"
#include "Sptr.hxx"
#include "Data.hxx"
#include "VovidaSipStack.hxx"
#include "RegisterMsg.hxx"
#include "StatusMsg.hxx"
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
        << " [ s|d ]"
        << endl;
        exit( -1 );
    }

    Data logFile = "/tmp/rtr";
    // logFile += getpid();
    // logFile += ".log";
    cpLogOpen( logFile.logData() );
    if ( argc >= 3 )
    {
        cpLogSetPriority( ( *(argv[2]) == 's' ? LOG_DEBUG_STACK : LOG_DEBUG ) );
    }

    // SipTransceiverFilter stack( atoi( argv[1] ) );
    SipTransceiver stack( "", atoi( argv[1] ) );

    cout << "\t<<< Recieve Register Message test >>>\n" << endl;
    cout << "     Local SIP Port : " << argv[1] << endl;
    cout << "           Log File : " << logFile.logData() << endl << endl;

    // Counters
    int i = 0;     // Register
    int r = 0;     // Response

    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;
    Sptr < RegisterMsg > registerMsg;

//    FunctionTimer tReply;

//    FunctionTimer tCheck;
//    tCheck.start();

    // TODO Add registration

    while ( 1 )
    {
        sipRcv = stack.receive();
        if ( sipRcv != 0 )
        {
            sipMsg = sipRcv->back();
            if ( sipMsg != 0 )
            {
                registerMsg.dynamicCast( sipMsg );
                if ( registerMsg == 0 )
                {
                    cpLog( LOG_ERR, "Not a register message" );
                }
                else
                {
                    ++i;
                    ++r;
                    if ( i % 100 == 0 )
                    {
                        cout << "I:" << i << " R:" << r << " ";
//                        tCheck.end();
//                        tCheck.print( "tCheck" );
//                        tCheck.start();
                        cout.flush();
                    }
                    // cout << "\rRegister " << i << " Response " << r << " Ack " << a;
                    // cout.flush();

                    StatusMsg statusMsg( *registerMsg, 200 );

//                    tReply.start();
                    stack.sendReply( statusMsg );
//                    tReply.end();
//                    tReply.print( "tReply" );
                }
            }
        }
        else
        {
            cpLog( LOG_ERR, "Received NULL from stack" );
        }
    }
    cout << endl << endl;

    return 0;
}
