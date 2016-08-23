#ifndef SipContentType_HXX
#define SipContentType_HXX

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

#include <map>
#include "Data.hxx"
#include "VException.hxx"
#include "cpLog.h"
#include "Sptr.hxx"
#include "symbols.hxx"
#include "SipHeader.hxx"
#include "SipParameterList.hxx"


namespace Vocal
{


enum SipContentTypeErrorType
{
    DECODE_CONTENTTYPE_FAILED,
    CONTENTTYPE_PARM_FAILED
    //may need to change this to be more specific
};


/// Exception handling class
class SipContentTypeParserException : public VException
{
    public:
        SipContentTypeParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       const int error = 0 );
        SipContentTypeParserException( const string& msg,
                                       const string& file,
                                       const int line,
                                       SipContentTypeErrorType i)
            : VException( msg, file, line, static_cast < int > (i))
            {
                value = i;
            }
    
        const SipContentTypeErrorType& getError() const
            {
                return value;
            }
    
        string getName() const ;

    private:
        SipContentTypeErrorType value;
    
};

/// data container for ContentType header
class SipContentType : public SipHeader
{
    public:
    
        /// construct a content type from the type and subtype
        SipContentType(const Data& gtype, const Data& gsubType, const string& local_ip);

        /// copy constructor
        SipContentType( const SipContentType& src);

        /// construct content type from unparsed text
        /// Dummy is just used to make the signature unique...remove it later.
        SipContentType( const Data& data, const string& local_ip, int dummy );
    
        ///
        bool operator ==(const SipContentType& other) const;
    
        ///
        const SipContentType& operator=( const SipContentType& src) ;
    
        ///
        Data encode() const;
  
        /// inline
        void setType (const Data& newtype) { 
            type = newtype; 
            type.lowercase(); 
        }
    
        /// inline
        void setSubType(const Data& newsubtype) { 
            subtype = newsubtype;
            subtype.lowercase(); 
        }
    
        ///
        void parse(const Data& data);
    
        ///
        void scanSipContentType(const Data& data);

        /// return the type of the content type
        Data getType() const;
    
        /// return the subtype of the content type
        Data getSubType() const;

        ///
        void setTokenDetails(const Data& token, const Data& tokenValue);
        ///
        void clearToken(const Data& token);

        ///
        Data getAttributeValue(const Data& token) const;

        ///
        void setMediatype(const Data& newmediatype);

        ///
        Data getMediatype();
        
        /** method for copying sip headers of any type without knowing
            which type */
   Sptr<SipHeader> duplicate() const;

        /// compare two headers of (possibly) the same class
        virtual bool compareSipHeader(SipHeader* msg) const;
    private:
    
        Data type;
        Data subtype;
        Data media;
        SipParameterList myParamList;
        void decode(const Data& data);
        friend class SipMsg;    
        friend class SipContentData;

        SipContentType(); // Not implemented
};
 
} // namespace Vocal

#endif
