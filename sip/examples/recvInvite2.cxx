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

static const char* const recvInviteVersion =
    "$Id: recvInvite2.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <fstream>

#include "cpLog.h"
#include "Sptr.hxx"
#include "Data.hxx"
#include "VovidaSipStack.hxx"
#include "InviteMsg.hxx"
#include "AckMsg.hxx"
#include "TimeFunc.hxx"
#include <ByeMsg.hxx>
#include "BaseUrl.hxx"

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
        << " { 200 | 302 <contact_URI> }"
        << " [ <number_messages> [ s|d ] ]"
        << endl;
        exit( -1 );
    }

    Data logFile = "/tmp/itr";

    cpLogOpen( logFile.logData() );

    cpLogSetPriority(LOG_INFO);

    int status = atoi( argv[2] );

    if ( ( argc >= 5 && status != 302 ) ||
            ( argc >= 6 && status == 302 ) )
    {
        if ( status == 302 )
        {
            cpLogSetPriority( ( *(argv[5]) == 's' ? LOG_DEBUG_STACK : LOG_DEBUG ) );
        }
        else
        {
            cpLogSetPriority( ( *(argv[4]) == 's' ? LOG_DEBUG_STACK : LOG_DEBUG ) );
        }
    }

    long lastCount = -1;
    if ( ( argc >= 4 && status != 302 ) ||
            ( argc >= 5 && status == 302 ) )
    {
        if ( status == 302 )
        {
            lastCount = atoi( argv[4] );
        }
        else
        {
            lastCount = atoi( argv[3] );
        }
    }

    //SipTransceiverFilter stack( atoi( argv[1] ) );
    SipTransceiver stack( argv[0], atoi( argv[1] ) );

    stack.setRandomLosePercent(0);

    string uri = "sip:user@vovida.com";
    if ( status == 302 )
    {
        if ( argc >= 4 )
        {
            uri = argv[3];
        }
        else
        {
            cerr << "Error: Need contact uri";
            exit( -1 );
        }
    }
    Sptr <BaseUrl> url;
    url = BaseUrl::decode( uri );
    SipContact contact;
    contact.setUrl( url );

    cout << "\t<<< Recieve Invite Message test >>>\n" << endl;
    cout << "     Local SIP Port : " << argv[1] << endl;
    cout << "      Return Status : " << argv[2] << endl;
    if ( status == 302 )
    {
        cout << "    302 contact URI : " << argv[3] << endl;
    }
    cout << "   Terminates after : " << lastCount << endl;
    cout << "           Log File : " << logFile.logData() << endl << endl;

    // Counters
    int i = 0;     // INVITE
    int r = 0;     // Response
    int a = 0;     // ACK
    int b = 0;     // BYE


    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;
    Sptr < InviteMsg > inviteMsg;
    Sptr < AckMsg > ackMsg;
    Sptr < ByeMsg > byeMsg;

    //    FunctionTimer tReply;
//    FunctionTimer tTotal;

//    FunctionTimer tCheck;
//    tCheck.start();

    // TODO Add registration

    while ( 1 )
    {
        sipRcv = stack.receive();
        if ( sipRcv != 0 )
        {
            if ( i == 0 )
            {
//                tCheck.start();
//                tTotal.start();
            }
            sipMsg = sipRcv->back();
            if ( sipMsg != 0 )
            {
                inviteMsg.dynamicCast( sipMsg );
                if ( inviteMsg == 0 )
                {
                    ackMsg.dynamicCast( sipMsg );
                    if ( ackMsg == 0 )
                    {
                        byeMsg.dynamicCast( sipMsg);
                        if (byeMsg == 0)
                        {
                            cpLog( LOG_ERR, "Not an Invite or Ack message" );
                        }
                        else
                        {
                            StatusMsg statusMsg( *byeMsg, 200 );
                            stack.sendReply( statusMsg );
                            ++b;
                            ++r;
                            if ( b % 10 == 0 )
                            {
                                cout << "I:" << i << " R:" << r << " A:" << a << " B:" << b << " ";
//                                tCheck.end();
//                                tCheck.print( "tCheck" );
//                               tCheck.start();
                                cout.flush();
                            }
                        }
                    }
                    else
                    {
                        ++a;
                        if ( (b == 0) && (a % 100 == 0) )
                        {
                            cout << "I:" << i << " R:" << r << " A:" << a << " B:" << b << " ";
//                            tCheck.end();
//                            tCheck.print( "tCheck" );
//                            tCheck.start();
                            cout.flush();
                        }
                    }
                }
                else
                {
                    ++i;
                    ++r;

                    StatusMsg statusMsg( *inviteMsg, status );

                    if ( status == 302 )
                    {
                        statusMsg.setNumContact( 0 );
                        statusMsg.setContact( contact );
                    }
                    else if ( status == 200)
                    {
                        // Add myself to the contact
                        Sptr <BaseUrl> myUrl;
                        myUrl = inviteMsg->getRequestLine().getUrl();

                        SipContact me;
                        me.setUrl( myUrl );
                        statusMsg.setNumContact( 0 );    // Clear
                        statusMsg.setContact( me );
                    }
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
        if ( a == lastCount && ( b == 0 || b == lastCount ) )    // Terminate it for Purify and Quanitify
        {
//            tTotal.end();
            cout << endl ;
//            tTotal.print( "tTotal" );

            cout << endl << "Wait...." << endl;
            sipRcv = 0;
            sipMsg = 0;
            inviteMsg = 0;
            ackMsg = 0;
            byeMsg = 0;

            sleep( 180 );
            stack.printSize();
#if SPTR_TRACEBACK
            sptrDebugDumpTraceback("traceback.out");
#endif
            cout << "Done" << endl;
//            stack.clear();
            exit( 1 );
        }
    }
    return 0;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
