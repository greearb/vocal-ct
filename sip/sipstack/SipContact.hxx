#ifndef SipContact_HXX
#define SipContact_HXX

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
static const char* const SipContactVersion =
    "$Id: SipContact.hxx,v 1.3 2004/10/25 23:21:14 greear Exp $";

#include <iostream>

#include "Data.hxx"
#include "SipExpires.hxx"
#include "BaseUrl.hxx"
#include "SipDate.hxx"
#include "SipHeader.hxx"
#include "SipParameterList.hxx"
#include "VException.hxx"

namespace Vocal
{

class SipDate;

enum SipContactErrorType
{
    DECODE_CONTACT_FAILED,
    URL_PARSE_FAILED,
    MANDATORY_ERR,
    CONTACT_PARSE_ADDPRAM_FAILED
    //may need to change this to be more specific
};


/// Exception handling class 
class SipContactParserException : public VException
{
    public:
        SipContactParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   const int error = 0 );
        SipContactParserException( const string& msg,
                                   const string& file,
                                   const int line,
                                   SipContactErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        const SipContactErrorType& getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipContactErrorType value;

};


/// data container for Contact header
class SipContact : public SipHeader
{
    public:
        class AllContactsTag {};
        static const AllContactsTag AllContacts;
        
        // constructor for creasting acontact with the value '*', used only by
        // register requests
        SipContact(const AllContactsTag & c, const string& local_ip);


        /*** Create by decoding the data string passed in. This is the decode
         * or parse.
         */
        SipContact( const Data& srcData, const string& local_ip,
                    UrlType uType = SIP_URL);


        SipContact( const SipContact& src);

        ///
        bool operator ==( const SipContact& src) const;

        ///
        bool operator<( const SipContact& src) const;

        ///
        const SipContact& operator=(const SipContact& src);
        ///
        Sptr<BaseUrl> getUrl() const;

        ///
        void setUrl( Sptr<BaseUrl> item);

        ///
        Data getDisplayName() const;

        ///
        void setDisplayName( const Data& item);

        ///
        bool isWildCard() const;

        ///
        Data getQValue() const;
        ///
        void setQValue( const Data& item);

        void setExpires( const SipExpires& item);
        ///
        SipExpires getExpires() const;
        bool isExpiresSet() const { return bexpires; }

        /*** return the encoded string version of this. This call
             should only be used inside the stack and is not part of
             the API */
        Data encode() const;

	/** method for copying sip headers of any type without knowing
            which type */
	Sptr<SipHeader> duplicate() const;

	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
        ///
        void setNullContact();
    private:

        Sptr<BaseUrl> url;
        UrlType urlType;
        Data displayName;
        Data qValue;
        SipExpires expires;
        SipParameterList myParameterList;

        bool bexpires;
        bool wildCard;

        /// Parser Methods!!
        void parse(const Data& data);
        void scanContactParam(const Data& data);
        void parseNameInfo(const Data& data);
        void parseUrl(const Data& data);
        void decode(const Data& data);

        SipContact(); //Not implemented
};

ostream&
operator<<(ostream& s, const SipContact &contact);
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
