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


static const char* const RadiusAttribute_cxx_Version =
    "$Id: RadiusAttribute.cxx,v 1.1 2004/05/01 04:15:22 greear Exp $";

#include <sys/types.h>
#include <netinet/in.h>
#include <cstdio>

#include "global.h"
#include "cpLog.h"
#include "support.hxx"
#include "RadiusAttribute.hxx"

#ifdef WIN32
#define snprintf _snprintf
#endif

RadiusAttribute::RadiusAttribute( const RadiusAttributeType type,
                                  const u_int8_t* value,
                                  const u_int8_t length,
                                  const u_int32_t vendorId ) :
    myType( type ),
    myVendorId( vendorId )
{
    assert( ( type != RA_VENDOR_SPECIFIC && vendorId == 0 ) ||
            ( type == RA_VENDOR_SPECIFIC && vendorId != 0 ) );

    if( type == RA_USER_PASSWORD && (length % 16 ) != 0 )
    {
        // Expand password to a multiple of 16 octets and fill it up with '\0'
        u_int8_t value16[ RadiusMaxAttributeLength ];
        memcpy( value16, value, length );
        memset( value16+length, '\0', 16 - (length % 16) );
        cpLog( LOG_DEBUG_STACK, "User-Password length %d -> %d",
                                    length,
                                    (length+16)&0xFFF0 );
        myValue = RadiusData( value16, (length+16)&0xFFF0 );
    }
    else
    {
        myValue = RadiusData( value, length );
    }
}


RadiusAttribute::RadiusAttribute( const RadiusAttributeType type,
                                  const RadiusData& value,
                                  const u_int32_t vendorId ) :
    myType( type ),
    myValue( value ),
    myVendorId( vendorId )
{
    assert( ( type != RA_VENDOR_SPECIFIC && vendorId == 0 ) ||
            ( type == RA_VENDOR_SPECIFIC && vendorId != 0 ) );

    if( type == RA_USER_PASSWORD )
    {
        if( value.length() % 16 )
        {
            // Expand password to a multiple of 16 octets and fill it up with '\0'
            u_int8_t value16[ RadiusMaxAttributeLength ];
            u_int16_t length = value.length();
            memcpy( value16, value.data(), length );
            memset( value16+length, '\0', 16 - (length % 16) );
            cpLog( LOG_DEBUG_STACK, "User-Password length %d -> %d",
                                    length,
                                    (length+16)&0xFFF0 );
            myValue = RadiusData( value16, (length+16)&0xFFF0 );
        }
    }
}

RadiusData
RadiusAttribute::encode() const
{
    RadiusData msgDat;

    msgDat.append( &myType, 1 );

    if( myType == RA_VENDOR_SPECIFIC )
    {
        //   Vendor-Specific Attribute format
        //
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |  Length       |            Vendor-Id
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //        Vendor-Id (cont)           |  String...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

        // 6 octets for Type, Length and Vendor-Id
        u_int8_t  msgLen  = myValue.length() + 6;
        msgDat.append( &msgLen, 1 );

        u_int32_t vendId  = htonl( myVendorId );
        msgDat.append( &vendId, 4 );

        msgDat += myValue;
    }
    else
    {
        //   Attribute format
        //
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  Value ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

        // 2 octets for Type and Length
        u_int8_t msgLen = myValue.length() + 2;
        msgDat.append( &msgLen, 1 );

        msgDat += myValue;
    }
    return msgDat;
}

string
RadiusAttribute::verbose() const
{
    char attrLenStr[4];
    snprintf( attrLenStr, 4, "%3d", myValue.length() + 2 );

    char tempBuff[ RadiusMaxAttributeLength ];

    string attrStr;

    switch( myType )
    {
        case RA_USER_NAME:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  String ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            memcpy( tempBuff, myValue.data(), myValue.length() );
            tempBuff[ myValue.length() ] = '\0';
            attrStr = "  User-Name (" + itos( myType ) + ") = \"" +
                      tempBuff + "\"\n";
            break;
        }
        case RA_USER_PASSWORD:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  String ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            memcpy( tempBuff, myValue.data(), myValue.length() );
            tempBuff[ myValue.length() ] = '\0';
            attrStr = "  User-Password (" + itos( myType ) + ") = \"" +
                      tempBuff + "\"\n";
            break;
        }
        case RA_CHAP_PASSWORD:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  CHAP Ident   |  String ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            // TODO: show CHAP Ident, but not password
            attrStr = "  CHAP-Password (" + itos( myType ) + ")\n";
            break;
        }
        case RA_NAS_IP_ADDRESS:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |            Address
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //            Address (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            snprintf( tempBuff, 16, "%d.%d.%d.%d",
                                     myValue.data()[0],
                                     myValue.data()[1],
                                     myValue.data()[2],
                                     myValue.data()[3] );
            attrStr = "  NAS-IP-Address (" + itos( myType ) + ") = " +
                      tempBuff + "\n";
            break;
        }
        case RA_NAS_PORT:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            u_int32_t port = ntohl( *(reinterpret_cast< const u_int32_t * >(myValue.data())) );
            attrStr = "  NAS-Port (" + itos( myType ) + ") = " +
                      itos( port ) + "\n";
            break;
        }
        case RA_SERVICE_TYPE:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            u_int32_t serviceType = ntohl( *(reinterpret_cast< const u_int32_t * >(myValue.data())) );
            attrStr = "  Service-Type (" + itos( myType ) + ") = " +
                      itos( serviceType ) + "\n";
            // TODO: more info
            break;
        }
        case RA_REPLY_MESSAGE:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  Text ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            memcpy( tempBuff, myValue.data(), myValue.length() );
            tempBuff[ myValue.length() ] = '\0';
            attrStr = "  Reply-Message (" + itos( myType ) + ") = \"" +
                      tempBuff + "\"\n";
            break;
        }
        case RA_STATE:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  String ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            attrStr = "  State (" + itos( myType ) + ") \n";
            // TODO: more info
            break;
        }
        case RA_CLASS:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  String ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            attrStr = "  Class (" + itos( myType ) + ") \n";
            // TODO: more info
            break;
        }
        case RA_VENDOR_SPECIFIC:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |  Length       |            Vendor-Id
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //        Vendor-Id (cont)           |  String...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            if (myValue.length() < 3 || myVendorId != 9 ||
	      myValue.data()[0] != 1 || myValue.data()[1] > myValue.length())
	    {
                // TODO: more info
		attrStr = "  Vendor-Specific (" + itos( myType ) + ") \n";
		break;
	    }
	    u_int8_t avpLength = myValue.data()[1] - 2;
	    memcpy( tempBuff, myValue.data() + 2, avpLength);
	    tempBuff[avpLength] = '\0';
	    attrStr = "  Cisco AVPair (" + itos( myValue.data()[0] ) +
		      ") = \"" + tempBuff + "\"\"\n";
            break;
        }
        case RA_SESSION_TIMEOUT:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            u_int32_t to = ntohl( *(reinterpret_cast< const u_int32_t * >(myValue.data())) );
            attrStr = "  Session-Timeout (" + itos( myType ) + ") = " +
                      itos( to ) + "\n";
            break;
        }
        case RA_CALLED_STATION_ID:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  String ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            memcpy( tempBuff, myValue.data(), myValue.length() );
            tempBuff[ myValue.length() ] = '\0';
            attrStr = "  Called-Station-Id (" + itos( myType ) + ") = \"" +
                      tempBuff + "\"\n";
            break;
        }
        case RA_CALLING_STATION_ID:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        //   |     Type      |    Length     |  String ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

            memcpy( tempBuff, myValue.data(), myValue.length() );
            tempBuff[ myValue.length() ] = '\0';
            attrStr = "  Calling-Station-Id (" + itos( myType ) + ") = \"" +
                      tempBuff + "\"\n";
            break;
        }
        case RA_ACCT_STATUS_TYPE:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            string acctTypeStr;
            u_int32_t acctType = ntohl( *(reinterpret_cast< const u_int32_t * >(myValue.data())) );
            switch( acctType )
            {
                case RAS_START:
                {
                    acctTypeStr = "Start (" + itos(acctType) + ")";
                    break;
                }
                case RAS_STOP:
                {
                    acctTypeStr = "Stop (" + itos(acctType) + ")";
                    break;
                }
                case RAS_INTERIM:
                {
                    acctTypeStr = "Interim-Update (" + itos(acctType) + ")";
                    break;
                }
                case RAS_ON:
                {
                    acctTypeStr = "Accounting-On (" + itos(acctType) + ")";
                    break;
                }
                case RAS_OFF:
                {
                    acctTypeStr = "Accounting-Off (" + itos(acctType) + ")";
                    break;
                }
                default:
                {
                    acctTypeStr = "Unknown (" + itos(acctType) + ")";
                }
            }
            attrStr = "  Acct-Status-Type (" + itos( myType ) + ") = " +
                      acctTypeStr + "\n";
            break;
        }
        case RA_ACCT_DELAY_TIME:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            u_int32_t duration = ntohl( *(reinterpret_cast< const u_int32_t * >(myValue.data())) );
            attrStr = "  Acct-Delay-Time (" + itos( myType ) + ") = " +
                      itos( duration ) + "\n";
            break;
        }
        case RA_ACCT_INPUT_OCTETS:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            attrStr = "  Acct-Input-Octets (" + itos( myType ) + ")\n";
            // TODO: more info
            break;
        }
        case RA_ACCT_OUTPUT_OCTETS:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            attrStr = "  Acct-Output-Octets (" + itos( myType ) + ")\n";
            // TODO: more info
            break;
        }
        case RA_ACCT_SESSION_ID:
        {
        //    0                   1                   2
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |  Text ...
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            memcpy( tempBuff, myValue.data(), myValue.length() );
            tempBuff[ myValue.length() ] = '\0';
            attrStr = "  Acct-Session-Id (" + itos( myType ) + ") = \"" +
                      tempBuff + "\"\n";
            break;
        }
        case RA_ACCT_AUTHENTIC:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            attrStr = "  Acct-Authentic (" + itos( myType ) + ")\n";
            // TODO: more info
            break;
        }
        case RA_ACCT_SESSION_TIME:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            u_int32_t duration = ntohl( *(reinterpret_cast< const u_int32_t * >(myValue.data())) );
            attrStr = "  Acct-Session-Time (" + itos( myType ) + ") = " +
                      itos( duration ) + "\n";
            break;
        }
        case RA_ACCT_INPUT_PACKETS:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            attrStr = "  Acct-Input-Packets (" + itos( myType ) + ")\n";
            // TODO: more info
            break;
        }
        case RA_ACCT_OUTPUT_PACKETS:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            attrStr = "  Acct-Output-Packets (" + itos( myType ) + ")\n";
            // TODO: more info
            break;
        }
        case RA_ACCT_TERMINATE_CAUSE:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            string causeStr;
            u_int32_t cause = ntohl( *(reinterpret_cast< const u_int32_t * >(myValue.data())) );
            switch( cause )
            {
                case RATC_USER_REQUEST:
                {
                    causeStr = "User Request (" + itos(cause) + ")";
                    break;
                }
                case RATC_LOST_CARRIER:
                {
                    causeStr = "Lost Carrier (" + itos(cause) + ")";
                    break;
                }
                case RATC_LOST_SERVICE:
                {
                    causeStr = "Lost Service (" + itos(cause) + ")";
                    break;
                }
                case RATC_IDLE_TIMEOUT:
                {
                    causeStr = "Idle Timeout (" + itos(cause) + ")";
                    break;
                }
                case RATC_SESSION_TIMEOUT:
                {
                    causeStr = "Session Timeout (" + itos(cause) + ")";
                    break;
                }
                case RATC_ADMIN_RESET:
                {
                    causeStr = "Admin Reset (" + itos(cause) + ")";
                    break;
                }
                case RATC_ADMIN_REBOOT:
                {
                    causeStr = "Admin Reboot (" + itos(cause) + ")";
                    break;
                }
                case RATC_PORT_ERROR:
                {
                    causeStr = "Port Error (" + itos(cause) + ")";
                    break;
                }
                case RATC_NAS_ERROR:
                {
                    causeStr = "NAS Error (" + itos(cause) + ")";
                    break;
                }
                case RATC_NAS_REQUEST:
                {
                    causeStr = "NAS Request (" + itos(cause) + ")";
                    break;
                }
                case RATC_NAS_REBOOT:
                {
                    causeStr = "NAS Reboot (" + itos(cause) + ")";
                    break;
                }
                case RATC_SERVICE_UNAVAILABLE:
                {
                    causeStr = "Service Unavailable (" + itos(cause) + ")";
                    break;
                }
                default:
                {
                    causeStr = "Unknown (" + itos(cause) + ")";
                }
            }
            attrStr = "  Acct-Terminate-Cause (" + itos( myType ) + ") = " +
                      causeStr + "\n";
            break;
        }
        case RA_NAS_PORT_TYPE:
        {
        //    0                   1                   2                   3
        //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //   |     Type      |    Length     |             Value
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //              Value (cont)         |
        //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

            u_int32_t portType = ntohl( *(reinterpret_cast< const u_int32_t * >(myValue.data())) );
            attrStr = "  NAS-Port-Type (" + itos( myType ) + ") = " +
                      itos( portType ) + "\n";
            // TODO: more info
            break;
        }
	case RA_DIGEST_RESPONSE:
	{
	//    0                   1                   2
	//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
	//   |     Type      |    Length     |  String ...
	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

	    memcpy( tempBuff, myValue.data(), myValue.length() );
	    tempBuff[ myValue.length() ] = '\0';
	    attrStr = "  Digest-Response (" + itos( myType ) + ") = \"" +
		      tempBuff + "\"\n";
	    break;
	}
	case RA_DIGEST_ATTRIBUTES:
	{
	//    0                   1                   2                   3
	//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//   |     Type      |    Length     |    Sub-Type   |  Sub-Lengh    |
	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//   |   String ...
	//   +-+-+-+-+-+-+-+-

	    u_int8_t subType = *(reinterpret_cast< const u_int8_t * >(myValue.data()));
	    u_int8_t subLength = *(reinterpret_cast< const u_int8_t * >(myValue.data() + 1));
	    string subAttrStr;

	    switch (subType)
	    {
	    case RA_DIGEST_REALM:
		subAttrStr = "Realm";
		break;
	    case RA_DIGEST_NONCE:
		subAttrStr = "Nonce";
		break;
	    case RA_DIGEST_METHOD:
		subAttrStr = "Method";
		break;
	    case RA_DIGEST_URI:
		subAttrStr = "URI";
		break;
	    case RA_DIGEST_ALGORITHM:
		subAttrStr = "Algorithm";
		break;
	    case RA_DIGEST_USER_NAME:
		subAttrStr = "User-Name";
		break;
	    default:
		subAttrStr = "Unknown";
		break;
	    }

	    memcpy( tempBuff, myValue.data() + 2, subLength - 2);
	    tempBuff[ subLength - 2] = '\0';
	    attrStr = "  Digest-Attributes (" + itos( myType ) + ") = \"" +
		      subAttrStr + " (" + itos( subType ) + ") = \"" +
		      tempBuff + "\"\"\n";
	    break;
	}
        default:
        {
            attrStr = "  Unknown (" + itos( myType ) + ")\n";
        }
    }
    return string( attrLenStr + attrStr );
}

// Local Variables:
// mode:c++
// c-file-style:"bsd"
// c-basic-offset:4
// c-file-offsets:((inclass . ++))
// indent-tabs-mode:nil
// End:

