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
static const char* const radiusClient_cxx_Version =
    "$Id: radiusClient.cxx,v 1.1 2004/05/01 04:15:22 greear Exp $";

#include <cstdio>

#ifdef __FreeBSD__
#include <sys/types.h>
#endif
#include <netinet/in.h>

#include "cpLog.h"
#include "UdpStack.hxx"
#include "TimeFunc.hxx"

#include "RadiusMessage.hxx"

#include "RadiusTestConfiguration.hxx"


int
main( int argc, char* argv[] )
{
    string configFile = "client.cfg";

    if( argc > 1 )
    {
        configFile = argv[1];
    }
    cout << "RADIUS Client" << endl;

    RadiusTestConfiguration* config;
    try
    {
        RadiusTestConfiguration::instance( configFile );
        config = RadiusTestConfiguration::instance();
    }
    catch( VRadiusException &e )
    {
        cerr << "Cannot read configuration data file " << configFile << endl;
        exit( -1 );
    }

    if( config->type() != "client" )
    {
        cerr << "Invalid type in " << configFile << endl;
        exit( -2 );
    }

    if( config->log_file() != "" )
    {
        cpLogOpen( config->log_file().c_str() );
    }

    cpLogSetPriority( config->log_level() );

    bool test_access = config->test_access();
    bool test_accounting = config->test_accounting();

    NetworkAddress serverAddress( config->server_host(),
                                  config->server_port() );
    NetworkAddress serverAccountingAddress( config->server_host(),
                                            config->server_accounting_port() );

    char hostName[ 256 ];
    if( gethostname( hostName, 255 ) != 0 )
    {
        cerr << "gethostname failed " << endl;
        exit( -3 );
    }
    NetworkAddress clientAddress( hostName, config->client_port() );

    UdpStack s(config->getLocalIp(), "" /* local_dev_to_bind_to */,
               0, clientAddress.getPort() );

    int response_timeout     = config->response_timeout() * 1000; // ms to us
    // int Acct_Session_Time    = config->Acct_Session_Time();
    // int Acct_Terminate_Cause = config->Acct_Terminate_Cause();

    // Client Data
    char secret[256];
    strncpy( secret, config->secret().c_str(), config->secret().size()+1 );
    RadiusData User_Name( config->User_Name().c_str(),
                                   config->User_Name().size() );
    RadiusData User_Password( config->User_Password().c_str(),
                                   config->User_Password().size() );
    RadiusData Called_Station_Id( config->Called_Station_Id().c_str(),
                                   config->Called_Station_Id().size() );
    RadiusData Calling_Station_Id( config->Calling_Station_Id().c_str(),
                                   config->Calling_Station_Id().size() );

    int numberOfPackets = config->number_of_packets();

    // Counters
    int accessRequest = 0;       // Client send
    int accessAccept  = 0;       // Server send
    int accessReject  = 0;       // Server send
    int accountingRequest  = 0;  // Client send
    int accountingResponse = 0;  // Server send

    NetworkAddress radiusServer("0.0.0.0");
    int maxPacketSize = RadiusMaxPacketSize;

    // u_int8_t reqAuth[ RadiusAuthenticatorLength ];

    RawMessage rawMsg;

    int packetLength;
    u_int8_t id = 0;

    RadiusAttribute attrUserName( RA_USER_NAME, User_Name );

    RadiusAttribute attrUserPassword( RA_USER_PASSWORD, User_Password );

    u_int32_t ip( clientAddress.getIp4Address() );
    RadiusData valNasIpAddress( &ip , sizeof(ip) );
    RadiusAttribute attrNasIpAddress( RA_NAS_IP_ADDRESS,
                                      valNasIpAddress );

    u_int32_t port( htonl( clientAddress.getPort() ) );
    RadiusData valNasPort( &port , sizeof(port) );
    RadiusAttribute attrNasPort( RA_NAS_PORT, valNasPort );

    u_int32_t serviceType( htonl(RAST_LOGIN) );
    RadiusData valServiceType( &serviceType , sizeof(serviceType) );
    RadiusAttribute attrServiceType( RA_SERVICE_TYPE, valServiceType );

    RadiusAttribute attrCallee( RA_CALLED_STATION_ID, Called_Station_Id );

    RadiusAttribute attrCaller( RA_CALLING_STATION_ID, Calling_Station_Id );

    RadiusData valAcctSessionId( "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com", 48 );
    RadiusAttribute attrAcctSessionId( RA_ACCT_SESSION_ID,
                                       valAcctSessionId );

    u_int32_t portType( htonl(RANPT_VIRTUAL) );
    RadiusData valNasPortType( &portType , sizeof(portType) );
    RadiusAttribute attrNasPortType( RA_NAS_PORT_TYPE, valNasPortType );

    RadiusAttribute attrClass( RA_CLASS, RadiusData( "Nas Ip:1.4.61.70", 16 ) );

    FunctionTimer tTotal;
    tTotal.start();

    for( ; ; )
    {
        if( test_access )
        {
            RadiusMessage accessReqMsg( RP_ACCESS_REQUEST );

            // Warning: assert is used for testing only
            assert( accessReqMsg.add( attrUserName ) );
            assert( accessReqMsg.add( attrUserPassword ) );
            assert( accessReqMsg.add( attrNasIpAddress ) );
            // assert( accessReqMsg.add( attrNasPort ) );
            assert( accessReqMsg.add( attrServiceType ) );
            // assert( accessReqMsg.add( attrCallee ) );
            // assert( accessReqMsg.add( attrCaller ) );
            assert( accessReqMsg.add( attrNasPortType ) );

            accessReqMsg.setIdentifier( ++id );
            accessReqMsg.calcAuthenticator( secret );

            const u_int8_t* reqAuth = accessReqMsg.getAuthenticator();

            cpLog( LOG_DEBUG, "Send\n[%s]", accessReqMsg.verbose().c_str() );
            s.transmitTo( reinterpret_cast<const char *>(accessReqMsg.data().buffer),
                          ntohs(accessReqMsg.data().msgHdr.length),
                          &serverAddress );
            accessRequest++;

            packetLength = s.receiveTimeout( reinterpret_cast<char *>(rawMsg.buffer),
                                             maxPacketSize,
                                             &radiusServer,
                                             0,
                                             response_timeout);
            if( packetLength >= RadiusMinPacketSize )
            {
                RadiusData msgRecvd( rawMsg.buffer,
                                     ntohs(rawMsg.msgHdr.length) );
                try
                {
                    RadiusMessage accessResp( msgRecvd, secret );
                    if( accessResp.type() == RP_ACCESS_ACCEPT )
                    {
                        if( accessResp.verifyResponseAuthenticator( reqAuth, secret ) )
                        {
                            try
                            {
                                attrClass = accessResp.get( RA_CLASS );
                            }
                            catch( VRadiusException &e )
                            {
                                cpLog( LOG_ERR, "Access Response: %s",
                                                e.getDescription().c_str() );
                            }
                            accessAccept++;
                        }
                        else
                        {
                            cpLog( LOG_ERR, "Cannot verify Response Authenticator" );
                        }
                    }
                    else if( accessResp.type() == RP_ACCESS_REJECT )
                    {
                        if( accessResp.verifyResponseAuthenticator( reqAuth, secret ) )
                        {
                            accessReject++;
                        }
                        else
                        {
                            cpLog( LOG_ERR, "Cannot verify Response Authenticator" );
                        }
                    }
                    else
                    {
                        cpLog( LOG_ERR, "Unexpected message %d", accessResp.type() );
                    }
                    cpLog( LOG_DEBUG, "Recv\n[%s]", accessResp.verbose().c_str() );
                }
                catch( VRadiusException &e )
                {
                    cpLog( LOG_ERR, "Access Response: %s",
                                    e.getDescription().c_str() );
                }
            }
            else if( packetLength == 0 )
            {
                cpLog( LOG_ERR, "Receive timeout" );
            }
            else if( packetLength > 0 )
            {
                cpLog( LOG_ERR, "Received invalid packet" );
            }
            else
            {
                cpLog( LOG_ERR, "UDP stack error" );
            }
        }
        if( test_accounting )
        {
            RadiusMessage acctReqMsg( RP_ACCOUNTING_REQUEST );

            // Warning: assert is used for testing only
            assert( acctReqMsg.add( attrUserName ) );
            assert( acctReqMsg.add( attrNasIpAddress ) );
            // assert( acctReqMsg.add( attrNasPort ) );
            assert( acctReqMsg.add( attrClass ) );
            assert( acctReqMsg.add( attrCallee ) );
            assert( acctReqMsg.add( attrCaller ) );

            u_int32_t acctType( htonl(RAS_START) );
            RadiusData valAcctStatus( &acctType, sizeof(acctType) );
            RadiusAttribute attrAcctStatus( RA_ACCT_STATUS_TYPE,
                                            valAcctStatus );
            assert( acctReqMsg.add( attrAcctStatus ) );

            assert( acctReqMsg.add( attrAcctSessionId ) );
            assert( acctReqMsg.add( attrNasPortType ) );

            acctReqMsg.setIdentifier( ++id );
            acctReqMsg.calcAuthenticator( secret );

            const u_int8_t* reqAuth = acctReqMsg.getAuthenticator();

            cpLog( LOG_DEBUG, "Send\n[%s]", acctReqMsg.verbose().c_str() );
            s.transmitTo( reinterpret_cast<const char *>(acctReqMsg.data().buffer),
                          ntohs(acctReqMsg.data().msgHdr.length),
                          &serverAccountingAddress );

            accountingRequest++;

            packetLength = s.receiveTimeout( reinterpret_cast<char *>(rawMsg.buffer),
                                             maxPacketSize,
                                             &radiusServer,
                                             0,
                                             response_timeout);
            if( packetLength >= RadiusMinPacketSize )
            {
                RadiusData msgRecvd( rawMsg.buffer,
                                     ntohs(rawMsg.msgHdr.length) );
                try
                {
                    RadiusMessage acctResp( msgRecvd, secret );
                    if( acctResp.type() == RP_ACCOUNTING_RESPONSE )
                    {
                        if( acctResp.verifyResponseAuthenticator( reqAuth, secret ) )
                        {
                            accountingResponse++;
                        }
                        else
                        {
                            cpLog( LOG_ERR, "Cannot verify Response Authenticator" );
                        }
                    }
                    else
                    {
                        cpLog( LOG_ERR, "Unexpected message %d", acctResp.type() );
                    }
                    cpLog( LOG_DEBUG, "Recv\n[%s]", acctResp.verbose().c_str() );
                }
                catch( VRadiusException &e )
                {
                    cpLog( LOG_ERR, "Accounting Response: %s",
                                    e.getDescription().c_str() );
                }
            }
            else if( packetLength == 0 )
            {
                cpLog( LOG_ERR, "Receive timeout" );
            }
            else if( packetLength > 0 )
            {
                cpLog( LOG_ERR, "Received invalid packet" );
            }
            else
            {
                cpLog( LOG_ERR, "UDP stack error" );
            }
        }
        if( numberOfPackets < 0 )
        {
            printf( "\rAccess Req: %d  Accept: %d   Rej: %d  Acct Req: %d  Resp: %d",
                    accessRequest,
                    accessAccept, 
                    accessReject, 
                    accountingRequest, 
                    accountingResponse );
            fflush(stdout);
        }
        if( (accessAccept + accessReject + accountingResponse) == numberOfPackets )
        {
            cout << endl;
            tTotal.end();
            //tTotal.print( "tTotal" );

            cout << "Access Req: " << accessRequest <<
                      "  Accept: " << accessAccept <<
                         "  Rej: " << accessReject <<
                    "  Acct Req: " << accountingRequest <<
                        "  Resp: " << accountingResponse << endl;

            // uncomment the following lines for purify
            // cout << endl << "Wait...." << endl;
            // sleep( 180 );
            // cout << "Done" << endl;
            exit( 1 );
        }
    }
    return 0;
}
