
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

static const char* const SipRedirect_hxx_Version =
    "$Id: SipRedirect.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#ifndef SipRedirect_HXX
#define SipRedirect_HXX

#include <map>

#include "global.h"
#include "Data.hxx"
#include "SipUrl.hxx"
#include "VException.hxx"
#include "Sptr.hxx"


namespace Vocal
{


class SipUrl;
enum SipRedirectErrorType
{
    DECODE_REDIRECT_FAILED,
    PARSE_RURL_FAILED,
    LIMIT_ERR,
    COUNT_FAILED,
    REASON_FAILED,
    DISPLAY_FAILED
    //may need to change this to be more specific
};

/// Exception handling class
class SipRedirectParserException : public VException
{
    public:
        ///
        SipRedirectParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    const int error = 0 );
        ///
        SipRedirectParserException( const string& msg,
                                    const string& file,
                                    const int line,
                                    SipRedirectErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipRedirectErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        ///
        SipRedirectErrorType value;

};




/// data container for Redirect header
class SipRedirect
{
    public:

        typedef map < Data , Data > TokenMapRedirect ;

        /// Create one with default values
        SipRedirect();
        ///
        ~SipRedirect();
        ///
        SipRedirect( const SipUrl& url);
        ///
        SipRedirect(const Data& data );
        ///
        SipRedirect(const SipRedirect&);
        ///
        Data encode();
        ///
        SipRedirect& operator = (const SipRedirect&);
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

        SipUrl getUrl() const;
        ///
        void setUrl(const SipUrl& tourl);
        ///
        bool operator ==(const SipRedirect& srcRedirect) const;
        //
        bool operator< (const SipRedirect& src) const;


        //
        void setDisplayName(const Data& name);
        //
        void parseToken(const Data& data);

        ///
        void parseTag( Data& data);

        ///
        Sptr < SipRedirect::TokenMapRedirect > getTokenDetails();

        ///
        void setTag(const Data& newtag) ;

        ///
        Data SipRedirect::getTag() const;

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

        void print();
    private:
        ///
        SipUrl rurl;
        Data rcounter;
        Data rlimit;
        Data rreason;
        Data displayName;
        Data tag;
        TokenMapRedirect tokenMap;
        void decode(const Data& data);
        void scanSipRedirect( const Data & tmpdata);
        void parse( const Data& reidrectdata);
        void parseNameInfo(const Data& data);
        void parseUrl(const Data&);
        void parseLimit(const Data& data);
        void parseCounter(const Data& data);
        void parseReason(const Data& data);

};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
