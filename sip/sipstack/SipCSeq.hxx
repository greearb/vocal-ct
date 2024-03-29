#ifndef SipCSeq_HXX
#define SipCSeq_HXX

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


static const char* const SipCSeqVersion =
    "$Id: SipCSeq.hxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include "symbols.hxx"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{


enum SipCSeqErrorType
{
    DECODE_CSeq_FAILED

    //may need to change this to be more specific
};

/// Exception handling class SipCSeqParserException
class SipCSeqParserException : public VException
{
    public:
        SipCSeqParserException( const string& msg,
                                const string& file,
                                const int line,
                                const int error = 0 );
        SipCSeqParserException( const string& msg,
                                const string& file,
                                const int line,
                                SipCSeqErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipCSeqErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipCSeqErrorType value;
};



/// data container for CSeq header
class SipCSeq : public SipHeader
{
    public:
        SipCSeq(const SipCSeq& src);
        const SipCSeq& operator=(const SipCSeq& src);
        /// Create one with default values
        SipCSeq( Method newMethod, const Data& cseqvalue, const string& local_ip);
        SipCSeq(const Data& newMethod, const Data& cseqvalue,const string& local_ip);
        SipCSeq( const Data& data, const string& local_ip );

        Data encode() const;

        int getNextCSeq() const;

        int getCSeq() const;

        void setCSeq(unsigned int cseq);

        void incrCSeq();

        Data getCSeqData() const
        {
            return cseq;
        }

        void setCSeqData(const Data& cseqData);

        void setMethod(const Data&);

        Data getMethod() const
        {
            return method;
        }
        void parse(const Data& data);
        void parseSipCSeq(const Data& data);
        void parseCSeq(const Data& data);
        void parseMethod(const Data& data);
        ///
        void scanSipCSeq(const Data& data);
        ///
        bool operator == (const SipCSeq& other) const;
        ///
        bool operator != (const SipCSeq& other) const
        { return !(*this == other);}
        ///
        bool operator < (const SipCSeq& other) const;

        ///
        bool operator > (const SipCSeq& other) const;
#if USE_HASH_MAP
        ///
        size_t hashfn() const;
#endif
	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;

        void decode(const Data& data);

    private:
        Data cseq;
        Data method;
        bool flag;  // TODO - this variable name is REALLY bad - CJ

        SipCSeq(); //Not implemented
};
 
} // namespace Vocal


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
