#ifndef Sip_Transaction_Id__hxx
#define Sip_Transaction_Id__hxx

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

static const char* const SipTransactionId_hxx_version =
    "$Id: SipTransactionId.hxx,v 1.4 2004/06/16 06:51:26 greear Exp $";

#include "Data.hxx"
#include <BugCatcher.hxx>

namespace Vocal
{

class SipMsg;
    
class SipTransactionId : public BugCatcher {
public:
   /**
    * the first level key type - it's been chosen to be Data
    * instead of MD5 unless we can be assured that MD5 will
    * provide BETTER randomness (in hashing)than a simple Data
    * string.
    *
    * this level will consist of To (NameAddress only), From
    * (NameAddress only) and the CallID - specifically the
    * CallLeg (w/o the tags)
    *
    * this level will group together all the messages for a
    * single CallLeg.
    */
   typedef Data KeyTypeI;

   /**
    * the second level key type will consist of [CSeq
    * Number]+[top Via Branch]
    * 
    * this is the level at which cancel and retransmissions take
    * effect
    */
   typedef Data KeyTypeII;

   /**
    * the third level key will consist of [Cseq Method], and is
    * required to * keep distinct request-response sequences
    * during the same Cseq & branch.
    *
    * its been chosen to be Data and not "Method" (enum from
    * symbols.hxx) 'coz SipCSeq returns method param as Data and
    * to convert it to enum will have to compare and test thru
    * all method strings, which can be ammortized by string
    * compare at lookup time - the plus being not to do the ugly
    * work here and keep maintaning it!
    *
    * NOTE: the method tag of Ack msgs will not be changed to
    * INVITE. it will be keyed as ACK, and for 200 filtering the
    * user will search in level3 list for key type ACK (there's
    * ack only for responses of invites, hence there'll be only
    * one ACK in the level3 list).
    */
   typedef Data KeyTypeIII;
   
   SipTransactionId(const SipMsg& sipMsg);
   SipTransactionId(const SipTransactionId& sipTransactionId);
   virtual ~SipTransactionId();

   SipTransactionId& operator= (const SipTransactionId& sipTransactionId);
   bool operator==(const SipTransactionId& sipTransactionId) const;
   bool operator<(const SipTransactionId& sipTransactionId) const;

   KeyTypeI& getLevel1() const;
   KeyTypeII& getLevel2() const;
   KeyTypeIII& getLevel3() const;
   bool getValid() const;

   virtual void clear();

   virtual string toString();

protected:
   SipTransactionId();

private:
   bool               valid;
   mutable KeyTypeI   level1;
   mutable KeyTypeII  level2;
   mutable KeyTypeIII level3;
};
 
} // namespace Vocal
#endif
