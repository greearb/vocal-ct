#ifndef SipCallLegData_hxx
#define SipCallLegData_hxx

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


static const char* const SipCallLegData_hxx_Version =
    "$Id: SipCallLegData.hxx,v 1.2 2004/06/15 00:30:11 greear Exp $";

#include "global.h"
#include <vector>

#include "SipCallLeg.hxx" 
#include "ContactData.hxx" 
#include "UaDef.hxx"
#include "SipMsg.hxx"
#include "SipSdp.hxx"
#include "SipCallLeg.hxx"
#include <BugCatcher.hxx>

using Vocal::SipMsg;
using Vocal::SipSdp;
using Vocal::SipCallLeg;
using namespace Vocal::SDP;


/** Object SipCallLegData
<pre>
 <br> Usage of this Class </br>

    Data container for a call-Leg that has one or more call-legs
    it is pairing with.The data also save the requests (INVITE, ACK) and
    responses (2xx) for the call-leg.

    CLeg  ------> Cleg2
          ------> CLeg3

Example:

 class ClassFoo
 {
    public:

    typedef vector<Sptr<SipCallLegData> > CallPeerList;

    ClassFoo();

    void doSomething(Sptr<UaBase> userAgent, Sptr<UaBase> peerAgent);

    void addPeer(const Sptr<SipCallLegData>& callLegData);

    private:
     CallPeerList myCallPeerList;
 }

 void
 ClassFoo::doSomething(Sptr<UaBase> userAgent, Sptr<UaBase> peerAgent)
 {
    const SipCallLeg& cLeg = userAgent->getCallLeg();
    const SipCallLeg& peerLeg = peerAgent->getCallLeg();

    //First update userAget data, with peer init
    Sptr<MultiLegCallData> umData = myMultiLegCallDataMap[cLeg];
 }

 void
 ClassFoo::addPeer(const Sptr<SipCallLegData>& callLegData)
 {
    myCallPeerList.push_back(callLegData);
 }

</pre>
*/
class SipCallLegData : public BugCatcher
{
   public:
      ///
      typedef vector<Sptr<ContactData> > ContactVector;

      ///
      SipCallLegData( const Sptr<SipMsg>& reqMsg );

      ///
      SipCallLegData( const SipCallLegData& src )
      {
          copyObj(src);
      }

      /// 
      const SipCallLegData& operator =( const SipCallLegData& src )
      {
          if(this != &src)
          {
              copyObj(src);
          }
          return *this;
      }

      ///
      void copyObj(const SipCallLegData& src) 
      {
          mySipCallLeg = src.mySipCallLeg;
          myLocalSdpData = src.myLocalSdpData;
          myRemoteSdpData = src.myRemoteSdpData;
          myRequestMsg = src.myRequestMsg;
          myContactVector.erase(myContactVector.begin(), myContactVector.end());
          for(ContactVector::const_iterator itr = src.myContactVector.begin(); 
                                itr != src.myContactVector.end(); itr++)
          {
              myContactVector.push_back(*itr);
          }
      }

      /**Insert an element in vector, insert in the begining 
      * if begining == true.
      */
      void addContact(const Sptr<ContactData>& contact, bool begining);

      ///
      void pushContact(const Sptr<ContactData>& contact);

      ///
      Sptr<ContactData> popContact();

      ///
      int getNumContacts() const { return myContactVector.size(); };

      /**Get the contact data at a certain index. By default
       * entry push last is returned.
       */
      Sptr<ContactData> getContactData(int index=-1) const;

      ///Set the INVITE being sent/received for the call-leg
      void setRequest(const Sptr<SipMsg>& sipMsg);
      ///Set the 2xx response sent/received for the call-leg
      void setResponse(const Sptr<SipMsg>& sipMsg);

      ///Set the Ack being sent/received for the call-leg.
      void setAck(const Sptr<SipMsg>& sipMsg);

      ///Set the local SDP answerd or offerd.
      void setLocalSdp(const Sptr<SipSdp>& sdpData) { myLocalSdpData = sdpData; } ;
      ///Set the remote SDP received
      void setRemoteSdp(const Sptr<SipSdp>& sdpData) { myRemoteSdpData = sdpData; } ;

      ///Valid values C_LIVE, C_HOLD, C_DISCONNECTED
      void setCallLegState(Vocal::UA::CallLegState cState) { myCallLegState = cState; };

      ///
      const Sptr<SipSdp> getLocalSdp() const { return myLocalSdpData; };
      ///
      const Sptr<SipSdp> getRemoteSdp() const { return myRemoteSdpData; };

      ///
      Vocal::UA::CallLegState getCallLegState() const { return myCallLegState; };

      ///
      const Sptr<SipMsg> getRequest() const { return myRequestMsg; };

      ///
      const Sptr<SipMsg> getResponse() const { return myResponseMsg; };

      ///
      const Sptr<SipMsg> getAck() const { return myAckMsg; };

      ///
      const SipCallLeg& getCallLeg() const { return *mySipCallLeg; };

      ///
      virtual ~SipCallLegData() { };

   protected:
      ///
      Sptr<SipSdp> myLocalSdpData;
      ///
      Sptr<SipSdp> myRemoteSdpData;

      ///
      Sptr<SipMsg> myRequestMsg;
      ///
      Sptr<SipMsg> myResponseMsg;
      ///
      Sptr<SipMsg> myAckMsg;
      ///
      Vocal::UA::CallLegState myCallLegState;
      /// 
      Sptr<SipCallLeg> mySipCallLeg;
      ///
      ContactVector myContactVector; 
};


#endif
