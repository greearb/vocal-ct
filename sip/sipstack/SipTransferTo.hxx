#ifndef SipTransferTo_HXX
#define SipTransferTo_HXX

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
static const char* const SipTransferToVersion =
    "$Id: SipTransferTo.hxx,v 1.2 2004/06/16 06:51:26 greear Exp $";



#include "Data.hxx"
#include "cpLog.h"
#include "BaseUrl.hxx"
#include "VException.hxx"
#include "SipHeader.hxx"
#include "Sptr.hxx"

namespace Vocal
{
    
///
enum SipTransferToErrorType
{
    DECODE_TRANSFER_TO_FAILED,
    PARSE_TRANSFER_URL_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipTransferToParserException
class SipTransferToParserException : public VException
{
    public:
        SipTransferToParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      const int error = 0 );
        SipTransferToParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      SipTransferToErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipTransferToErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipTransferToErrorType value;

};



/// data container for TransferTo header
class SipTransferTo : public SipHeader
{
    public:
        ///
        SipTransferTo( Sptr <BaseUrl> url, const string& local_ip);
        ///
        SipTransferTo( const Data& data, const string& local_ip,  UrlType uType = SIP_URL);
        ///
        SipTransferTo(const SipTransferTo&);

        ///
        Data encode() const;
        ///
        SipTransferTo& operator = (const SipTransferTo&);
        ///
        void setPort(int newport);
        ///
        int getPort();

        ///
        void setPortData(const Data& newport);
        ///
        Data getPortData();

        ///
        void setUser(const Data& newuser);
        ///
        Data getUser();
        ///
        void setDisplayName(const Data& name);
        ///
        Data getDisplayName();
        ///
        void setHost(const Data& newhost);
        ///
        Data getHost();
        ///
        Sptr<BaseUrl> getUrl();
        ///
        void setUrl(Sptr <BaseUrl> tourl);

        ///
        bool operator ==(const SipTransferTo& srcTransferTo) const;

        ///
        void parse(const Data& data);
        void parseSipTransferTo(const Data& data);
        ///
        void scanSipTransferTo(const Data& data);
        ///
        void parseNameInfo(const Data& data);

        ///
        void parseUrl(const Data& data);

	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Sptr<BaseUrl> toUrl;
        UrlType urlType;
        Data displayName;
        void decode(const Data& data);
        friend class SipMsg;
        SipTransferTo(); // Not Implemented
};

 
} // namespace Vocal

#endif
