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

static const char* const InviteMsgTest_cxx_Version =
    "$Id: InviteMsgTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipAuthorization.hxx"
#include <cassert>
#include "cpLog.h"
#include "InviteMsg.hxx"
#include "Verify.hxx"
#include "Sptr.hxx"
#include "BaseUrl.hxx"
#include "SipUser.hxx"
#include "SipVia.hxx"
#include "SipSdp.hxx"
#include "SipDiversion.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;
    
//message sent by telecaster
char* msgtext1[] = { 
        "INVITE sip:5452@172.19.174.171:5060;user=phone SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.4\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=d1faef3fff22fbedaa794b8ea2abc7e5.2\r\n\
Via: SIP/2.0/UDP 172.19.174.170:5060\r\n\
From: \"5451\"<sip:5451@172.19.175.150>\r\n\
To: <sip:5452@172.19.175.150;user=phone>\r\n\
Call-ID: c4943000-75748-7058-2e323731@172.19.174.170\r\n\
CSeq: 101 INVITE\r\n\
Expires: 180\r\n\
Accept: application/sdp\r\n\
Record-Route: <sip:5452@172.19.175.150:5060;maddr=172.19.175.150>,<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>\r\n\
Contact: <sip:5451@172.19.174.170:5060>\r\n\
Content-Length: 228\r\n\
User-Agent: Cisco IP Phone/ Rev. 1/ SIP enabled\r\n\
Content-Type: application/sdp\r\n\
\r\n\
v=0\r\n\
o=CiscoSystemsSIP-IPPhone-UserAgent 21803 10790 IN IP4 172.19.174.170\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.174.170\r\n\
t=0 0\r\n\
m=audio 28844 RTP/AVP 0 8 18 100\r\n\
a=rtpmap:0 pcmu/8000\r\n\
a=rtpmap:100 telephone-event/8000\r\n\
a=fmtp:100 0-11\r\n\
\r\n",
// this is the second message
"INVITE sip:7013000@172.19.175.70:5060;user=phone;phone-context=unknown SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.175.71:5060;branch=c419c2f0a1077bdf16b34df6384becc5.1\r\n\
Via: SIP/2.0/UDP 172.19.175.226:52901\r\n\
From: \"7010300\"<sip:7010300@172.19.175.226>\r\n\
To: <sip:7013000@172.19.175.71;user=phone;phone-context=unknown>\r\n\
Call-ID: 5CF43908-1A820032-0-4D8E29AC@172.19.175.226\r\n\
CSeq: 101 INVITE\r\n\
Proxy-Authorization: Basic VovidaClassXSwitch\r\n\
Max-Forwards: 5\r\n\
Expires: 60\r\n\
Record-Route: <sip:7013000@172.19.175.71:5060;maddr=172.19.175.71>\r\n\
Contact: <sip:7010300@172.19.175.226:5060;user=phone>\r\n\
Content-Length: 138\r\n\
Timestamp: 732245189\r\n\
User-Agent: Cisco VoIP Gateway/ IOS 12.x/ SIP enabled\r\n\
Content-Type: application/sdp\r\n\
\r\n\
v=0\r\n\
o=CiscoSystemsSIP-GW-UserAgent 6459 1977 IN IP4 172.19.175.226\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.175.226\r\n\
t=0 0\r\n\
m=audio 20752 RTP/AVP 0\r\n\
\r\n",
"INVITE sip:vivekg@chair.dnrc.bell-labs.com SIP/2.0
TO :
 sip:vivekg@chair.dnrc.bell-labs.com ;   tag    = 1a1b1f1H33n
From   : \"J Rosenberg \\\\\\\"\" <sip:jdrosen@lucent.com>
  ;
  tag = 98asjd8
CaLl-Id
 : 0ha0isndaksdj@10.1.1.1
cseq: 8
  INVITE
Via  : SIP  /   2.0 
 /UDP 
    135.180.130.133
Subject :
NewFangledHeader:   newfangled value
 more newfangled value
Content-Type: application/sdp
v:  SIP  / 2.0  / TCP     12.3.4.5   ;
  branch  =   9ikj8  ,
 SIP  /    2.0   / UDP  1.2.3.4   ; hidden   
m:\"Quoted string \\\"\\\"\" <sip:jdrosen@bell-labs.com> ; newparam = newvalue ;
  secondparam = secondvalue  ; q = 0.33
  ,
 tel:4443322

v=0
o=mhandley 29739 7272939 IN IP4 126.5.4.3
s= 
c=IN IP4 135.180.130.88
m=audio 49210 RTP/AVP 0 12
m=video 3227 RTP/AVP 31
a=rtpmap:31 LPC/8000

",
        "INVITE sip:5452@172.19.174.171;user=phone SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.174.170:5060\r\n\
From: \"5451\"<sip:5451@172.19.175.150;user=phone>\r\n\
To: <sip:5452@172.19.175.150;user=phone>\r\n\
Call-ID: c4943000-75748-7058-2e323731@172.19.174.170\r\n\
CSeq: 101 INVITE\r\n\
Expires: 180\r\n\
Accept: application/sdp\r\n\
Record-Route: <sip:5452@172.19.175.150:5060;maddr=172.19.175.150>,<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>\r\n\
Contact: <sip:5451@172.19.174.170:5060>\r\n\
Diversion: <sip:5451@172.19.174.170:5060>;reason=no-answer\r\n\
Content-Length: 228\r\n\
User-Agent: Cisco IP Phone/ Rev. 1/ SIP enabled\r\n\
Content-Type: application/sdp\r\n\
\r\n\
v=0\r\n\
o=CiscoSystemsSIP-IPPhone-UserAgent 21803 10790 IN IP4 172.19.174.170\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.174.170\r\n\
t=0 0\r\n\
m=audio 28844 RTP/AVP 0 8 18 100\r\n\
a=rtpmap:0 pcmu/8000\r\n\
a=rtpmap:100 telephone-event/8000\r\n\
a=fmtp:100 0-11\r\n\
\r\n"

};    
    
    
    
    
    

void testEncodeDecode()
{
    {
	string host_addr = theSystem.gethostAddress();
	Data url;
	url = "sip:destination@[fe80::202:96ff:fe01:1d7e]:5060";
	Sptr <BaseUrl> baseurl = BaseUrl::decode(url, host_addr);
	Sptr <SipUrl> tourl;
	tourl.dynamicCast(baseurl);
	
	cout << tourl->getHost() << endl;
	test_verify(tourl->getHost() == "[fe80::202:96ff:fe01:1d7e]");
	int listenPort = 5060;
	int rtpPort = 3456;
	cpLog(LOG_DEBUG, "Completed the Url decoding");
	
	InviteMsg invite(tourl, host_addr, listenPort, rtpPort);
	
	Data data = invite.encode();
	cout << " before decoding " << data.logData() << endl;
	InviteMsg newInvite(data, host_addr);
	test_verify (invite.getRequestLine().getHost() == "[fe80::202:96ff:fe01:1d7e]");
    }

    string host_addr = theSystem.gethostAddress();
    Data url;
    url = "sip:destination@192.168.5.12";
    Sptr <BaseUrl> baseurl = BaseUrl::decode(url, host_addr);
    Sptr <SipUrl> tourl;
    tourl.dynamicCast(baseurl);
    
    int listenPort = 5060;
    int rtpPort = 3456;
    cpLog(LOG_DEBUG, "Completed the Url decoding");

    InviteMsg invite(tourl, host_addr, listenPort, rtpPort);

    Data data = invite.encode();
    //cout << " before decoding " << data.logData() << endl;
    InviteMsg newInvite(data, host_addr);

    Data newdata = newInvite.encode();
    
    InviteMsg testInvite(newdata, host_addr);
    
    //cout << "New Invite Msg after encoding " << newdata.logData() << endl;
    
    
    test_verify (newInvite == testInvite);
    
    //test for telecaster msg

    Data tempInvite(msgtext1[0]);
    
    InviteMsg invite1(tempInvite, host_addr);
    
    Data invStr1 = invite1.encode();
    
    InviteMsg testInvite1(invStr1, host_addr);
    
     test_verify (invite1 == testInvite1); 
    
    //cout << "Invite msg - for telecaster msg BEFORE decoding: " << tempInvite.logData() << endl;
    //cout << "Invite msg - for telecaster msg AFTER decoding: " << invStr1.logData() << endl;
    
    //test for callgen msg
    Data tempInvite2(msgtext1[1]);
    
    InviteMsg invite2(tempInvite2, host_addr);
    
    Data invStr2 = invite2.encode();
    
    InviteMsg testInvite2(invStr2, host_addr);
    
    test_verify( testInvite2 == invite2);
    
    //cout << "Invite msg - for call gen msg BEFORE decoding: " << tempInvite2.logData() << endl;
    
     // cout << "Invite msg - for call gen msg AFTER decoding: " << invStr2.logData() << endl;
    
    
#if 0
    SipTo to(" <sip:7013000@172.19.175.71;user=phone;phone-context=unknown>");
    
    cout << to.encode().logData() << endl;
    
    SipRecordRoute rec("<sip:7013000@172.19.175.71:5060;maddr=172.19.175.71>");
    cout << rec.encode().logData() << endl;
    
    
    SipContact cont( "<sip:7010300@172.19.175.226:5060;user=phone>");
    cout << cont.encode().logData() << endl;
#endif


    {
	InviteMsg invite(msgtext1[2], host_addr);
	cout << invite.encode() << endl;
    }

    {

	
    }
}


char* msgtext[] = 
{ 
    "INVITE sip:5452@172.19.174.171:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.4\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=d1faef3fff22fbedaa794b8ea2abc7e5.2\r\n\
Via: SIP/2.0/UDP 172.19.174.170:5060\r\n\
From: \"5451\"<sip:5451@172.19.175.150>\r\n\
To: <sip:5452@172.19.175.150>\r\n\
Call-ID: c4943000-75748-7058-2e323731@172.19.174.170\r\n\
CSeq: 101 INVITE\r\n\
Expires: 180\r\n\
Accept: application/sdp\r\n\
Record-Route: <sip:5452@172.19.175.150:5060;maddr=172.19.175.150>,<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>\r\n\
Contact: <sip:5451@172.19.174.170:5060>,<sip:6000@172.19.174.170:5060>,<sip:7000@172.19.174.170:5060>\r\n\
Content-Length: 228\r\n\
User-Agent: Cisco IP Phone/ Rev. 1/ SIP enabled\r\n\
Content-Type: application/sdp\r\n\
\r\n\
v=0\r\n\
o=CiscoSystemsSIP-IPPhone-UserAgent 21803 10790 IN IP4 172.19.174.170\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.174.170\r\n\
t=0 0\r\n\
m=audio 28844 RTP/AVP 0 8 18 100\r\n\
a=rtpmap:0 pcmu/8000\r\n\
a=rtpmap:100 telephone-event/8000\r\n\
a=fmtp:100 0-11\r\n\
\r\n",
    "INVITE sip:5452@172.19.174.171:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.4\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=d1faef3fff22fbedaa794b8ea2abc7e5.2\r\n\
Via: SIP/2.0/UDP 172.19.174.170:5060\r\n\
From: \"5451\"<sip:5451@172.19.175.150>\r\n\
To: <sip:5452@172.19.175.150>\r\n\
Call-ID: c4943000-75748-7058-2e323731@172.19.174.170\r\n\
CSeq: 101 INVITE\r\n\
Expires: 180\r\n\
Accept: application/sdp\r\n\
Record-Route: <sip:5452@172.19.175.150:5060;maddr=172.19.175.150>,<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>\r\n\
Contact: <sip:5451@172.19.174.170:5060>,<sip:6000@172.19.174.170:5060>,<sip:7000@172.19.174.170:5060>\r\n\
Content-Length: 228\r\n\
User-Agent: Cisco IP Phone/ Rev. 1/ SIP enabled\r\n\
Content-Type: application/sdp\r\n\
\r\n\
v=0\r\n\
o=CiscoSystemsSIP-IPPhone-UserAgent 21803 10790 IN IP4 172.19.174.170\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.174.170\r\n\
t=0 0\r\n\
m=audio 28844 RTP/AVP 0 8 18 100\r\n\
a=rtpmap:0 pcmu/8000\r\n\
a=rtpmap:100 telephone-event/8000\r\n\
a=fmtp:100 0-11\r\n",
    "INVITE sip:bko@bko-lnx:5070 SIP/2.0\r
Via: SIP/2.0/UDP 128.107.140.140:5060\r
From: Phone<sip:6399@128.107.140.140:5060;user=phone>\r
To: bko<sip:bko@bko-lnx:5070>\r
Call-ID: 922b14475cfd3fbe10fd3fbec27b0608@128.107.140.140\r
CSeq: 1 INVITE\r
Subject: VovidaINVITE\r
Contact: <sip:6399@128.107.140.140:5060;user=phone>\r
Content-Length: 230\r
\r
v=0\r
o=- 1943909507 1943909507 IN IP4 128.107.140.140\r
s=VOVIDA Session\r
c=IN IP4 128.107.140.140\r
t=3195510838 0\r
m=audio 10000 RTP/AVP 0 101\r
a=rtpmap:0 PCMU/8000\r
a=rtpmap:101 telephone-event/8000\r
a=ptime:20\r
a=fmtp:101 0-11\r
",
    0
};     


char* contactList[] = 
{
    "Contact: <sip:5451@172.19.174.170:5060>\r\n",
    "Contact: <sip:6000@172.19.174.170:5060>\r\n",
    "Contact: <sip:7000@172.19.174.170:5060>\r\n",
    0
};


char* viaList[] =
{
    "Via: SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.4\r\n",
    "Via: SIP/2.0/UDP 172.19.175.150:5060;branch=d1faef3fff22fbedaa794b8ea2abc7e5.2\r\n",
    "Via: SIP/2.0/UDP 172.19.174.170:5060\r\n",
    0
};


void testDecode()
{
    string host_addr = theSystem.gethostAddress();
    {
	InviteMsg invite(msgtext[2], host_addr);
    }
    {
	InviteMsg invite(msgtext[1], host_addr);

	SipContactList list = invite.getContactList();

	int j = 0;
	SipContactList::iterator i;
	for(i = list.begin(); i != list.end(); ++i)
	{
	    test_verify((*i)->encode() == contactList[j]);
	    j++;
	}

	SipViaList vias = invite.getViaList();

	j = 0;
	SipViaList::iterator k;
	for(k = vias.begin(); k != vias.end(); ++k)
	{
	    test_verify((*k)->encode() == viaList[j]);
	    j++;
	}

	test_verify(list.begin() != list.end());
	test_verify(!list.empty());

	Sptr<BaseUrl> burl = invite.getRequestLine().getUrl();
	Sptr<SipUrl> url;

	url.dynamicCast(burl);

	test_verify(url != 0);

    }
    {
	InviteMsg empty("", host_addr);
	SipContactList list = empty.getContactList();

	test_verify(list.begin() == list.end());
	test_verify(list.empty());
    }
    {
	InviteMsg invite1(msgtext[1], host_addr);
	InviteMsg invite2(msgtext[1], host_addr);

	test_verify(invite1 == invite2);
	invite2.forceParse();
	test_verify(invite1 == invite2);
	if(invite1 != invite2)
	{
	    cout << invite1.encode();
	    cout << "-------------------------------------\n";
	    cout << invite2.encode();
	}
	SipFrom from = invite2.getFrom();
	Sptr<BaseUrl> burl = from.getUrl();
	Sptr<SipUrl> url;
	url.dynamicCast(burl);
	Sptr<BaseUser> user = new SipUser("asdfasfd");
	url->setUser(user);
	from.setUrl(url);
	invite2.setFrom(from);
	test_verify(invite1 != invite2);
	InviteMsg invite3(msgtext[0], host_addr);

	invite3.encode();
	test_verify(invite1 == invite3);
    }
    {
	InviteMsg invite1(msgtext[1], host_addr);
	Data text = invite1.encode();
	SipVia via = invite1.getVia(0);
	invite1.removeVia(0);
	invite1.setVia(via, 0);
	test_verify(text == invite1.encode());
//	cout << "hi" << endl;
//	cout << invite1.encode() << endl;
    }
    {
	InviteMsg invite1(msgtext[1], host_addr);
	Data text = invite1.encode();
	SipVia via = invite1.getVia(1);
	invite1.removeVia(1);
	invite1.setVia(via, 1);
	test_verify(text == invite1.encode());
//	cout << "hi2" << endl;
//	cout << invite1.encode() << endl;
    }

}


void testDuplicate()
{
    string host_addr = theSystem.gethostAddress();
    // test duplication of an InviteMsg

    Sptr<SipUrl> toUrl = new SipUrl(string("sip:bko@bko-lnx:5070"), host_addr);
    InviteMsg msg( toUrl, host_addr, 5060 );

    Sptr<SipContentData> content = msg.getContentData(0);

    test_verify(content != 0);

    Sptr<SipSdp> sdp;
    sdp.dynamicCast(content);

    test_verify(sdp != 0);

    {
	InviteMsg copy(msg);
	
	Sptr<SipContentData> copy_content = msg.getContentData(0);
	
	test_verify(copy_content != 0);
	
	Sptr<SipSdp> copy_sdp;
	copy_sdp.dynamicCast(copy_content);
	
	test_verify(copy_sdp != 0);
    }

    {
	InviteMsg copy("", host_addr);
	copy = msg;
	
	Sptr<SipContentData> copy_content = msg.getContentData(0);
	
	test_verify(copy_content != 0);

#if 0	
	// this is not implemented
	Sptr<SipSdp> copy_sdp;
	copy_sdp.dynamicCast(copy_content);
	
	test_verify(copy_sdp != 0);
	test_verify(*copy_sdp == *sdp);
#endif
    }
    {
	// verify that the message has only a single MIME type
	test_verify(msg.getContentType() 
		    == SipContentType("application", "SDP",  host_addr));
	cout << msg.getContentType().encode()<< endl;

    }

    {
	// verify the encoding
	Data result = msg.encode();

	cout << result;
    }
    {
	test_verify(msg == msg);
	InviteMsg copy(msg);

	test_verify(copy == msg);
	InviteMsg copy2("", host_addr);
	copy2 = msg;
	test_verify(copy2 == msg);
	
    }

    {
	InviteMsg copy(msgtext1[5], host_addr);

	test_verify(copy.getFrom().getUser() == "5451");
	test_verify(copy.getTo().getUser() == "5452");

        SipDiversion dive = copy.getDiversion();
        cerr << "REASON:" << dive.getReason().c_str() << endl;

	Sptr<SipCallLeg> mySipCallLeg = new SipCallLeg(copy.computeCallLeg());
	
	//	test_verify(mySipCallLeg->getFrom().getUser() == "5451");
	//	test_verify(mySipCallLeg->getTo().getUser() == "5452");
    }

}


int main()
{
    testEncodeDecode();
    testDecode();
    testDuplicate();

    return test_return_code(33);
}
