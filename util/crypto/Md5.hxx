#ifndef MD5_HXX_
#define MD5_HXX_

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

static const char* const Md5_hxx_version =
    "$Id: Md5.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "Data.hxx"
#include "vmd5.h"

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to do cryptographic things.
 */
namespace Crypto
{


/** class to implement the MD5 message digest algorithm.
 */
class Md5
{
    public:
	/// constructor
	Md5();

	/// copy constructor -- returns the same state as the original
	Md5(const Md5& src);

	/// initialize and take a data
	Md5(const Data& data);

	/// equality operator 
	bool operator==(const Md5& src) const;

	/// less than operator
	bool operator<(const Md5& src) const;

	/// greater than operator
	bool operator>(const Md5& src) const;

	/// set the digest in an empty md5
	void digest(const Data& data);

	/// hash function
	size_t hashfn() const;

	/// destructor
	~Md5();

    private:
	MD5Context myContext;
	unsigned char myDigest[16];
};


inline
Md5::Md5()
{
    memset(myDigest, 0, 16);
}


inline
Md5::Md5(const Md5& src)
{
    memcpy(myDigest, src.myDigest, 16);
}


inline
Md5::Md5(const Data& data)
{
    MD5Init(&myContext);
    LocalScopeAllocator lo;
    MD5Update(&myContext, 
	      reinterpret_cast<const unsigned char*>(data.getData(lo)), 
	      static_cast<unsigned int>(data.length()));
    MD5Final(myDigest, &myContext);
}


inline void
Md5::digest(const Data& data)
{
    MD5Init(&myContext);
    LocalScopeAllocator lo;
    MD5Update(&myContext, 
	      reinterpret_cast<const unsigned char*>(data.getData(lo)), 
	      static_cast<unsigned int>(data.length()));
    MD5Final(myDigest, &myContext);
}


inline size_t
Md5::hashfn() const
{
    return *reinterpret_cast<size_t const*>(myDigest);
}


inline
Md5::~Md5()
{
}


inline bool
Md5::operator==(const Md5& src) const
{
    if(&src == this)
    {
	return true;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) == 0);
    }
}


inline bool
Md5::operator<(const Md5& src) const
{
    if(&src == this)
    {
	return false;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) < 0);
    }
}


inline bool
Md5::operator>(const Md5& src) const
{
    if(&src == this)
    {
	return false;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) > 0);
    }
}


} // namespace Crypto
} // namespace Vocal
#endif
