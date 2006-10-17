#ifndef SipUrl_HXX
#define SipUrl_HXX

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


#include <string>
#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "VException.hxx"
#include "BaseUrl.hxx"
#include "BaseUser.hxx"
#include "Sptr.hxx"
#include "EmbeddedObj.hxx"
#include "NetworkAddress.h"
#include <misc.hxx>

/// Exception handling class

namespace Vocal
{
    
enum SipUrlErrorType
{
    DECODE_FAILED,
    SIP_NOT_PRESENT,
    USER_INFO_ERR,
    MANDATORY_ITEM_MISSING,
    NOT_VALID_URL_PARMS,
    NOT_VALID_TRANSORT_PARM,
    NOT_VALID_USER_PHONE_PARM,
    NOT_VALID_TTL_PARM,
    NOT_VALID_URL_DATA 
};

///
class SipUrlParserException : public VException {
public:
   ///
   SipUrlParserException( const string& msg,
                          const string& file,
                          const int line,
                          const int error = 0 );
    
   ///
   SipUrlParserException( const string& msg,
                          const string& file,
                          const int line,
                          SipUrlErrorType i)
         : VException( msg, file, line, static_cast < int > (i))
      {
         value = i;
      }
   ///
   SipUrlErrorType getError() const
      {
         return value;
      }
   ///
   string getName() const ;
private:
   SipUrlErrorType value;

};

/// data container for URLs of type sip:
class SipUrl : public BaseUrl
{
public:

   ///
   // local_ip cannot be "" here, must be the local IP we are bound to locally
   // or 'hostaddress' if we are not specifically bound.
   explicit SipUrl(const Data& data, const string& local_ip, bool lr=false)
      throw (SipUrlParserException &) ;
   ///
   SipUrl(const SipUrl& );
    
   ///
   virtual ~SipUrl();

   // Help up-cast safely.
   virtual bool isSipUrl() const { return true; }

   ///
   bool areEqual(Sptr<BaseUrl>) const;
   ///
   bool isLessThan(Sptr<BaseUrl>) const;
   /// comparison operator is used by others
   virtual bool areEqual(const BaseUrl& other) const;

   ///
   bool operator<(const SipUrl& srcUrl) const;
   ///
   bool operator>(const SipUrl& srcUrl) const;
    
   ///
   bool operator ==(const SipUrl& srcUrl) const;
    
   ///
   SipUrl& operator =(const SipUrl& srcUrl);

   ///
   Sptr<BaseUrl> duplicate() const;
   ///
   Data getUserValue() const ;  
   ///
   bool setUserValue(const Data& newuser, const Data& newUserParam = "") ; 
   ///
   void setUser(Sptr<BaseUser> newUser);
   ///
   Sptr <BaseUser> getUser() const;
   ///
   Sptr<EmbeddedObj> getEscObj() {return EscObj;}
   ///
   const Data& getHost() const ;
   ///
   void setEscObj(Sptr<EmbeddedObj> newescobj);
   ///
   void setHost(const Data& newhost) ;
   ///
   const Data& getPort() const ;
    
   ///
   void setPort(const Data& newport) ;

   void setPort(uint16 newport);
    
   ///
   const Data& getPasswd() const ;
    
   ///
   void setPasswd(const Data& newpasswd) ;
    
   ///
   const Data& getTransportParam() const ;
    
   ///
   void setTransportParam(const Data& newtransport) ;
    
   ///
   const Data& getMaddrParam() const ;
    
   ///
   void setMaddrParam(const Data& newmaddr) ;

   ///
   bool isLooseRouterPresent() const;
        
   ///
   bool operator != (const SipUrl& srcUrl) const
      { return !(*this == srcUrl);}
    

   ///
   const Data& getTtlData() const ;
    
   ///
   void setTtlParam(const Data& newttl) throw(SipUrlParserException&);
    
   ///
   const Data& getTtlParam() const;
    
   ///        
   const Data& getUserParam() const ;
    
   ///
   void setUserParam(const Data& newuserParm) ;
    
   ///
   const Data& getMethodParam() const ;
    
   ///
   void setMethodParam(const Data& newmethodParm);
    
   ///
   const Data& getOtherParam() const ;
    
   ///
   void setOtherParam(const Data& newotherParm);
    
   ///
   void setOtherName(const Data& newOtherName) ;
    
   ///
   const Data& getOtherName() const;
    
   ///
   const Data& getHeaders() const ;
    
   ///
   void setHeaders(const Data& newheader) ;
    
    
   /// return the encoded string version of this. This call should only be
   ///used inside the stack and is not part of the API 
   Data encode() const ;
    
   ///
   Data getNameAddr() const;

   /// get a unique key for a transaction id
   Data getUniqueKey() const;
    
   ///
   Data getUrlParam() const;
    
   /// 
   void initializeFrom();
    
   ///
   void initializeTo();
        
   /// get name and address without the port if it is the default
   Data getNameAddrNoPort() const;

   ///parser functions
   void decode(const Data& data);
   ///
   UrlType getType() const;
   ///

   NetworkAddress asNetworkAddress() const;
private:

   string local_ip; //As configured, not decoded by anything.

   Data schemeName;
   Sptr<BaseUser> user;
   Data passwd;
   Data host;
   Data port;
   // URL Parameters
   Data transportParam;
   Data ttlParam;
   Data maddrParam;
   Data userParam;
   Data methodParam;
   Data otherParam;
   Data otherName;
   Data headers;
   bool lrParam;
   bool ipv6Addr;
   Sptr<EmbeddedObj> EscObj;
   ///
   bool fastDecode( Data myData );
   ///
   bool parseIpv6Address(Data& inData);

};

 
} // namespace Vocal

#endif
