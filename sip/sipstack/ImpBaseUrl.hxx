#ifndef ImpBaseUrl_HXX
#define ImpBaseUrl_HXX

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

#include <string>
#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "VException.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"


namespace Vocal
{

enum ImpBaseUrlErrorType
{
    IMP_DECODE_FAILED,
    IMP_USER_INFO_ERR,
    IMP_MANDATORY_ITEM_MISSING,
    IMP_NOT_VALID_URL_DATA 
};

/// Exception handling class ImpBaseUrlParserException
class ImpBaseUrlParserException : public VException
{
    public:
        ///
        ImpBaseUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               const int error = 0 );
    
        ///
        ImpBaseUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               ImpBaseUrlErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        ImpBaseUrlErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const
        {
            return "ImpBaseUrlParserException";
        }
    private:
        ImpBaseUrlErrorType value;

};

/// data container for Url header
class ImpBaseUrl : public BaseUrl
{
    public:
	/// Create one with default values
	ImpBaseUrl();
	///
	explicit ImpBaseUrl(const Data& data, const Data type);
	///
	ImpBaseUrl(const ImpBaseUrl& );
    
	///
        bool areEqual(Sptr<BaseUrl>) const;
        ///
        bool isLessThan(Sptr<BaseUrl>) const;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const;

        ///
	bool operator<(const ImpBaseUrl& srcUrl) const;
	///
	bool operator>(const ImpBaseUrl& srcUrl) const;
    
	///
	bool operator ==(const ImpBaseUrl& srcUrl) const;
    
	///
	ImpBaseUrl& operator =(const ImpBaseUrl& srcUrl);

        ///
        virtual Sptr<BaseUrl> duplicate() const = 0;
	///
	Data getUserValue() const ;  
	///
	bool setUserValue(const Data& newuser , const Data& noUse="") ; 
	///
	const Data& getHost() const ;
	///
	void setHost(const Data& newhost) ;
    
	///
	bool operator != (const ImpBaseUrl& srcUrl) const
        { return !(*this == srcUrl);}
    
	/// return the encoded string version of this. This call should only be
	///used inside the stack and is not part of the API 
	Data encode() const ;
    
        /// get a unique key for a transaction id
        Data getUniqueKey() const;
    
	/// get name and address 
	Data getNameAddr() const;

	///parser functions
	void decode(const Data& data);
	///
	virtual UrlType getType() const = 0;

        ///
        const Data& getSchemeName() const { return schemeName; };
	///
	///Should be removed from the base class as pure virtual
	void setUser(Sptr<BaseUser> newUser) { assert(0); }
	///Should be removed from the base class as pure virtual
	Sptr <BaseUser> getUser() const { assert(0); return 0; };
    protected:
        Data schemeName;
        Data user;
        Data host;
        bool fastDecode( Data myData );
        ///
};
 
}// namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
