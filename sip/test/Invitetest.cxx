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

static const char* const Invitetest_cxx_Version =
    "$Id: Invitetest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Data.hxx>
#include <cpLog.h>
#include <parse3tuple.h>
#include "InviteMsg.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "ByeMsg.hxx"
#include "CancelMsg.hxx"
#include "RegisterMsg.hxx"
#include "SipTransceiver.hxx"
#include "SipContact.hxx"
#include "SipContact.hxx"

using namespace Vocal;

int main()
{
    //The url is :
    //goofy@vovida.com
    cpLogSetLabel ("Invitetest");
    cpLogSetPriority (LOG_DEBUG);

/**
    parse3tuple ("SystemInfo.cfg", parseSystemInfo);

      Data url = " INVITE sip:goofy@vovida.com SIP/2.0\r\n";
      url+= "Via: SIP/2.0/UDP csvax.cs.caltech.edu;branch=8348\r\n";
      url+= "From: goofy <sip:mjh@isi.edu>\r\n";
      url+= "To: goofy2<sip:schooler@caltech.edu>\r\n";
      url+= "Call-ID: 234567\r\n";
      url+= "CSeq: 1 INVITE\r\n";
      url+= "Subject: SIP..\r\n";
      url+= "Content-Type: application/sdp\r\n";
      url+= "Content-Length:187\r\n";
      url+= "\r\n";
      url+= "v=0\r\n";
      url+= "o=user1 3456 1234 IN IP4 128.3.4.5\r\n";
      url+= "c=IN IP4 128.3.4.5\r\n";
      url+= "m=audio 3456 RTP/AVP 0\r\n";
    */


    //if the above works, try this:

    //Data url = "goofy:goofy_password@192.168.5.12:5060;transport=udp;user=phone;tag=4567";

    Data url;
    //send invite to lab2
    url = "sip:destination@192.168.5.130";
    Sptr <BaseUrl> tourl;
    tourl = BaseUrl::decode(url);

    if (tourl.getPtr() != 0)
    {
	if (tourl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
    
	    string h = (sipUrl.dynamicCast(tourl))->getHost().convertString();
	    string p = (sipUrl.dynamicCast(tourl))->getPort().convertString();
	    cout << "host is " << h << endl;
	    cout << "port is " << p << endl;
	}
    }

    int listenPort = 5060;
    int rtpPort = 3456;
    cpLog(LOG_DEBUG, "Completed the Url decoding");

    InviteMsg invite(tourl, listenPort, rtpPort);




    //set the contact field.

    Data cturl = "sip:contact@192.168.5.110";
    Sptr <BaseUrl> contactUrl;
    contactUrl = BaseUrl::decode(cturl);

    SipContact contact;
    contact.setUrl(contactUrl);
    invite.setContact(contact);


    Data msg = invite.encode();

    cout << "Invite Msg";
    cout << endl;

    string strmsg = msg.convertString();
    cout << strmsg;
    cout << endl;
    SipTransceiver trans;
    trans.sendAsync(invite);




#if 0
    //decode the Invite and check.
    InviteMsg invite2(msg);

    //encode the invite2.
    Data msg2 = invite2.encode();
    cout << "Invite Msg after decoding";

    string strmsg2 = msg2.convertString();
    cout << strmsg2;
    cout << endl;

    SipTransceiver trans;  //defaulted to 5060
    trans.sendReply(invite);
    trans.sendAsync(invite);

    /**  SipMsg* tempmsg = SipMsg::decode( msg );
     
      if ( tempmsg )
        {
          Data newMsg = tempmsg->encode();
     
          cout << "decoded and reencoded message is \n";
          cout << newMsg.logData() << endl;
        }
     
      cout << "decoded and reencoded message is \n";
            cout << newMsg.logData() << endl;
     
    */
    //form the status msg.
#endif

    StatusMsg status(invite, 200);
    trans.sendReply(status);

    Data Statusmsg = status.encode();

    cout << "Status Msg";
    cout << endl;

    string statusStr = Statusmsg.convertString();
    cout << statusStr;
    cout << endl;
    trans.sendReply(status);
#if 0
    //decode.
    StatusMsg statusmsg2(Statusmsg);

    Data Statusmsg2 = statusmsg2.encode();
    cout << "StatusMsg after decoding" << endl;
    string statusStr2 = Statusmsg2.convertString();
    cout << statusStr2;
    cout << endl;
    trans.sendAsync(Statusmsg);
#endif

    //send ACK.
    AckMsg ack(status);

    Data ackmsg = ack.encode();

    string ackstr = ackmsg.convertString();
    cout << "Ack Msg";
    cout << endl;
    cout << ackstr;
    cout << endl;
    trans.sendAsync(ack);
#if 0
    AckMsg ackMsg2(ackmsg);
    Data ackmsg2 = ackMsg2.encode();
    string ackstr2 = ackmsg2.convertString();

    cout << "Ack after decoding";
    cout << ackstr2;
    cout << endl;


    //test for receiving a  Command.

    ByeMsg bye(ack);

    Data byemsg = bye.encode();

    string byeStr = byemsg.convertString();

    cout << "Bye msg" << endl;

    cout << byeStr;
    cout << endl;

    //decode Bye
    ByeMsg bye2(byemsg);
    Data byemsg2 = bye2.encode();
    string byestr = byemsg2.convertString();
    cout << "Bye msg after decoding" << endl;
    cout << byestr;
    cout << endl;


    //test for receiving a Status.
    ByeMsg bye2(status);

    Data byemsg2 = bye2.encode();

    string bye2Str = byemsg2.convertString();

    cout << "Bye msg on status" << endl;

    cout << bye2Str;

    cout << endl;
    CancelMsg cancel(invite);

    Data cancelmsg = cancel.encode();

    string cancelstr = cancelmsg.convertString();

    cout << "Cancel msg" << endl;

    cout << cancelstr;

    cout << endl;
    //decode

    CancelMsg cancelmsg2(cancelmsg);
    Data cancel2 = cancelmsg2.encode();
    string cancelstr2 = cancel2.convertString();
    cout << "Cancel msg after decoding" << endl;
    cout << cancelstr2;


    cout << endl;
#endif
    //Register msg

    RegisterMsg reg;
    reg.setContact(contact);
    Data registermsg = reg.encode();

    string regstr = registermsg.convertString();

    cout << "Register msg" << endl;

    cout << regstr;

    cout << endl;
#if 0
    //decode
    RegisterMsg regmsg(registermsg);
    Data regmsg2 = regmsg.encode();
    string regstr2 = regmsg2.convertString();
    cout << "Reg after decoding" << endl;
    cout << regstr2 << endl;

#endif

    return 1;
}
