#ifndef BaseUrl_HXX
#define BaseUrl_HXX

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

static const char* const BaseUrl_hxx_Version =
    "$Id: BaseUrl.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
    
#include "global.h"
#include "Data.hxx"
#include "Sptr.hxx"
#include "BaseUser.hxx"


namespace Vocal
{
    
/// URL Types    
typedef enum
{
    SIP_URL,
    TEL_URL,
    IM_URL,
    PRES_URL,
    UNKNOWN_URL,
    NULL_URL
} UrlType;
 

/// class for Url types
class BaseUrl
{
    public:
	/// Create one with default values
	BaseUrl();
	///
	virtual ~BaseUrl();
	//parser functions
	static Sptr<BaseUrl> decode(const Data& data, const string& local_ip);
	///
	virtual Data encode() const = 0;
	///
	virtual Sptr<BaseUser> getUser() const = 0;
	///
	virtual Data getUserValue() const = 0;
	///
	virtual Data getUniqueKey() const = 0;
	///
	virtual void setUser(Sptr <BaseUser> ) = 0;
	///
	virtual bool setUserValue(const Data&, const Data& dummy = "") = 0;
	///
	virtual UrlType getType() const = 0;
        ///
        virtual bool isLooseRouterPresent() const { return false; }
	///
	virtual Sptr<BaseUrl> duplicate() const = 0;
	///
	virtual bool areEqual(Sptr<BaseUrl>) const = 0;
	///
	virtual bool isLessThan(Sptr<BaseUrl>) const = 0;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const = 0;

        virtual ostream& dump(ostream& s) const;
        
#if 0
	/// friend operator for equality
	friend bool operator==(const BaseUrl& left, const BaseUrl& right);
#endif
};


/** this method duplicates a URL.  if the second parameter is true, it
 * returns a NullUrl instead of NULL if the original is NULL or
 * NullUrl.  if the second parameter is false (default), it returns
 * NULL if the original is NULL or NullUrl.
 */

Sptr<BaseUrl> duplicateUrl(const Sptr<BaseUrl> original, 
			   bool returnNullUrl = false);

ostream&
operator<<(ostream& s, const Sptr<BaseUrl>& url);


// end of vocal namespace
}

// 24/11/03 fpi
// WorkAround Win32
#if USE_HASH_MAP

#if !defined(__SUNPRO_CC)
#include <hash_map.h>
#if ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1) )
namespace __gnu_cxx
#else
namespace std
#endif
{
    struct hash< Sptr < Vocal::BaseUrl > >
    {
        size_t operator() ( const Sptr < Vocal::BaseUrl >& src ) const
        {
            return __stl_hash_string(src->encode().logData());
        }
    };
// end of either __gnu_cxx or std namespace
}

// end for __linux__
#endif
#endif



/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

// end for BaseUrl_HXX
#endif
