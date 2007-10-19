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


#include "InviteMsg.hxx" 
#include "StatusMsg.hxx" 
#include "AckMsg.hxx" 
#include "SipRoute.hxx" 
#include "SystemInfo.hxx" 
#include "UaBase.hxx" 
#include "SipRecordRoute.hxx"
#include "SipVia.hxx"
#include "BasicAgent.hxx"

using namespace Vocal;
using namespace Vocal::UA;

using Vocal::InviteMsg;
using Vocal::StatusMsg;
using Vocal::UA::UaBase;

// 26/1/04 fpi
// ALLOW field
const string UaBase::allowField("INVITE,ACK,CANCEL,BYE,OPTIONS");

int UaBase::_cnt = 0;

UaBase::UaBase( const char* class_name,
                const Sptr<SipMsg>& reqMsg, Sptr<SipTransceiver> stack,
                BasicAgent* controllerAgent, BaseFacade* _facade,
                const char* dbg_id)
      : SipCallLegData(reqMsg),
        facade(_facade),
        myStack(stack),
        myLocalCSeq(reqMsg->getCSeq()),
        callId(*(reqMsg.getPtr())),
        instanceName(dbg_id),
        myControllerAgent(controllerAgent)
{
   _cnt++;
   cpLog(LOG_ERR, "Creating UaBase, this: %p  debug_id: %s  call-id: %s\n",
         this, instanceName.c_str(), callId.getCallId().c_str());
   myState = UaStateFactory::instance().getState(U_STATE_IDLE);

}// constructor


Sptr<BasicAgent>  UaBase::getControllerAgent() {
   return myControllerAgent;
}

void UaBase::receivedMsg(const Sptr<SipMsg>& sipMsg) {
   if (sipMsg->getType() == SIP_STATUS) {
      cpLog(LOG_DEBUG, "(%s:%p) received STATUS message %s, state: %s ",
            instanceName.c_str(), this,
            sipMsg->encode().logData(),
            myState->toString().c_str());
      myState->recvStatus(*this, sipMsg);
   }
   else {
      cpLog(LOG_DEBUG, "(%s:%p) received NON-STATUS message %s, state: %s ",
            instanceName.c_str(), this,
            sipMsg->encode().logData(),
            myState->toString().c_str());
      myState->recvRequest(*this, sipMsg);
   }
}

int UaBase::sendMsg(Sptr<SipMsg> sipMsg, const char* dbg) {
   cpLog(LOG_DEBUG_STACK , "(%s:%p) Processing message %s ",
         instanceName.c_str(), this,
         sipMsg->encode().logData());
   if (sipMsg->getType() == SIP_STATUS) {
      return myState->sendStatus(*this, sipMsg, dbg);
   }
   else {
      return myState->sendRequest(*this, sipMsg, dbg);
   }
}//sendMsg

Sptr<InviteMsg>
UaBase::createInvite(const Sptr<InviteMsg>& invMsg, bool changeCallId,
                     const string& _localIp, unsigned short _sipPort,
                     const string& natHost, int transport,
                     const NetworkAddress& proxyAddress)
{
    Sptr<InviteMsg> inviteMsg = new InviteMsg(*invMsg);

    // 26/1/04 fpi
    // change ALLOW header
    inviteMsg->setNumAllow(0);
    inviteMsg->setAllow(Data(allowField), 0);

    //Remove the contact header 
    inviteMsg->setNumContact(0);
    //Set the contact as B2b
    Sptr<BaseUrl> bUrl = inviteMsg->getFrom().getUrl();
    Sptr<SipUrl> mUrl;
    mUrl.dynamicCast(bUrl);
    //TODO - Handle the case with TelUrl
    assert(mUrl != 0);
   
    mUrl->setHost(natHost);
    mUrl->setPort(Data(_sipPort));
 
    SipContact me("", _localIp);
    me.setUrl(mUrl.getPtr());
    inviteMsg->setContact(me);

    //Clear VIA and set B2b as first via
    inviteMsg->flushViaList();
    SipVia via("", _localIp);
    via.setHost(_localIp);
    via.setPort(_sipPort);
    if (transport == 2) {
       via.setTransport("TCP");
    }
    inviteMsg->setVia(via);

    inviteMsg->setNumRecordRoute(0);
    inviteMsg->setNumRoute(0);

    if (changeCallId) {
       //Set the new callId 
       SipCallId id("", _localIp);
       inviteMsg->setCallId(id);
       //Set the request URI to outbound proxy
       SipRequestLine& reqLine = inviteMsg->getMutableRequestLine();
       Sptr<SipUrl> sUrl;
       sUrl.dynamicCast(reqLine.getUrl());
       sUrl->setMaddrParam("");
       //Look for magic "!" in the request line
       //if found parse it and send it to the contact found in it
       string sUser = sUrl->getUserValue().logData();
       string::size_type pos;
       if ((pos = sUser.find("!")) != string::npos) {
          string user = sUser.substr(0, pos);
          string hostPort = sUser.substr(pos+1);
          string host = hostPort;
          pos = hostPort.find("_");
          int nextHopPort = 0;
          if (pos != string::npos) {
             string prt = hostPort.substr(pos+1);
             nextHopPort = atoi(prt.c_str());
             sUrl->setPort(Data(nextHopPort));
             host = hostPort.substr(0, pos);
          }
          NetworkAddress na (host);
          string nextHopAddr = na.getIpName().convertString();
          sUrl->setUserValue(user);
          sUrl->setHost(nextHopAddr);
       }
       else {
          string outboundProxyAddr = proxyAddress.getIpName().convertString();
          int outboundProxyPort =  proxyAddress.getPort();
          
          // 20/01/04 fpi
          // I think it should be possible to do not specify
          // a proxy. So I use "0.0.0.0" value or proxyAddress
          // string length equal to 0 with "do not use proxy"
          // meaning			  
          if (outboundProxyAddr.length() && (outboundProxyAddr != "0.0.0.0")) {
             sUrl->setHost(outboundProxyAddr);
             sUrl->setPort(Data(outboundProxyPort));
          }
          
          if (!sUrl->getUserValue().length()) {
             //set the user to be to
             sUrl->setUserValue(inviteMsg->getTo().getUser());
          }
       }
       if (transport == 2) {
          sUrl->setTransportParam("tcp");
       }
       reqLine.setUrl(sUrl.getPtr());
    }

    return inviteMsg;
}

Sptr<InviteMsg>
UaBase::createReInvite(const Sptr<InviteMsg>& invMsg) {
    Sptr<InviteMsg> inviteMsg = createInvite(invMsg, false, getMyLocalIp(),
                                             getMySipPort(),
                                             getNatHost(), getTransport(),
                                             *(getProxyAddr()));
	
	 // 26/1/04 fpi
	 // change ALLOW header
	 inviteMsg->setNumAllow(0);
	 inviteMsg->setAllow(Data(allowField), 0);

    //This is a reInvite case, will use the route we have saved in the
    //begining of the dialog

    assert(myRouteList.size());
    inviteMsg->setRouteList(myRouteList);
    SipRoute siproute = inviteMsg->getRoute(0);
    inviteMsg->removeRoute(0);
    SipRequestLine& reqLine = inviteMsg->getMutableRequestLine();
    reqLine.setUrl( siproute.getUrl() );
    inviteMsg->setCallId(getRequest()->getCallId());
    //Get the CSeq from the local CSeq, increment it and use it.
    myLocalCSeq.incrCSeq();
    inviteMsg->setCSeq(myLocalCSeq);
    return inviteMsg;
}

      
//Sptr<StatusMsg> // VEER
void
UaBase::sendReplyForRequest(const Sptr<SipMsg>& sipMsg, int statusCode, 
                            Sptr<SipContentData> contentData,
                            bool memorizeResponse)
{
    assert(sipMsg->getType() != SIP_STATUS);
    //Send status message
    Sptr<SipCommand> sipCmd;
    sipCmd.dynamicCast(sipMsg);
    assert(sipCmd != 0);
    Sptr<StatusMsg> sendSMsg = new StatusMsg(*sipCmd, statusCode);
    if (contentData != 0) {
       sendSMsg->setNumContentData(0);
       sendSMsg->setContentData(contentData.getPtr());
    }
    cpLog(LOG_DEBUG, "(%s) sending status %s", instanceName.c_str(), sendSMsg->encode().logData());
    myStack->sendReply(sendSMsg, "UaBase::sendReplyForRequest");
    if (memorizeResponse) {
       setResponse(sendSMsg.getPtr());
    }

    // return sendSMsg; // VEER
}

void
UaBase::ackStatus(const Sptr<SipMsg>& msg, Sptr<SipSdp> sipSdp)
{
    Sptr<StatusMsg> sMsg;
    sMsg.dynamicCast(msg);
    assert(sMsg != 0);
    if(sMsg->getStatusLine().getStatusCode() >= 200)
    {
        Sptr<AckMsg> ackMsg = new AckMsg(*sMsg, sMsg->getLocalIp());
        //if(sMsg->getStatusLine().getStatusCode() > 200)
        {
            //Ack should have the same request line and via as the
            //Original invite
            Sptr<SipCommand> invMsg;
            invMsg.dynamicCast(getRequest());
            assert(invMsg != 0);
            ackMsg->flushViaList();
 
            SipVia via = invMsg->getVia(0);
            ackMsg->setVia(via, 0);
            ackMsg->setRouteList(myRouteList);
            ackMsg->removeRoute(0);
 
            Sptr<SipUrl> sUrl;
            sUrl.dynamicCast(invMsg->getRequestLine().getUrl());
            SipRequestLine rLine(SIP_ACK, sUrl.getPtr(), sMsg->getLocalIp());
            ackMsg->setRequestLine(rLine);
        }

        // 28/1/04 fpi
        // adding slowstart management
        if (sipSdp != 0) {
           ackMsg->setNumContentData(0);
           ackMsg->setContentData(sipSdp.getPtr());
        }

        myStack->sendAsync(ackMsg.getPtr(), "UaBase::ackStatus");
    }
}



UaBase::~UaBase() {
   assertNotDeleted();
   //cerr << "UaBase::~UaBase:" << myAgentRole << endl;
   _cnt--;
   cpLog(LOG_ERR, "UaBase: (%s:%p) Deleting instance, call-id: %s",
         instanceName.c_str(), this, callId.getCallId().c_str());
   // Make sure the call is purged soon after...
   myStack->setCallPurgeTimer(callId, vgetCurMs() + (10 * 1000));
   clearRouteList();
}

void UaBase::clearRouteList() {
   myRouteList.clear();
}


void 
UaBase::saveRouteList(const Sptr<SipMsg>& msg, bool reverse) {
    clearRouteList();
    SipRecordRouteList rrList = msg->getrecordrouteList();
    SipRecordRouteList::iterator iter = rrList.begin();

    //Reverse the recordRoute list and form a route list
    // (A,B,C) -> (C,B, A)
    while ( iter != rrList.end() ) {
        Sptr< BaseUrl > baseUrl = (*iter)->getUrl();
        if( baseUrl->getType() == TEL_URL )
        {
            cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
            continue;
        }
        Sptr< SipUrl > sipUrl;
        sipUrl.dynamicCast( baseUrl );
        assert( sipUrl != 0 );

        SipRoute* route = new SipRoute("", msg->getLocalIp());
cpLog(LOG_DEBUG, "**** Adding route %d: %s", reverse, baseUrl->encode().logData());
        if(reverse)
        {
            route->setUrl( baseUrl );
            myRouteList.insert( myRouteList.begin(), route );
        }
        else
        {
            route->setUrl( baseUrl );
            myRouteList.push_back( route );
        }
        iter++;
    }

    //Append the contacts at the end of the route list
    int numContact = msg->getNumContact();
    if ( numContact )
    {
        SipContact contact = msg->getContact( numContact - 1 );
        SipRoute* route = new SipRoute("", msg->getLocalIp());
        route->setUrl( contact.getUrl() );
        myRouteList.push_back( route );
    }
}

void 
UaBase::fixSdpForNat(Sptr<SipMsg> sipMsg, const Data& natIp)
{
   bool fixSdp = false;
   const SipVia& natVia = sipMsg->getVia(0);
   string addr1 = natVia.getHost().c_str();
   if((addr1 != "") && (natIp.length()))
   {
       NetworkAddress netaddr1(addr1);
       NetworkAddress netaddr2(natIp.c_str());

       int numRecRoute = sipMsg->getNumRecordRoute();
       if(numRecRoute > 0) 
       {
          //do nothing
          return;
       }
 
       if ( netaddr1.getIpName() != netaddr2.getIpName())
       {
           fixSdp = true;
       }
    }
    if(fixSdp && natIp.length())
    {
        switch(sipMsg->getType())
        {
            case SIP_STATUS:
            {
                Sptr<StatusMsg> sMsg;
                sMsg.dynamicCast(sipMsg);
                if(sMsg->getStatusLine().getStatusCode() == 200)
                {
                    //Fix the Contact, handle multiple contacts TODO
                    SipContact c = sMsg->getContact();
                    Sptr<SipUrl> sUrl;
                    sUrl.dynamicCast(c.getUrl());
                    assert(sUrl != 0);
                    sUrl->setHost(natIp);
                    c.setUrl(sUrl.getPtr());
                    sMsg->setNumContact(0);
                    sMsg->setContact(c);
                }
                else {
                   //Wrong status message type, return
                   return;
                }
            }
            //Follow through
            case SIP_INVITE:
            case SIP_ACK:
            {
                cpLog(LOG_DEBUG, "**Setting (%s) in SDP", natIp.c_str());
                //Modify the SDP
                Sptr<SipSdp> sdp;
                sdp.dynamicCast(sipMsg->getContentData(0));
                if(sdp != 0)
                {
                    SdpSession& sdpSess = sdp->getSdpDescriptor();
                    SdpConnection connection;
                    connection.setUnicast(natIp.c_str());
                    sdpSess.setConnection(connection);
                }
            }
            break;
            default:
            break;
            
        }
    }
}


void UaBase::setState(Sptr<UaState> state) {
   string cs("NULL");
   if (myState != 0) {
      cs = myState->toString();
   }
   else {
      cs = "Undetermined state";
   }

   string ns("NULL");
   if (state != 0) {
      ns = state->toString();
   }
   else {
      ns = "Undetermined state";
   }

   cpLog(LOG_DEBUG, "UaBase::setState:  %s:%p changing state from: %s to: %s(%p)\n",
         instanceName.c_str(),
         this, cs.c_str(), ns.c_str(), state.getPtr());

   myState = state;
}

void UaBase::setControllerAgent(Sptr<BasicAgent> a) {
   // NOTE:  If the BasicAgent is deleting itself, then it will set
   // this to NULL to keep us from de-referencing stale memory.

   myControllerAgent = a;
}
