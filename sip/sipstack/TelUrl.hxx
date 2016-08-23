#ifndef TelUrl_HXX
#define TelUrl_HXX

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
#include "SipTelSubscriberUser.hxx"

namespace Vocal
{
    
/// Exception handling class TelUrlParserException

enum TelUrlErrorType
{
    TEL_DECODE_FAILED,
    TEL_NOT_PRESENT,
    TEL_USER_INFO_ERR,
    TEL_MANDATORY_ITEM_MISSING,
    TEL_NOT_VALID_URL_PARMS,
    TEL_NOT_VALID_TRANSORT_PARM,
    TEL_NOT_VALID_USER_PHONE_PARM,
    TEL_NOT_VALID_TTL_PARM,
    TEL_NOT_VALID_URL_DATA 
};

///
class TelUrlParserException : public VException
{
    public:
        ///
        TelUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               const int error = 0 );
        ///
        TelUrlParserException( const string& msg,
                               const string& file,
                               const int line,
                               TelUrlErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        TelUrlErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    
    private:
        TelUrlErrorType value;

};

/// data container for URLs of type tel:
class TelUrl : public BaseUrl
{
    public:
	/// Create one with default values
	TelUrl();
	///
	TelUrl(const Data& data) throw(TelUrlParserException &) ;
	///
	TelUrl(const TelUrl& );
    
        // Help up-cast safely.
        virtual bool isSipUrl() const { return false; }

	///
        Sptr<BaseUrl> duplicate() const;
        ///
        bool areEqual(Sptr<BaseUrl>) const;
        ///
        bool isLessThan(Sptr<BaseUrl>) const;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const;
        ///
	bool operator<(const TelUrl& srcUrl) const;
	///
	bool operator>(const TelUrl& srcUrl) const;
    
	///
	bool operator ==(const TelUrl& srcUrl) const;
    
	///
	TelUrl& operator =(const TelUrl& srcUrl);
	///
	bool setUserValue(const Data& newUser, const Data& dummy = "" );
	///used inside the stack and is not part of the API 
	Data encode() const ;

	///parser functions
	void decode(const Data& data);
    
	///
	Data getUserValue() const;
    
	///
	Sptr <BaseUser> getUser() const;
    
	///
	void setUser(Sptr <BaseUser> newUser);
  
	///
	UrlType getType() const;
        /// get a unique key for a transaction id
        Data getUniqueKey() const;
    
    
	///
    private:
	///
	Data schemeName;
	/// 
	Sptr <SipTelSubscriberUser> user;
    
	///
	bool fastDecode( Data myData );
};
 
} // namespace Vocal


#endif
