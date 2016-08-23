#ifndef SIP_RAW_HEADER_HXX_
#define SIP_RAW_HEADER_HXX_

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
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
 * IN EXCESS OF ,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

#include "SipHeader.hxx"
#include <set>
#include "Sptr.hxx"

namespace Vocal
{


/** this is the container inside of SipMsg which holds a single
 * header, both in raw form (without parsing), and if the parsing has
 * been done, as a SipHeader object.
*/

class SipRawHeader: public BugCatcher
{
    public:
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
	SipRawHeader(const string& _local_ip);
	SipRawHeader(const SipRawHeader& src);

	Data headerName;
	Data headerNameOriginal;
	Data headerValue;
	SipHeaderType headerType;
	mutable Sptr<SipHeader> header;
	mutable Sptr<SipRawHeader> next;

        /// deep copy from src to this 
        void deepCopy(const SipRawHeader& src);
        /// duplicate this (deep copying) and return a ptr to the copy
        SipRawHeader* duplicate() const;
        /// compare whether these two are equal (deep comparison)
	bool operator==(const SipRawHeader& src) const;
        /// compare whether these two are not equal (deep comparison)
	bool operator!=(const SipRawHeader& src) const;

	/// decode the header
	bool decode(const Data& headerData);
	/// encode this header (deep encoding)
	bool encode(Data* msg) const;

        /// parse whatever is currently in this header
        bool parse() const;
        /** parse whatever is currently in this header and all other
            linked headers */
        bool parseDeep() const;
        /// shallow comparison only (the operator== is deep)
        bool shallowEquals(const SipRawHeader& src) const;
        /** shallow comparison only against a theoretical NUL ptr the
            NUL ptr stands in for an empty SipRawHeader of the proper
            type, which encodes to no text
        */
        bool shallowEqualsNul() const;

    protected:
        string local_ip;

    private:
        // Restricted.
        SipRawHeader();
}; 

inline SipRawHeader::SipRawHeader(const string& _local_ip)
    : headerName(),
      headerNameOriginal(),
      headerValue(),
      headerType(SIP_UNKNOWN_HDR),
      header(),
      local_ip(_local_ip)
{
}

inline SipRawHeader::SipRawHeader(const SipRawHeader& src)
    : headerName(src.headerName),
      headerNameOriginal(src.headerNameOriginal),
      headerValue(src.headerValue),
      headerType(src.headerType),
      header(src.header),
      local_ip(src.local_ip)
{
}
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
