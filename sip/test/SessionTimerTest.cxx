
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
static const char* const sendInviteVersion =
    "$Id: SessionTimerTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

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
#include "SessionTimer.hxx"

using namespace Vocal;


void cCB(SipCallLeg cLeg)
{
    cpLog(LOG_INFO,  "Session Expired for callLeg (%s)****" , cLeg.encode().logData());
}

///
int
main( int argc, char* argv[] )
{
    if ( argc < 3 )
    {
        cerr << "Usage: "
        << argv[0]
        << " <local_SIP_port>"
        << " <msg_file>"
        << " [ s|d ]"
        << endl;
        exit( -1 );
    }

    Data logFile = "/tmp/its";
    logFile += getpid();
    logFile += ".log";
 //   cpLogOpen( logFile.logData() );

    cpLogSetPriority(LOG_INFO);
    cout << argc << endl;
    if ( argc >= 4 )
    {
        cpLogSetPriority( (*(argv[3]) == 's' ? LOG_DEBUG_STACK : LOG_DEBUG ) );
    }

    int numOfMessages = 1;
    int sendBye = 0;   // Don't send BYE message by default

    Sptr<SipTransceiver> stackP = new SipTransceiver(  argv[0], atoi( argv[1] ) );
    SipTransceiver& stack = *stackP;

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

    SessionTimer& sTimer = SessionTimer::instance(stackP);;
    sTimer.registerCallBack(cCB);


    sendBye = 0;

    cout << "\t<<< Send Invite Message test >>>\n" << endl;
    cout << "     Local SIP Port : " << argv[1] << endl;
    cout << "           Log File : " << logFile.logData() << endl;

    Data msg(str);
    InviteMsg inviteMsg( msg );
    cout << endl << "<<< Message to send begin >>>" << endl << inviteMsg.encode().logData() << endl;
    cout << "<<< Message to send end >>>" << endl << endl;


    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;
    Sptr < StatusMsg > statusMsg;
    int status = 0;

    FunctionTimer tTotal;
    tTotal.start();

    FunctionTimer tCheck;
    tCheck.start();

    numOfMessages = 1;
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

        // Get response
        while ( 1 )
        {
            sipRcv = stack.receive( -1 );
            if ( sipRcv != 0 )
            {
                sipMsg = sipRcv->back();
                if ( sipMsg != 0 )
                {
                    statusMsg.dynamicCast( sipMsg );
                    if ( statusMsg == 0 )
                    {
                        Sptr<InviteMsg > iMsg;
                        iMsg.dynamicCast(sipMsg);
                        if(iMsg != 0)
                        {
                            //WHen UAS is refreshing, start the timer 
                            if(!sTimer.timerExists(*iMsg))
                                sTimer.startTimerFor(*iMsg, iMsg->getSessionExpires().getDelta().convertInt());
                            StatusMsg sMsg(*iMsg, 200);
                            sMsg.setSessionExpires(iMsg->getSessionExpires());
                            sMsg.setMinSE(iMsg->getMinSE());
                            stack.sendReply(sMsg);
                            sTimer.processResponse(sMsg);
                        }
                    }
                    else
                    {
                        status = statusMsg->getStatusLine().getStatusCode();
                        Data method = statusMsg->getCSeq().getMethod();
                        if ( status == 200 )
                        {
                            //When UAC is refreshing, process status
                            if(sTimer.timerExists(inviteMsg))
                            {
                                sTimer.processResponse(*statusMsg);
                            }
                            else
                            {
                                int sessionInterval = inviteMsg.getSessionExpires().getDelta().convertInt();
                                Data refresher = inviteMsg.getSessionExpires().getRefreshParam();
                                //Get the status message expire interval
                                if(!statusMsg->getSessionExpires().isEmpty())
                                {
                                    const SipSessionExpires& se = statusMsg->getSessionExpires();
                                    sessionInterval =  se.getDelta().convertInt();
                                    if(se.getRefreshParam().length())
                                        refresher = se.getRefreshParam();
                                }
                                if(refresher == "uac")
                                {
                                    //When UAC is refreshing, start timer
                                    sTimer.startTimerFor(*statusMsg);
                                }
                            }
                            if (method == "BYE")
                            {
                                break;
                            }
                            else
                            {
                                // Send ACK
                                AckMsg ackMsg( *statusMsg );
                                stack.sendAsync( ackMsg );
                                if (sendBye)
                                {
                                    ByeMsg byeMsg( *statusMsg );
                                    stack.sendAsync( byeMsg );
                                }
                            }
                        }
                        else
                        {
                                cerr << "Non-200 response code :"<< status << endl;
                        }
                    }
                }
            }
            else
            {
                cpLog( LOG_ERR, "Received NULL from stack" );
                break;
            }
        }
    }
    cout << endl;

    cout << "Done " << endl;

    exit( 0 );
}
