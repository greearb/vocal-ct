
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

static const char* const RegistrationManager_cxx_Version =
    "$Id: RegistrationManager.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "SipVia.hxx"
#include "SystemInfo.hxx"
#include "RegistrationManager.hxx"
#include "UaConfiguration.hxx"
#include "UaFacade.hxx"
#include "Lock.hxx"
#include "NetworkConfig.hxx"

using namespace Vocal;
using namespace Vocal::UA;
using Vocal::Threads::Lock;

///
RegistrationManager::RegistrationManager( Sptr < SipTransceiver > sipstack )
{
    sipStack = sipstack;
    shutdown = false;

    cpLog(LOG_DEBUG, "Starting Registration Mananger");
    //start the registration thread
    registrationThread.spawn(registrationThreadWrapper, this);
    cpLog( LOG_DEBUG, "Spawning the registration thread");

    addRegistration();
}

///
RegistrationManager::~RegistrationManager()
{
    shutdown = true;
    registrationFifo.add(0);
    registrationThread.join();
    flushRegistrationList();
}

///
void*
RegistrationManager::registrationThreadWrapper(void *regSession)
{
    assert( regSession );

    RegistrationManager* regManager = static_cast < RegistrationManager* > (regSession);
    regManager->registrationMain();
    return 0;
}

///
void
RegistrationManager::registrationMain()
{
    cpLog(LOG_DEBUG, "RegistrationManager::registrationMain: starting registrationMain");

    while (1)
    {
        cpLog(LOG_DEBUG, "RegistrationManager::registrationMain: getting next registration event");

        Registration* registration = registrationFifo.getNext();
        if(shutdown) break;
        if ( 0 == registration )
            continue;

        Lock lock(registrationMutex);
        RegisterMsg registerMsg = registration->getNextRegistrationMsg();

        if ( 0 != sipStack )
        {
            cpLog(LOG_DEBUG, "sending register message");
            sipStack->sendAsync( registerMsg );
            Sptr<RegisterMsg> rMsg = new RegisterMsg(registerMsg);
            UaFacade::instance().postInfo(rMsg);
        }

        //add another to the fifo in the event that response never came
        int delay = registration->getDelay();
	if(delay){
	    FifoEventId eventId = registrationFifo.addDelayMs(registration, delay);
	    registration->setTimerId(eventId);
	}
    }
}

///
Registration*
RegistrationManager::findRegistration(const StatusMsg& statusMsg)
{
    Registration* registration = 0;

    RegistrationList::iterator iter = registrationList.begin();
    while ( iter != registrationList.end() )
    {
        RegisterMsg regMsg = (*iter)->getRegistrationMsg();
        if ( regMsg.computeCallLeg() == statusMsg.computeCallLeg() )
        {
            registration = (*iter);
            break;
        }
        iter++;
    }

    return registration;
}

///
void
RegistrationManager::addRegistration(const Registration& item)
{
    Registration* registration = new Registration(item);

    if ( 0 != registration )
        registrationList.push_back(registration);
}

///
void
RegistrationManager::flushRegistrationList()
{
    RegistrationList::iterator iter = registrationList.begin();
    while ( iter != registrationList.end() )
    {
        if ( *iter )
        {
            registrationFifo.cancel((*iter)->getTimerId());
            delete (*iter);
        }
        iter++;
    }
    registrationList.clear();
}

///
bool
RegistrationManager::handleRegistrationResponse(const StatusMsg& statusMsg)
{
    Lock lock(registrationMutex);

    Registration* registration = findRegistration(statusMsg);

    if ( !registration )
        return false;


    bool ret = true;

    cpLog(LOG_DEBUG, "RegistrationManager::handling response to a register message");
    //Cancel the timer
    registrationFifo.cancel(registration->getTimerId());

    cpLog(LOG_DEBUG, "RegistrationManager::updating registration information");
    int delay = registration->updateRegistrationMsg(statusMsg);

    if( registration->getStatusCode() == 100 )
    {
	const int DEFAULT_DELAY = 60000;   // 60 sec.
	delay = DEFAULT_DELAY;
    }

    if((registration->getStatusCode()  == 401) ||
      (registration->getStatusCode()  == 407))
    {
	delay = 0;
        cpLog(LOG_DEBUG, "The new delay is %d", delay);
	FifoEventId eventId = registrationFifo.addDelayMs(registration, delay);
	registration->setTimerId(eventId);
    }
    else if(delay){
	FifoEventId eventId = registrationFifo.addDelayMs(registration, delay);
	registration->setTimerId(eventId);
    } else {
	ret = true;
    }
    
    return ret;
}

///
void
RegistrationManager::startRegistration()
{
    Lock lock(registrationMutex);
    RegistrationList::iterator iter = registrationList.begin();

    while ( iter != registrationList.end() )
    {
        FifoEventId eventId = registrationFifo.addDelayMs((*iter), 0);
        (*iter)->setTimerId(eventId);
        iter++;
    }
}

void
RegistrationManager::addRegistration(int check)
{
    Lock lock(registrationMutex);
    flushRegistrationList();
    UaConfiguration& config = UaConfiguration::instance();

    string regOn = config.getValue(RegisterOnTag);
    if((regOn == "false") || (regOn == "False"))
    {
        cpLog(LOG_INFO, "Registration is turned off");
        return;
    }

    string toAddress = config.getValue(RegisterToTag);
    if(toAddress.length() == 0)
    {
        cpLog(LOG_ERR, "No registration server found, using Proxy server as registrar");
        //Use Proxy Address
        toAddress = config.getValue(ProxyServerTag);
        if(toAddress.length() == 0)
        {
            cpLog(LOG_ERR, "No Proxy server found, giving up registration.");
            config.show();
            return;
        }
    }

    NetworkAddress rs(toAddress);

    // The first REGISTER message
    RegisterMsg registerMsg(config.getMyLocalIp());

    // Set Request line
    Data reqUrlString;
    SipRequestLine& reqLine = registerMsg.getMutableRequestLine();
    if(NetworkConfig::instance().isDualStack() && 
       NetworkAddress::is_valid_ip6_addr(toAddress))
    {
        reqUrlString = Data( string("sip:[") + toAddress + "]");
    }
    else
    {
        reqUrlString = Data( string("sip:") + toAddress );
    }
    if(config.getValue(SipTransportTag) == "TCP")
    {
        reqUrlString += ";transport=tcp;";
    }
    Sptr< SipUrl > reqUrl = new SipUrl( reqUrlString, config.getMyLocalIp() );

    assert( reqUrl != 0 );
    reqLine.setUrl( reqUrl );

    // Set From header
    string port = config.getValue(LocalSipPortTag);
    
    SipFrom sipfrom = registerMsg.getFrom();
    sipfrom.setDisplayName( config.getValue(DisplayNameTag) );
    sipfrom.setUser( config.getValue(UserNameTag) );
    sipfrom.setHost( config.getMyLocalIp() );
    sipfrom.setPort( port );
    registerMsg.setFrom( sipfrom );

    // Set To header
    const Data regToUrlStr = reqUrlString;
    SipUrl regToUrl( regToUrlStr, config.getMyLocalIp() );
    SipTo sipto = registerMsg.getTo();
    sipto.setDisplayName( config.getValue(DisplayNameTag) );
    sipto.setUser( config.getValue(UserNameTag) );
    sipto.setHost( regToUrl.getHost() );
    sipto.setPortData( regToUrl.getPort() );
    registerMsg.setTo( sipto );

    // Set Via header
    SipVia sipvia = registerMsg.getVia();
    sipvia.setPort( Data(port).convertInt() );
    sipvia.setTransport(  Data(config.getValue(SipTransportTag)) );

    registerMsg.removeVia();
    registerMsg.setVia( sipvia );

    // Set Contact header
    Sptr< SipUrl > contactUrl = new SipUrl("", config.getMyLocalIp());
    SipContact myContact("", config.getMyLocalIp());
    if(!check)
    {
	contactUrl->setUserValue( config.getValue(UserNameTag), "phone" );
        if( UaConfiguration::instance().getValue(NATAddressIPTag).length())
        {
           contactUrl->setHost( Data(UaConfiguration::instance().getValue(NATAddressIPTag)));

        } else
        {
           contactUrl->setHost( Data( UaConfiguration::instance().getMyLocalIp() ) );
        }

	contactUrl->setPort( port );
	if(config.getValue(SipTransportTag) == "TCP")
	{
	    contactUrl->setTransportParam( Data("tcp") );
	} 
    
	myContact.setUrl( contactUrl );
    } else {
	myContact.setNullContact();
    }
    registerMsg.setNumContact( 0 );
    registerMsg.setContact(myContact);

    // Set Expires header
    SipExpires sipExpires("", config.getMyLocalIp());
    if(!check){
	sipExpires.setDelta( config.getValue(RegisterExpiresTag) );
    } else {
	sipExpires.setDelta(0);
    }
    registerMsg.setExpires( sipExpires );

    Registration registration( registerMsg );
    addRegistration( registration );
}
