
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

#define LOG_DBG_RM LOG_DEBUG_STACK

#include "SipVia.hxx"
#include "SystemInfo.hxx"
#include "RegistrationManager.hxx"
#include "UaConfiguration.hxx"
#include "UaFacade.hxx"

using namespace Vocal;
using namespace Vocal::UA;

///
RegistrationManager::RegistrationManager( Sptr < SipTransceiver > sipstack ) {
   sipStack = sipstack;
   
   cpLog(LOG_DBG_RM, "Starting Registration Mananger");
   
   addRegistration(0);
}

///
RegistrationManager::~RegistrationManager() {
   flushRegistrationList();
}


void RegistrationManager::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                               uint64 now) {
   RegistrationList::iterator i;
   for (i = registrationList.begin(); i != registrationList.end(); i++) {
      Sptr<Registration> r = *i;
      if (r->getNextRegister() <= now) {
         doRegistration(r, now);
      }
   }
}

int RegistrationManager::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                                int& maxdesc, uint64& timeout, uint64 now) {
   RegistrationList::iterator i;
   for (i = registrationList.begin(); i != registrationList.end(); i++) {
      Sptr<Registration> r = *i;
      uint64 nextRegister = r->getNextRegister();
      if (nextRegister < now + timeout) {
         if (nextRegister <= now) {
            timeout = 0;
         }
         else {
            timeout = nextRegister - now;
         }
      }
   }
   return 0;
}


int RegistrationManager::doRegistration(Sptr<Registration> registration, uint64 now) {

   Sptr<RegisterMsg> registerMsg = registration->getNextRegistrationMsg();

   if (sipStack != 0) {
      cpLog(LOG_DBG_RM, "sending register message");
      sipStack->sendAsync( registerMsg.getPtr(), "doRegistration" );
      UaFacade::instance().postMsg(registerMsg.getPtr(), true, "doRegistration");
   }

   registration->setNextRegister(now + registration->getDelayMs());
   return 0;
}

///
Sptr<Registration>
RegistrationManager::findRegistration(const StatusMsg& statusMsg) {
   Sptr<Registration> registration;

   RegistrationList::iterator iter = registrationList.begin();
   while ( iter != registrationList.end() ) {
      Sptr<RegisterMsg> regMsg = (*iter)->getRegistrationMsg();
      if ( regMsg->computeCallLeg() == statusMsg.computeCallLeg() ) {
         cpLog(LOG_DBG_RM, "Found registration...\n");
         registration = (*iter);
         break;
      }
      else {
         cpLog(LOG_DBG_RM, "regMsg callLeg: %s  statusMsg callLeg: %s  didn't match.\n",
               regMsg->computeCallLeg().toString().c_str(),
               statusMsg.computeCallLeg().toString().c_str());
         // So, match on Call-ID instead.  But this will be a backup.
         if (regMsg->getCallId() == statusMsg.getCallId()) {
            registration = *iter;
            cpLog(LOG_DEBUG, "Call-IDs matched, will use this for backup.");
            // Continue, will prefer the original matching logic, just in case that
            // is a good idea! --Ben
         }
         else {
            cpLog(LOG_DBG_RM, "regMsg callId: %s  statusMsg callId: %s  didn't match.\n",
                  regMsg->getCallId().toString().c_str(),
                  statusMsg.getCallId().toString().c_str());
         }
      }
      iter++;
   }//while

   return registration;
}//findRegistration

///
void RegistrationManager::addRegistration(Sptr<Registration> item) {
   registrationList.push_back(item);
}

///
void
RegistrationManager::flushRegistrationList() {
    registrationList.clear();
}

///
bool RegistrationManager::handleRegistrationResponse(const StatusMsg& statusMsg) {

   Sptr<Registration> registration = findRegistration(statusMsg);

   if (registration == 0) {
      cpLog(LOG_DBG_RM, "Could not find registration for statusMsg\n");
      return false;
   }

   // Delay in seconds.
   int delay = registration->updateRegistrationMsg(statusMsg);
   cpLog(LOG_ERR, "RegistrationManager::updating registration information, delay: %d, statusCode: %d",
         delay, registration->getStatusCode());

   if ( registration->getStatusCode() == 100 ) {
      delay = DEFAULT_DELAY_MS / 1000;
   }

   if ((registration->getStatusCode()  == 401) ||
       (registration->getStatusCode()  == 407)) {
      cpLog(LOG_DBG_RM, "The new delay is %d, status code: %d",
            delay, registration->getStatusCode());
      registration->setNextRegister(vgetCurMs() + (delay * 1000));
   }
   else if (delay) {
      registration->setNextRegister(vgetCurMs() + (delay * 1000));
   }
   else {
      // Register again in a few minutes.
      registration->setNextRegister(vgetCurMs() + DEFAULT_DELAY_MS);
   }
   return true;
}//handleRegistrationResponse


void RegistrationManager::addRegistration(int check) {
   flushRegistrationList();
   UaConfiguration& config = UaConfiguration::instance();

   string regOn = config.getValue(RegisterOnTag);
   if (strcasecmp(regOn.c_str(), "FALSE") == 0) {
      cpLog(LOG_DBG_RM, "Registration is turned off");
      return;
   }

   string toAddress = config.getValue(RegisterToTag);
   if (toAddress.length() == 0) {
      cpLog(LOG_ERR, "No registration server found, using Proxy server as registrar");
      //Use Proxy Address
      toAddress = config.getValue(ProxyServerTag);
      if (toAddress.length() == 0) {
         cpLog(LOG_ERR, "No Proxy server found, giving up registration.");
         config.show();
         return;
      }
   }

   NetworkAddress rs(toAddress);

   // The first REGISTER message
   Sptr<RegisterMsg> registerMsg = new RegisterMsg(config.getMyLocalIp());

   // Set Request line
   Data reqUrlString;
   SipRequestLine& reqLine = registerMsg->getMutableRequestLine();
   reqUrlString = Data( string("sip:") + toAddress );
   
   if (strcasecmp(config.getValue(SipTransportTag).c_str(), "TCP") == 0) {
      reqUrlString += ";transport=tcp;";
   }
   Sptr< SipUrl > reqUrl = new SipUrl( reqUrlString, config.getMyLocalIp() );

   reqLine.setUrl( reqUrl.getPtr());

   // Set From header
   string port = config.getValue(LocalSipPortTag);
    
   SipFrom sipfrom = registerMsg->getFrom();
   Sptr<SipFrom> sfp = UaFacade::generateSipFrom();
   sipfrom.setDisplayName( config.getValue(DisplayNameTag) );
   sipfrom.setUser(sfp->getUser());
   sipfrom.setHost(sfp->getHost());
   sipfrom.setPort(sfp->getPort());
   registerMsg->setFrom( sipfrom );

   // Set To header
   const Data regToUrlStr = reqUrlString;
   SipUrl regToUrl( regToUrlStr, config.getMyLocalIp() );
   SipTo sipto = registerMsg->getTo();
   sipto.setDisplayName( config.getValue(DisplayNameTag) );
   sipto.setUser(sipfrom.getUser());
   sipto.setHost( regToUrl.getHost() );
   sipto.setPortData( regToUrl.getPort() );
   registerMsg->setTo( sipto );

   // Set Via header
   SipVia sipvia = registerMsg->getVia();
   sipvia.setPort(atoi(port.c_str()));
   sipvia.setTransport(config.getValue(SipTransportTag));

   registerMsg->removeVia();
   registerMsg->setVia( sipvia );

   // Set Contact header
   Sptr< SipUrl > contactUrl = new SipUrl("", config.getMyLocalIp());
   SipContact myContact("", config.getMyLocalIp());
   if (!check) {
      contactUrl->setUserValue(UaFacade::getBareUserName(), "phone" );
      if ( UaConfiguration::instance().getValue(NATAddressIPTag).length()) {
         contactUrl->setHost( Data(UaConfiguration::instance().getValue(NATAddressIPTag)));
      }
      else {
         contactUrl->setHost( Data( UaConfiguration::instance().getMyLocalIp() ) );
      }

      contactUrl->setPort( port );
      if (strcasecmp(config.getValue(SipTransportTag).c_str(), "TCP") == 0) {
         contactUrl->setTransportParam( Data("tcp") );
      } 
    
      myContact.setUrl( contactUrl.getPtr() );
   }
   else {
      myContact.setNullContact();
   }
   registerMsg->setNumContact( 0 );
   registerMsg->setContact(myContact);

   // Set Expires header
   SipExpires sipExpires("", config.getMyLocalIp());
   if (!check){
      sipExpires.setDelta( config.getValue(RegisterExpiresTag) );
   } else {
      sipExpires.setDelta(0);
   }
   registerMsg->setExpires( sipExpires );

   Sptr<Registration> registration = new Registration(registerMsg);
   addRegistration( registration );
}
