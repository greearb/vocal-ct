#ifndef SipWarning_HXX
#define SipWarning_HXX

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
static const char* const SipWarningVersion =
    "$Id: SipWarning.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "Data.hxx"
#include "SipHeader.hxx"

namespace Vocal
{
    
///
enum SipWarningErrorType
{
    DECODE_WARNING_FAILED,
    SCAN_WARNING_FAILED
    //may need to change this to be more specific
};

///
class SipWarningParserException : public VException
{
    public:
        ///
        SipWarningParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );

        ///
        SipWarningParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipWarningErrorType i )
        : VException( msg, file, line, static_cast < int > ( i ) )
        {
            value = i;
        }
        ///
        SipWarningErrorType getError() const
        {
            return value;
        }
        ///
        string getName() const ;

    private:
        SipWarningErrorType value;

};


/// data container for Warning header
class SipWarning : public SipHeader
{
    public:

        /*** Create by decoding the data string passed in. This is the decode
             or parse.  */
        SipWarning( const Data& srcData, const string& local_ip );
        SipWarning( const SipWarning& src );

        ///
        const SipWarning& operator=( const SipWarning& src );

       ///
       bool operator ==(const SipWarning & src) const;

        ///
        int getCode();

        ///
        void setCode( const int& newCode );

        ///
        void scanWarning( const Data &tmpData );
        ///
        Data getCodeData();

        ///
        void setCodeData( const Data& newCodeData );

        ///
        Data getAgent();

        ///
        void setAgent( const Data& newAgent );

        ///
        Data getText();

        ///
        void setText( const Data& newText );


        /*** return the encoded string version of this. This call should only be
        used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	SipHeader* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data code;  // may be accessed as int or Data
        Data agent;
        Data text;
        void decode( const Data& retryData );
        SipWarning(); // Not Implemented
};

 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */


#endif
