#ifndef SIP_PARAMETER_LIST_HXX
#define SIP_PARAMETER_LIST_HXX

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

static const char* const SipParameterList_hxx_Version =
    "$Id: SipParameterList.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <map>

#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "Sptr.hxx"
#include "Mutex.hxx"
#include "Lock.hxx"
#include "VException.hxx"

/// Exception handling class

namespace Vocal
{


enum SipParameterListErrorType
{
    PARAMETER_LIST_DECODE_FAILED
};

///
class SipParameterListParserException : public VException
{
    public:
        ///
        SipParameterListParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         const int error = 0 );
    
        ///
        SipParameterListParserException( const string& msg,
                                         const string& file,
                                         const int line,
                                         SipParameterListErrorType i)
            : VException( msg, file, line, static_cast < int > (i))
            {
                value = i;
            }

        ///
        SipParameterListErrorType getError() const
            {
                return value;
            }

        ///
        string getName() const ;
    private:
        SipParameterListErrorType value;

};

/** data container for Parameter Lists.  at the moment, we assume that
    there can be at most one parameter of any given pname, as that
    makes life easiest for the moment.  i don't think this is a
    guaranteed assumption, but it makes life better, and it seems to
    match up to NIST's approach, so we'll take it.  */

class SipParameterList : public std::map <Data, Data>
{
    public:
	/// Create one with default values
        SipParameterList(char delimiter=';');
	///
	SipParameterList(const Data& data, char delimiter=';') throw(SipParameterListParserException &) ;
	///
	SipParameterList(const SipParameterList& src );
    
	///
	~SipParameterList();

	/** return the encoded string version of this. This call
            should only be used inside the stack and is not part of
            the API */
	Data encode() const ;

	///parser functions
	void decode(Data data, char delimiter=';', bool eatWhitespace=true);

        ///
        Data getValue(const Data& key) const;

        ///
        void setValue(const Data& key, const Data& value);

        /// clear the value for key 
        void clearValue(const Data& key);

        ///
        const SipParameterList&  operator=(const SipParameterList& src)
        {
            if(this != &src)
            {
                Threads::Lock lock1(src.myMutex);
                Threads::Lock lock2(myMutex);
					 // 24/11/03 fpi
					 // WorkAround Win32
					 // ! std::map<Data, Data>::operator=(src);
                map<Data, Data>::operator=(src);
                myDelimiter = src.myDelimiter; 
            }
            return *this;
        }

    private:
        char     myDelimiter;
        mutable Threads::Mutex   myMutex;

};

 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
