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


static const char* const RadiusMessage_cxx_Version =
    "$Id: RadiusMessage.cxx,v 1.1 2004/05/01 04:15:22 greear Exp $";

#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>

#include "global.h"
#include "cpLog.h"
#include "vmd5.h"
#include "support.hxx"
#include "RadiusMessage.hxx"

#ifdef WIN32
#define snprintf _snprintf
#endif


/** Create a RADIUS Message with type, no Attriubte yet
    Useful for composing a request
 */
RadiusMessage::RadiusMessage( const RadiusPacketType code )
{
    cpLog( LOG_DEBUG_STACK, "Create packet with code %d", code );

    // These are the requests that we handle now
    assert( code == RP_ACCOUNTING_REQUEST ||
            code == RP_ACCESS_REQUEST );

    memset( &myData, 0, sizeof( struct RadiusHdr ) );
    myData.msgHdr.code = code;
    myData.msgHdr.length = htons( sizeof( struct RadiusHdr ) );
    memset( &myRequestAuthenticator, 0, RadiusAuthenticatorLength );

    // Attributes are added later
    // Identifier is set and Authenticator is calculated when message is sent
}
 

/** Create a RADIUS Message with type, identifier and an Authenticator
    No Attriubte yet
    Use for composing a response
 */
RadiusMessage::RadiusMessage( const RadiusPacketType code,
                              const u_int8_t requestId,
                              const u_int8_t *requestAuth )
{
    cpLog( LOG_DEBUG_STACK, "Create packet with code %d", code );

    assert( requestAuth != 0 );

    // These are the responses that we handle now
    assert( code == RP_ACCOUNTING_RESPONSE ||
            code == RP_ACCESS_REJECT ||
            code == RP_ACCESS_CHALLENGE ||
            code == RP_ACCESS_ACCEPT );

    memset( &myData, 0, sizeof( struct RadiusHdr ) );
    myData.msgHdr.code = code;
    myData.msgHdr.identifier = requestId;
    myData.msgHdr.length = htons( sizeof( struct RadiusHdr ) );

    // save original request Authenticator for later use
    memcpy( &myRequestAuthenticator, requestAuth, RadiusAuthenticatorLength );

    // Attributes are added later
    // Authenticator is calculated when message is sent
}
 
/** Create a RADIUS Message from data
    Use when a message is received
 */
RadiusMessage::RadiusMessage( const RadiusData rawMsg, const char* secret )
    throw( VRadiusException& )
{
    cpLog( LOG_DEBUG_STACK, "Create packet with raw data" );

    u_int16_t msgLen = rawMsg.length();

    if( msgLen < RadiusMinPacketSize || msgLen > RadiusMaxPacketSize )
    {
        throw VRadiusException( "Invalid packet size",
                                __FILE__,
                                __LINE__ );
    }

    // Save raw packet
    memcpy( myData.buffer, rawMsg.data(), msgLen );

    if( msgLen != ntohs(myData.msgHdr.length) )
    {
        throw VRadiusException( "Packet size and length field don't match",
                                __FILE__,
                                __LINE__ );
    }

    try
    {
        decodeAttributes( secret );
    }
    catch( VRadiusException &e )
    {
        throw e;
    }

    if( myData.msgHdr.code == RP_ACCOUNTING_REQUEST )
    {
        // Verify Accounting Request Authenticator
        if( verifyAccountingRequestAuthenticator( secret ) == false )
        {
            throw VRadiusException( "Accounting Request Authentication failed",
                                    __FILE__,
                                    __LINE__ );
        }
    }
    // Access Request Authenticator is not verified
    // Response Authenticator should be verified with the original
    //   request Authenticator later
}


/** Decode Attributes
    If an Attribute is received in an Access-Request but an
    exception is thrown, an Access-Reject SHOULD be transmitted.
    If an Attribute is received in an Access-Accept, Access-Reject
    or Access-Challenge packet and an exception is thrown, the
    packet must either be treated as an Access-Reject or else
    silently discarded.

    TODO: Enforce attribute quantity guide Section 5.44 of RFC 2865
 */
void
RadiusMessage::decodeAttributes( const char* secret )
    throw( VRadiusException& )
{
    u_int16_t totalLen = ntohs( myData.msgHdr.length );

    cpLog( LOG_DEBUG_STACK,
           "Received code=%d id=%d length=%d",
           myData.msgHdr.code, myData.msgHdr.identifier, totalLen);
    
    // Move to the beginning of Attributes
    u_int32_t parsed = sizeof( RadiusHdr );
    const u_int8_t *msgPtr = myData.buffer + parsed;

    const u_int8_t            *attrPtr;

    RadiusAttributeType attrType;
    u_int8_t            attrLen;

    u_int32_t vendorId;    // For VSA

    // Build Attribute list
    while( parsed < totalLen )
    {
        attrPtr  = msgPtr;
        attrType = *attrPtr++;
        attrLen  = *attrPtr++;
        msgPtr += attrLen;
        parsed += attrLen;

        if( (attrLen < 2) || (parsed > totalLen) )
        {
            throw VRadiusException( "Invalid Attribute length",
                                    __FILE__,
                                    __LINE__ );
        }

        if( attrType == RA_VENDOR_SPECIFIC )    // VSA
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
            if( attrLen > 8 )
            {
                // Extract Vendor-Id
                vendorId = ntohl( *(reinterpret_cast< const u_int32_t * >(attrPtr)) );
                attrPtr += 4;                // Move pointer to Vendor type
                // Exclude Type, Length and Vendor-Id
                RadiusData rdat( attrPtr, attrLen - 6 );
                RadiusAttribute attr( attrType, rdat, vendorId );
                myAttributes.push_back( attr );
                // Multiple subattributes may be encoded within a single
                // Vendor-Specific attribute. They should further be decoded
                // by a vendor-specific application.
            }
            else
            {
                throw VRadiusException( "VSA has invalid length",
                                        __FILE__,
                                        __LINE__ );
            }
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
            if( attrType == RA_USER_PASSWORD )
            {
                if( myData.msgHdr.code != RP_ACCESS_REQUEST )
                {
                    throw VRadiusException( "User-Password Attribute in Access-Request",
                                            __FILE__,
                                            __LINE__ );
                }
                if( ((attrLen-2)%16) != 0 )
                {
                    throw VRadiusException( "Invalid User-Password Attribute Length",
                                            __FILE__,
                                            __LINE__ );
                }
                // Decode password
                RadiusData hiddenPassword( attrPtr, attrLen - 2 );
                u_int8_t decodedPassword[ 256 ];
                decodeUserPassword( decodedPassword, secret, hiddenPassword );
                // Store decoded password in attribute list
                RadiusData password( decodedPassword, attrLen - 2 );
                RadiusAttribute attr( attrType, password );
                myAttributes.push_back( attr );
            }
            else
            {
                // Exclude Type and Length
                RadiusData rdat( attrPtr, attrLen - 2 );
                RadiusAttribute attr( attrType, rdat );
                myAttributes.push_back( attr );
            }
        }
    }
}

bool
RadiusMessage::verifyAccountingRequestAuthenticator( const char* secret )
{
    cpLog( LOG_DEBUG_STACK, "Verify Accounting Request Authenticator" );

    assert( myData.msgHdr.code == RP_ACCOUNTING_REQUEST );

    // Copy of Authenticator
    u_int8_t auth[ RadiusAuthenticatorLength ];
    memcpy( auth, myData.msgHdr.authenticator, RadiusAuthenticatorLength );

    // Re-calculate
    calcAccountingRequestAuthenticator( secret );

    // Compare
    return( 0 == memcmp( auth,
                         myData.msgHdr.authenticator,
                         RadiusAuthenticatorLength ) );
}

bool
RadiusMessage::verifyResponseAuthenticator( const u_int8_t *reqAuth,
                                            const char* secret )
{
    cpLog( LOG_DEBUG_STACK, "Verify Response Authenticator" );

    assert( myData.msgHdr.code == RP_ACCESS_ACCEPT ||
            myData.msgHdr.code == RP_ACCESS_REJECT ||
            myData.msgHdr.code == RP_ACCOUNTING_RESPONSE );

    // Copy Request Authenticator
    memcpy( myRequestAuthenticator, reqAuth, RadiusAuthenticatorLength );

    // Copy Response Authenticator
    u_int8_t respAuth[ RadiusAuthenticatorLength ];
    memcpy( respAuth, myData.msgHdr.authenticator, RadiusAuthenticatorLength );

    // Re-calculate
    calcResponseAuthenticator( secret );

    return( 0 == memcmp( respAuth,
                         myData.msgHdr.authenticator,
                         RadiusAuthenticatorLength ) );
}

bool
RadiusMessage::add( const RadiusAttribute& attr )
{
    cpLog( LOG_DEBUG_STACK, "Add Attribute of type %d", attr.type() );

    u_int16_t attrLen = attr.length();
    u_int16_t currLen = ntohs( myData.msgHdr.length );
    if( (currLen + attrLen) > RadiusMaxPacketSize )
    {
        cpLog( LOG_ERR, "Cannot add %d octets, current length is %d",
                        attrLen, myData.msgHdr.length );
        return false;
    }

    memcpy( myData.buffer + currLen,
            attr.encode().data() ,
            attrLen );
    myData.msgHdr.length = htons( currLen + attrLen );

    myAttributes.push_back( attr );

    return true;
}

/** Encode User-Password in Access-Request according to RFC 2865 Section 5.2

      Call the shared secret S and the pseudo-random 128-bit Request
      Authenticator RA.  Break the password into 16-octet chunks p1, p2,
      etc.  with the last one padded at the end with nulls to a 16-octet
      boundary.  Call the ciphertext blocks c(1), c(2), etc.  We'll need
      intermediate values b1, b2, etc.

         b1 = MD5(S + RA)       c(1) = p1 xor b1
         b2 = MD5(S + c(1))     c(2) = p2 xor b2
                .                       .
                .                       .
                .                       .
         bi = MD5(S + c(i-1))   c(i) = pi xor bi

      The String will contain c(1)+c(2)+...+c(i) where + denotes
      concatenation.
 */
void
RadiusMessage::encodeUserPassword( u_int8_t* newPassword,
                                   const char* secret,
                                   const RadiusData& oldPassword )
{
    cpLog( LOG_DEBUG_STACK, "Encode User-Password" );

    int secretLen = strlen(secret);
    assert( secretLen < 256 );

    int hashLen = strlen(secret) + RadiusAuthenticatorLength;
    u_int8_t hashInput[ 256 + RadiusAuthenticatorLength ];

    // Secret + Request Authenticator
    memcpy( hashInput, secret, secretLen );
    memcpy( hashInput + secretLen,
            myData.msgHdr.authenticator,
            RadiusAuthenticatorLength );

    assert( (oldPassword.length() % 16) == 0 );

    const u_int8_t* p = oldPassword.data();
    u_int8_t b[ 16 ];

    unsigned int j;
    int i;
    for( i = 0; i < oldPassword.length(); i += sizeof(b) )
    {
        if( i )
        {
            memcpy( hashInput + secretLen,
                    newPassword + (i - sizeof(b)),
                    sizeof(b) );
        }
        calcMD5( b, hashInput, hashLen );
        for( j = 0; j < sizeof(b); ++j )
        {
            newPassword[i+j] = *(p+i+j) ^ b[j];
        }
    }
}

/** Decode User-Password in Access-Request

      Reversal of encodeUserPassword above
 */
void
RadiusMessage::decodeUserPassword( u_int8_t* newPassword,
                                   const char* secret,
                                   const RadiusData& oldPassword )
{
    cpLog( LOG_DEBUG_STACK, "Decode User-Password" );

    int secretLen = strlen(secret);
    assert( secretLen < 256 );

    int hashLen = strlen(secret) + RadiusAuthenticatorLength;
    u_int8_t hashInput[ 256 + RadiusAuthenticatorLength ];

    memcpy( hashInput, secret, secretLen );

    assert( (oldPassword.length() % 16) == 0 );

    const u_int8_t* p = oldPassword.data();
    u_int8_t b[ 16 ];

    unsigned int j;
    int i;
    for( i = oldPassword.length() - sizeof(b); i >= 0; i -= sizeof(b) )
    {
        if( i )
        {
            memcpy( hashInput + secretLen,
                    p + (i - sizeof(b)),
                    sizeof(b) );
        }
        else
        {
            memcpy( hashInput + secretLen,
                    myData.msgHdr.authenticator,
                    RadiusAuthenticatorLength );
        }
        calcMD5( b, hashInput, hashLen );
        for( j = 0; j < sizeof(b); ++j )
        {
            newPassword[i+j] = *(p+i+j) ^ b[j];
        }
    }
}

/** Hide User-Password in Access-Request
 */
void
RadiusMessage::hideUserPassword( const char* secret )
{
    cpLog( LOG_DEBUG_STACK, "Hide User-Password" );

    assert( myData.msgHdr.code == RP_ACCESS_REQUEST );

    RadiusData password;
    try
    {
        password = get( RA_USER_PASSWORD ).value();
    }
    catch( VRadiusException& e )
    {
        cpLog( LOG_WARNING, "%s", e.getDescription().c_str() );
        return;
    }
    
    u_int8_t hiddenPassword[ 256 ];
    encodeUserPassword( hiddenPassword, secret, password );

    // Replace password in raw message with hidden password
    int offset = RadiusPacketHeaderSize;
    for ( RadiusAttrIter itr = myAttributes.begin();
          itr != myAttributes.end();
          itr++ )
    {   
        if( itr->type() == RA_USER_PASSWORD )
        {
            memcpy( myData.buffer + offset + 2,
                    hiddenPassword,
                    password.length() );
        }
        else
        {
            offset += itr->length();
        }
    }
}

/** Calculate and set the Authenticator
    Must be called right before the packet is sent
 */
void
RadiusMessage::calcAuthenticator( const char* secret )
{
    switch( myData.msgHdr.code )
    {
        case RP_ACCESS_REQUEST:
        {
            calcAccessRequestAuthenticator();
            hideUserPassword( secret );
            break;
        }
        case RP_ACCESS_ACCEPT:
        case RP_ACCESS_REJECT:
        case RP_ACCOUNTING_RESPONSE:
        case RP_ACCESS_CHALLENGE:
        {
            calcResponseAuthenticator( secret );
            break;
        }
        case RP_ACCOUNTING_REQUEST:
        {
            calcAccountingRequestAuthenticator( secret );
            break;
        }
        default:
        {
            cpLog( LOG_ERR, "Cannot calculate Authenticator for code %d",
                            myData.msgHdr.code );
            assert( 0 );
        }
    }
}

/** Calculate and set the myRequestAuthenticator for Access-Request packets
 */
void
RadiusMessage::calcAccessRequestAuthenticator()
{
    // Generate a global and temporal unique Authenticator
    // Request Authenticator = MD5( time + ip + id )
    try
    {
        u_int8_t hashInput[ 256 ];
        unsigned int t = time( NULL );
        memcpy( hashInput, &t, sizeof(t) );
        int hashLen = sizeof(t);
        memcpy( hashInput+hashLen,
                get( RA_NAS_IP_ADDRESS ).value().data(),
                get( RA_NAS_IP_ADDRESS ).value().length() );
        hashLen += get( RA_NAS_IP_ADDRESS ).value().length();
        memcpy( hashInput+hashLen,
                &myData.msgHdr.identifier,
                sizeof(myData.msgHdr.identifier) );
        hashLen += sizeof(myData.msgHdr.identifier);

        // Calculate
        calcMD5( myRequestAuthenticator, hashInput, hashLen );

        // Set Authenticator in Access Request raw message
        memcpy( myData.msgHdr.authenticator,
                myRequestAuthenticator,
                RadiusAuthenticatorLength );
    }
    catch( VRadiusException &e )
    {
        cpLog( LOG_ERR, "Calculate Access Request Authenticator: %s",
                                    e.getDescription().c_str() );
    }
}

/** Calculate and set the Authenticator for Response (Access-Accept,
    Access-Reject, Access-Challenge or Accounting Response) packets
 */
void
RadiusMessage::calcResponseAuthenticator( const char* secret )
{
    // See RFC 2865 Section 3 and RFC 2866 Section 3
    // MD5( Code + ID + Length + myRequestAuthenticator + Attributes + Secret )

    u_int16_t msgLen = ntohs( myData.msgHdr.length );

    // TODO: make buffer bigger to accommodate secret
    assert( (msgLen + strlen(secret)) <= RadiusMaxPacketSize );

    // Load original request's Authenticator
    memcpy( myData.msgHdr.authenticator,
            myRequestAuthenticator,
            RadiusAuthenticatorLength );

    // Load secret behind message
    memcpy( myData.buffer + msgLen, secret, strlen(secret) );

    u_int8_t auth[ RadiusAuthenticatorLength ];

    // Calculate
    calcMD5( auth, myData.buffer, msgLen + strlen(secret) );

    // Set Authenticator in response packet
    memcpy( myData.msgHdr.authenticator,
            auth,
            RadiusAuthenticatorLength );
}

/** Calculate and set the Authenticator for Accounting request packets
 */
void
RadiusMessage::calcAccountingRequestAuthenticator( const char* secret )
{
    // See RFC 2866 Section 3
    // MD5( Code + ID + Length + 16 zero octets + Attributes + Secret )

    cpLog( LOG_DEBUG_STACK, "Calculate Accounting Request Authenticator" );

    u_int32_t msgLen = ntohs( myData.msgHdr.length );

    assert( (msgLen + strlen(secret)) <= RadiusMaxPacketSize );

    memset( myData.msgHdr.authenticator, 0, RadiusAuthenticatorLength );

    // Load secret behind message
    memcpy( myData.buffer + msgLen, secret, strlen(secret) );

    // Calculate
    calcMD5( myRequestAuthenticator, myData.buffer, msgLen + strlen(secret) );

    // Set Authenticator in request packet
    memcpy( myData.msgHdr.authenticator,
            myRequestAuthenticator,
            RadiusAuthenticatorLength );
}

void
RadiusMessage::calcMD5( u_int8_t *digest,
                        const u_int8_t *buf,
                        const u_int32_t bufLen )
{
    cpLog( LOG_DEBUG_STACK, "Calculate MD5" );

    MD5Context context;
    MD5Init( &context );
    MD5Update( &context, buf, bufLen );
    MD5Final( digest, &context );
}


RadiusData
RadiusMessage::encodeAttributes() const
{
    RadiusData msg;

    for( RadiusAttrIter itr = myAttributes.begin();
         itr != myAttributes.end();
         itr++ )
    {
        msg += (*itr).encode();
    }
    return msg;
}


const RadiusAttribute&
RadiusMessage::get( const RadiusAttributeType t ) const
    throw( VRadiusException& )
{
    for ( RadiusAttrIter itr = myAttributes.begin();
          itr != myAttributes.end();
          itr++ )
    {
        if( itr->type() == t )
        {
            // Found one
            return *itr;
        }
    }
    string e( "No attributes of type " + itos( t ) + " found in Attribute list" );
    // Not found
    throw VRadiusException( e, __FILE__, __LINE__ );
}


list< RadiusAttribute >
RadiusMessage::getAll( const RadiusAttributeType t ) const
{
    list< RadiusAttribute > attrList;

    for ( RadiusAttrIter itr = myAttributes.begin();
          itr != myAttributes.end();
          itr++ )
    {
        if( itr->type() == t )
        {
            attrList.push_back( *itr );
        }
    }
    return attrList;
}


// Get a human readable representation of the message
string
RadiusMessage::verbose() const
{
    return string( hexDump() + headerDump() + attributesVerbose() );
}

// Raw message in hex for debugging
string
RadiusMessage::hexDump() const
{
    string s;        // Complete output (all lines)
    string a;        // "Ascii" portion of a line
    u_int8_t temp;   // One octet
    u_int8_t hi;     // High hex digit of an octet
    u_int8_t low;    // Low hex digit of an octet
    char buf[4];     // Output buffer for one octet
    char cntBuf[6];  // Output buffer for counter at the beginning of a line
    int msgLen = ntohs(myData.msgHdr.length);

    int i;
    for( i = 0; i < msgLen; i++ )
    {
        if( 0 == (i % 16) ) // 16 in a line
        {
            snprintf( cntBuf, 6, "%04x ", i );
            s += a + "\n" + cntBuf;
            a = "   ";
        }
        temp = myData.buffer[i];
        hi  = (temp & 0xf0) / 16;
        low = (temp & 0xf);
        snprintf( buf, 4, " %x%x", hi, low );
        if( temp < 0x20 || temp > 0x7E)
        {
            a += ".";
        }
        else
        {
           // ! a += reinterpret_cast<u_char>(temp);
			  a += temp;
        }
        s += buf;
    }
    // Last line only:
    // Pad with ' ' to make the "ascii" interpreted part line up with
    //   the previous line
    int j = i % 16;
    string b;    // Blanks
    if( j )
    {
        b = string( (16-j)*3, ' ' );
    }
    s += b + a + "\n";
    return s;
}

// Get a human readable representation of the message header
string
RadiusMessage::headerDump() const
{
    string codeStr;
    switch( myData.msgHdr.code )
    {
        case RP_ACCESS_REQUEST:
        {
            codeStr = "Access-Request (1)";
            break;
        }
        case RP_ACCESS_ACCEPT:
        {
            codeStr = "Access-Accept (2)";
            break;
        }
        case RP_ACCESS_REJECT:
        {
            codeStr = "Access-Reject (3)";
            break;
        }
        case RP_ACCOUNTING_REQUEST:
        {
            codeStr = "Accounting-Request (4)";
            break;
        }
        case RP_ACCOUNTING_RESPONSE:
        {
            codeStr = "Accounting-Response (5)";
            break;
        }
        case RP_ACCESS_CHALLENGE:
        {
            codeStr = "Access-Challenge (11)";
            break;
        }
        default:
        {
            codeStr = "Unknown (" + itos( myData.msgHdr.code ) + ")";
        }
    }
    return string( "\n  1 Code   = " + codeStr +
                   "\n  1 ID     = " + itos( myData.msgHdr.identifier ) +
                   "\n  2 Length = " + itos( ntohs(myData.msgHdr.length) ) +
                   "\n 16 Authenticator\n" );
}

string
RadiusMessage::attributesVerbose() const
{
    string s = "\nAttributes:\n";

    for ( RadiusAttrIter itr = myAttributes.begin();
          itr != myAttributes.end();
          itr++ )
    {
        s += itr->verbose();
    }
    return s;
}


// Local Variables:
// mode:c++
// c-file-style:"bsd"
// c-basic-offset:4
// c-file-offsets:((inclass . ++))
// indent-tabs-mode:nil
// End:

