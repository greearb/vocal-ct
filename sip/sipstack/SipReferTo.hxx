#ifndef SipReferTo_HXX
#define SipReferTo_HXX

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


#include "Data.hxx"

#include "VException.hxx"
#include "cpLog.h"
#include <map>
#include "BaseUrl.hxx"
#include "Sptr.hxx"
#include "SipHeader.hxx"

namespace Vocal
{


enum SipReferToErrorType
{
    DECODE_REFERTO_FAILED,
    PARSE_REFERURL_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipReferToParserException
class SipReferToParserException : public VException
{
    public:
        SipReferToParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipReferToParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipReferToErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipReferToErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipReferToErrorType value;

};


/// data container for ReferTo header
class SipReferTo : public SipHeader
{
    public:

        ///
        SipReferTo( const Data& data, const string& local_ip );
        ///
        SipReferTo(const SipReferTo&);

        ///
        SipReferTo(Sptr <BaseUrl> url, const string& local_Ip);
        ///
        Data encode() const;
        ///
        SipReferTo& operator = (const SipReferTo&);
        ///
        Data getUrlType() const;
        ///
        Data getUrlString() const;

        const Data& getDisplayName() const { return displayName; }
        const Data& getAfterAngleQuote() const { return afterAngleQuote; }
                

        ///
        bool operator ==(const SipReferTo& srcReferTo) const;



	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        ///
        Data urlType;
        Data address;
        Data displayName;
        Data afterAngleQuote;
        bool angleQuotes;

        void decode(const Data& data);
        void parse(const Data& data);
        void scanSipReferTo(const Data& data);
        void parseUrl(const Data& data);

        friend class SipMsg;
        SipReferTo(); // Not Implemented
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
