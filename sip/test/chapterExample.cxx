#include "Data.hxx"
#include "InviteMsg.hxx"
#include "SipSdp.hxx"
#include "SipVia.hxx"

using namespace Vocal;
using namespace Vocal::SDP;

char* example = 
"INVITE sip:6713@192.168.26.180:6060;user=phone SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.22.36:6060\r\n\
From: UserAgent<sip:6710@192.168.22.36:6060;user=phone>\r\n\
To: 6713<sip:6713@192.168.26.180:6060;user=phone>\r\n\
Call-ID: 96561418925909@192.168.22.36\r\n\
CSeq: 1 INVITE\r\n\
Subject: VovidaINVITE\r\n\
Contact: <sip:6710@192.168.22.36:6060;user=phone>\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 168\r\n\
\r\n\
v=0\r\n\
o=- 238540244 238540244 IN IP4 192.168.22.36\r\n\
s=VOVIDA Session\r\n\
c=IN IP4 192.168.22.36\r\n\
t=3174844751 0\r\n\
m=audio 23456 RTP/AVP 0\r\n\
a=rtpmap:0 PCMU/8000\r\n\
a=ptime:20\r\n\
\r\n";


/* the following code is an example of how to create a SIP message
   using the VOCAL SIP API.  The message created will closely resemble
   the example message, above.  Not all of the code below is strictly
   necessary -- specifically, a number of the headers, such as the Via
   list, the Subject, and the like are filled out with reasonable
   default values for user agents.

   in a real example, you would probably call
   SipTransciever::sendAsync() to send off the message via the network
   instead of encode()ing it and printing it out.

*/

Data exampleEncode()
{

    Sptr<SipUrl> toUrl 
	= new SipUrl(Data("sip:6713@192.168.26.180:6060;user=phone"));

    Sptr<SipUrl> myUrl 
	= new SipUrl(Data("sip:6710@192.168.26.180:6060;user=phone"));


    // this sets the To: URL and the Request-URI to toUrl.
    // in addition, the constructor sets default values for many of the
    // headers
    InviteMsg msg(toUrl, 6060);

    // set the From: to myUrl with display name "UserAgent"
    SipFrom myFrom;
    myFrom.setDisplayName("UserAgent");
    myFrom.setUrl(myUrl);
    msg.setFrom(myFrom);

    // set the Call ID -- normally, you should let the InviteMsg
    // constructor create the call id for you
    SipCallId myId;
    myId.setHost("192.168.22.36");
    myId.setLocalId("96561418925909");
    msg.setCallId(myId);

    // set the Contact
    SipContact myContact;
    myContact.setUrl(myUrl);

    msg.setNumContact(0);
    msg.setContact(myContact);

    // set the Via

    SipVia myVia;
    myVia.setHost("192.168.22.36");
    myVia.setPort(6060);

    msg.setNumVia(0);
    msg.setVia(myVia);


    // set the SDP information

    SdpSession mySession;

    mySession.setUserName("-");
    mySession.setAddress("192.168.22.36");
    mySession.setSessionId(238540244);
    mySession.setVersion(238540244);

    mySession.setSessionName("VOVIDA Session");

    SdpConnection myConnection;
    myConnection.setUnicast("192.168.22.36");
    mySession.setConnection(myConnection);

    SdpMedia* myMedia = new SdpMedia;

    myMedia->setPort(23456);
    myMedia->setMediaType(MediaTypeAudio);
    myMedia->setTransportType(TransportTypeRTP);


    ValueAttribute* myValue = new ValueAttribute();
    myValue->setAttribute( "ptime" );
    myValue->setValue( "20" );


    SdpRtpMapAttribute* myRtpAttrib = new SdpRtpMapAttribute();
    myRtpAttrib->setPayloadType( 0 );
    myRtpAttrib->setEncodingName( "PCMU" );
    myRtpAttrib->setClockRate( 8000 );

    MediaAttributes* myAttrib = new MediaAttributes();
    myAttrib->addValueAttribute(myValue);
    myAttrib->addmap(myRtpAttrib);
    myMedia->setMediaAttributes(myAttrib);

    mySession.addMedia(myMedia);

    Sptr<SipSdp> mySdp;

    mySdp.dynamicCast(msg.getContentData(0));

    if(mySdp != 0)
    {
	mySdp->setSdpDescriptor(mySession);
    }
    else
    {
	// no good!
	cout << "no good!\n";
    }

    return msg.encode();
}


/* here is a simple SIP URL output function */

void outputSipUrl(Sptr<BaseUrl> url)
{
    if(url->getType() == SIP_URL)
    {
	// this code assumes that the URL is a SIP URL.  Again, we
	// must dynamicCast to get at the SipUrl specific methods.
	Sptr<SipUrl> sipUrl;
	sipUrl.dynamicCast(url);
	
	cout
	    << "    User: " << sipUrl->getUserValue() << "\n"
	    << "    Host: " << sipUrl->getHost() << "\n"
	    << "    Port: " << sipUrl->getPort() << "\n";
    }
}



/* the following code shows how to decode the above example message.
   Normally, the message would be what you would get returned to you via
   SipTransciever::receive() .  
*/


void exampleDecode()
{
    Sptr<SipMsg> msg = SipMsg::decode(Data(example));

    // at this point, msg is of type Sptr<SipMsg>, so we don't know if
    // it's an INVITE or not.

    if(msg->getType() == SIP_INVITE)
    {
	// if the message is an INVITE, this code gets run:

	Sptr<InviteMsg> invite;
	invite.dynamicCast(msg);

	// now, invite is a sptr to msg but of type InviteMsg.

	Sptr<BaseUrl> url = invite->getRequestLine().getUrl();

	if(url->getType() == SIP_URL)
	{
	    // this code assumes that the URL is a SIP URL.  Again, we
	    // must dynamicCast to get at the SipUrl specific methods.
	    Sptr<SipUrl> sipUrl;
	    sipUrl.dynamicCast(url);

	    cout << "Request-URI\n"
		 << "    User: " << sipUrl->getUserValue() << "\n"
		 << "    Host: " << sipUrl->getHost() << "\n"
		 << "    Port: " << sipUrl->getPort() << "\n";
	}

	// now, get the From: line

	SipFrom from = invite->getFrom();

	cout << "From\n"
	     << "    Display Name: " << from.getDisplayName() << "\n";
	outputSipUrl(from.getUrl());

	// same thing for the To: line

	SipTo to = invite->getTo();

	cout << "To\n"
	     << "    Display Name: " << to.getDisplayName() << "\n";
	outputSipUrl(to.getUrl());

	// get the 0th via
	SipVia via = invite->getVia(0);

	cout << "Via\n"
	     << "    Host: " << via.getHost() << "\n"
	     << "    Port: " << via.getPort() << "\n";


	// get the Contact

	SipContact contact = invite->getContact(0);
	cout << "Contact\n";
	outputSipUrl(contact.getUrl());

	// now, for the SDP
	Sptr<SipSdp> remoteSdp;
	remoteSdp.dynamicCast (invite->getContentData(0));
	if(remoteSdp != 0)
	{
	    // we hope this SDP is the remote one
	    
	    // we can use the simplified SDP interface to get useful
	    // information
	    
	    cout << "SDP\n"
		 << "    Remote Address: " 
		 << remoteSdp->getConnAddress() << "\n"
		 << "    Remote RTP Port: " << remoteSdp->getRtpPort() << "\n"
		 << endl;

	    // to get more detailed information, we need to get an
	    // SdpSession and look there.  Here is one example of
	    // doing that:

	    SdpSession session = remoteSdp->getSdpDescriptor();

	    SdpMedia* media = session.getMediaList().front(); // get the 0th element
	    if(media != 0)
	    {
		MediaAttributes* attrib = media->getMediaAttributes();
		if(attrib != 0)
		{
		    vector < ValueAttribute* > *valueVector =
			attrib->getValueAttributes();
		    if(valueVector != 0)
		    {
			cout << "  Attribute: " 
			     << ((*valueVector)[0])->getAttribute() 
			     << " Value: "
			     << ((*valueVector)[0])->getValue()
			     << "\n";
		    }
		}
	    }
	}


    }

}



int main()
{
    cout << exampleEncode() << endl;

    cout << endl;

    exampleDecode();
    return 0;
}
