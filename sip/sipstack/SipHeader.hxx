#ifndef SIP_HEADER_HXX_
#define SIP_HEADER_HXX_

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
 * IN EXCESS OF ,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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


#include "Data.hxx"
#include "Sptr.hxx"
#include "VException.hxx"
#include <deque>
#include "NetworkAddress.h"


namespace Vocal
{


//Exception handling for all headers of the message 
enum SipHeaderErrorType
{
    CSEQ_OUT_OF_RANGE,
    VIA_NOT_PRESENT,
    INDEX_OUT_OF_BOUND,
    TTL_OUT_OF_RANGE,
    OUT_OF_RANGE
};

///class to handle exceptions of SipHeaders.
class SipHeaderException
{
    public:
        SipHeaderException(SipHeaderErrorType i) { value = i; }
        SipHeaderErrorType value;
};

/** enum of all SipHeader types.  

* If you add another Header type, you must:<p>
* <ol>
* <li> change the table of text encodings for this enum in
* sipHeaderTable (in SipHeader.cxx)
*
* <li> add it to SipHeader::decodeHeader()
*
* <li> add the header to the sipHeaderOutputOrder in
* SipRawHeaderContainer.cxx
* </ol>
* please keep this in alphabetical order for cleanliness.
* <p>
* <b>remember that SIP_UNKNOWN_HDR must be last.</b>
*
*/
enum SipHeaderType
{
    SIP_ACCEPT_HDR,
    SIP_ACCEPT_ENCODING_HDR,
    SIP_ACCEPT_LANGUAGE_HDR,
    SIP_ALLOW_HDR,
    SIP_ALSO_HDR,
    SIP_AUTHORIZATION_HDR,

    SIP_CALLID_HDR,
    SIP_CALL_LEG_HDR,
    SIP_CONTACT_HDR,
    SIP_CONTENT_DATA_HDR,
    SIP_CONTENT_DISPOSITION_HDR,
    SIP_CONTENT_ENCODING_HDR,
    SIP_CONTENT_LANGUAGE_HDR,
    SIP_CONTENT_LENGTH_HDR,
    SIP_CONTENT_TYPE_HDR,
    SIP_CSEQ_HDR,

    SIP_DATE_HDR,
    SIP_DIVERSION_HDR,

    SIP_ENCRYPTION_HDR,
    SIP_EXPIRES_HDR,
    SIP_EVENT_HDR,

    SIP_FROM_HDR,

    SIP_HIDE_HDR,

    SIP_IN_REPLY_TO_HDR,

    SIP_MAX_FORWARDS_HDR,
    SIP_MIME_VERSION_HDR,

    SIP_ORGANIZATION_HDR,
    SIP_OSP_AUTHORIZATION_HDR,

    SIP_PRIORITY_HDR,
    SIP_PROXY_AUTHENTICATE_HDR,
    SIP_PROXY_AUTHORIZATION_HDR,
    SIP_PROXY_REQUIRE_HDR,

    SIP_RECORD_ROUTE_HDR,
    SIP_REFERRED_BY_HDR,
    SIP_REFER_TO_HDR,
    SIP_REPLACES_HDR,
    SIP_REQUIRE_HDR,
    SIP_RESPONSE_KEY_HDR,
    SIP_RETRY_AFTER_HDR,
    SIP_ROUTE_HDR,

    SIP_SERVER_HDR,
    SIP_SESSION_HDR,
    SIP_SUBJECT_HDR,
    SIP_SUBSCRIPTION_NOTIFY_HDR,
    SIP_SUPPORTED_HDR,

    SIP_TIMESTAMP_HDR,
    SIP_TO_HDR,
    SIP_TRANSFER_TO_HDR,

    SIP_UNSUPPORTED_HDR,
    SIP_USER_AGENT_HDR,

    SIP_VIA_HDR,

    SIP_WARNING_HDR,
    SIP_WWW_AUTHENTICATE_HDR,

    SIP_SESSION_EXPIRES_HDR,
    SIP_MIN_SE_HDR,

    SIP_UNKNOWN_HDR // this header needs to be kept last -- see
    // sipHeaderNameCount
};

/// this is the number of headers in the SipHeaderType enum.
const int sipHeaderNameCount = SIP_UNKNOWN_HDR + 1;

///
SipHeaderType headerTypeDecode(const Data& headerName);
///
Data headerTypeEncode(const SipHeaderType header);


/// Base class for all SIP headers
class SipHeader: public BugCatcher {
public:
   static  void    init();
   /// virtual destructor
   virtual ~SipHeader() { _cnt--; };
   /// encode function returns the encoded form of the header
   virtual Data encode() const = 0;
   /// factory for constructing sip headers given the type
   static SipHeader* decodeHeader( SipHeaderType type, const Data& value,
                                   const string& local_ip);
   static int getInstanceCount() { return _cnt; }

   /// virtual method for copying sip headers of any type without knowing which type
   virtual Sptr<SipHeader> duplicate() const = 0;
   /// compare two headers of (possibly) the same class
   virtual bool compareSipHeader(SipHeader* msg) const = 0;

   ///
   bool isEmpty() const { return emptyFlg; };

   const string& getLocalIp() const { return local_ip; }

protected:
   // local_ip cannot be "" here, must be the local IP we are bound to locally
   // or 'hostaddress' if we are not specifically bound.
   SipHeader(const string& _local_ip) {
      emptyFlg= true;
      local_ip = _local_ip;
      _cnt++;
   }
   // local_ip cannot be "" here, must be the local IP we are bound to locally
   // or 'hostaddress' if we are not specifically bound.
   SipHeader(const Data& val, const string& _local_ip) {
      emptyFlg= false;
      local_ip = _local_ip;
      _cnt++;
   }
   bool emptyFlg;

   string local_ip;

private:
   static unsigned int _cnt;
   SipHeader(); //Not Implemented
};
   
} // namespace Vocal
#endif
