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


#include "SipEvent.hxx" 
#include "CallTimerEvent.hxx" 
#include "CallDB.hxx" 
#include "UaCallControl.hxx" 
#include "UaFacade.hxx" 
#include "CallAgent.hxx" 
#include "GuiEvent.hxx" 
#include "SdpHandler.hxx" 
#include "UaConfiguration.hxx" 
#include "SipUrl.hxx" 
#include "TelUrl.hxx" 
#include "CryptoRandom.hxx" 
#include "SipUserAgent.hxx" 
#include "SipTransactionId.hxx" 
#include "SipVia.hxx"
#include "UaHardwareEvent.hxx"
#include "DeviceEvent.hxx"
#include "UaCommandLine.hxx"
#include "MediaController.hxx"
#include "NotifyMsg.hxx"
#include "SubscribeMsg.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SipTextData.hxx"
#include "RandomHex.hxx"
#include "gua.hxx"


#ifdef USE_MPEGLIB
#include "VideoDevice.hxx"
#endif

using namespace Vocal;
using namespace UA;

UaCallControl* UaCallControl::myInstance = 0;

#define NUM_TAG_RANDOMNESS 4

bool 
UaCallControl::processEvent(Sptr<SipProxyEvent> event) {
   cpLog(LOG_DEBUG, "UaCallControl::processEvent, event: %s", event->toString().c_str());
   if (CallControl::processEvent(event)) {
      cpLog(LOG_DEBUG, "Event handled by the base class");

      Sptr<SipEvent> sipEvent;
      sipEvent.dynamicCast(event);

      string m("SIP_PROXY_EVENT: ");
           
      if (sipEvent != 0) {
         Sptr<SipMsg> sipMsg = sipEvent->getSipMsg();
         if (sipMsg != 0) {
            UaFacade::instance().postMsg(sipMsg, false);
            return true;
         }
         else {
            m += event->name();
         }
      }
      else {
         m += event->name();
      }

      UaFacade::instance().postMsg(m);

      //Already handled by the base class, so return
      return true;
   }
   //handle all-kinds of events, from GUI, CLI here

   Sptr<SipEvent> sipEvent;
   sipEvent.dynamicCast(event);
   if (sipEvent != 0) {
      cpLog(LOG_DEBUG, "Handling SIP event");
      //Only Incoming INVITE is accepted, rest is an error
      Sptr<SipMsg> sipMsg = sipEvent->getSipMsg();
      assert(sipMsg != 0);
      if (sipMsg->getType() != SIP_INVITE) {
         UaFacade::instance().postMsg(sipMsg, false);
         if (sipMsg->getType() == SIP_REGISTER) {
            Sptr<SipCommand> sCommand;
            sCommand.dynamicCast(sipMsg);
            Sptr<StatusMsg> sMsg = new StatusMsg(*sCommand, 501);
            UaFacade::instance().getSipTransceiver()->sendReply(sMsg);
            UaFacade::instance().postMsg(sMsg.getPtr(), true);
            cpLog(LOG_DEBUG, "Replying the Above msg (%s)\n",
                  sMsg->encode().logData());
            return true;
         }
         if ((sipMsg->getType() == SIP_STATUS) ) {
            if (sipMsg->getCSeq().getMethod() == REGISTER_METHOD) {
               Sptr<StatusMsg> statusMsg;
               statusMsg.dynamicCast(sipMsg);
               if (statusMsg->getStatusLine().getStatusCode() == 200) {
                  cpLog(LOG_DEBUG, "Expires for statusMsg: %s\n",
                        statusMsg->getExpires().encode().c_str());
                  cpLog(LOG_DEBUG, "Contact for statusMsg: %s\n",
                        statusMsg->getContact().encode().c_str());
                  if ((statusMsg->getExpires().getDelta().convertInt())) {
                     UaFacade::instance().postMsg("REGISTERED ");
                  }
                  else {
                     UaFacade::instance().postMsg("REGISTRATIONEXPIRED ");
                  }
               }
               cpLog(LOG_DEBUG, "Sending registration response to RegManager.\n");
               UaFacade::instance().getRegistrationManager()->handleRegistrationResponse(*statusMsg);
            }
            else if (sipMsg->getCSeq().getMethod() == CANCEL_METHOD) {
               UaFacade::instance().postMsg("L_HANGUP ");
            }
         }
         else if (sipMsg->getType() == SIP_SUBSCRIBE) {
            Sptr<SipCommand> sCommand;
            int _myPort = atoi((UaConfiguration::instance().getValue(LocalSipPortTag)).c_str());
            sCommand.dynamicCast(sipMsg);
            Sptr<StatusMsg> sMsg = new StatusMsg(*sCommand, 200);
            UaFacade::instance().getSipTransceiver()->sendReply(sMsg);
            Sptr<SubscribeMsg> sbMsg;
            sbMsg.dynamicCast(sipMsg);
            SipSubsNotifyEvent ev("Notify", UaConfiguration::instance().getMyLocalIp());
            Sptr<NotifyMsg> nMsg = new NotifyMsg(*sbMsg, ev,
                                                 UaConfiguration::instance().getMyLocalIp());
            SipVia sipVia("", UaConfiguration::instance().getMyLocalIp());
            sipVia.setprotoVersion("2.0");
            sipVia.setHost(Data(UaConfiguration::instance().getMyLocalIp()) );
            sipVia.setPort(_myPort);
            nMsg->flushViaList();
            nMsg->setVia(sipVia);
            if (sbMsg->getNumContact() == 1) {
               SipRoute route("", UaConfiguration::instance().getMyLocalIp());
               route.setUrl(sbMsg->getContact().getUrl());
               nMsg->setRoute(route);
                 
               //remove the contact header.
               nMsg->setNumContact(0);
            }
            SipContact myContact("", UaConfiguration::instance().getMyLocalIp());
            Sptr< SipUrl > contactUrl = new SipUrl("", UaConfiguration::instance().getMyLocalIp());
            contactUrl->setHost( Data( UaConfiguration::instance().getMyLocalIp() ) );
            contactUrl->setPort(Data(_myPort));
            myContact.setUrl( contactUrl.getPtr() );
            nMsg->setContact(myContact);
            SipUserAgent uAgent("Vovida-SIP-SoftPhone/1.5.0 (www.vovida.org)",
                                UaConfiguration::instance().getMyLocalIp());
            nMsg->setUserAgent(uAgent);
            Sptr<SipUrl> tmpinetSipUrl;
            tmpinetSipUrl.dynamicCast(sbMsg->getContact().getUrl());
            assert(tmpinetSipUrl != 0);
            // Create the XML blob as per IETF Draft "A Data format for
            // Presense using XML"
            Data buf;
            buf += "<?xml version=\"1.0\"?>";
            buf += "\r\n";
            buf +="<!DOCTYPE presence";
            buf += "\r\n";
            buf += "PUBLIC \"-//IETF//DTD RFCxxxx XPIDF 1.0//EN\" \"xpidf.dtd\">";
            buf +=  "\r\n";
            buf += "<presence>";
            buf += "\r\n";
            buf += "<presentity uri=\"";
            buf += tmpinetSipUrl->getNameAddr();
            buf += "\" />";
            buf += "\r\n";
            buf +="<atom id=\"";
            buf += RandomHex::get(4);
            buf += "\">";
            buf += "\r\n";
            buf += "<address uri=\"";
            buf +=  contactUrl->getNameAddr();
            buf +=  ";user=ip\" priority=\"0.800000\">";
            buf += "\r\n";
            buf += "<status status=\"open\" />";
            buf += "\r\n";
            buf += "<duplex duplex=\"full\" />";
            buf += "\r\n";
            buf += "</address>";
            buf += "\r\n";
            buf += "</atom>";
            buf += "\r\n";
            buf += "</presence>";
              
            SipTextData Txt(buf,"xpidf+xml");
            nMsg->setContentData(&Txt);

            // Send Notify Message to whoever SUBSCRIBED
            UaFacade::instance().getSipTransceiver()->sendAsync(nMsg.getPtr());
              
         }
         else if (sipMsg->getType() == SIP_BYE) {
            Sptr<SipCommand> sCommand;
            sCommand.dynamicCast(sipMsg);
            Sptr<StatusMsg> sMsg = new StatusMsg(*sCommand, 200);
            UaFacade::instance().getSipTransceiver()->sendReply(sMsg);
            cpLog(LOG_ERR, "Out of dialog message (%s)", sipMsg->encode().logData());
            cpLog(LOG_DEBUG, "Replying the Above msg to stop flodding (%s)\n", sMsg->encode().logData());
         }
         else if ((sipMsg->getType() != SIP_ACK) && (sipMsg->getType() != SIP_SUBSCRIBE)) {
            Sptr<SipCommand> sCommand;
            sCommand.dynamicCast(sipMsg);
            Sptr<StatusMsg> sMsg = new StatusMsg(*sCommand, 501);
            UaFacade::instance().getSipTransceiver()->sendReply(sMsg);
            cpLog(LOG_ERR, "Not Implemented message (%s)", sipMsg->encode().logData());
            cpLog(LOG_DEBUG, "Replying the Above msg (%s)\n", sMsg->encode().logData());
         }
         return true;
      }
      //Send 4xx if busy
      Sptr<SipCommand> sCmd;
      sCmd.dynamicCast(sipMsg);
      if (busy(sCmd)) {
         cpLog(LOG_INFO, "Agent is busy, ignoring the call\n");
         return true;
      }
      Sptr<InviteMsg> inMg;
      inMg.dynamicCast(sipMsg);
      Sptr<SipSdp> remoteSdp;
      remoteSdp.dynamicCast(inMg->getContentData(0));
        
      if (remoteSdp == 0) {
         // no SDP, so abort
         Sptr<SipCommand> Cmd;
         Cmd.dynamicCast(sipMsg);
         Sptr<StatusMsg> sMsg = new StatusMsg(*Cmd, 606);
         UaFacade::instance().getSipTransceiver()->sendReply(sMsg);
         cpLog(LOG_ERR, "No SDP in INVITE message (%s)", 
               sipMsg->encode().logData());
         cpLog(LOG_DEBUG, "Replying the Above msg (%s)\n", 
               sMsg->encode().logData());
         return true;
      }

      if (!(inMg->getContentLength().getLength().convertInt() == 0)
          && (remoteSdp->getRtpPort() == 0)){
         Sptr<SipCommand> Cmd;
         Cmd.dynamicCast(sipMsg);
         Sptr<StatusMsg> sMsg = new StatusMsg(*Cmd, 606);
         UaFacade::instance().getSipTransceiver()->sendReply(sMsg);
         cpLog(LOG_ERR, "No Port in the m line of SDP in INVITE message (%s)", 
               sipMsg->encode().logData());
         cpLog(LOG_DEBUG, "Replying the Above msg (%s)\n", sMsg->encode().logData());
         return true;
      }

      int callId = myCallMap.size();

      //Create the CallAgent to control UaServer
      Sptr<CallAgent> cAgent = new CallAgent(callId, sipMsg, &(UaFacade::instance()),
                                             Vocal::UA::A_SERVER);

      //Persist the agent for the duration of the call
      myCallMap[cAgent->getId()] = cAgent;

      //Post message to the GUI
      strstream s;
      s << "RINGING " << sipMsg->getFrom().getUser().logData() << endl << ends;
      UaFacade::instance().postMsg(s.str());
      s.freeze(false);

      //Send the 180 back to the caller
      Sptr<SipCommand> sCommand;
      sCommand.dynamicCast(sipMsg);
      Sptr<StatusMsg> sMsg = new StatusMsg(*sCommand, 180);
      cAgent->getInvokee()->sendMsg(sMsg.getPtr());
      //UaFacade::instance().getSipTransceiver()->sendReply(sMsg);

      if ((UaFacade::instance().getMode() == CALL_MODE_ANNON) ||
          (UaFacade::instance().getMode() == CALL_MODE_VMAIL) || 
          (UaFacade::instance().getMode() == CALL_MODE_SPEECH)
         ) {
         cAgent->acceptCall();
      }
        
      //UaServer state-machine will take the call from here
      return true;
   }//if was SIP event


   Sptr<GuiEvent> gEvent;
   gEvent.dynamicCast(event);
   if (gEvent != 0) {
      cpLog(LOG_DEBUG, "Got GuiEvent...\n");
      handleGuiEvents(gEvent);
      return true;
   }

   Sptr<UaHardwareEvent> hEvent;
   hEvent.dynamicCast(event);
   if (hEvent != 0) {
      //Get the event id
      cpLog(LOG_ERR, "Got HardwareEvent...\n");
      int id = hEvent->myId;
      Sptr<CallAgent> cAgent;
      if (myCallMap.count(id)) {
         cAgent.dynamicCast(myCallMap[id]);
         if (hEvent->type == HardwareAudioType) {
            if (hEvent->request.type == AudioStop) {
               cAgent->doBye();
               UaFacade::instance().postMsg("AUDIO_STOP");
            }
            else {
               cpLog(LOG_ERR, "ERROR: Un-handled hardware event, type: %d\n",
                     hEvent->request.type);
               assert(0); //TODO:  Remove assert.
            }
         }
         else {
            cpLog(LOG_ERR, "ERROR: Un-handled event type: %d\n",
                  hEvent->type);
            assert(0); //TODO:  Remove assert.
         }
      }
      else {
         cpLog(LOG_ERR, "Did not find agent for id:%d" ,id);
      }
      return true;
   }
    
   Sptr<DeviceEvent> dEvent;
   dEvent.dynamicCast(event);
   if (dEvent != 0) {
      //Get the event id
      int id = dEvent->id;
      Sptr<CallAgent> cAgent;
      if(myCallMap.count(id)) {
         cAgent.dynamicCast(myCallMap[id]);
         if (dEvent->type == DeviceEventHookDown) {
            //VMCP server sent an event, cleanup
            cAgent->doBye();
         }
#if 0
         if (dEvent->type == DeviceEventReferUrl) {
            SipUrl url(dEvent->item);
            cAgent->doBlindXfer(url);
//		cAgent->doBye();
         }
#endif
      }
      else {
         cpLog(LOG_DEBUG, "Did not find agent for id:%d" ,id);
      }
      return true;
   }
    
   return true;
}

void
UaCallControl::handleGuiEvents(Sptr<GuiEvent> gEvent) {
   cpLog(LOG_DEBUG, "Handling GUI event Key (%s), value:(%s)",
         gEvent->getKey().c_str(), gEvent->getValue().c_str());
   switch (gEvent->getType()) {
   case G_ACCEPT: {
      Sptr<CallAgent> cAgent = getActiveCall();
      if (cAgent != 0) {
         cpLog(LOG_DEBUG, "Accept Call:");
         cAgent->acceptCall();
      }
      break;
   }
   case G_STOP: {
      Sptr<CallAgent> cAgent = getActiveCall();
      if (cAgent != 0) {
         cpLog(LOG_DEBUG, "Stopping Call:");
         //BYE or CANCEL
         cAgent->stopCall();
      }
      else {
         cpLog(LOG_ERR, "No active calls found to stop.");
         UaFacade::instance().postMsg("ON_STOP_NO_CALLS_ACTIVE");
      }
      break;
   }
   case G_PURGE: {
      Sptr<CallAgent> cAgent = getActiveCall();
      if (cAgent != 0) {
         cpLog(LOG_ERR, "PURGE:  Forcefully Stopping Call.");
         //BYE or CANCEL
         cAgent->stopCall();
         cAgent->endCall(); //bigger stick
      }
      else {
         cpLog(LOG_ERR, "ERROR: No active calls found to purge.");
         UaFacade::instance().postMsg("ON_PURGE_NO_CALLS_ACTIVE");
      }
      break;
   }
   case G_INVITE: {
      //UaFacade::instance().postMsg("TRYING ");
      //Initiate an Invite to the remote party
      string value = gEvent->getValue();
      initiateInvite(value, "G_INVITE");
      break;
   }
   case G_HOLD: {
      Sptr<CallAgent> cAgent = getActiveCall();
      if (cAgent != 0) {
         cpLog(LOG_DEBUG, "Stopping Call:");
         //BYE or CANCEL
         cAgent->processHold();
      }
      else {
         cpLog(LOG_ERR, "No active calls found to hold.");
      }
      break;
   }
   case G_RESUME: {
      Sptr<CallAgent> cAgent = getActiveCall();
      if (cAgent != 0) {
         cpLog(LOG_DEBUG, "Resuming Call:");
         //BYE or CANCEL
         cAgent->processResume();
      }
      else {
         cpLog(LOG_ERR, "No active calls found to resume.");
      }
      break;
   }
   case G_DOSUBSCRIBE: {
      UaConfiguration::instance().parseConfig();
      if (UaFacade::instance().getRegistrationManager() != NULL) {
         UaFacade::instance().getRegistrationManager()->flushRegistrationList();
         UaFacade::instance().getRegistrationManager()->addRegistration(1);
      }
      break;
   }
   case G_REGISTRATIONEXPIRED: {
      //Preferences changed, do the regsitration
      //Parse the configuration data
      UaConfiguration::instance().parseConfig();
      if (UaFacade::instance().getRegistrationManager() != NULL) {
         UaFacade::instance().getRegistrationManager()->flushRegistrationList();
         UaFacade::instance().getRegistrationManager()->addRegistration(1);
      }
      break;
   }
   case G_PREF: {
      //Preferences changed, do the regsitration
      //Parse the configuration data
      //Save the OLD value of SIP port, since SIP port can not
      //be modified from the config file, however parseConfig
      //will try to set it to the default. So set the port back to
      //saved port
      int sPort = atoi(UaConfiguration::instance().getValue(LocalSipPortTag).c_str());
      UaConfiguration::instance().parseConfig();
      //set the proxy server
      string pxServer = UaConfiguration::instance().getValue(ProxyServerTag);
      UaFacade::instance().getProxyAddr()->setHostName(pxServer);
      string natIp = UaConfiguration::instance().getValue(NATAddressIPTag);
      //set the NATIP address
      UaFacade::instance().setNatHost(natIp);
      
      UaConfiguration::instance().setValue(LocalSipPortTag, Data(sPort).c_str());
      if (UaFacade::instance().getRegistrationManager() != 0) {
         UaFacade::instance().getRegistrationManager()->flushRegistrationList();
         UaFacade::instance().getRegistrationManager()->addRegistration(1);
      }
      break;
   }
   case G_SHUTDOWN: {
      gua_running = false;
      break;
   }
   default:
      cpLog(LOG_DEBUG, "TODO...");
      break;
   }//switch
}//handleGuiEvents


UaCallControl& UaCallControl::instance() {
   if (myInstance == 0) {
      myInstance = new UaCallControl();
   }
   return *myInstance;
}

void UaCallControl::destroy() {
   cpLog(LOG_DEBUG, "UaCallControl::destroy");
   delete myInstance;
   myInstance = 0;
}

void UaCallControl::initiateInvite(const string& to, const char* debug) {
   //Parse the to string
   //1000-> sip:1000@proxy.com;user=phone
   //xvz->  sip:xyz@proxy.com;
   //xyz@domain.com -> sip:xyz@<domain SIP server IP>
   //xyz@fqdn  ->      sip:xyz@fqdn

   // First, make sure that we are not already in call.
   if (myCallMap.size()) {
      cpLog(LOG_ERR, "ERROR:  myCallMap has size: %d, should be empty, debug: %s\n",
            myCallMap.size(), debug);
      return;
   }
   else {
      cpLog(LOG_DEBUG_STACK, "initiateInvite, debug: %s\n", debug);
   }

   //Case 1: sip:user@...
   string::size_type pos = to.find_first_of(":@");
   Sptr<BaseUrl> toUrl; 
   if (pos != string::npos) {
      toUrl = parseUrl(to);
      if (toUrl == 0) {
         //Case 2
         //user@xyz.com
         //Assume it to be a SIP url
         string mTo("sip:");
         mTo += to;
         toUrl = parseUrl(mTo);
         if (toUrl == 0) {
            //Give-up, url is not the right type
            UaFacade::instance().postMsg("ERROR Invalid URL");
            return;
         }
      }
   }
   else {
      //Only user part
      toUrl = new SipUrl("", UaConfiguration::instance().getMyLocalIp());
      Sptr<SipUrl> sUrl;
      sUrl.dynamicCast(toUrl);
      sUrl->setUserValue(to);
      sUrl->setHost(UaConfiguration::instance().getValue(ProxyServerTag));
   }
   
   int sipPorti = atoi(UaConfiguration::instance().getValue(LocalSipPortTag).c_str());
   string rtpPort =  UaConfiguration::instance().getValue(MinRtpPortTag);
   
   Sptr<InviteMsg> msg = new InviteMsg( toUrl, UaConfiguration::instance().getMyLocalIp(),
                                        sipPorti ,atoi(rtpPort.c_str()) );

   Sptr<SipFrom> fromUrl = UaFacade::generateSipFrom();

   fromUrl->setDisplayName(UaConfiguration::instance().getValue(DisplayNameTag));
   
   
   CryptoRandom random;
   unsigned char tempTag[NUM_TAG_RANDOMNESS];
   int len = random.getRandom(tempTag, NUM_TAG_RANDOMNESS);
   Data fromTag;
   if (len > 0) {
      fromTag = convertToHex(tempTag, NUM_TAG_RANDOMNESS);
   }
   fromUrl->setTag(fromTag);
   msg->setFrom(*(fromUrl.getPtr()));
   
   
   // Set transport in Via:
   SipVia via = msg->getVia();
   msg->removeVia();
   via.setTransport( UaConfiguration::instance().getValue(SipTransportTag) );
   Data viaBranch("z9hG4bK");
   viaBranch += msg->computeBranch2(sipPorti);
   via.setBranch(viaBranch);
   msg->setVia( via );
   
   SipUserAgent uAgent(UaFacade::VOIP_ID_STRING,
                       UaConfiguration::instance().getMyLocalIp());
   msg->setUserAgent(uAgent);
   

   // Set Contact header
   Sptr< SipUrl > myUrl = new SipUrl("", UaConfiguration::instance().getMyLocalIp());
   myUrl->setUserValue(UaFacade::getBareUserName());
   SipContact myContact("", UaConfiguration::instance().getMyLocalIp());
   if ( UaConfiguration::instance().getValue(NATAddressIPTag).length()) {
      myUrl->setHost( Data(UaConfiguration::instance().getValue(NATAddressIPTag)));
   } else {
      myUrl->setHost( Data( UaConfiguration::instance().getMyLocalIp() ) );
   }
   myUrl->setPort( UaConfiguration::instance().getValue(LocalSipPortTag) );
   myContact.setUrl( myUrl.getPtr() );
   msg->setNumContact( 0 );    // Clear old contact
   msg->setContact( myContact );


   SipRequestLine& reqLine = msg->getMutableRequestLine();
   reqLine.setTransportParam(UaConfiguration::instance().getValue(SipTransportTag));
   
   Sptr<SipSdp> sipSdp;
   sipSdp.dynamicCast ( msg->getContentData( 0 ) );
   assert(sipSdp != 0);
   Data NAT_HOST = UaConfiguration::instance().getValue(NATAddressIPTag);
#ifdef USE_MPEGLIB
   int video  = atoi(UaConfiguration::instance().getValue(VideoTag).c_str()); 
   //Even if video mode is on, when running in command line mode force the
   //non-video option
   if (video && (!UaCommandLine::instance()->getIntOpt("cmdline"))) {
      Data sdpData;
      Sptr<VideoDevice> vDevice;
      vDevice.dynamicCast(UaFacade::instance().peekMediaDevice());
      assert(vDevice != 0);
      SdpSession& localSdp = sipSdp->getSdpDescriptor();
      localSdp.flushMediaList();
      vDevice->generateSdpData(localSdp);
   }
   else {
      SdpSession localSdp = MediaController::instance().createSession();
      if (NAT_HOST.length()) {
         setHost(localSdp, NAT_HOST);
      }
      sipSdp->setSdpDescriptor(localSdp);
   }
#else
   SdpSession localSdp = MediaController::instance().createSession("UaCallControll::initiateInvite");
   if (NAT_HOST.length()) {
      setHost(localSdp, NAT_HOST);
   }
   sipSdp->setSdpDescriptor(localSdp);
#endif
   
   int callId = myCallMap.size();
   
   //Create call-agent to handle the call from now on
   Sptr<CallAgent> cAgent = new CallAgent(callId, msg.getPtr(), &(UaFacade::instance()),
                                          Vocal::UA::A_CLIENT);
   
   //Persist the agent for the duration of the call
   myCallMap[cAgent->getId()] = cAgent;
   //UaClient state-machine will take the call from here
}//initiateInvite


bool
UaCallControl::busy(Sptr<SipCommand> sipMsg)
{
    Sptr<CallAgent> cAgent = getActiveCall(sipMsg.getPtr());
    if(cAgent != 0)
    {
        cpLog(LOG_DEBUG, "A call is already active");
        Sptr<StatusMsg> statusMsg = new StatusMsg(*sipMsg, 486);
        UaFacade::instance().getSipTransceiver()->sendReply(statusMsg);
        return true;
    }
    return false;
}

Sptr<CallAgent>
UaCallControl::getActiveCall(Sptr<SipMsg> sipMsg) {
   cpLog(LOG_DEBUG, "UaCallControl::getActiveCall");
   if (myCallMap.size()) {
      cpLog(LOG_DEBUG, "UaCallControl::getActiveCall");
      //Get the call from the call-map
      for (CallMap::iterator itr = myCallMap.begin();
          itr != myCallMap.end(); itr++) {
            
         Sptr<CallAgent> cAgent;
         cAgent.dynamicCast((*itr).second);
         if (cAgent->isActive()) {
            CallControlMode cMode = UaFacade::instance().getMode(); 
            if ((sipMsg != 0) && 
                (( cMode == CALL_MODE_ANNON) ||
                 ( cMode == CALL_MODE_VMAIL) || 
                 ( cMode == CALL_MODE_SPEECH))) {
               //Get the callId 
               SipTransactionId id(*sipMsg);
               SipTransactionId id2(*(cAgent->getInvokee()->getRequest()));
               if (id == id2) {
                  //Same request coming again
                  //do Nothing
                  return 0;
               }
               else {
                  cAgent = 0;
                  continue;
               }
            }
            return cAgent;
         }
      }
   }
   return 0;
}

Sptr<BaseUrl> UaCallControl::parseUrl(const string& to) {
   cpLog(LOG_DEBUG, "Parsing URL:%s", to.c_str());
   string::size_type pos = to.find_first_of(":");
   Sptr<BaseUrl> toUrl;
   try {
      //First try to parse Url
      //if it is not a valid URL it will throw an
      //exception
      SipParserMode::instance().setStrictMode(true);
      string left = to.substr(0, pos);
      if (left == "sip") {
         cpLog(LOG_DEBUG, "SIP_URL:%s", to.c_str());
         toUrl = new SipUrl(to, UaConfiguration::instance().getMyLocalIp());
      }
      else if(left == "tel") {
         cpLog(LOG_DEBUG, "TEL_URL:%s", to.c_str());
         toUrl = new TelUrl(to);
      }
      else {
         cpLog(LOG_DEBUG, "UNKNOWN URL:%s", to.c_str());
         SipParserMode::instance().setStrictMode(false);
         return 0;
      }
      SipParserMode::instance().setStrictMode(false);
   }
   catch (VException& e) {
      cpLog(LOG_DEBUG, "UNKNOWN URL:%s", to.c_str());
      return 0;
   }
   return toUrl;
}//parseUrl



