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
static const char* const RadiusTestConfiguration_hxx_Version =
    "$Id: RadiusTestConfiguration.hxx,v 1.1 2004/05/01 04:15:22 greear Exp $";

#include <string>

#include "VRadiusException.hxx"

class RadiusTestConfiguration
{
public:
    static RadiusTestConfiguration* instance();

    static void instance( const string filename )
        throw( VRadiusException& );

    static void parseCallBack( char* tag, char* type, char* value );

    void parse( char* tag, char* type, char* value );

    string type()                   const { return my_type; }

    string log_file()               const { return my_log_file; }

    int log_level() const;

    int    server_port()            const { return my_server_port; }

    int    server_accounting_port() const { return my_server_accounting_port; }

    int    response_delay()         const { return my_response_delay; }

    int    random_reject_percent()  const { return my_random_reject_percent; }

    int    packet_lost_percent()    const { return my_packet_lost_percent; }

    string secret()                 const { return my_secret; }

    string getLocalIp()             const { return my_local_ip; }

    string accept_Reply_Message()   const { return my_accept_Reply_Message; }

    string reject_Reply_Message()   const { return my_reject_Reply_Message; }

    int    number_of_packets()      const { return my_number_of_packets; }

    int    client_port()            const { return my_client_port; }

    string server_host()            const { return my_server_host; }

    int    response_timeout()       const { return my_response_timeout; }

    bool   test_access()            const { return my_test_access; }

    bool   test_accounting()        const { return my_test_accounting; }

    string User_Name()              const { return my_User_Name; }

    string User_Password()          const { return my_User_Password; }

    int    Session_Timeout()        const { return my_Session_Timeout; }

    string Called_Station_Id()      const { return my_Called_Station_Id; }

    string Calling_Station_Id()     const { return my_Calling_Station_Id; }

    int    Acct_Session_Time()      const { return my_Acct_Session_Time; }

    int    Acct_Terminate_Cause()   const { return my_Acct_Terminate_Cause; }

private:
    string my_type;
    string my_log_file;
    string my_log_level;
    string my_local_ip;
    int    my_server_port;
    int    my_server_accounting_port;
    int    my_response_delay;
    int    my_random_reject_percent;
    int    my_packet_lost_percent;
    string my_secret;
    string my_accept_Reply_Message;
    string my_reject_Reply_Message;
    int    my_number_of_packets;

    int    my_client_port;
    string my_server_host;
    int    my_response_timeout;
    bool   my_test_access;
    bool   my_test_accounting;
    string my_User_Name;
    string my_User_Password;
    int    my_Session_Timeout;
    string my_Called_Station_Id;
    string my_Calling_Station_Id;
    int    my_Acct_Session_Time;
    int    my_Acct_Terminate_Cause;

    static RadiusTestConfiguration* config;
};

