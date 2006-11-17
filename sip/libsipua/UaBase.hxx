#ifndef UaBase_hxx
#define UaBase_hxx

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
#include <vector>
#include "BaseFacade.hxx"
#include "InviteMsg.hxx" 
#include "SipCallLegData.hxx" 
#include "SipProxyEvent.hxx" 
#include "SipTransceiver.hxx" 
#include "UaStateFactory.hxx" 

namespace Vocal {

namespace UA {


/** Object UaBase
<pre>
   This should be called by the application to setup the
   proxy, sip-port and transport(1= UDP, 2=TCP)

</pre>
*/


class BasicAgent;

/** Base class for Server/Client side User agents. The class maintains its
*   own local sequence for any request generated either as UAS or UAC.
*/
class UaBase : public SipCallLegData {
public:
   typedef vector <Sptr<SipRoute> > RouteList;

   /**Constructor
    * @param reqMsg - Message that causes its creation (UAS, msg received),
    (UAC, msg being sent).
    * @param stack - Ref. to sipstack being used to send/receive SIP msgs.
    * @param controllerAgent - Ref. to the call-controller agent, managing
    a call.
    * @param facade  The facade, keeper of global & config info.
    * @param dbg_id  Identifier used for debugging purposes.
    */
   UaBase( const char* class_name, const Sptr<SipMsg>& reqMsg,
           Sptr<SipTransceiver> stack,
           BasicAgent* controllerAgent,
           BaseFacade* facade, const char* dbg_id);

   ///
   const string& className() { return _class_name; }

   ///
   virtual ~UaBase();

   ///Set the ua-agent state 
   virtual void setState(Sptr<UaState> state);

   /**Gets called by the application when a message is received on the
    * calleg, the agent is representing.
    */
   virtual void receivedMsg(const Sptr<SipMsg>& sipMsg);

   /// Returns < 0 on failure
   virtual int sendMsg(Sptr<SipMsg> sipMsg, const char* dbg);

   ///Returns true if was UAC when created
   virtual bool isAClient() const { return (myAgentRole == A_CLIENT); }
   ///Returns true if was UAS when created
   virtual bool isAServer() const { return (myAgentRole == A_SERVER); }

   /**Constructs a BYE message and sends it.
    * Derived classes define how to construct the BYE msg.
    */
   virtual Sptr<SipMsg> sendBye() = 0;

   ///Create a status message for the received request.
   virtual  void sendReplyForRequest(const Sptr<SipMsg>& sipMsg, 
                                     int statusCode,
                                     Sptr<SipContentData> contentData=0,
                                     bool memorizeResponse = true);
   ///Acknowledge a status
   virtual void ackStatus(const Sptr<SipMsg>& msg, Sptr<SipSdp> sipSdp = 0);

   ///
   static void fixSdpForNat(Sptr<SipMsg> sipMsg, const Data& natIp);

    
   ///Utility function to create an invite from an invite message
   static Sptr<InviteMsg> createInvite(const Sptr<InviteMsg>& invMsg,
                                       bool changeCallId,
                                       const string& _localIp,
                                       unsigned short _sipPort,
                                       const string& natHost, int transport,
                                       const NetworkAddress& proxyAddress);

   ///Constructs a re-invite message from a Invite, updates the CSEQ.
   Sptr<InviteMsg> createReInvite(const Sptr<InviteMsg>& invMsg);


   /**Keeps a copy of the route list received in a request/response.
    * if UAC, route list is saved from the 200 OK received and order
    * is reversed. if UAS, route list is saved from the INVITE received.
    */
   void saveRouteList(const Sptr<SipMsg>& msg, bool reverse); 
   ///
   RouteList getRouteList() const { return myRouteList; };

   ///Initialize the local seq number
   void setLocalCSeq(SipCSeq seq) { myLocalCSeq = seq; };
   ///
   const SipCSeq&  getLocalCSeq() const { return myLocalCSeq; };

   ///
   Sptr<SipTransceiver> getSipTransceiver() { return myStack; }

   virtual unsigned short getMySipPort() const { return facade->getLocalSipPort(); }
   virtual string getMyLocalIp() const { return facade->getLocalIp(); }
   virtual NetworkAddress* getProxyAddr() { return facade->getProxyAddr(); }
   virtual const string& getNatHost() { return facade->getNatHost(); }
   virtual int getTransport() { return facade->getTransport(); }

   Sptr<BasicAgent>  getControllerAgent();
   void setControllerAgent(Sptr<BasicAgent> a);

   Sptr<UaState> getState() { return myState; }
   
   // 26/1/04 fpi
   // ALLOW field
   static const string allowField;

protected:

   ///
   void clearRouteList();
   ///
   void setRole(AgentRole role) { myAgentRole = role; };
   ///
   AgentRole    myAgentRole;

   BaseFacade* facade; // Knows where all global info is.

   ///
   RouteList    myRouteList;
   ///
   Sptr<SipTransceiver> myStack;
   ///
   SipCSeq myLocalCSeq;

   SipTransactionId callId; // So we can clean it up in the destructor

private:

   string instanceName; //Used for debugging.
   string _class_name;

   ///
   Sptr<UaState>     myState;

   /* Not supported */
   UaBase();
   UaBase( const UaBase& src );
   UaBase& operator=(const UaBase& rhs);

   Sptr<BasicAgent> myControllerAgent;
   
};

}

}

#endif
