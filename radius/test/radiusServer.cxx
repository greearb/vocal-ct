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
static const char* const radiusServer_cxx_Version =
    "$Id: radiusServer.cxx,v 1.1 2004/05/01 04:15:22 greear Exp $";

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
    string configFile = "server.cfg";

    if( argc > 1 )
    {
        configFile = argv[1];
    }
    cout << "RADIUS Server" << endl;

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

    if( config->type() != "server" )
    {
        cerr << "Invalid type in " << configFile << endl;
        exit( -2 );
    }

    if( config->log_file() != "" )
    {
        cpLogOpen( config->log_file().c_str() );
    }

    cpLogSetPriority( config->log_level() );

    int serverPort     = config->server_port();
    // int accountingPort = config->server_accounting_port();
    UdpStack s(config->getLocalIp(), "" /* local_dev_to_bind_to */,
               0, serverPort );
    // TODO: Use both ports

    // int responseDelay       = config->response_delay();
    // int randomRejectPercent = config->random_reject_percent();
    // int packetLostPercent   = config->packet_lost_percent();
    int sessionTimeout      = config->Session_Timeout();

    // Client Data
    char secret[256];
    strncpy( secret, config->secret().c_str(), config->secret().size()+1 );
    RadiusData acceptReplyMessage( config->accept_Reply_Message().c_str(),
                                   config->accept_Reply_Message().size() );
    RadiusData rejectReplyMessage( config->reject_Reply_Message().c_str(),
                                   config->reject_Reply_Message().size() );

    int numberOfPackets = config->number_of_packets();

    // Counters
    int accessRequest = 0;       // Client send
    int accessAccept  = 0;       // Server send
    int accessReject  = 0;       // Server send
    int accountingRequest  = 0;  // Client send
    int accountingResponse = 0;  // Server send

    NetworkAddress client("0.0.0.0");
    int maxPacketSize = RadiusMaxPacketSize;

    u_int8_t reqAuth[ RadiusAuthenticatorLength ];

    u_int32_t timeout( htonl(sessionTimeout) );
    RadiusData valSessinTimeout( &timeout, sizeof(timeout) );
    RadiusAttribute attrSessionTimeout( RA_SESSION_TIMEOUT,
                                        valSessinTimeout );

    int packetLength;

    FunctionTimer tTotal;

    for( ; ; )
    {
        RawMessage rawMsg;
        packetLength = s.receiveFrom( reinterpret_cast<char *>(rawMsg.buffer),
                                      maxPacketSize,
                                      &client);
        if( accessRequest == 0 && accountingRequest == 0 )
        {
            tTotal.start();
        }
        cpLog( LOG_DEBUG, "Packet length %d from %s:%d",
                          packetLength,
                          client.getIpName().c_str(),
                          client.getPort() );

        if( packetLength >= RadiusMinPacketSize )
        {
            // TODO: simulate packet lost

            memcpy( reqAuth,
                    rawMsg.msgHdr.authenticator,
                    RadiusAuthenticatorLength );

            RadiusData msgRecvd( rawMsg.buffer, ntohs(rawMsg.msgHdr.length) );
            try
            {
                RadiusMessage radiusMsg( msgRecvd, secret );

                cpLog( LOG_DEBUG, "Recv\n[%s]", radiusMsg.verbose().c_str() );

                if( radiusMsg.type() == RP_ACCOUNTING_REQUEST )
                {
                    accountingRequest++;
                    RadiusMessage acctRespMsg( RP_ACCOUNTING_RESPONSE,
                                               rawMsg.msgHdr.identifier,
                                               reqAuth );

                    acctRespMsg.calcAuthenticator( secret );

                    s.transmitTo( reinterpret_cast<const char *>(acctRespMsg.data().buffer),
                                  ntohs(acctRespMsg.data().msgHdr.length),
                                  &client );

                    cpLog( LOG_DEBUG, "Send\n[%s]", acctRespMsg.verbose().c_str() );
                    accountingResponse++;
                }
                else if( radiusMsg.type() == RP_ACCESS_REQUEST )
                {
                    accessRequest++;
                    // TODO: simulate reject

                    RadiusMessage accessAcceptMsg( RP_ACCESS_ACCEPT,
                                                   rawMsg.msgHdr.identifier,
                                                   reqAuth );

                    RadiusData valClass( "Accounting Cookie", 17 );
                    RadiusAttribute attrClass( RA_CLASS, valClass );
                    assert( accessAcceptMsg.add( attrClass ) );

                    // Warning: assert is used for testing only
                    assert( accessAcceptMsg.add( attrSessionTimeout ) );

                    accessAcceptMsg.calcAuthenticator( secret );

                    s.transmitTo( reinterpret_cast<const char *>(accessAcceptMsg.data().buffer),
                                  ntohs(accessAcceptMsg.data().msgHdr.length),
                                  &client );

                    cpLog( LOG_DEBUG, "Send\n[%s]", accessAcceptMsg.verbose().c_str() );
                    accessAccept++;
                }
                else
                {
                    cpLog( LOG_ERR, "Discard packet type %d", radiusMsg.type() );
                }
            }
            catch( VRadiusException &e )
            {
                cpLog( LOG_ERR, "Accounting Request: %s",
                                e.getDescription().c_str() );
            }
        }
        else if( packetLength > 0 )
        {
            cpLog( LOG_ERR, "Received invalid packet" );
        }
        else
        {
            cpLog( LOG_ERR, "UDP stack error" );
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

        if( (accessRequest + accountingRequest) == numberOfPackets )
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
