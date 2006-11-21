#ifndef SipCallId_HXX
#define SipCallId_HXX

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
#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"

namespace Vocal
{

enum SipCallIdErrorType
{
    DECODE_CALLID_FAILED

    //may need to change this to be more specific
};

/// Exception handling class 
class SipCallIdParserException : public VException
{
    public:
        SipCallIdParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  const int error = 0 );
        SipCallIdParserException( const string& msg,
                                  const string& file,
                                  const int line,
                                  SipCallIdErrorType i)
            : VException( msg, file, line, static_cast < int > (i))
            {
                value = i;
            }
        const SipCallIdErrorType& getError() const
            {
                return value;
            }
        string getName() const ;
    private:
        SipCallIdErrorType value;

};
/// data container for CallId header
class SipCallId : public SipHeader {
public:

   ///
   SipCallId( const SipCallId& other );

   ///
   void setData(const Data& item);

   ///
   Data getData() const;
   Data getLocalId() const {
      return localid;
   }
   Data getHost() const {
      return host;
   }
   void setLocalId(const Data & newlocalid) {
      localid = newlocalid;
   }
   void setHost(const Data & newhost) {
      host = newhost;
   }
   //
   bool operator == ( const SipCallId& other ) const;
        
   //
   bool operator != (const SipCallId& other) const
      { return !(*this == other);}
   //
   const SipCallId& operator = ( const SipCallId& src );
   
   //
   bool operator < ( const SipCallId& other ) const;
   
   //
   bool operator >( const SipCallId& other ) const;
   
   
   /*** Create by decoding the data string passed in. This is the decode
        or parse.  */
   // local_ip cannot be "" here, must be the local IP we are bound to locally
   // or 'hostaddress' if we are not specifically bound.
   SipCallId( const Data& data, const string& local_ip );
   
   /*** return the encoded string version of this. This call should only be
        used inside the stack and is not part of the API */
   Data encode() const;
   
   Data getCallIdData() const;
   
   size_t hashfn()const;
   
   /// method for copying sip headers of any type without knowing which type
   Sptr<SipHeader> duplicate() const;
   /// compare two headers of (possibly) the same class
   virtual bool compareSipHeader(SipHeader* msg) const;
   
   string toString() const { string s(getData().c_str()); return s; }
   
private:
   Data localid;
   Data host;
   size_t hash;
   void decode(const Data& data);
   void getHash();
   void parse(const Data& data);
   void scanSipCallId(const Data& data);
   void parseLocalId(const Data& data);
   void parseHost(const Data& data);
   
   friend class SipMsg;
};

ostream& operator<<(ostream& strm, const SipCallId& id);
} // namespace Vocal

#endif
