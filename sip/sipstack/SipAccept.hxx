#ifndef SipAccept_HXX
#define SipAccept_HXX

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

static const char* const SipAccept_hxx_Version =
    "$Id: SipAccept.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{

enum SipAcceptErrorType
{
    DECODE_ACCEPT_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipAcceptParserException : public VException
{
    public:
        SipAcceptParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  const int error = 0 );
        SipAcceptParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  SipAcceptErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipAcceptErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipAcceptErrorType value;

};

/// data container for Accept header
class SipAccept : public SipHeader
{
    public:

        ///
        const SipAccept& operator =(const SipAccept&);
        ///
        bool operator ==( const SipAccept& src) const;
        ///
        SipAccept( const SipAccept& src);

        // tdata can be "" and we will not try to decode.
        SipAccept( const Data& tdata, const string& local_ip);

        Data getMediaType() const;
        ///
        void setMediaType(const Data& srcmediaType);
        ///
        Data getMediaSubtype() const;
        ///
        void setMediaSubtype(const Data& srcmediaSubtype);
        ///
        Data getqValue() const;
        ///
        void setqValue(const Data& srcqValue);
        ///
        Data getMediaToken() const;
        ///
        void setMediaToken(const Data& srcToken);
        ///
        Data getMediaTokenValue() const;
        ///
        void setMediaTokenValue(const Data& newmediatokenvalue); // {mediatokenValue=newmediatokenvalue;}
        ///
        void setTokenValue(const Data& srcTokenValue);
      
        ///
        void setAcceptExtensionParm(const Data& newaccepttoken);
        ///
        void setAcceptExtensionValue(const Data& newaccepttokenvalue);
        ///
        Data getAccpetExtensionParm() const;
        ///
        Data getAccpetExtensionValue() const;
        ///
        Data encode() const;
	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;


	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        
        Data mediaType;
        Data mediaSubtype;
        Data qValue;
        Data mediatoken;
        Data mediatokenValue;
        Data accepttoken;
        Data accepttokenValue;
        /// used for parsing
        bool allmedia; 
        ///used for parsing
        bool flagmedia;
        void decode(const Data& data);

        ///Parse functions
        void scanAccept(const Data &tmpdata);
        ///
        void parseMediaParm(const Data& tmpdata);
        ///
        void parseMediaParms(const Data& data, const Data& value);
        ///
        void parseMediaExtension(const Data& extensiondata, const Data& extensionvalue);
        ///
        void parseAcceptParm(const Data & tempdata);
        ///
        void parseAcceptParms(const Data& data);
        ///
        void parseAcceptExtension(const Data& extensiondata, const Data& extensionvalue);

        SipAccept(); //Not Implemented
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
