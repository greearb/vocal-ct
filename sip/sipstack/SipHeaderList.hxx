#ifndef SIP_HEADER_LIST_HXX_
#define SIP_HEADER_LIST_HXX_

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

static const char* const SipHeaderList_hxx_Version =
    "$Id: SipHeaderList.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipRawHeader.hxx"
#include "Sptr.hxx"

namespace Vocal
{


/** this class contains a list of headers. */
template <class Header>
class SipHeaderList
{
    public:
	class iterator {
	    public:
		iterator() : myIterator()
		{}
		iterator(Sptr<SipRawHeader> src) : myIterator(src)
		{}
		iterator(const iterator& src) : myIterator(src.myIterator)
		{}
		~iterator()
		{}
		const iterator& operator=(const iterator& src)
		{
		    if(this != &src)
		    {
			myIterator = src.myIterator;
		    }
		    return *this;
		}
		const iterator& operator++() 
		{
		    if(myIterator != 0)
		    {
			myIterator = myIterator->next;
		    }
		    return *this;
		}
		iterator operator++(int) 
		{
		    iterator tmp = *this;
		    if(myIterator != 0)
		    {
			myIterator = myIterator->next;
		    }
		    return tmp;
		}
		Sptr <Header> operator*()
		{
		    myIterator->parse();
		    Sptr<Header> x;
		    x.dynamicCast(myIterator->header);
		    return x;
		}

		bool operator==(const iterator& orig) const
		{
		    return (myIterator == orig.myIterator);
		}

		bool operator!=(const iterator& orig) const
		{
		    return (myIterator != orig.myIterator);
		}

	    private:
		Sptr <SipRawHeader> myIterator;
	};


	/// default constructor
	SipHeaderList();

	/// constructor from RawHeader
	SipHeaderList(Sptr<SipRawHeader> orig);
	/// copy constructor
	SipHeaderList(const SipHeaderList& src);
	/// assignment operator
	const SipHeaderList& operator= (const SipHeaderList& src);

	iterator begin() const
	{
	    return myHeader;
	}

	iterator end() const
	{
	    return iterator(0);
	}

	bool empty() const
	{
	    return (myHeader == 0);
	}


    private:

	Sptr<SipRawHeader> myHeader;
};


template <class Header>
inline
SipHeaderList<Header>::SipHeaderList()
{
}

template <class Header>
inline
SipHeaderList<Header>::SipHeaderList(Sptr<SipRawHeader> orig)
    : myHeader(orig)
{
}


template <class Header>
inline
SipHeaderList<Header>::SipHeaderList (const SipHeaderList& src)
    : myHeader(src.myHeader)
{
}


template <class Header>
inline 
const SipHeaderList<Header>& 
SipHeaderList<Header>::operator= (const SipHeaderList& src)
{
    if(&src != this)
    {
	myHeader = src.myHeader;
    }
    return *this;
}
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
