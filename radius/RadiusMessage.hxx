#ifndef RadiusMessage_hxx
#define RadiusMessage_hxx

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

static const char* const RadiusMessage_hxx_Version =
    "$Id: RadiusMessage.hxx,v 1.1 2004/05/01 04:15:22 greear Exp $";

#include <string>
#include <list>

#include "RadiusData.hxx"
#include "radius.h"
#include "RadiusAttribute.hxx"
#include "VRadiusException.hxx"


///
typedef list<RadiusAttribute>::const_iterator RadiusAttrIter;


/** RADIUS Packet as defined in RFC 2865 Section 3.
    <pre>
    Data Format
    
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   |                         Authenticator                         |
   |                                                               |
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Attributes ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-
   </pre>
 */
class RadiusMessage
{
    public:
    
        /** Create a RADIUS Message with type, no Attriubte yet
            Useful for composing a request
         */
        RadiusMessage( const RadiusPacketType code );
        
        /** Create a RADIUS Message with type and an Authenticator
            No Attriubte yet
            Useful for composing a response
         */
        RadiusMessage( const RadiusPacketType code,
                       const u_int8_t requestIdentifer,
                       const u_int8_t *requestAuth );
        
        /** Create a RADIUS Message from data

            Use when a message is received

            Request Authenticator is verified automatically. However,
              RadiusMessage::verifyResponseAuthenticator(auth,secret)
              should be called later for Responses.

            If an exception is thrown, the message should be handled
            (discarded or rejected) according to the RFCs.
         */
        RadiusMessage( const RadiusData data, const char* secret )
            throw( VRadiusException& );

        ///
        RadiusMessage( const RadiusMessage& rhs )
        {
            copyRhsToThis( rhs );
        }

        ///
        RadiusMessage& operator=( const RadiusMessage& rhs )
        {
            if( this != &rhs )
            {
                copyRhsToThis( rhs );
            }
            return *this;
        }

        ///
        void copyRhsToThis( const RadiusMessage& rhs )
        {
            memcpy( &myRequestAuthenticator,
                    rhs.myRequestAuthenticator,
                    RadiusAuthenticatorLength );
            myAttributes = rhs.myAttributes;
            myData = rhs.myData;
        }

        ///
        RadiusPacketType type()
        {
            return myData.msgHdr.code;
        }

        /// Encode all Attributes in this list
        RadiusData encodeAttributes() const;

        /// TODO: throw exception instead
        bool add( const RadiusAttribute& attr );

        /// Get the number of attributes
        u_int32_t attributeCount() const
        {
            return myAttributes.size();
        }

        /**
           Get the first instance of Attribute t from the list
           Use getAll( RadiusAttributeType ) to retrieve multiple
             attributes with the same type 
         */
        const RadiusAttribute& get( const RadiusAttributeType attrType ) const
            throw( VRadiusException& );

        /**
           Get the list of all attributes of type attrType
         */
        list< RadiusAttribute > getAll( const RadiusAttributeType attrType ) const;

        /**
            For RADIUS requests only
            Must be called before Authenticator is calculated
         */
        void setIdentifier( const u_int8_t& id )
        {
            myData.msgHdr.identifier = id;
        }

        ///
        u_int8_t getIdentifier() const
        {
            return myData.msgHdr.identifier;
        }

        /** Calculate and set the Authenticator
            Must be called right before the packet is sent
         */
        void calcAuthenticator( const char* secret );

        /** Get the Authenticator
         */
        const u_int8_t* getAuthenticator() const
        {
            return myRequestAuthenticator;
        }

        /** Verify a response with the original request's Authenticator
         */
        bool verifyResponseAuthenticator( const u_int8_t *reqAuth,
                                          const char* secret );

        /** Get raw message data for sending
         */
        const RawMessage& data() const
        {
            return myData;
        }

        /// Get a human readable representation of all attributes
        string verbose() const;

        /// Raw message in hex for debugging
        string hexDump() const;

        /// Get a human readable representation of the message header
        string headerDump() const;

        /// Get a human readable representation of the message
        string attributesVerbose() const;

    private:

        /// Disable default constructor
        RadiusMessage()
        {
        }

        /** Decode Attributes
            If an Attribute is received in an Access-Request but an
            exception is thrown, an Access-Reject SHOULD be transmitted.
            If an Attribute is received in an Access-Accept, Access-Reject
            or Access-Challenge packet and an exception is thrown, the
            packet must either be treated as an Access-Reject or else
            silently discarded.
         */
        void decodeAttributes( const char* secret )
            throw( VRadiusException& );

        ///
        bool verifyAccountingRequestAuthenticator( const char* secret );

        /** Encode User-Password in Access-Request
              oldPassword length must be a multiple of 16 octets
         */
        void encodeUserPassword( u_int8_t* newPassword,
                                 const char* secret,
                                 const RadiusData& oldPassword );

        /** Decode User-Password in Access-Request
              oldPassword length must be a multiple of 16 octets
         */
        void decodeUserPassword( u_int8_t* newPassword,
                                 const char* secret,
                                 const RadiusData& oldPassword );

        /// Hide User-Password in Access-Request
        void hideUserPassword( const char* secret );

        /// Calculate and set myRequestAuthenticator for Access-Request packets
        void calcAccessRequestAuthenticator();

        /** Calculate and set the Authenticator for response (Access-Accept,
            Access-Reject, Access-Challenge or Accounting Response) packets
         */
        void calcResponseAuthenticator( const char* secret );

        /// Calculate and set the Authenticator for Accounting request packets
        void calcAccountingRequestAuthenticator( const char* secret );

        void calcMD5( u_int8_t *digest,
                      const u_int8_t *buf,
                      const u_int32_t bufLen );

        /// Request Authenticator
        u_int8_t myRequestAuthenticator[ RadiusAuthenticatorLength ];

        // TODO: may need a flag to keep myAttrList and myData in sync

        /// Attribute list
        list< RadiusAttribute > myAttributes;

        /// RADIUS packet (in network byte order)
        RawMessage myData;
};

// Local Variables:
// mode:c++
// c-file-style:"bsd"
// c-basic-offset:4
// c-file-offsets:((inclass . ++))
// indent-tabs-mode:nil
// End:

#endif
