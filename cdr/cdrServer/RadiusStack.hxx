#ifndef RadiusStack_hxx
#define RadiusStack_hxx

/* ====================================================================
 * The Vovida Software License, Version 1.0
 *
 * Copyright (c) 1999, 2000 Vovida Networks, Inc.  All rights reserved.
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
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by Vovida Networks,
 *        Inc. (http://www.vovida.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must 
 *    not be used to endorse or promote products derived from this 
 *    software without prior written permission. For written permission, 
 *    please contact vocal\@vovida.org.
 *
 * 5. Products derived from this software may not be called "VOCAL",
 *    nor may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL VOVIDA NETWORKS, INC. OR ITS 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by Vovida 
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see 
 * <http://www.vovida.org/>.
 *
 */

////////////////////////////////////////////////////////////////////////
//
//  Parts of this code were copied from the free distribution available at:
//
//  Livingston Enterprises, Inc.
//  6920 Koll Center Parkway
//  Pleasanton, CA   94566
//
//  Copyright 1992 Livingston Enterprises, Inc.
//
//  Permission to use, copy, modify, and distribute this software for any
//  purpose and without fee is hereby granted, provided that this
//  copyright and permission notice appear on all copies and supporting
//  documentation, the name of Livingston Enterprises, Inc. not be used
//  in advertising or publicity pertaining to distribution of the
//  program without specific prior permission, and notice be given
//  in supporting documentation that copying and distribution is by
//  permission of Livingston Enterprises, Inc.
//
//  Livingston Enterprises, Inc. makes no representations about
//  the suitability of this software for any purpose.  It is
//  provided "as is" without express or implied warranty.
//
////////////////////////////////////////////////////////////////////////


static const char* const RadiusStack_hxx_Version =
    "$Id: RadiusStack.hxx,v 1.1 2004/05/01 04:14:55 greear Exp $";

#include "global.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include "VRadiusException.hxx"

/// Global constants

const int ACCT_VECTOR_LEN = 16;
const int PW_AUTH_UDP_PORT = 1812;
const int PW_ACCT_UDP_PORT = 1813;

class NetworkAddress;
class UdpStack;

/** Data structure definitions **/

/// Used for send and receive buffers
union BufType
{
    /// attribute buffer
    unsigned char buffer[4096];

    /// header for attribute buffer (attributes must follow header)
    struct AcctHdr
    {                                               ///
        unsigned char code;                         ///
        unsigned char id;                           ///
        unsigned short int length;                  ///
        unsigned char authVector[ACCT_VECTOR_LEN];  ///
    }

    acctHdr;
};


/**
    RadiusStack communicates with a radius server based on
    the radius protocol (See RFC 2139)
 
    This implementation is meant to work as a base class for
    a client application. The client should define it's own
    methods to deal with different VSA's (Vendor Specific
    Attributes).
 
    NOTE: most of the work has been done on the accounting
    functionality, there is little support for the
    authorization functionality.
**/
class RadiusStack
{
    public:
        ///
        enum PacketType
        {                                     ///
            PW_AUTHENTICATION_REQUEST = 1,    ///
            PW_AUTHENTICATION_ACK = 2,        ///
            PW_AUTHENTICATION_REJECT = 3,     ///
            PW_ACCOUNTING_REQUEST = 4,        ///
            PW_ACCOUNTING_RESPONSE = 5,       ///
            PW_ACCOUNTING_STATUS = 6,         ///
            PW_PASSWORD_REQUEST = 7,          ///
            PW_PASSWORD_ACK = 8,              ///
            PW_PASSWORD_REJECT = 9,           ///
            PW_ACCOUNTING_MESSAGE = 10,       ///
            PW_ACCESS_CHALLENGE = 11          ///
        };
        ///
        enum AttribType
        {                                     ///
            PW_USER_NAME = 1,                 ///
            PW_PASSWORD = 2,                  ///
            PW_CHAP_PASSWORD = 3,             ///
            PW_NAS_IP_ADDRESS = 4,            ///
            PW_NAS_PORT_ID = 5,               ///

            PW_VENDOR_SPECIFIC = 26,          ///
            PW_CALLED_STATION_ID	= 30, ///
            PW_CALLING_STATION_ID = 31,       ///

            PW_ACCT_STATUS_TYPE = 40,         ///
            PW_ACCT_DELAY_TIME = 41,          ///
            PW_ACCT_INPUT_OCTETS = 42,        ///
            PW_ACCT_OUTPUT_OCTETS = 43,       ///
            PW_ACCT_SESSION_ID = 44,          ///
            PW_ACCT_AUTHENTIC = 45,           ///
            PW_ACCT_SESSION_TIME = 46,        ///
            PW_ACCT_INPUT_PACKETS = 47,       ///
            PW_ACCT_OUTPUT_PACKETS = 48,      ///
            PW_ACCT_TERMINATE_CAUSE = 49      ///
        };
        ///
        enum AcctStatusType
        {                                     ///
            ACCT_START = 1,                   ///
            ACCT_STOP = 2,                    ///
            ACCT_ON = 7,                      ///
            ACCT_OFF = 8                      ///
        };

    public:
 
        /** local_ip can be "" if you want the default, or you can specify it
         * to bind to a particular local interface.
         */
        RadiusStack(const string& local_ip);

        /// Destructor
        virtual ~RadiusStack();

        /// Initialize connection with radius server
        virtual void initialize( const string &server,
                                 const string &secretKey,
                                 const int retries ) throw(VRadiusException&);

        /// Set up the values for the connection
        void setupConnection();

        /// Sent when the client starts
        bool accountingOn();

        /// Sent when the client terminates
        bool accountingOff();

        /// Sent when subscriber should be authenticated
        bool accessRequest( const string &callId,
                            const string &passwd );

        /// Sent when gateway wants to start a call
        bool accountingStartCall( const string &from,
                                  const string &to,
                                  const string &callId );

        /// Sent when the call is terminated
        bool accountingStopCall( const string &callId );


    protected:

        /**
         * Process the VSA (vendor specific attributes)
         * Called by evaluateRecvBuffer() to deal with
         * messages from the radius server
         */
        virtual void processVsa( const unsigned char vsaType,
                                 unsigned char *ptr,
                                 int bufLen ) {}

        /// Fills the m_sendBuffer with accounting ON message
        void createAcctOnMsg();

        /// Fills the m_sendBuffer with accounting OFF message
        void createAcctOffMsg();

        /// Fills the m_sendBuffer with accounting start call message
        void createAcctStartCallMsg( const string &from,
                                     const string &to,
                                     const string &callId );

        /// Fills the m_sendBuffer with accounting stop call message
        void createAcctStopCallMsg( const string &callId );

        /// Fills the m_sendBuffer with access request  message
        void createAccessRqstMsg( const string &callId,
                                  const string &passwd );

        /// Fill m_sendBuffer header with data, overwrites existing data
        int addHeader( const PacketType type );

        /// Calculate MD5 hash and fill field in m_sendBuffer
        void addMD5();

        /// Adds attribute to m_sendBuffer, returns length of attribute
        int addAttribute( const AttribType type, const unsigned long int value );

        /// Adds attribute to m_sendBuffer, returns length of attribute
        int addAttribute( const AttribType type, const unsigned short int value );

        /// Adds attribute to m_sendBuffer, returns length of attribute
        int addAttribute( const AttribType type, const unsigned char value );

        /// Adds attribute to m_sendBuffer, returns length of attribute
        int addAttribute( const AttribType type,
                          const unsigned char* value,
                          const int sizeOfValue );

        /// Adds vsaAttribute to m_sendBuffer, returns length of vsaAttribute
        int addVsaAttribute( const unsigned char vsaType,
                             const unsigned long int value,
                             const unsigned long int vendorId );

        /// Adds vsaAttribute to m_sendBuffer, returns length of vsaAttribute
        int addVsaAttribute( const unsigned char vsaType,
                             const unsigned short int value,
                             const unsigned long int vendorId );

        /// Adds vsaAttribute to m_sendBuffer, returns length of vsaAttribute
        int addVsaAttribute( const unsigned char vsaType,
                             const unsigned char value,
                             const unsigned long int vendorId );

        /// Adds vsaAttribute to m_sendBuffer, returns length of vsaAttribute
        int addVsaAttribute( const unsigned char vsaType,
                             const unsigned char* value,
                             const int sizeOfValue,
                             const unsigned long int vendorId );

        /// sends message and waits for reply
        bool handshake();

        /// Send data from m_sendBuffer. Blocks until all the data is sent
        bool sendRadius();

        /// Puts data in m_recvBuffer
        int recvRadius();

        /// evaluate attribute list in receive m_recvBuffer
        bool evaluateRecvBuffer();

        /// validate md5 sum of accounting message in m_recvBuffer
        bool verifyAcctRecvBuffer() throw(VRadiusException&);

        /// validate md5 sum of authentication message in m_recvBuffer
        bool verifyAuthRecvBuffer() throw(VRadiusException&)
        {
            return (true);
        }

        /// md5Calc is used to find the md5 sum
        int md5Calc( unsigned char *output,
                     unsigned char *input,
                     unsigned inLen );


    protected:

        int m_retries;                        ///

        string m_serverName;                  ///

        /// network byte order
        unsigned long int m_serverAddr;       ///
        int m_serverPort;                     ///

        string m_clientName;                  ///

        /// network byte order
        unsigned long int m_clientAddr;       ///
        int m_clientPort;                     ///

        unsigned char m_requestId;            ///
        unsigned char m_responseId;           ///
        string m_secretKey;                   ///

        bool m_connected;                     ///
        AcctStatusType m_acctStatusType;      ///

        BufType m_sendBuffer;                 ///
        BufType m_recvBuffer;                 ///
        int m_sendBufferLen;                  ///
        int m_recvBufferLen;                  ///
        bool m_recvBufferValid;               ///


        string m_localIp;
        NetworkAddress *m_networkAddr;        ///
        UdpStack *m_udpConnection;            ///

private:
        RadiusStack(); // Must specify local-ip, so can't use this one.
        RadiusStack(const RadiusStack& src); // not implemented.
        RadiusStack& operator=(const RadiusStack& rhs); // not implemented.
};
#endif
