#ifndef SipStatusLine_HXX
#define SipStatusLine_HXX

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

static const char* const SipStatusLine_hxx_Version =
    "$Id: SipStatusLine.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "Data.hxx"
#include "VException.hxx"

namespace Vocal
{
    
enum SipStatusLineErrorType
{
    DECODE_STATUSLINE_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipStatusLineParserException : public VException
{
    public:
        SipStatusLineParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      const int error = 0 );
        SipStatusLineParserException( const string& msg,
                                      const string& file,
                                      const int line,
                                      SipStatusLineErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipStatusLineErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipStatusLineErrorType value;

};

/**

   data container for the Status Line in a SIP response.  The status
   line is the one in the form
   <p>
   <pre>
   SIP/2.0 200 OK
   </pre>
   which is the first line of a SIP response.
*/

class SipStatusLine
{
    public:
        /// Create one with default values
        SipStatusLine();

        /// constructor
        SipStatusLine( const Data& scrData );

        /// copy constructor
        SipStatusLine( const SipStatusLine& src );

        /// operator =
        SipStatusLine& operator=( const SipStatusLine& src );

        /// equality operator
        bool operator ==(const SipStatusLine& src) const;

        ///
        int getStatusCode() const;

        ///
        void setStatusCode( const int& newStatusCode);

        ///
        const Data& getStatusCodeData();

        ///
        void setStatusCodeData( const Data& newStatusCode);

        ///
        void scanSipStatusLine(const Data & scandata);

        ///
        const Data& getReasonPhrase() const;

        ///
        void setReasonPhrase( const Data& newReasonPhrase);

        ///
        const Data& getVersion();

        ///
        void setVersion( const Data& newVersion);

        ///
        void parseVersion( const Data& newVer);

        ///
        const Data& getProtocol();

        ///
        void setProtocol( const Data& newProtocol);

        ///
        void setStatusLine( const Data& newstatusLine );

        ///
        const Data& getStatusLine();

        ///
        Data encode() const;

        ///
        void setDefaultReason(Data findStatusCode);

    private:

        void setReasonPhrase(int code);
        void decode(const Data& data);

        Data statusCode;  // may be accessed as an int or as Data
        Data reasonPhrase;
        Data version;
        Data protocol;
        Data statusLine;  //returned during an encode, no accessors
        friend class StatusMsg;
};

 
} // namespace Vocal

/* Local Variables: */
/* c-file-style:"stroustrup" */
/* c-basic-offset:4 */
/* c-file-offsets:((inclass . ++)) */
/* indent-tabs-mode:nil */
/* End: */

#endif
