#ifndef ContactData_hxx
#define ContactData_hxx

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


static const char* const ContactData_hxx_Version =
    "$Id: ContactData.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include "global.h"
#include <vector>
#include "SipCallLeg.hxx"


class SipCallLegData;

/** Object ContactData

<pre>
<br> Usage of this Class </br>

    This a Data container for holding the Contact Data for a call-leg.The
    ContactData holds the list of all the call-legs that are
    pairing with a given call-leg.

    UaBase callleg1, callleg2, callleg3;

    <br> Sptr<ContactData> contactData = new ContactData(callleg2);
          Sptr<ContactData> contactData2 = new ContactData(callleg3); </br>

    callleg1->pushContact(contactData);
    callleg1->pushContact(contactData2);

    For detailed Informantion about extensive use of this object please
    refer to /vocal/sip/libsipua/CallDB.cxx

</pre>

*/
class ContactData 
{
   public:
      ///
      typedef vector<Sptr<SipCallLegData> > CallPeerList;

      /// Create one with default values
      ContactData();

      /// Create one with contact values
      ContactData(const Sptr<SipCallLegData>& cLegData);

      ///
      ContactData( const ContactData& src );

      /// 
      const ContactData& operator =( const ContactData& src );

      ///
      void copyObj(const ContactData& src );

      ///
      void addPeer(const Sptr<SipCallLegData>& callLegData);

      ///
      const CallPeerList& getPeerList() const { return myCallPeerList; };
   private:
      ///
      CallPeerList  myCallPeerList;
};


#endif
