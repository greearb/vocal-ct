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

static const char* const autoCall_cxx_version =
    "$Id: autoCall.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include <cstdlib>
#include <fstream>

#include "cpLog.h"
#include "Sptr.hxx"
#include "Data.hxx"
#include "VovidaSipStack.hxx"
#include "RegisterMsg.hxx"
#include "TimeFunc.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;

struct MyConfig
{
	string toAddress;
	string port;
	string displayName;
	string userName;
	string registerExpires;
};

RegisterMsg createRegister( MyConfig& config )
{
    RegisterMsg registerMsg;

    // Set Request line
    SipRequestLine& reqLine = registerMsg.getMutableRequestLine();
    Data reqUrlString = Data( string("sip:") + config.toAddress );

#if 0
    if(config.getValue(SipTransportTag) == "TCP")
    {
        reqUrlString += ";transport=tcp;";
    }
#endif

    Sptr< SipUrl > reqUrl = new SipUrl( reqUrlString );
    assert( reqUrl != 0 );
    reqLine.setUrl( reqUrl );

    // Set From header
    string port = config.port;
    
    SipFrom sipfrom = registerMsg.getFrom();
    sipfrom.setDisplayName( config.displayName );
    sipfrom.setUser( config.userName );
    sipfrom.setHost(theSystem.gethostAddress());
    sipfrom.setPort( config.port );
    registerMsg.setFrom( sipfrom );

    // Set To header
    const Data regToUrlStr = Data( string("sip:") + config.toAddress);
    SipUrl regToUrl( regToUrlStr );
    SipTo sipto = registerMsg.getTo();
    sipto.setDisplayName( config.displayName );
    sipto.setUser( config.userName );
    sipto.setHost( regToUrl.getHost() );
    sipto.setPortData( regToUrl.getPort() );
    registerMsg.setTo( sipto );

    // Set Via header
    SipVia sipvia = registerMsg.getVia();
    sipvia.setPort( Data(port).convertInt() );
    sipvia.setTransport(  "UDP" );

    registerMsg.removeVia();
    registerMsg.setVia( sipvia );

    // Set Contact header
    Sptr< SipUrl > contactUrl = new SipUrl;
    contactUrl->setUserValue( config.userName, "phone" );
    contactUrl->setHost( Data( theSystem.gethostAddress() ) );
    contactUrl->setPort( atoi( port.c_str() ) );

#if 0
    if(config.getValue(SipTransportTag) == "TCP")
    {
        contactUrl->setTransportParam( Data("tcp") );
    }
#endif
    SipContact myContact;
    myContact.setUrl( contactUrl );
    registerMsg.setNumContact( 0 );
    registerMsg.setContact(myContact);

    // Set Expires header
    SipExpires sipExpires;
    sipExpires.setDelta( config.registerExpires );
    registerMsg.setExpires( sipExpires );
    return registerMsg;
}


// looping code

void processReceiveCall( SipTransceiver& stack)
{
    MyConfig reg1001;

    reg1001.toAddress = theSystem.gethostAddress();
    reg1001.port = "5002";
    reg1001.displayName = "User1001";
    reg1001.userName = "1001";
    reg1001.registerExpires = "3600";

    RegisterMsg register1001 = createRegister( reg1001 );

    // Send REGISTER
    stack.sendAsync( register1001 );

    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;

    // Get response
    while ( 1 )
    {
	sipRcv = stack.receive();
	if ( sipRcv != 0 )
	{
	    sipMsg = sipRcv->back();
	    if ( sipMsg != 0 )
	    {
		// do stuff
	    }
	}
	else
	{
	    cpLog( LOG_ERR, "Received NULL from stack" );
	}
    }
}


void processSendCall( SipTransceiver& stack)
{
    MyConfig reg1000;
    reg1000.toAddress = theSystem.gethostAddress();

    reg1000.port = "5000";

    reg1000.displayName = "User1000";

    reg1000.userName = "1000";

    reg1000.registerExpires = "3600";


    RegisterMsg register1000 = createRegister( reg1000 );

    // Send REGISTER
    stack.sendAsync( register1000 );

    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;

    // Get response
    while ( 1 )
    {
	sipRcv = stack.receive();
	if ( sipRcv != 0 )
	{
	    sipMsg = sipRcv->back();
	    if ( sipMsg != 0 )
	    {
		// do stuff
	    }
	}
	else
	{
	    cpLog( LOG_ERR, "Received NULL from stack" );
	}
    }

}


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

    SipTransceiver stack1000( "sendRegister", 5000 );

    SipTransceiver stack1001( "sendRegister", 5002 );




/*
  struct MyConfig
  {
	string toAddress;
	string port;
	string displayName;
	string userName;
	string registerExpires;
};
*/







    exit( 0 );
}
