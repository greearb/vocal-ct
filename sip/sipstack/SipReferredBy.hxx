#ifndef SipReferredBy_HXX
#define SipReferredBy_HXX

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

#include "global.h"


#include <map>

#include "Data.hxx"
#include "VException.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"
#include "SipHeader.hxx"

namespace Vocal
{


enum SipReferredByErrorType
{
    DECODE_REFERREDBY_FAILED,
    PARSE_REFERRERURL_FAILED,
    PARSE_REFERENCEDURL_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipReferredByParserException
class SipReferredByParserException : public VException
{
    public:
        SipReferredByParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      const int error = 0 );
        SipReferredByParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      SipReferredByErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipReferredByErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipReferredByErrorType value;

};



/// data container for ReferredBy header
class SipReferredBy : public SipHeader
{
    public:

        /// Create one with default values
        typedef map < Data , Data > ReferredByPgpMap;


        ///
        virtual ~SipReferredBy();
        ///
        SipReferredBy( const Data& data, const string& local_ip,
                       UrlType uType = SIP_URL);
        ///
        SipReferredBy(const SipReferredBy&);
        ///
        Data encode() const;
        ///
        SipReferredBy& operator = (const SipReferredBy&);

        ///
        bool operator ==(const SipReferredBy& srcReferredBy) const;

        ///
        Data getReferencedUrl() const;
        ///

        Sptr <BaseUrl> getReferrerUrl() const;
        ///
        void setDisplayName(const Data& name);
        ///
        void parseNameInfo(const Data& data);


        void setReferrerUrl(Sptr <BaseUrl> sipurl) ;

        void setReferencedUrl(const Data& data) ;

        ///
        Data getAuthScheme() const
        {
            return authScheme;
        }

        ///
        void setTokenDetails(const Data& token, const Data& tokenValue);

        ///
        const ReferredByPgpMap& getTokenDetails();

        ///
        Data getTokenValue(const Data& token);

        ///
        void scanAuthTokens( const Data& data);

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Sptr <BaseUrl> referrerUrl;
        UrlType urlType;
        Data referencedUrl;
        Data displayName;
        Data address;

        ReferredByPgpMap pgpMap;
        Data authScheme;
        //Data basicCookie;

        void decode(const Data& data);
        void parse(const Data& data);
        void scanReferrerUrl(const Data& data);
        void scanReferencedUrl(const Data& data);
        void parseUrl(const Data& data);

        friend class SipMsg;

        SipReferredBy(); // Not Implemented
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
