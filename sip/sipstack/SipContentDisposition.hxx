#ifndef SipContentDisposition_HXX
#define SipContentDisposition_HXX

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

static const char* const SipContentDispositionVersion
= "$Id: SipContentDisposition.hxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Vocal
{

enum SipContentDispositionErrorType
{
    DECODE_CONTENTDISPOSITION_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipContentDispositionParserException : public VException
{
    public:
        SipContentDispositionParserException( const string& msg,
                                              const string& file,
                                              const int line,
                                              const int error = 0 );
        SipContentDispositionParserException( const string& msg,
                                              const string& file,
                                              const int line,
                                              SipContentDispositionErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipContentDispositionErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContentDispositionErrorType value;

};

/// data container for ContentDisposition header
class SipContentDisposition : public SipHeader
{
    public:

        /*** Create by decoding the data string passed in. This is the decode
             or parse. */
        SipContentDisposition( const Data& newData, const string& local_ip );

        SipContentDisposition(const SipContentDisposition& src);
        SipContentDisposition& operator= ( const SipContentDisposition & src);
        bool operator== ( const SipContentDisposition & src) const;
        void setDispositionType(const Data& newdispositiontype)
        {
            dispositiontype = newdispositiontype ;
        }
        void setAttribute(const Data& newAttribute)
        {
            attribute = newAttribute;
        }
        void setValue(const Data& newvalue)
        {
            value = newvalue;
        }
        void setHandleParm(const Data& newhandleparm)
        {
            handleparm = newhandleparm;
        }
        Data getAttribute() const
        {
            return attribute;
        }
        Data getValue() const
        {
            return value;
        }
        Data getHandleParm() const
        {
            return handleparm;
        }
        Data getDispositionType() const
        {
            return dispositiontype ;
        }
       
        /*** return the encoded string version of this. This call should only be
             used inside the stack and is not part of the API */
        Data encode() const;



	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        Data attribute;
        Data value;
        Data handleparm;
        Data dispositiontype;
        void decode(const Data& data);
        void scanSipContentDisposition(const Data& data);
        void parseDispositionParm(const Data &tmepdata);
        void parseParms(const Data &tpdata);
        void parseFinParms(const Data &tpdata, const Data &tvalue);  
    
        friend class SipMsg;
        friend class SipCommand;

        SipContentDisposition(); // Not Implemented
};
 
} // namespace Vocal

#endif
