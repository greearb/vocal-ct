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
static const char* const RadiusTestConfiguration_cxx_Version =
    "$Id: RadiusTestConfiguration.cxx,v 1.1 2004/05/01 04:15:22 greear Exp $";

#include <cstdio>
#include <cassert>

#include "parse3tuple.h"
#include "cpLog.h"

#include "RadiusTestConfiguration.hxx"

typedef enum
{
    Tag_type,
    Tag_Config_log_file,
    Tag_log_level,
    Tag_local_ip,
    Tag_server_port,
    Tag_server_accounting_port,
    Tag_response_delay,
    Tag_random_reject_percent,
    Tag_packet_lost_percent,
    Tag_secret,
    Tag_accept_Reply_Message,
    Tag_reject_Reply_Message,
    Tag_number_of_packets,
    Tag_client_port,
    Tag_server_host,
    Tag_response_timeout,
    Tag_test_access,
    Tag_test_accounting,
    Tag_User_Name,
    Tag_User_Password,
    Tag_Session_Timeout,
    Tag_Called_Station_Id,
    Tag_Calling_Station_Id,
    Tag_Acct_Session_Time,
    Tag_Acct_Terminate_Cause,
    Tag_Max
} RadiusTestConfigTags;

static const char* RadiusTestConfigTagString [] =
{
    "type",
    "log_file",
    "log_level",
    "local_ip",
    "server_port",
    "server_accounting_port",
    "response_delay",
    "random_reject_percent",
    "packet_lost_percent",
    "secret",
    "accept_Reply_Message",
    "reject_Reply_Message",
    "number_of_packets",
    "client_port",
    "server_host",
    "response_timeout",
    "test_access",
    "test_accounting",
    "User_Name",
    "User_Password",
    "Session_Timeout",
    "Called_Station_Id",
    "Calling_Station_Id",
    "Acct_Session_Time",
    "Acct_Terminate_Cause",
    "Unknown"
};

RadiusTestConfiguration* RadiusTestConfiguration::config = 0;


RadiusTestConfiguration*
RadiusTestConfiguration::instance()
{
    assert( config != 0 );
    return config;
}

void
RadiusTestConfiguration::instance( const string filename )
    throw( VRadiusException& )
{
    assert( config == 0 );

    config = new RadiusTestConfiguration;
    assert( config != 0 );
    
    if( !parse3tuple( filename.c_str(), parseCallBack ) )
    {
        throw VRadiusException( "Parse Configuration file failed",
                                __FILE__,
                                __LINE__ );
    }
}

void
RadiusTestConfiguration::parseCallBack( char* tag, char* type, char* value )
{
    instance()->parse( tag, type, value );
}

void
RadiusTestConfiguration::parse( char* tag, char* type, char* value )
{
    int i;
    switch ( strConst2i( tag, RadiusTestConfigTagString, Tag_Max ) )
    {
        case Tag_type:
        {
            my_type = value;
            break;
        }
        case Tag_Config_log_file:
        {
            my_log_file = value;
            break;
        }
        case Tag_log_level:
        {
            my_log_level = value;
            break;
        }
        case Tag_server_port:
        {
            sscanf( value, "%d", &i );
            my_server_port = i;
            break;
        }
        case Tag_local_ip:
        {
            my_local_ip = value;
            break;
        }
        case Tag_server_accounting_port:
        {
            sscanf( value, "%d", &i );
            my_server_accounting_port = i;
            break;
        }
        case Tag_response_delay:
        {
            sscanf( value, "%d", &i );
            my_response_delay = i;
            break;
        }
        case Tag_random_reject_percent:
        {
            sscanf( value, "%d", &i );
            my_random_reject_percent = i;
            break;
        }
        case Tag_packet_lost_percent:
        {
            sscanf( value, "%d", &i );
            my_packet_lost_percent = i;
            break;
        }
        case Tag_secret:
        {
            my_secret = value;
            break;
        }
        case Tag_accept_Reply_Message:
        {
            my_accept_Reply_Message = value;
            break;
        }
        case Tag_reject_Reply_Message:
        {
            my_reject_Reply_Message = value;
            break;
        }
        case Tag_number_of_packets:
        {
            sscanf( value, "%d", &i );
            my_number_of_packets = i;
            break;
        }
        case Tag_client_port:
        {
            sscanf( value, "%d", &i );
            my_client_port = i;
            break;
        }
        case Tag_server_host:
        {
            my_server_host = value;
            break;
        }
        case Tag_response_timeout:
        {
            sscanf( value, "%d", &i );
            my_response_timeout = i;
            break;
        }
        case Tag_test_access:
        {
            my_test_access = ( (string(value) == "true") ? true : false );
            break;
        }
        case Tag_test_accounting:
        {
            my_test_accounting = ( (string(value) == "true") ? true : false );
            break;
        }
        case Tag_User_Name:
        {
            my_User_Name = value;
            break;
        }
        case Tag_User_Password:
        {
            my_User_Password = value;
            break;
        }
        case Tag_Session_Timeout:
        {
            sscanf( value, "%d", &i );
            my_Session_Timeout = i;
            break;
        }
        case Tag_Called_Station_Id:
        {
            my_Called_Station_Id = value;
            break;
        }
        case Tag_Calling_Station_Id:
        {
            my_Calling_Station_Id = value;
            break;
        }
        case Tag_Acct_Session_Time:
        {
            sscanf( value, "%d", &i );
            my_Acct_Session_Time = i;
            break;
        }
        case Tag_Acct_Terminate_Cause:
        {
            sscanf( value, "%d", &i );
            my_Acct_Terminate_Cause = i;
            break;
        }
        default:
        {
            cpLog( LOG_WARNING, "Unknown Tag: %s", tag );
        }

    }
}

int
RadiusTestConfiguration::log_level() const
{
    if( my_log_level == "ERR" )
    {
        return LOG_ERR;
    }
    else if( my_log_level == "INFO" )
    {
        return LOG_INFO;
    }
    else if( my_log_level == "DEBUG" )
    {
        return LOG_DEBUG;
    }
    else if( my_log_level == "DEBUG_STACK" )
    {
        return LOG_DEBUG_STACK;
    }
    return LOG_ERR;
}

