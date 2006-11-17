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


#include "ByeMsg.hxx"
#include "InviteMsg.hxx"
#include "CancelMsg.hxx"
#include "SystemInfo.hxx"
#include "ControlStateFactory.hxx"
#include "UaCallControl.hxx"
#include "UaServer.hxx"
#include "UaFacade.hxx"
#include "SdpHandler.hxx"
#include "CallDB.hxx"
#include "UaClient.hxx"
#include "CallAgent.hxx"
#include "UaConfiguration.hxx"
#include "SipServer.hxx"
#include "SipDiversion.hxx"
#include "Authenticate.hxx"
#include "FileMediaDevice.hxx"
#include "UaCommandLine.hxx"
#ifdef USE_VM
#include "VmcpDevice.hxx"
#endif
#ifdef USE_MPEGLIB
#include "VideoDevice.hxx"
#endif

#include "MediaController.hxx"

using namespace Vocal;
using namespace Vocal::UA;

int CallAgent::_cnt = 0;

//long CallAgent::myRotatingId = 0;

CallAgent::CallAgent(int callId, Sptr<SipMsg> sipMsg, UaFacade* _facade, AgentRole aRole)
      : BasicAgent(callId, "CallAgent") , myRole(aRole), myActiveFlg(true),
        freedMedia(false)
{ 
    _cnt++;
    facade = _facade;
    myState = ControlStateFactory::instance().getState(INIT);
    //Create new UserAgent
    if (aRole == A_CLIENT) {
        char buf[64];
        snprintf(buf, 63, "CallAgent-C:%d\n", callId);
        buf[63] = 0;
        //Create the invite message
        myInvokee = new UaClient(sipMsg, facade->getSipTransceiver(), this,
                                 facade, buf);
        myState->makeCall(*this);
    }
    else {
        char buf[64];
        snprintf(buf, 63, "CallAgent:%d\n", callId);
        buf[63] = 0;
        myInvokee = new UaServer(sipMsg, facade->getSipTransceiver(), this,
                                 facade, buf);
        myInvokee->receivedMsg(sipMsg);
        myState->ringing(*this);
        myActiveFlg = true;
    }

    //Add the callleg in CallDb
    CallDB::instance().addCallLeg(myInvokee);
};


CallAgent::~CallAgent() { 
   assertNotDeleted();
   freeMedia();
   _cnt--;
}


Sptr<StatusMsg>
CallAgent::doholdresume200OKstuff(const Sptr<SipMsg>& msg, SdpSession& remoteSdp ) {
   Sptr<StatusMsg> statusMsg;
   Sptr<SipCommand> sCommand;
   sCommand.dynamicCast(myInvokee->getRequest());
   assert(sCommand != 0);
   statusMsg = new StatusMsg(*sCommand, 200);

   // Set Contact header
   Sptr< SipUrl > myUrl = new SipUrl("", myInvokee->getMyLocalIp());
   myUrl->setUserValue(UaFacade::getBareUserName());
   myUrl->setHost(myInvokee->getMyLocalIp());
   myUrl->setPort(Data(myInvokee->getMySipPort()));
   SipContact myContact("", myInvokee->getMyLocalIp());
   myContact.setUrl( myUrl.getPtr() );
   statusMsg->setNumContact( 0 );    // Clear old contact
   statusMsg->setContact( myContact );
   SipServer lServer(UaFacade::VOIP_ID_STRING, myInvokee->getMyLocalIp());
   statusMsg->setServer(lServer);

   SipSdp localSdp(myInvokee->getMyLocalIp());
   int sessionId =
      myInvokee->getLocalSdp()->getSdpDescriptor().getSessionId();
   SdpSession localS =
      MediaController::instance().getSdp(sessionId, remoteSdp);
   setHost(localS, myInvokee->getNatHost());
   localSdp.setSdpDescriptor(localS);
   statusMsg->setContentData(&localSdp, 0);
   return statusMsg;
}

void CallAgent::placeCall() {
   cpLog(LOG_DEBUG, "CallAgent::placeCall()");
   Sptr<SipMsg> sipMsg = myInvokee->getRequest();
   myInvokee->sendMsg(sipMsg, "CA: place call");
   facade->postMsg(sipMsg, true);
}


void CallAgent::doBye() {
   if (!myState->canBye()) {
      cpLog(LOG_ERR, "CallAgent::doBye(), this: %p, myState: %s cannot bye!",
            this, myState->className().c_str());
      
      // Try to end call, maybe that will work!
      endCall();

      return;
   }

   bool could_post = false;
   cpLog(LOG_DEBUG_STACK, "CallAgent::doBye(), this: %p", this);
   Sptr<SipMsg> bMsg = myInvokee->sendBye();
   if (bMsg != 0) {
      facade->postMsg(bMsg, true);
      could_post = true;
   }

   endCall();

   if (!could_post) {
      strstream str;
      str << "L_HANGUP BYE" << ends;
      facade->postMsg(str.str());
      str.freeze(false);
   }
}//doBye


void CallAgent::endCall() {
   cpLog(LOG_DEBUG_STACK, "CallAgent::endCall, this: %p  in state: %s\n",
         this, myState->className().c_str());
   assertNotDeleted();

   if (!myActiveFlg) {
      cpLog(LOG_ERR, "Call Agent no more active, ignoring endCall(), this: %p", this);
      return;
   }

   cpLog(LOG_DEBUG_STACK, "CallAgent::endCall(), this: %p", this);
   
   try {
      freeMedia();

      // Transit the controller state
      myState->bye(*this);  //-> TEAR_DOWN
      while (myState->end(*this) < 0) {  //-> INIT
         // NOTE:  It is REQUIRED that states eventually transition to a state
         // where 'end' cannot fail.
         cpLog(LOG_ERR, "Failed to end in previous state, trying again in state: %s\n",
               myState->className().c_str());
         // Continue
      }
      facade->notifyCallEnded();
   }
   catch(CInvalidStateException& e) {
      cpLog(LOG_ERR, "Unexpected Error ? :(:%s", e.getDescription().c_str());
   }
}//endCall


void CallAgent::freeMedia() {
   cpLog(LOG_DEBUG_STACK, "in CallAgent::freeMedia, freedMedia: %d\n",
         (int)(freedMedia));
   if (! freedMedia) {
      if (myInvokee != 0) {
         int sessionId = myInvokee->getLocalSdp()->getSdpDescriptor().getSessionId();
         cpLog(LOG_DEBUG_STACK, "Freeing session: %d\n", sessionId);
         MediaController::instance().freeSession(sessionId);
      }
      if (facade != 0) {
         facade->releaseMediaDevice(getId());
      }
      freedMedia = true;
   }
}//freeMedia

void CallAgent::doCancel() {
   cpLog(LOG_DEBUG, "CallAgent::doCancel(), STARTING");
   try {
      freeMedia();
      
      // Transit the controller state
      myState->cancel(*this);
      while (myState->end(*this) < 0) {  //-> INIT
         // NOTE:  It is REQUIRED that states eventually transition to a state
         // where 'end' cannot fail.
         cpLog(LOG_ERR, "Failed to end in previous state, trying again in state: %s\n",
               myState->className().c_str());
         // Continue
      }
   }
   catch(CInvalidStateException& e) {
      cpLog(LOG_ERR, "Unexpacted Error ? :(:%s", e.getDescription().c_str());
   }
   cpLog(LOG_DEBUG, "CallAgent::doCancel(), DONE");
}//doCancel

void CallAgent::inCall() {
   cpLog(LOG_DEBUG, "CallAgent::inCall()");
   myState->inCall(*this);

#ifdef USE_MPEGLIB
   int video  = atoi(UaConfiguration::instance().getValue(VideoTag).c_str());
   //Even if video mode is on, when running in command line mode force the
   //non-video option
   if (video && (!UaCommandLine::instance()->getIntOpt("cmdline"))) {
      Sptr<VideoDevice> vDevice;
      vDevice.dynamicCast(facade->getMediaDevice(getId()));
      assert(vDevice != 0);
      vDevice->startMedia(myInvokee->getRemoteSdp()->getSdpDescriptor());
   }
   else {
      startSession(myInvokee->getLocalSdp()->getSdpDescriptor(), 
                   myInvokee->getRemoteSdp()->getSdpDescriptor());
   }
#else
   startSession(myInvokee->getLocalSdp()->getSdpDescriptor(), 
                myInvokee->getRemoteSdp()->getSdpDescriptor());
#endif
}//inCall

void CallAgent::callFailed() {
   cpLog(LOG_DEBUG, "CallAgent::callFailed()");
   myState->cancel(*this);
   while (myState->end(*this) < 0) {  //-> INIT
      // NOTE:  It is REQUIRED that states eventually transition to a state
      // where 'end' cannot fail.
      cpLog(LOG_ERR, "Failed to end in previous state, trying again in state: %s\n",
            myState->className().c_str());
      // Continue
   }

   strstream str;
   str << "L_HANGUP CALL_FAILED" << ends;
   facade->postMsg(str.str());
   str.freeze(false);
}

///
void CallAgent::receivedRequest(UaBase& agent, const Sptr<SipMsg>& msg) {
   //Notify GUI
   facade->postMsg(msg, false);
   myState->recvReq(*this, msg);
}

///
void CallAgent::receivedStatus(UaBase& agent, const Sptr<SipMsg>& msg) {
   try {
      myState->recvStatus(*this, msg);
      Sptr<StatusMsg> statusMsg;
      statusMsg.dynamicCast(msg);
      assert(statusMsg != 0);
      if (statusMsg->getStatusLine().getStatusCode() == 200 &&
          statusMsg->getCSeq().getMethod() == INVITE_METHOD ) {
         agent.ackStatus(msg);
      }
      
      if (statusMsg->getStatusLine().getStatusCode() == 302) {
         Sptr<InviteMsg> inviteMsg;
         inviteMsg.dynamicCast(myInvokee->getRequest());
         SipRequestLine& reqLine = inviteMsg->getMutableRequestLine();
         SipContactList contactList = statusMsg->getContactList();
         SipContactList::iterator iter = contactList.begin();
         Sptr < SipUrl > iterSipUrl;
         iterSipUrl.dynamicCast( ( *iter )->getUrl() );
         reqLine.setUrl(iterSipUrl.getPtr());
         SipCSeq localseq = agent.getLocalCSeq();
         localseq.incrCSeq();
         agent.setLocalCSeq(localseq);
         inviteMsg->setCSeq(localseq);
         agent.getSipTransceiver()->sendAsync(inviteMsg.getPtr(), "CallAgent::receivedStatus");
      }
      else if (statusMsg->getStatusLine().getStatusCode() == 480) {
         freeMedia();
      }

      //Notify GUI
      facade->postMsg(msg, false);
   }
   catch(CInvalidStateException& e) {
      cpLog(LOG_ERR, "Unexpacted status:%s", e.getDescription().c_str());
   }
}//receivedStatus


int CallAgent::sendCancel() {
   int rv;
   Sptr<SipCommand> sCommand;
   sCommand.dynamicCast(myInvokee->getRequest());
   Sptr<CancelMsg> cMsg = new CancelMsg(*sCommand);
   rv = myInvokee->sendMsg(cMsg.getPtr(), "CA Send cancel");
   facade->postMsg(cMsg.getPtr(), true);
   return rv;
}

void CallAgent::acceptCall() {
   //Can only accept call when I am server
   if (myRole != A_SERVER)
      return;

   try {
      myState->acceptCall(*this);
      Sptr<StatusMsg> statusMsg;
      Sptr<SipCommand> sCommand;
      sCommand.dynamicCast(myInvokee->getRequest());
      assert(sCommand != 0);
      statusMsg = new StatusMsg(*sCommand, 200);

      // Set Contact header
      Sptr< SipUrl > myUrl = new SipUrl("", myInvokee->getMyLocalIp());
      myUrl->setUserValue(UaFacade::getBareUserName());
      myUrl->setHost(myInvokee->getMyLocalIp());
      myUrl->setPort(Data(myInvokee->getMySipPort()));
      SipContact myContact("", myInvokee->getMyLocalIp());
      myContact.setUrl( myUrl.getPtr() );
      statusMsg->setNumContact( 0 );    // Clear old contact
      statusMsg->setContact( myContact );

      SipServer lServer(UaFacade::VOIP_ID_STRING, myInvokee->getMyLocalIp());
      statusMsg->setServer(lServer);

      Sptr<SipSdp> remoteSdp;
      remoteSdp.dynamicCast(sCommand->getContentData(0));
      
      if (remoteSdp == 0) {
         // do something safe
         return;
      }

      Data S_host = myInvokee->getNatHost();

      //Create a session
#ifdef USE_MPEGLIB
      SipSdp localSdp;
      SdpSession localS = MediaController::instance().createSession();
      localSdp.setSdpDescriptor(localS);
      int video  = atoi(UaConfiguration::instance().getValue(VideoTag).c_str());
      //Even if video mode is on, when running in command line mode force the
      //non-video option
      if (video && (!UaCommandLine::instance()->getIntOpt("cmdline"))) {
         //Offer Video also, Would be nice to move to the Libmedia library when the
         //library integrate the MPEG4 codec. For now encoding/decoding happens
         //deep down in MPEG4IP
         Sptr<VideoDevice> vDevice;
         vDevice.dynamicCast(facade->getMediaDevice(getId()));
         assert(vDevice != 0);
         vDevice->setupSession(remoteSdp->getSdpDescriptor(),
                               localSdp.getSdpDescriptor());
      }
#else
      SipSdp localSdp(myInvokee->getMyLocalIp());
      SdpSession localS = MediaController::instance().createSession(remoteSdp->getSdpDescriptor(),
                                                                    "CallAgent::acceptCall");
      setHost(localS, S_host);
      localSdp.setSdpDescriptor(localS);
#endif
      
      statusMsg->setContentData(&localSdp, 0);
      myInvokee->sendMsg(statusMsg.getPtr(), "CA Accept Call");
      facade->postMsg(statusMsg.getPtr(), true);
   }
   catch(CInvalidStateException& e) {
      cpLog(LOG_ERR, "Invalid state transition:%s", e.getDescription().c_str());
   }
}//acceptCall


void CallAgent::stopCall() {
   cpLog(LOG_DEBUG_STACK, "Stopping call, in state: %s\n",
         myState->className().c_str());
   
   while (myState->end(*this) < 0) {  //-> INIT
      // NOTE:  It is REQUIRED that states eventually transition to a state
      // where 'end' cannot fail.
      cpLog(LOG_ERR, "Failed to end in previous state, trying again in state: %s\n",
            myState->className().c_str());
      // Continue
   }

   // Make sure the media is freed.
   freeMedia();
}//stopCall


void CallAgent::setDeleted() {
   myActiveFlg = false;
   cpLog(LOG_DEBUG_STACK, "CallAgent::setDeleted:%d", getId());
   CallDB::instance().removeCallData(*myInvokee);

   UaCallControl::instance().removeAgent(getId());
   myInvokee = NULL; // Have to cut the reference counter loop!
}

int CallAgent::sendBusy() {
   Sptr<SipCommand> sCommand;
   sCommand.dynamicCast(myInvokee->getRequest());
   Sptr<StatusMsg> sMsg = new StatusMsg(*sCommand, 486);
   myInvokee->sendMsg(sMsg.getPtr(), "CA send busy");
   facade->postMsg(sMsg.getPtr(), true);
   myState->cancel(*this);
   
   // Ughhh, kill a recursive loop
   int rv = 0;
   while (1) {
      if (myState == ControlStateFactory::instance().getState(UAS_RINGING)) {
         return -1; // cause it's 'end' calls
      }
      rv = myState->end(*this);  //-> INIT
      if (rv >= 0) {
         break;
      }

      // NOTE:  It is REQUIRED that states eventually transition to a state
      // where 'end' cannot fail.
      cpLog(LOG_ERR, "Failed to end in previous state, trying again in state: %s\n",
            myState->className().c_str());
      // Continue
   }//while
   return rv;
}//sendBusy


void CallAgent::doAuthentication(Sptr<StatusMsg> sMsg) {
   //User has been challenged, send the invite with Authorization header
   Data user = UaConfiguration::instance().getValue(UserNameTag);
   Data password = UaConfiguration::instance().getValue(PasswordTag);
   Sptr<SipCommand> sCommand;
   sCommand.dynamicCast(myInvokee->getRequest());
   SipCSeq cSeq = sCommand->getCSeq();
   cSeq.incrCSeq();
   sCommand->setCSeq(cSeq);

   assert(sCommand != 0);
   authenticateMessage(*sMsg, *sCommand, user, password);
   placeCall();
}

void CallAgent::processHold() {
   cpLog(LOG_DEBUG, "in CallAgent::processHold this should be invoke in UAC Hold");
   Sptr<InviteMsg> inviteMsg;
   inviteMsg.dynamicCast(myInvokee->getRequest());
   SipFrom fom = inviteMsg->getFrom();
   SipContact cont = inviteMsg->getContact(0);
   Sptr<BaseUrl> cUrl = cont.getUrl();
   Sptr<SipUrl> mcUrl;
   mcUrl.dynamicCast(cUrl);
   SipTo to = inviteMsg->getTo();
   cpLog(LOG_DEBUG, "CallAgent::processHold() Contact host is %s",mcUrl->getHost().logData());
   cpLog(LOG_DEBUG, "CallAgent::processHold() System host is %s",
         myInvokee->getMyLocalIp().c_str());
          
   if (mcUrl->getHost() != Data(myInvokee->getMyLocalIp())){
      SipFrom from = inviteMsg->getFrom();
      from.setDisplayName("");
      Sptr<BaseUrl> bUrl = to.getUrl();
      Sptr<SipUrl> mUrl;
      mUrl.dynamicCast(bUrl);
      assert(mUrl != 0);
      cpLog(LOG_DEBUG, "CallAgent::processHold() From url  is %s",
            mUrl->encode().logData());
      from.setUrl(mUrl.getPtr());
      inviteMsg->setFrom(from);
      
      // To
	
      Sptr<BaseUrl> bUrl1 = fom.getUrl();
      to.setUser(fom.getUser());
      to.setDisplayName(fom.getDisplayName());
      Sptr<SipUrl> mUrl1;
      mUrl1.dynamicCast(bUrl1);
      assert(mUrl1 != 0);
      to.setUrl(mUrl1.getPtr());
      inviteMsg->setTo(to);
      cpLog(LOG_DEBUG, "CallAgent::processHold() To  is %s", to.encode().logData());
      cpLog(LOG_DEBUG, "CallAgent::processHold() From is %s", from.encode().logData());
   } else {
      //do nothing since We are UAC before, we don't need to flip 
      // TO & FROM
   }
   //    SipCSeq localseq = myInvokee->getLocalCSeq();
   //    localseq.incrCSeq();
   //    myInvokee->setLocalCSeq(localseq);
   //    inviteMsg->setCSeq(localseq);

   Sptr<SipSdp> sipSdp;
   sipSdp.dynamicCast ( inviteMsg->getContentData( 0 ) );
   assert(sipSdp != 0);

   unsigned int sId = 
      myInvokee->getLocalSdp()->getSdpDescriptor().getSessionId();
   SdpSession localSdp = MediaController::instance().getSdp(sId, VSDP_SND);
   sipSdp->setSdpDescriptor(localSdp);
   
   cpLog(LOG_DEBUG, "CallAgent::processHold() is Sending re-invite %s",
         inviteMsg->encode().logData());
   myInvokee->sendMsg(inviteMsg.getPtr(), "CA process hold"); 
}

void CallAgent::processResume() {
   cpLog(LOG_DEBUG, "CallAgent::processResume() this should be invoke in UAC Resume ");
   Sptr<InviteMsg> inviteMsg;
   inviteMsg.dynamicCast(myInvokee->getRequest());
   //    SipCSeq localseq = myInvokee->getLocalCSeq();
   //    localseq.incrCSeq();
   //    myInvokee->setLocalCSeq(localseq);
   //    inviteMsg->setCSeq(localseq);
   
   Sptr<SipSdp> sipSdp;
   sipSdp.dynamicCast ( inviteMsg->getContentData( 0 ) );
   assert(sipSdp != 0);
   unsigned int sId = 
      myInvokee->getLocalSdp()->getSdpDescriptor().getSessionId();
   SdpSession localSdp = MediaController::instance().getSdp(sId);
   sipSdp->setSdpDescriptor(localSdp);
   
   cpLog(LOG_DEBUG, "Sending InviteMsg %s",inviteMsg->encode().logData() );
   myInvokee->sendMsg(inviteMsg.getPtr(), "process resume");
}//processResume


void CallAgent::doResume(Sptr<SipMsg>& msg) {
   cpLog(LOG_WARNING, "CallAgent::doResume(),  msg is %s", msg->encode().logData());
   Sptr<StatusMsg> smsg;
   smsg.dynamicCast(msg);
   cpLog(LOG_DEBUG, "CallAgent::processHold() is Sending re-invite %s",
         msg->encode().logData());
   Sptr<SipSdp> remoteSdp;
   remoteSdp.dynamicCast(smsg->getContentData(0));
   myInvokee->setRemoteSdp(remoteSdp);
   assert(remoteSdp != 0);
   
   unsigned int sId = myInvokee->getLocalSdp()->getSdpDescriptor().getSessionId();
   int rv;
   rv = MediaController::instance().resumeSession(sId, myInvokee->getLocalSdp()->getSdpDescriptor(),
                                                  remoteSdp->getSdpDescriptor());
   if (rv < 0) {
      cpLog(LOG_ERR, "ERROR:  resumeSession failed: %d,  Stopping call.", rv);
      stopCall();
   }
   else {
      facade->postMsg("INCALL_RESUME");
      myState->inCall(*this);
   }
}//doResume


void CallAgent::dohold() {
   cpLog(LOG_DEBUG, "CallAgent::dohold() this should be invoke in UAC Hold");
   //UAC just stop the Media since recevied 200 OK for RE-INVITE
   unsigned int sId = myInvokee->getLocalSdp()->getSdpDescriptor().getSessionId();
   MediaController::instance().suspendSession(sId);
   myState->inHold(*this);
}

void CallAgent::reqResume(Sptr<SipMsg>& msg) {
   cpLog(LOG_WARNING, "CallAgent::reqResume() this should be invoked in UAS Resume");
   try {
      Sptr<SipCommand> sCommand;
      sCommand.dynamicCast(myInvokee->getRequest());
      assert(sCommand != 0);
      Sptr<SipSdp> remoteSdp;
      remoteSdp.dynamicCast(sCommand->getContentData(0));
      assert(remoteSdp != 0);
      Sptr<StatusMsg> statusMsg = doholdresume200OKstuff(msg,
                                                         remoteSdp->getSdpDescriptor());
      myInvokee->sendMsg(statusMsg.getPtr(), "reqResume");
      int sId = myInvokee->getLocalSdp()->getSdpDescriptor().getSessionId();
      int rv;
      rv = MediaController::instance().resumeSession(sId, myInvokee->getLocalSdp()->getSdpDescriptor(),
                                                     remoteSdp->getSdpDescriptor());
      if (rv < 0) {
         cpLog(LOG_ERR, "ERROR:  resumeSession failed: %d,  Stopping call.", rv);
         stopCall();
      }
      else {
         facade->postMsg("INCALL_RESUME");
         myState->inCall(*this);
      }
   }
   catch (CInvalidStateException& e) {
      cpLog(LOG_ERR, "Invalid state transition:%s", e.getDescription().c_str());
   }
}

void CallAgent::hold(UaBase& agent, const Sptr<SipMsg>& msg) {
   try {
      Sptr<SipCommand> sCommand;
      sCommand.dynamicCast(myInvokee->getRequest());
      assert(sCommand != 0);
      Sptr<SipSdp> remoteSdp;
      remoteSdp.dynamicCast(sCommand->getContentData(0));
      assert(remoteSdp != 0);
      Sptr<StatusMsg> statusMsg = doholdresume200OKstuff(msg,
                                                         remoteSdp->getSdpDescriptor());
      myInvokee->sendMsg(statusMsg.getPtr(), "hold");
      
      unsigned int sId = myInvokee->getLocalSdp()->getSdpDescriptor().getSessionId();	
      MediaController::instance().suspendSession(sId);
      facade->postMsg("R_HOLD");
      facade->postMsg(statusMsg.getPtr(), true);
      myState->inHold(*this);
   }
   catch (CInvalidStateException& e) {
      cpLog(LOG_ERR, "Invalid state transition:%s", e.getDescription().c_str());
   }
}//hold

void CallAgent::startSession(SdpSession& localSdp, SdpSession& remoteSdp) {
   cpLog(LOG_DEBUG, "CallAgent::startSession...\n");
   Sptr<MediaDevice> mDevice;
   unsigned sessionId = localSdp.getSessionId();
   
   MediaController::instance().addToSession(localSdp, remoteSdp);
   
   mDevice = facade->getMediaDevice(getId()); 
   MediaController::instance().addDeviceToSession(sessionId, mDevice);
   
   if (facade->getMode() == CALL_MODE_ANNON) {
      Sptr<FileMediaDevice> mFDevice;
      mFDevice.dynamicCast(mDevice);
      string toUser = myInvokee->getRequest()->getTo().getUser().logData();
      toUser += ".wav";
      mFDevice->setFileToPlay(toUser);
      MediaController::instance().startSession(sessionId, VSDP_SND);
   }
   else if (facade->getMode() == CALL_MODE_VMAIL) {
#ifdef USE_VM
      Data caller, called, sReason;
      
      Sptr<SipMsg> sMsg = myInvokee->getRequest();
      cpLog(LOG_DEBUG, "******* INVITE:%s", sMsg->encode().c_str());
      caller = sMsg->getFrom().getUser();
      if (sMsg->getNumDiversion()) {
         SipDiversion sDiversion = sMsg->getDiversion();
         Sptr<SipUrl> sUrl;
         sUrl.dynamicCast(sDiversion.getUrl());
         if (sUrl != 0) {
            called = sUrl->getUserValue();
         }
         sReason = sDiversion.getReason();
      }
      else {
         called = sMsg->getTo().getUser();
      }
      Sptr<VmcpDevice> vDevice;
      vDevice.dynamicCast(mDevice);
      assert(vDevice != 0);
      vDevice->provideCallInfo(caller, called, sReason);
      MediaController::instance().startSession(sessionId, VSDP_SND_RECV);
#endif
   }
   else {
      MediaController::instance().startSession(sessionId, VSDP_SND_RECV);
   }
}//startSession

