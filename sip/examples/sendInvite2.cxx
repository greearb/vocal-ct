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
static const char* const sendInviteVersion =
    "$Id: sendInvite2.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include <cstdlib>
#include <fstream>

#include "cpLog.h"
#include "Sptr.hxx"
#include "Data.hxx"
#include "VovidaSipStack.hxx"
#include "InviteMsg.hxx"
#include "AckMsg.hxx"
#include "TimeFunc.hxx"
#include "ByeMsg.hxx"
#include "VThread.hxx"
#include "SipTransceiverFilter.hxx"

using namespace Vocal;

int s = 0;
int a = 0;
int b = 0;
int sendBye = 0;   // Don't send BYE message by default
int i;

void* ackMsg(void* sipstack)
{
    int status = 0;

    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;
    Sptr < StatusMsg > statusMsg;

    SipTransceiverFilter* stack = static_cast < SipTransceiverFilter* > (sipstack);

    while (1)
    {
        sipRcv = stack->receive( 32000 );
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
                    status = statusMsg->getStatusLine().getStatusCode();
                    Data method = statusMsg->getCSeq().getMethod();
                    if ( status >= 200 )
                    {
                        if (method == "BYE")
                        {
                            //break;
                        }

                        else
                        {
                            // Send ACK
                            AckMsg ackMsg( *statusMsg );
                            ++a;
                            stack->sendAsync( ackMsg );
                            //if (sendBye)
                            if (sendBye)
                            {
                                ByeMsg byeMsg( *statusMsg );
                                stack->sendAsync( byeMsg );
                                b++;
                            }
                            if (sendBye == 0)
                            {
                                //break;
                            }

                        }
                    }
                }
            }
        }
        else
        {
            cpLog( LOG_ERR, "Received NULL from stack" );
            //break;
        }

    }
    return 0;
}


void* printStats(void*)
{
    while (1)
    {
        cout << "I:" << i << " S:" << s << " A:" << a << " B:" << b << "\r";
        sleep(1);
    }
    return 0;
}





///
int
main( int argc, char* argv[] )
{
    VThread replyThread;
    VThread printStatsThread;

    printStatsThread.spawn(&printStats, 0);

    if ( argc < 4 )
    {
        cerr << "Usage: "
        << argv[0]
        << " <local_SIP_port>"
        << " <msg_file_name>"
        << " <bye | nobye> "
        << " [ <#messages> [ <interval(ms)> [ s|d ] ] ]"
        << endl;
        exit( -1 );
    }

    Data logFile = "/tmp/its";
    // logFile += getpid();
    // logFile += ".log";
    cpLogOpen( logFile.logData() );

    cpLogSetPriority(LOG_INFO);

    if ( argc >= 7 )
    {
        cpLogSetPriority( (*(argv[6]) == 's' ? LOG_DEBUG_STACK : LOG_DEBUG ) );
    }

    int numOfMessages = 1;
    int interval = 2000000;     // 2 seconds

    SipTransceiverFilter stack( "", atoi( argv[1] ) );
    //    SipTransceiver stack( atoi( argv[1] ) );

    replyThread.spawn(&ackMsg, &stack);

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

    string byeOption = argv[3];
    if (byeOption == "bye")
    {
        sendBye = 1;
    }
    else
    {
        sendBye = 0;
    }

    // Optional command line arguments
    if ( argc >= 5 )
    {
        numOfMessages = atoi( argv[4] );
    }

    if ( argc >= 6 )
    {
        interval = atoi( argv[5] ) * 1000;
    }



    cout << "\t<<< Send Invite Message test >>>\n" << endl;
    cout << "     Local SIP Port : " << argv[1] << endl;
    cout << "         Input File : " << argv[2] << endl;
    cout << " Number of Messages : " << numOfMessages << endl;
    cout << "           Interval : " << interval << endl;
    cout << "           Log File : " << logFile.logData() << endl;
    cout << endl << "<<< Message to send begin >>>" << endl << str << endl;
    cout << "<<< Message to send end >>>" << endl << endl;

    InviteMsg inviteMsg( str );


//    FunctionTimer tTotal;
//    tTotal.start();

    //FunctionTimer tCheck;
    //    tCheck.start();

    // TODO Add registration
    for ( int i = 1; i <= numOfMessages; i++ )
    {
        SipCallId callId;
        inviteMsg.setCallId( callId );

        // Note: this is needed as the stack won't code the from user
        // field otherwise
        SipFrom from = inviteMsg.getFrom();
        from.setUser(Data("-"));
        inviteMsg.setFrom(from);

        // Send INVITE
        stack.sendAsync( inviteMsg );

        //	tCheck.end();
        //	tCheck.print( "tCheck" );
        //	tCheck.start();

        if ( interval > 0 )
        {
            usleep( interval );
        }
    }
    cout << endl;

//    tTotal.end();
//    tTotal.print( "tTotal" );

    cout << endl << "Wait...." << endl;
    time_t original, mytime;
    mytime = 0;
    original = time(0);

    //    sipRcv = 0;
    //    sipMsg = 0;
    //    statusMsg = 0;

    while ( mytime < 300 )
    {
        mytime = time(0) - original;
        //	sipRcv = stack.receive( 10000 );
    }

    stack.printSize();
    cout << "Done " << endl;

    exit( 0 );
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
