#ifndef SIP_RAW_HEADER_CONTAINER_HXX_
#define SIP_RAW_HEADER_CONTAINER_HXX_

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

static const char* const SipRawHeaderContainer_hxx_Version =
    "$Id: SipRawHeaderContainer.hxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include <set>
#include <vector>

#include "SipRawHeader.hxx"
#include "SipHeaderList.hxx"

namespace Vocal
{


/**

   this object is the container which holds all of the headers of a
   SipMsg.  It is a vector of Headers, where the index of the vector
   is indexed by the SipHeaderType.
   <p>

   You should only need to fiddle with this if you are changing how
   SipMsg works.

   <p>
   
*/
class SipRawHeaderContainer : public std::vector < Sptr<SipRawHeader> >
{
    public:
	/// constructor
	SipRawHeaderContainer();
        /// destructor
        ~SipRawHeaderContainer();
        /// modify the copy constructor to deep copy by default
        SipRawHeaderContainer(const SipRawHeaderContainer& src);

        /// assignment operator
        const SipRawHeaderContainer& operator=(const SipRawHeaderContainer& src);
        /// equality operator -- slow but necessary
        bool operator==(const SipRawHeaderContainer& src) const;
        /// inequality operator -- slow but necessary
        bool operator!=(const SipRawHeaderContainer& src) const;

	/// insert a header
	void insertManyHeader(Sptr<SipRawHeader> header);
	/// insert a header
	void insertHeader(Sptr<SipRawHeader> header, int pos=-1);
	/// get a header by type
	Sptr<SipRawHeader> getHeader(SipHeaderType type, int index=0) const;
	/// replace a header with another
	void replaceHeader(SipHeaderType type, Sptr<SipRawHeader> header);

        /// get a header or create one
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        Sptr<SipRawHeader> getHeaderOrCreate(SipHeaderType type, const string& local_ip,
                                             int index=0) const;

	/// decode a series of headers passed in in parseData
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
	bool decode(Data* headers, const string& local_ip);
	/// encode a series of headers passed in in parseData
	bool encode(Data* msg) const;

        /// mask out certain headers
        void mask(std::set <SipHeaderType>& mask );

        /// get the parsed header by modifying p (the smart pointer to the header)
        template <class HeaderType> void
        getParsedHeader(Sptr<HeaderType>& p, SipHeaderType type, 
                        const string& local_ip, int index = 0) const {
            Sptr<SipRawHeader> rawHeader = getHeaderOrCreate(type, local_ip, index);

            rawHeader->parse();
    
            p = (HeaderType*)(rawHeader->header.getPtr());
        }

        /// get a list of headers of a single type (in a SipHeaderList container)
        template <class HeaderType> void
        getHeaderList(SipHeaderList<HeaderType>* p, SipHeaderType type) const
        {
            Sptr<SipRawHeader> rawHeader = getHeader(type);
            *p = SipHeaderList<HeaderType>(rawHeader);
        }

        /// return the number of headers of a given type
        int getNumHeaders( SipHeaderType type ) const;

        /// remove the nth header (coalescing the list)
        void removeHeader ( SipHeaderType type, int index=-1 /* last element */ );

        /// set the number of items (coalescing the list)
        void setNumHeaders ( SipHeaderType type, int number );

        /// add a header at a given position
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        void appendHeader ( SipHeaderType type, const Sptr<SipHeader> p,
                            const string& local_ip, int pos=-1 );

        /// force all headers to be parsed
        void forceParse();
};



template <class HeaderType> HeaderType&
getParsed( 
    const SipRawHeaderContainer& container,
    SipHeaderType type, 
    const string& local_ip,
    int index = 0 )
{
    Sptr<HeaderType> p;
    container.getParsedHeader(p, type, local_ip, index);
    return *p;
}
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
          
#endif
