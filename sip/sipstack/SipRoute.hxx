#ifndef SipRoute_HXX
#define SipRoute_HXX

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

static const char* const SipRoute_hxx_Version =
    "$Id: SipRoute.hxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "BaseUrl.hxx"
#include "global.h"
#include "symbols.hxx"
#include "Data.hxx"
#include "cpLog.h"
#include "SipHeader.hxx"
#include "Sptr.hxx"
#include "VException.hxx"

namespace Vocal
{

class SipRecordRoute;
    
enum SipRouteErrorType
{
    DECODE_ROUTE_FAILED,
    ROUTE_URL_PARSE_FAIL

    //may need to change this to be more specific
};

/// Exception handling class
class SipRouteParserException : public VException
{
    public:
        ///
        SipRouteParserException( const string& msg,
                                 const string& file,
                                 const int line,
                                 const int error = 0 );
        ///
        SipRouteParserException( const string& msg,
                                 const string& file,
                                 const int line,
                                 SipRouteErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        ///
        SipRouteErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;
    private:
        ///
        SipRouteErrorType value;

};


class SipRouteParserAddParmException : public SipRouteParserException
{
    public:
        ///
        SipRouteParserAddParmException( const string& msg,
                                        const string& file,
                                        const int line,
                                        const int error = 0 );
        ///
        string getName( void ) const;
};

/// Exception handling class SipUrlParserUrlParmsException
class SipRouteParserUrlException : public SipRouteParserException
{
    public:
        ///
        SipRouteParserUrlException( const string& msg,
                                    const string& file,
                                    const int line,
                                    const int error = 0 );
        ///
        string getName( void ) const;
};





/// data container for Route header
class SipRoute : public SipHeader
{
    public:

        /*** Create by decoding the data string passed in. This is the decode
             or parse. */
        SipRoute( const Data& srcData, const string& local_ip,
                  UrlType uType = SIP_URL);
        ///
        SipRoute( const SipRoute& src );
        ///
        explicit SipRoute( const SipRecordRoute& rr);
        
        ///
        void parse(const Data & tmpdata);
        ///
        void parseUrl(const Data & data);
        ///
        void setDisplayName(const Data & newdisplayname)
        {
            displayname = newdisplayname;
        }
        ///
        Data getDisplayName() const
        {
            return displayname;
        }
        ///
        Sptr<BaseUrl> getUrl() const;

        ///
        void setUrl( Sptr <BaseUrl> url);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

        ///
        const SipRoute& operator =(const SipRoute& src);

        ///
        bool operator ==(const SipRoute& src) const;
	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Sptr <BaseUrl> url;
        UrlType urlType;
        Data displayname;
        void decode(const Data& data);
        SipRoute(); // Not Implemented
};

ostream&
operator<<(ostream& s, const SipRoute& route);

 
} // namespace Vocal

#endif
