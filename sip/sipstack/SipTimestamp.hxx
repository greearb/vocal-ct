#ifndef SipTimestamp_HXX
#define SipTimestamp_HXX

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
#include "SipHeader.hxx"
static const char* const SipTimestampVersion =
    "$Id: SipTimestamp.hxx,v 1.2 2004/06/16 06:51:26 greear Exp $";



#include "Data.hxx"
#include "symbols.hxx"

namespace Vocal
{
    
enum SipTimestampErrorType
{
    DECODE_TIMESTAMP_FAILED

    //may need to change this to be more specific
};

/// Exception handling class
class SipTimestampParserException : public VException
{
    public:
        SipTimestampParserException( const string& msg,
                                     const string& file,
                                     const int line,
                                     const int error = 0 );
        SipTimestampParserException( const string& msg,
                                     const string& file,
                                     const int line,
                                     SipTimestampErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipTimestampErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipTimestampErrorType value;

};
/// data container for Timestamp header
class SipTimestamp : public SipHeader
{
    public:
        /*** Create by decoding the data string passed in. This is the decode
             or parse.  */
        SipTimestamp( const Data& srcData, const string& local_ip );
        SipTimestamp( const SipTimestamp& src );

        const SipTimestamp& operator=(const SipTimestamp& rhs)
        {
            if (this != &rhs)
            {
                time = rhs.time;
                delay = rhs.delay;
                ;

            }
            return *this;
        }

     
        ///
        bool operator ==( const SipTimestamp& src ) const;

        ///
        int getTime();

        ///
        void setTime( const int& newTime);
        void scanSipTimestamp(const Data& data);
        ///
        int getDelay();

        ///
        void setDelay( const int& newDelay);

        ///
        Data getTimeData();

        ///
        void setTimeData( const Data& newTime);

        ///
        Data getDelayData();

        ///
        void setDelayData( const Data& newDelay);

        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;

	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data time;
        Data delay;
        void decode(const Data& data);
        friend class SipMsg;
        SipTimestamp(); // Not Implemented
};

 
} // namespace Vocal

#endif
