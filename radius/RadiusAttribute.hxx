#ifndef RadiusAttribute_hxx
#define RadiusAttribute_hxx

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

static const char* const RadiusAttribute_hxx_Version =
    "$Id: RadiusAttribute.hxx,v 1.1 2004/05/01 04:15:22 greear Exp $";

#include "RadiusData.hxx"
#include "radius.h"

/** RADIUS Attribute as defined in RFC 2865 Section 5.
    <pre>
    Attribute Format
    
    0                   1                   2
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
   |     Type      |    Length     |  Value ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
   
   
    Vendor-Specific Attribute format
    
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |  Length       |            Vendor-Id
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        Vendor-Id (cont)           |  String...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
   </pre>
 */
class RadiusAttribute
{
    public:
    
        /// Create a RADIUS Attribute
        RadiusAttribute( const RadiusAttributeType type,
                         const u_int8_t* value,
                         const u_int8_t length,
                         const u_int32_t vendorId = 0 );
        
        ///

        /// Create a RADIUS Attribute
        RadiusAttribute( const RadiusAttributeType type,
                         const RadiusData& value,
                         const u_int32_t vendorId = 0 );
        
        ///

        RadiusAttribute( const RadiusAttribute& rhs )
        {
            copyRhsToThis( rhs );
        }

        ///
        RadiusAttribute& operator=( const RadiusAttribute& rhs )
        {
            if( this != &rhs )
            {
                copyRhsToThis( rhs );
            }
            return *this;
        }

        ///
        void copyRhsToThis( const RadiusAttribute& rhs )
        {
            myType = rhs.myType;
            myValue = rhs.myValue;
            myVendorId = rhs.myVendorId;
        }

        /// Get Attribute type
        RadiusAttributeType type() const
        {
            return myType;
        }
    
        /// Check if this is a Vendor Specific Attribute (VSA)
        bool isVsa() const
        {
            return myType == RA_VENDOR_SPECIFIC;
        }
    
        /// Get the vendor ID, 0 also indicates this is not a VSA
        u_int32_t vendorId() const
        {
            return myVendorId;
        }
    
        /// Get the total length of this Attribute
        u_int16_t length() const
        {
            // 2 octets for Type and Length
            return myValue.length() + 2 + ((myVendorId == 0) ? 0 : 4);
        }

        /// Encode the Attribute
        // TODO: return a RawMessage instead to enhance performance?
        RadiusData encode() const;
    
        /// Get the Attribute value field
        const RadiusData& value() const
        {
            return myValue;
        }

        /// Get a human readable representation of this attribute
        string verbose() const;

    private:

        /// Disable default constructor
        RadiusAttribute()
        {
        }

        /// RADIUS Attribute Type
        RadiusAttributeType  myType;

        /** Attribute Value
            Multiple subattributes may be encoded within a single
            Vendor-Specific attribute. They should further be decoded
            by a vendor-specific application.
         */
        RadiusData           myValue;
    
        /// VSA Vendor-Id, 0 for non-VSA Attributes
        u_int32_t            myVendorId;
};

// Local Variables:
// mode:c++
// c-file-style:"bsd"
// c-basic-offset:4
// c-file-offsets:((inclass . ++))
// indent-tabs-mode:nil
// End:

#endif
