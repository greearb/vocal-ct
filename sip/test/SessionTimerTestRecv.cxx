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
    "$Id: SessionTimerTestRecv.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

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
#include "ByeMsg.hxx"
#include "SessionTimer.hxx"

using namespace Vocal;

char* inv_msg =
"INVITE sip:6400@128.107.140.141:5064;user=phone SIP/2.0\r\n\
Via: SIP/2.0/UDP 128.107.140.141:5065\r\n\
From: 6500<sip:6500@128.107.140.141:5065>\r\n\
To: 6400<sip:6400@128.107.140.141:5064;user=phone>\r\n\
Call-ID: 247515495@128.107.140.140\r\n\
CSeq: 1 INVITE\r\n\
Subject: VovidaINVITE\r\n\
Contact: <sip:6500@128.107.140.141:5065>\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 168\r\n\
\r\n\
v=0\r\n\
o=- 99513809 99513809 IN IP4 128.107.140.140\r\n\
s=VOVIDA Session\r\n\
c=IN IP4 128.107.140.140\r\n\
t=962892393 0\r\n\
m=audio 3456 RTP/AVP 0\r\n\
a=rtpmap:0 PCMU/8000\r\n\
a=ptime:20\r\n\
\r\n";
void
cCB(SipCallLeg cLeg)
{
    cpLog(LOG_INFO, "****Session Expired for call-leg (%s)", cLeg.encode().logData());
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
        << " <sessionInterval(sec)>"
        << " [ s|d ]"
        << endl;
        exit( -1 );
    }

    Data logFile = "/tmp/itr";
    //cpLogOpen( logFile.logData() );

    cpLogSetPriority(LOG_INFO);

    int status = 200;
    int sessionInterval = atoi(argv[2]);

    if ( ( argc >= 4 ))
    {
        cpLogSetPriority( ( *(argv[3]) == 's' ? LOG_DEBUG_STACK : LOG_DEBUG ) );
    }

    Sptr<SipTransceiver> stackP = new SipTransceiver( "", atoi( argv[1] ) );
    SessionTimer& sTimer = SessionTimer::instance(stackP);
    sTimer.registerCallBack(cCB);
    SipTransceiver& stack = *stackP;

    stack.setRandomLosePercent(0);

    string uri = "sip:user@vovida.com";
    Sptr<SipUrl> url = new SipUrl( uri );
    SipContact contact;
    contact.setUrl( url );

    cout << "\t<<< Recieve Invite Message test >>>\n" << endl;
    cout << "     Local SIP Port : " << argv[1] << endl;
    cout << "           Log File : " << logFile.logData() << endl << endl;


    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;
    Sptr < InviteMsg > inviteMsg;
    Sptr < AckMsg > ackMsg;
    Sptr < ByeMsg > byeMsg;

    // TODO Add registration

    while ( 1 )
    {
        sipRcv = stack.receive();
        if ( sipRcv != 0 )
        {
            sipMsg = sipRcv->back();
            if ( sipMsg != 0 )
            {
                inviteMsg.dynamicCast( sipMsg );
                if ( inviteMsg == 0 )
                {
                    ackMsg.dynamicCast( sipMsg );
                    Sptr<StatusMsg> sMsg ;
                    sMsg.dynamicCast( sipMsg );
                    if ( ackMsg == 0 )
                    {
                        byeMsg.dynamicCast( sipMsg);
                        if (sMsg != 0)
                        { 
                            // Send ACK
                            AckMsg ackMsg( *sMsg );
                            stack.sendAsync( ackMsg );
                            //WHen UAS is refreshing, process status
                            sTimer.processResponse(*sMsg);
                        }
                        else if(byeMsg != 0)
                        {
                            StatusMsg statusMsg( *byeMsg, 200 );
                            stack.sendReply( statusMsg );
                        }
                    }
                    else if(sMsg != 0)
                    {
                    }
                }
                else
                {
                    StatusMsg statusMsg( *inviteMsg, status );
                    if ( status == 200)
                    {
                        // Add myself to the contact
                        Sptr<BaseUrl> myUrl;
                        myUrl = inviteMsg->getRequestLine().getUrl();

                        //Check if the session-timer is supported and 
                        //Invite msg has Session_Expires header
                        SipContact me;
                        me.setUrl( myUrl );
                        statusMsg.setNumContact( 0 );    // Clear
                        statusMsg.setContact( me );
                        SipSessionExpires se;
                        se.setDelta(Data(sessionInterval));
                        statusMsg.setSessionExpires(se);
                    }
                    stack.sendReply( statusMsg );
                    if(!inviteMsg->getSessionExpires().isEmpty() && (status == 200))
                    {
                        if(!sTimer.timerExists(*inviteMsg))
                        {
                            if(inviteMsg->getSessionExpires().getRefreshParam() == "uas")
                            {
                                InviteMsg iMsg(inv_msg);
                                iMsg.setSessionExpires(inviteMsg->getSessionExpires());
                                iMsg.setMinSE(inviteMsg->getMinSE());
                                iMsg.setCallId(statusMsg.getCallId());
                                sessionInterval = iMsg.getSessionExpires().getDelta().convertInt();
                                sTimer.startTimerFor(iMsg, sessionInterval, VS_SEND);
                            }
                            else
                            {
                                ///When UAC is refreshing, start the timer
                                sTimer.startTimerFor(*inviteMsg, sessionInterval);
                                sTimer.processResponse(statusMsg);
                            }
                        }
                        else
                        {
                            ///When UAC is refreshing, start the timer
                            sTimer.processResponse(statusMsg);
                        }
                    } 
                }
            }
        }
        else
        {
            cpLog( LOG_ERR, "Received NULL from stack" );
        }
    }
    return 0;
}
