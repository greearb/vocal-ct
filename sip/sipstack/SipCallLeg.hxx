#ifndef SipCallLeg_HXX
#define SipCallLeg_HXX

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
static const char* const SipCallLegVersion =
    "$Id: SipCallLeg.hxx,v 1.3 2004/06/16 06:51:25 greear Exp $";



#include "Data.hxx"

#include "SipFrom.hxx"
#include "SipCallId.hxx"
#include "SipCSeq.hxx"
#include "SipTo.hxx"
#include "SipHeader.hxx"

namespace Vocal
{

/// data container for CallLeg header
class SipCallLeg : public SipHeader {
public:
   /// Create one with default values
   // local_ip cannot be "" here, must be the local IP we are bound to locally
   // or 'hostaddress' if we are not specifically bound.
   SipCallLeg(const string& local_ip);
   virtual ~SipCallLeg() {}

   // local_ip cannot be "" here, must be the local IP we are bound to locally
   // or 'hostaddress' if we are not specifically bound.
   SipCallLeg( const SipFrom& from, const SipTo& to, 
               const SipCallId& callId, const string& local_ip);

   SipCallLeg( const SipCallLeg& src);
   SipCallLeg& operator=( const SipCallLeg& rhs );

   virtual bool operator == (const SipCallLeg& src) const;
   virtual bool operator != (const SipCallLeg& src) const;

   virtual bool operator < (const SipCallLeg& src) const;

   SipFrom getFrom() const;
   void setFrom(const SipFrom& newfrom);

   SipTo getTo() const;
   void setTo(const SipTo& newto);

   const SipCallId& getCallId() const;
   void setCallId(const SipCallId& newcallId);

   const SipCSeq& getCSeq() const;
   void setCSeq(const SipCSeq& newcseq);

   /*** return the encoded string version of this. This call should only be
        used inside the stack and is not part of the API */
   Data encode() const;

   /// method for copying sip headers of any type without knowing which type
   Sptr<SipHeader> duplicate() const;

   /// compare two headers of (possibly) the same class
   virtual bool compareSipHeader(SipHeader* msg) const;
   
   virtual string toString() const { return encode().c_str(); }

private:
   Data from;
   Data to;
   SipCallId callId;
   SipCSeq cseq;
   
   /*** Create by decoding the data string passed in. This is the decode
        or parse. This call should only be used inside the stack and is
        not part of the API */
   //NOTE:  Implementation was:  assert(0).  Disabling entirely. --Ben
   SipCallLeg( const Data& srcData );
   SipCallLeg();
};

} // namespace Vocal


inline bool
operator<( const Sptr<Vocal::SipCallLeg>& left, const Sptr<Vocal::SipCallLeg>&  right)
{
    return (*left < *right);
}
#endif
