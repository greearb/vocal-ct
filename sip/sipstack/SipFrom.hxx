#ifndef SIPFROM_HXX_
#define SIPFROM_HXX_

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


#include <map>

#include "global.h"
#include "Data.hxx"
#include "BaseUrl.hxx"
#include "VException.hxx"
#include "Sptr.hxx"
#include "SipHeader.hxx"

namespace Vocal
{

enum SipFromErrorType
{
    DECODE_FROM_FAILED,
    URL_FAILED,
    USERINFO_ERROR,
    PARSE_ADDPRAM_ERR
    //may need to change this to be more specific
};

class SipTo;

/// Exception handling class SipFromParserException
class SipFromParserException : public VException
{
    public:
        SipFromParserException( const string& msg,
                                const string& file,
                                const int line,
                                const int error = 0 );
        SipFromParserException( const string& msg,
                                const string& file,
                                const int line,
                                SipFromErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipFromErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipFromErrorType value;

};


/// data container for From header
class SipFrom : public SipHeader
{
    public:

        typedef map < Data , Data, less < Data > > TokenMapFrom;

        ///
        SipFrom(Sptr <BaseUrl> fromurl, const string& local_ip);
        ///
        SipFrom( const Data& data, const string& local_ip,
                 UrlType uType = SIP_URL);
        ///
        SipFrom(const SipFrom& src);

        ///
        explicit SipFrom(const SipTo& to);
        ///
        const bool operator<(const SipFrom&) const;
        ///
        bool operator>(const SipFrom&) const;
        ///
        bool operator ==(const SipFrom&) const ;
        ///
        bool operator != (const SipFrom& aFrom) const
        { return !(*this == aFrom);}
        ///
        const SipFrom& operator =(const SipFrom&);
        ///
        Data encode() const;
        ///
        void setPort(const Data& newport);
        void setPort(int newport);
        ///
        Data getPortData();
        ///
        int getPort() const;
        ///
        void setUser(const Data& newuser);
        ///
        Data getUser() const;
        ///
        void setDisplayName(const Data& name);
        ///
        const Data& getDisplayName() const;
        ///
        void setHost(const Data& newhost);
        ///
        Data getHost() const;
        ///
        const Data& getTag() const;
        ///
        void setTag(const Data& item);

        void setToken(const Data & newtoken)
        {
            token = newtoken;
        }

        Data getToken() const
        {
            return token;
        }

        void setQstring(const Data & newqstring)
        {
            qstring = newqstring ;
        }

        Data getQstring() const
        {
            return qstring;
        }
        ///
        Sptr <BaseUrl> getUrl() const;
        ///
        void setUrl(Sptr <BaseUrl> item);
        ///
        void setTokenDetails(const Data& token, const Data& tokenValue);
        ///
        const TokenMapFrom& getTokenDetails() const;

        ///
        Data getTokenValue(const Data& token) const;

        ///Parser Funtions
        void decode(const Data& data);

	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:

        Data displayName;
        Sptr <BaseUrl> fromUrl;
        UrlType urlType; //need this while creating Url, while setting for user
    
        Data tag;
        Data token;
        Data qstring;
        TokenMapFrom tokenMap;
        void parse(const Data& data);
        void parseSipFrom(const Data& data);
        void scanSipFrom(const Data& data);
        void parseNameInfo(const Data& data);
        void parseAddrParam(const Data& data);
        void parseUrl(const Data& data);
        void parseToken(const Data& data, const Data& value);
        void parseAddrParamExt(const Data& newdat);
        void parseTag(const Data& token, const Data& vdata);

        friend class SipMsg;

        SipFrom(); //Not implemented
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
