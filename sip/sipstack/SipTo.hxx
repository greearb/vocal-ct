#ifndef SipTo_HXX
#define SipTo_HXX

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

static const char* const SipTo_hxx_Version =
    "$Id: SipTo.hxx,v 1.4 2004/06/16 06:51:26 greear Exp $";


#include "global.h"
#include "Data.hxx"
#include "BaseUrl.hxx"
#include "VException.hxx"
#include "cpLog.h"
#include <map>
#include "Sptr.hxx"
#include "SipHeader.hxx"

namespace Vocal
{

class SipFrom;    

enum SipToErrorType
{
    DECODE_TO_FAILED,
    PARSE_URL_FAILED,
    USERINFO_ERR,
    PARSE_ADDPRAM_FAILED
    //may need to change this to be more specific
};

/// Exception handling class SipToParserException
class SipToParserException : public VException
{
    public:
        SipToParserException( const string& msg,
                              const string& file,
                              const int line,
                              const int error = 0 );
        SipToParserException( const string& msg,
                              const string& file,
                              const int line,
                              SipToErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipToErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipToErrorType value;

};



/// data container for To header
class SipTo : public SipHeader
{
    public:
        typedef map < Data , Data, less < Data > > TokenMapTo;
        ///
        SipTo( Sptr <BaseUrl> url, const string& local_ip);
        ///
        SipTo( const Data& data, const string& local_ip, UrlType type = SIP_URL);
        ///
        SipTo(const SipTo&);
        ///
        explicit SipTo(const SipFrom& from);

        ///
        Data encode() const;
        ///
        const SipTo& operator = (const SipTo&);
        ///
        void setPort(int newport);
        ///
        int getPort() const;

        ///
        void setPortData(const Data& newport);
        void setPortData(int newport);

        ///
        Data getPortData() const ;

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
        Sptr <BaseUrl> getUrl() const;
        ///
        void setUrl(Sptr <BaseUrl>  tourl);

        const Data& getTag() const
        {
            return tag;
        }

        void setTag(const Data& newtag)
        {
            tag = newtag;
        }

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
        const bool operator<(const SipTo&) const;
        ///
        const bool operator>(const SipTo&) const;

        ///
        const bool operator ==(const SipTo& srcTo) const;
        ///
        const bool operator != (const SipTo& srcTo) const
        { return !(*this == srcTo);}

        void setTokenDetails(const Data& token, const Data& tokenValue);

        const TokenMapTo& getTokenDetails() const;

        Data getTokenValue(const Data& token) const;

        void decode(const Data& data);
	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;

	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
private:
    ///
    Sptr <BaseUrl> toUrl;
    ///
    UrlType urlType;
    ///
    Data displayName;
    Data tag;
    Data token;
    Data qstring;
    TokenMapTo tokenMap;
    void parse(const Data& data);
    void parseSipTo(const Data& data);
    void scanSipTo(const Data& data);
    void parseNameInfo(const Data& data);
    void parseAddrParam(const Data& data);
    void parseUrl(const Data& data);
    void parseToken(const Data& data, const Data& value);
    void parseAddrParamExt(const Data& newdat);
    void parseTag(const Data& tok, const Data& vdata);
    
    friend class SipMsg;
        SipTo(); // Not Implemented
    
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
