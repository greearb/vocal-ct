#ifndef SipDiversion_HXX
#define SipDiversion_HXX

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

static const char* const SipDiversion_hxx_version =
    "$Id: SipDiversion.hxx,v 1.3 2004/06/16 06:51:25 greear Exp $";

#include <map>

#include "global.h"
#include "BaseUrl.hxx"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "Sptr.hxx"
#include "VException.hxx"


namespace Vocal
{

class BaseUrl;
enum SipDiversionErrorType
{
    DECODE_DIVERSION_FAILED,
    PARSE_RURL_FAILED,
    LIMIT_ERR,
    COUNT_FAILED,
    REASON_FAILED,
    DISPLAY_FAILED
    //may need to change this to be more specific
};

/// Exception handling class SipDiversionParserException
class SipDiversionParserException : public VException
{
    public:
        ///
        SipDiversionParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    const int error = 0 );
        ///
        SipDiversionParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    SipDiversionErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipDiversionErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        ///
        SipDiversionErrorType value;

};




/// data container for Diversion header

class SipDiversion : public SipHeader
{
    public:

        typedef map < Data , Data > TokenMapDiversion ;

        ///
        virtual ~SipDiversion();
        ///
        SipDiversion( Sptr<BaseUrl> url, const string& local_ip);
        ///
        SipDiversion(const Data& data, const string& local_ip,
                     UrlType uType = SIP_URL);

        ///
        SipDiversion(const SipDiversion&);

        ///
        Data encode() const;
        ///
        SipDiversion& operator = (const SipDiversion&);
        ///
        void setReason(const Data& reason);
        ///
        Data getReason() const;
        ///
        void setHost(const Data& newhost);
        ///
        Data getHost() const;

        ///
        void setCounter(const Data& count);
        ///
        Data getCounter() const ;

        ///
        void setLimit(const Data& newuser);
        ///
        Data getLimit() const;
        ///

        Sptr<BaseUrl> getUrl() const;
        ///
        void setUrl(Sptr <BaseUrl> tourl);
        ///
        bool operator ==(const SipDiversion& srcDiversion) const;
        //
        bool operator< (const SipDiversion& src) const;


        //
        void setDisplayName(const Data& name);
        //
        void parseToken(const Data& data);

        ///
        void parseTag( Data& data);

        ///
        TokenMapDiversion* createTokenDetails();

        ///
        void setTag(const Data& newtag) ;

        ///
        Data getTag() const;

        ///
        Data isToken(const Data& sdata );

        ///
        bool isCounter(const Data& sdata);

        ///
        bool isLimit(const Data& sdata);

        ///
        bool isReason(const Data& sdata);

        ///
        bool isEmpty(const Data& sdata);

        ///
        void setTokenDetails(const Data& token, const Data& tokenValue);

        ///
    //  void setDextensionDetails(const Data& token, const Data& tokenValue);

        ///

        void print();
	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Sptr<BaseUrl> rurl;
        UrlType urlType;
        Data rcounter;
        Data rlimit;
        Data rreason;
        Data displayName;
        Data tag;
        TokenMapDiversion tokenMap;
    //  TokenMapDiversion divextension;
        void decode(const Data& data);
        void scanSipDiversion( const Data & tmpdata);
        void parse( const Data& reidrectdata);
        void parseNameInfo(const Data& data);
        void parseUrl(const Data&);
        void parseLimit(const Data& data);
        void parseCounter(const Data& data);
        void parseReason(const Data& data);

        SipDiversion(); //Not implemented
};
 
} // namespace Vocal


#endif
