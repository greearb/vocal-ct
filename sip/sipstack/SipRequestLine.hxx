#ifndef SipRequestLine_HXX
#define SipRequestLine_HXX

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
static const char* const SipRequestLineVersion =
"$Id: SipRequestLine.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "Data.hxx"
#include "SipUrl.hxx"
#include "symbols.hxx"
#include "BaseUrl.hxx"
#include "VException.hxx"

namespace Vocal
{

class SipMsg;
class SipUrl;

enum SipRequestLineErrorType
{
    DECODE_REQUESTLINE_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipRequestLineParserException : public VException
{
    public:
        SipRequestLineParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipRequestLineParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipRequestLineErrorType i)
            : VException( msg, file, line, static_cast < int > (i))
            {
                value = i;
            }
        const SipRequestLineErrorType& getError() const
            {
                return value;
            }
        string getName() const ;
    private:
        SipRequestLineErrorType value;

};

/** 
    data container for the RequestLine of a SIP request msg.
    The request line is the one in the form
   <p>
   <pre>
   INVITE sip:a@b.com SIP/2.0
   </pre>
   which is the first line of a SIP request (in this case, INVITE).
*/

class SipRequestLine
{
    public:
        /// Create one with default values
        SipRequestLine(const string& local_ip, UrlType type = SIP_URL);
        ///
        SipRequestLine(const SipRequestLine& src);

        ///
        void setTransportParam(const Data& transport);

        ///
        Data getTransportParam() const;

        ///
        Data getHost() const;

        ///
        void setHost(const Data& host);
        ///
        void setPort(const Data& port);
        ///
        void setUrl(Sptr <BaseUrl> url);
        /// get the request URI
        Sptr <BaseUrl> getUrl() const;

        ///
        Data getMethod() const;
        ///
        void setMethod(const Data& newmethod);
        ///
        SipRequestLine( const Data& data, const string& local_ip );
        ///
        SipRequestLine(Method method, Sptr<BaseUrl> url, const string& local_ip);
        ///
        SipRequestLine(const Data& data, Sptr<BaseUrl> url, const string& local_ip);
        ///
        Data encode() const;
        ///
        void setProtocol(const Data& newprotocol);
        ///
        void setVersion(const Data& newversion);
        ///
        void parseVersion(const Data& newver);
        ///
        Data getProtocol() const;
        ///
        Data getVersion() const;
        ///
        const SipRequestLine& operator=(const SipRequestLine& src);
        ///
        bool operator==(const SipRequestLine& src) const; 

        /** decode the text of a RequestLine.
            @param data   text to be decoded
            @exception   thrown if there is an error parsing
        */
        void decode(const Data& data) throw (SipRequestLineParserException&);

        const string& getLocalIp() const { return local_ip; }

    private:

        /// scan the SIP request line
        void scanSipRequestLine(const Data & scandata) 
            throw (SipRequestLineParserException &);

        Sptr<BaseUrl> requestUrl;
        UrlType urlType;
        Data method;
        Data version;
        Data protocol;
        string local_ip;

        SipRequestLine(); //Not implemented
};

} // namespace Vocal


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
