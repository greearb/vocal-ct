#ifndef SipContentLength_HXX
#define SipContentLength_HXX

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
static const char* const SipContentLengthVersion =
    "$Id: SipContentLength.hxx,v 1.2 2004/06/16 06:51:25 greear Exp $";




#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Vocal
{


enum SipContentLengthErrorType
{
    DECODE_CONTENTLENGTH_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipContentLengthParserException : public VException
{
    public:
        SipContentLengthParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         const int error = 0 );

        SipContentLengthParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         SipContentLengthErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }

        SipContentLengthErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContentLengthErrorType value;
};

/// data container for ContentLength header
class SipContentLength : public SipHeader
{
    public:

        ///
        SipContentLength( const SipContentLength& src);
        ///
        SipContentLength( const Data& data, const string& local_ip );
        ///
        void parse(const Data& data);
        ///
        Data encode() const;
        ///
        void setLengthData(const Data& contentlngth);
        ///
        void setLength(int contentlngth);
        ///
        Data getLength() const;
        ///
        bool operator ==( const SipContentLength& src) const;
        ///
        const SipContentLength& operator=( const SipContentLength& src);

	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data contentLength;
        void decode(const Data& data);
        friend class SipMsg;

        SipContentLength(); //Not implemented
};
 
} // namespace Vocal
#endif
