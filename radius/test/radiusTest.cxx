/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2001 Vovida Networks, Inc.  All rights reserved.
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

static const char* const radiusTest_cxx_Version =
    "$Id: radiusTest.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <sys/types.h>
#include <netinet/in.h>
#include <cstdio>

#include "cpLog.h"
#include "Verify.hxx"
#include "RadiusMessage.hxx"


// RADIUS test (automatic test framework) program
int
main()
{
    cpLogSetPriority( LOG_DEBUG_STACK );
    // cpLogSetPriority( LOG_INFO );

    const char * const secret = "shhhh";
    u_int8_t identifier = 0;

    // Attributes
    RadiusAttribute attrUserName( RA_USER_NAME,
                                  reinterpret_cast<const u_int8_t *>("vovida"),
                                  6 );   // Test include \0 at the end

    RadiusAttribute attrUserPassword( RA_USER_PASSWORD,
                                      reinterpret_cast<const u_int8_t *>("01234567890123456"),
                                      17 );

    u_int32_t ip( htonl(0x80818283) );    // 128.129.130.131
    RadiusAttribute attrNasIpAddress( RA_NAS_IP_ADDRESS,
                                      reinterpret_cast<const u_int8_t *>(&ip),
                                      sizeof(ip) );

    u_int32_t port( htonl(18) );
    RadiusData valNasPort( &port , sizeof(port) );
    RadiusAttribute attrNasPort( RA_NAS_PORT, valNasPort );

    u_int32_t serviceType( htonl(RAST_LOGIN) );
    RadiusData valServiceType( &serviceType , sizeof(serviceType) );
    RadiusAttribute attrServiceType( RA_SERVICE_TYPE, valServiceType );

    char displayMessage[ RadiusMaxAttributeLength - 2 ];
    u_int32_t timeout( htonl(36000) );
    snprintf( displayMessage,
              sizeof(displayMessage),
              "Credit: %d",
              ntohl(timeout) );
    RadiusData valReplyMessage1( displayMessage, strlen(displayMessage) );
    RadiusAttribute attrReplyMessage1( RA_REPLY_MESSAGE, valReplyMessage1 );

    RadiusData valReplyMessage2( "Access Denied", 13 );
    RadiusAttribute attrReplyMessage2( RA_REPLY_MESSAGE, valReplyMessage2 );

    snprintf( displayMessage, sizeof(displayMessage), "Credit: %d", 0 );
    RadiusData valReplyMessage3( displayMessage, strlen(displayMessage) );
    RadiusAttribute attrReplyMessage3( RA_REPLY_MESSAGE, valReplyMessage3 );

    RadiusData valSessinTimeout( &timeout, sizeof(timeout) );
    RadiusAttribute attrSessionTimeout( RA_SESSION_TIMEOUT, valSessinTimeout );

    RadiusData valCallee( "555-1212", 9 );
    RadiusAttribute attrCallee( RA_CALLED_STATION_ID, valCallee );

    RadiusData valCaller( "Anonymous", 10 );
    RadiusAttribute attrCaller( RA_CALLING_STATION_ID, valCaller );

    u_int32_t acctType( htonl(RAS_START) );
    RadiusData valAcctStatus( &acctType, sizeof(acctType) );
    RadiusAttribute attrAcctStatusStart( RA_ACCT_STATUS_TYPE, valAcctStatus );

    acctType = htonl(RAS_INTERIM);
    RadiusData valAcctStatus2( &acctType, sizeof(acctType) );
    RadiusAttribute attrAcctStatusInterim( RA_ACCT_STATUS_TYPE, valAcctStatus2 );

    acctType = htonl(RAS_STOP);
    RadiusData valAcctStatus3( &acctType, sizeof(acctType) );
    RadiusAttribute attrAcctStatusStop( RA_ACCT_STATUS_TYPE, valAcctStatus3 );

    RadiusData valAcctSessionId( "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com", 49 );
    RadiusAttribute attrAcctSessionId( RA_ACCT_SESSION_ID, valAcctSessionId );

    u_int32_t duration( htonl(180) );
    RadiusData valAcctSessionTime( &duration, sizeof(duration) );
    RadiusAttribute attrAcctSessionTime( RA_ACCT_SESSION_TIME, valAcctSessionTime );

    u_int32_t cause( htonl(RATC_SESSION_TIMEOUT) );
    RadiusData valAcctTerminateCause( &cause, sizeof(cause) );
    RadiusAttribute attrAcctTerminateCause( RA_ACCT_TERMINATE_CAUSE, valAcctTerminateCause );

    u_int32_t portType( htonl(RANPT_VIRTUAL) );
    RadiusData valNasPortType( &portType , sizeof(portType) );
    RadiusAttribute attrNasPortType( RA_NAS_PORT_TYPE, valNasPortType );


    u_int8_t accessReqAuth[ RadiusAuthenticatorLength ];

    string resultRecv;
    string resultSend;


    cpLog( LOG_INFO, "\n-------------------- Test 1 ----------------------\n                  Access-Request\n" );

    // At the client side

    RadiusMessage accessReqMsg( RP_ACCESS_REQUEST );

    // Warning: assert is used for testing only
    assert( accessReqMsg.add( attrUserName ) );
    assert( accessReqMsg.add( attrUserPassword ) );
    assert( accessReqMsg.add( attrNasIpAddress ) );
    assert( accessReqMsg.add( attrServiceType ) );
    assert( accessReqMsg.add( attrNasPortType ) );

    accessReqMsg.setIdentifier( ++identifier );
    accessReqMsg.calcAuthenticator( secret );

    resultSend = accessReqMsg.hexDump();

    // At the server side

    RawMessage udpMsg1( accessReqMsg.data() );
    RadiusData msgReceived1( udpMsg1.buffer, ntohs(udpMsg1.msgHdr.length) );

    try
    {
        RadiusMessage accessReqMsg2( msgReceived1, secret );
        resultRecv = accessReqMsg2.hexDump();

        test_verify( resultSend == resultRecv );

        cpLog( LOG_INFO, "Verbose\n[%s]", accessReqMsg2.verbose().c_str() );
    }
    catch( VRadiusException &e )
    {
        cpLog( LOG_ERR, "Test failed - %s", e.getDescription().c_str() );
    }


    cpLog( LOG_INFO, "\n-------------------- Test 2 ----------------------\n                  Access-Accept\n" );

    memcpy( accessReqAuth,
            udpMsg1.msgHdr.authenticator,
            RadiusAuthenticatorLength );

    // At the server side

    RadiusMessage accessAcceptMsg( RP_ACCESS_ACCEPT,
                                   udpMsg1.msgHdr.identifier,
                                   accessReqAuth );

    RadiusAttribute attrClass( RA_CLASS, RadiusData( "Nas Ip:1.4.61.70", 16 ) );

    // Warning: assert is used for testing only
    assert( accessAcceptMsg.add( attrReplyMessage1 ) );
    assert( accessAcceptMsg.add( attrClass ) );
    assert( accessAcceptMsg.add( attrSessionTimeout ) );

    accessAcceptMsg.calcAuthenticator( secret );
    resultSend = accessAcceptMsg.hexDump();

    // At the client side

    RawMessage udpMsg2( accessAcceptMsg.data() );
    RadiusData msgReceived2( udpMsg2.buffer, ntohs(udpMsg2.msgHdr.length) );

    try
    {
        RadiusMessage accessAcceptMsg2( msgReceived2, secret );
        resultRecv = accessAcceptMsg2.hexDump();

        test_verify( accessAcceptMsg2.verifyResponseAuthenticator(accessReqAuth,
                                                                  secret) &&
                     resultSend == resultRecv );

        cpLog( LOG_INFO, "Verbose\n[%s]", accessAcceptMsg2.verbose().c_str() );
    }
    catch( VRadiusException &e )
    {
        cpLog( LOG_ERR, "Test failed - %s", e.getDescription().c_str() );
    }


    cpLog( LOG_INFO, "\n-------------------- Test 3 ----------------------\n                  Access-Reject\n" );

    // At the server side

    RadiusMessage accessRejectMsg( RP_ACCESS_REJECT,
                                   udpMsg1.msgHdr.identifier,
                                   accessReqAuth );

    // Warning: assert is used for testing only
    assert( accessRejectMsg.add( attrReplyMessage2 ) );
    assert( accessRejectMsg.add( attrReplyMessage3 ) );

    accessRejectMsg.calcAuthenticator( secret );
    resultSend = accessRejectMsg.hexDump();

    // At the client side

    RawMessage udpMsg3( accessRejectMsg.data() );
    RadiusData msgReceived3( udpMsg3.buffer, ntohs(udpMsg3.msgHdr.length) );

    try
    {
        RadiusMessage accessRejectMsg2( msgReceived3, secret );
        resultRecv = accessRejectMsg2.hexDump();

        test_verify( accessRejectMsg2.verifyResponseAuthenticator(accessReqAuth,
                                                                  secret) &&
                     resultSend == resultRecv );

        cpLog( LOG_INFO, "Verbose\n[%s]",
                         accessRejectMsg.verbose().c_str() );
    }
    catch( VRadiusException &e )
    {
        cpLog( LOG_ERR, "Test failed - %s", e.getDescription().c_str() );
    }


    cpLog( LOG_INFO, "\n-------------------- Test 4 ----------------------\n             Accounting-Requesst Start\n" );

    // At the client side

    RadiusMessage acctReqMsg( RP_ACCOUNTING_REQUEST );

    // Warning: assert is used for testing only
    assert( acctReqMsg.add( attrUserName ) );
    assert( acctReqMsg.add( attrNasIpAddress ) );
    assert( acctReqMsg.add( attrNasPort ) );
    assert( acctReqMsg.add( accessAcceptMsg.get( RA_CLASS ) ) );
    assert( acctReqMsg.add( attrCallee ) );
    assert( acctReqMsg.add( attrCaller ) );
    assert( acctReqMsg.add( attrAcctStatusStart ) );
    assert( acctReqMsg.add( attrAcctSessionId ) );

    acctReqMsg.setIdentifier( ++identifier );
    acctReqMsg.calcAuthenticator( secret );

    resultSend = acctReqMsg.hexDump();

    // At the server side

    RawMessage udpMsg4( acctReqMsg.data() );
    RadiusData msgReceived4( udpMsg4.buffer, ntohs(udpMsg4.msgHdr.length) );

    try
    {
        RadiusMessage acctReqMsg2( msgReceived4, secret );

        resultRecv = acctReqMsg2.hexDump();

        test_verify( resultSend == resultRecv );

        cpLog( LOG_INFO, "Verbose\n[%s]", acctReqMsg2.verbose().c_str() );
    }
    catch( VRadiusException &e )
    {
        cpLog( LOG_ERR, "Test failed - %s", e.getDescription().c_str() );
    }


    cpLog( LOG_INFO, "\n-------------------- Test 5 ----------------------\n               Accounting-Response\n" );

    // At the server side

    const u_int8_t* reqAuth = acctReqMsg.getAuthenticator();

    RadiusMessage acctRespMsg( RP_ACCOUNTING_RESPONSE,
                               udpMsg4.msgHdr.identifier,
                               reqAuth );
    acctRespMsg.calcAuthenticator( secret );

    resultSend = acctRespMsg.hexDump();

    // At the client side

    RawMessage udpMsg5( acctRespMsg.data() );
    RadiusData msgReceived5( udpMsg5.buffer, ntohs(udpMsg5.msgHdr.length) );
    try
    {
        RadiusMessage acctRespMsg2( msgReceived5, secret );

        if( acctRespMsg2.type() == RP_ACCOUNTING_RESPONSE )
        {
            if( acctRespMsg2.verifyResponseAuthenticator( reqAuth, secret ) )
            {
                resultRecv = acctRespMsg2.hexDump();

                test_verify( resultSend == resultRecv );

                cpLog( LOG_INFO, "Verbose\n[%s]", acctRespMsg2.verbose().c_str() );
            }
            else
            {
                cpLog( LOG_ERR, "Cannot verify Response Authenticator" );
            }
        }
    }
    catch( VRadiusException &e )
    {
        cpLog( LOG_ERR, "Test failed - %s", e.getDescription().c_str() );
    }


    cpLog( LOG_INFO, "\n-------------------- Test 6 ----------------------\n            Accounting-Requesst Interim\n" );

    // At the client side

    RadiusMessage acctInterimMsg( RP_ACCOUNTING_REQUEST );

    // Warning: assert is used for testing only
    assert( acctInterimMsg.add( acctReqMsg.get( RA_USER_NAME ) ) );
    assert( acctInterimMsg.add( acctReqMsg.get( RA_NAS_IP_ADDRESS ) ) );
    assert( acctInterimMsg.add( acctReqMsg.get( RA_NAS_PORT ) ) );
    assert( acctInterimMsg.add( accessAcceptMsg.get( RA_CLASS ) ) );
    assert( acctInterimMsg.add( acctReqMsg.get( RA_CALLED_STATION_ID ) ) );
    assert( acctInterimMsg.add( acctReqMsg.get( RA_CALLING_STATION_ID ) ) );
    assert( acctInterimMsg.add( attrAcctStatusInterim ) );
    assert( acctInterimMsg.add( acctReqMsg.get( RA_ACCT_SESSION_ID ) ) );

    acctInterimMsg.setIdentifier( ++identifier );
    acctInterimMsg.calcAuthenticator( secret );

    resultSend = acctInterimMsg.hexDump();

    // At the server side

    RawMessage udpMsg6( acctInterimMsg.data() );
    RadiusData msgReceived6( udpMsg6.buffer, ntohs(udpMsg6.msgHdr.length) );

    try
    {
        RadiusMessage acctReqMsg2( msgReceived6, secret );

        resultRecv = acctReqMsg2.hexDump();

        test_verify( resultSend == resultRecv );

        cpLog( LOG_INFO, "Verbose\n[%s]", acctReqMsg2.verbose().c_str() );
    }
    catch( VRadiusException &e )
    {
        cpLog( LOG_ERR, "Test failed - %s", e.getDescription().c_str() );
    }


    cpLog( LOG_INFO, "\n-------------------- Test 7 ----------------------\n             Accounting-Requesst Stop\n" );

    // At the client side

    RadiusMessage acctStopMsg( RP_ACCOUNTING_REQUEST );

    // Warning: assert is used for testing only
    assert( acctStopMsg.add( acctReqMsg.get( RA_USER_NAME ) ) );
    assert( acctStopMsg.add( acctReqMsg.get( RA_NAS_IP_ADDRESS ) ) );
    assert( acctStopMsg.add( acctReqMsg.get( RA_NAS_PORT ) ) );
    assert( acctStopMsg.add( acctReqMsg.get( RA_CALLED_STATION_ID ) ) );
    assert( acctStopMsg.add( acctReqMsg.get( RA_CALLING_STATION_ID ) ) );
    assert( acctStopMsg.add( attrAcctStatusStop ) );
    assert( acctStopMsg.add( acctReqMsg.get( RA_ACCT_SESSION_ID ) ) );
    assert( acctStopMsg.add( attrAcctSessionTime ) );
    assert( acctStopMsg.add( attrAcctTerminateCause ) );

    acctStopMsg.setIdentifier( ++identifier );
    acctStopMsg.calcAuthenticator( secret );

    resultSend = acctStopMsg.hexDump();

    // At the server side

    RawMessage udpMsg7( acctStopMsg.data() );
    RadiusData msgReceived7( udpMsg7.buffer, ntohs(udpMsg7.msgHdr.length) );

    try
    {
        RadiusMessage acctReqMsg2( msgReceived7, secret );

        resultRecv = acctReqMsg2.hexDump();

        test_verify( resultSend == resultRecv );

        cpLog( LOG_INFO, "Verbose\n[%s]", acctReqMsg2.verbose().c_str() );
    }
    catch( VRadiusException &e )
    {
        cpLog( LOG_ERR, "Test failed - %s", e.getDescription().c_str() );
    }

    cpLog( LOG_INFO, "\n--------------------  End  ----------------------\n");


    return test_return_code( 7 );
}

