#ifndef REGISTERMSG_H
#define REGISTERMSG_H

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

#include "SipCommand.hxx"

namespace Vocal
{
/**
    Implements REGISTER message.
    <p>
    <b>Example</b>
    <p>
    <pre>
    RegisterMsg registerMsg;

    // Set Request line
    Data reqUrlString;
    SipRequestLine& reqLine = registerMsg.getMutableRequestLine();
    reqUrlString = Data( string("sip:") + toAddress );
    Sptr< SipUrl > reqUrl = new SipUrl( reqUrlString );
    reqLine.setUrl( reqUrl );

    //Set from
    SipFrom sipfrom = registerMsg.getFrom();
    sipfrom.setDisplayName("xyx");
    sipfrom.setUser("xyx");
    sipfrom.setHost(hostAddress);
    sipfrom.setPort(localSipPort );
    registerMsg.setFrom(sipfrom );

    // Set To header
    const Data regToUrlStr = reqUrlString;
    SipUrl regToUrl( regToUrlStr );
    SipTo sipto = registerMsg.getTo();
    sipto.setDisplayName("xyx');
    sipto.setUser("xyx");
    sipto.setHost(regToUrl.getHost() );
    sipto.setPortData(regToUrl.getPort() );
    registerMsg.setTo(sipto );

    // Set Via header
    SipVia sipvia = registerMsg.getVia();
    sipvia.setPort( localSipPort);
    registerMsg.removeVia();
    registerMsg.setVia( sipvia );

    //Add contact
    SipContact myContact;
    Sptr< SipUrl > contactUrl = new SipUrl;
    contactUrl->setUserValue("xyx");
    contactUrl->setPort( localPort );
    myContact.setUrl( contactUrl );
    registerMsg.setNumContact( 0 );
    registerMsg.setContact(myContact);

    SipExpires sipExpires;
    SipExpires.setDelta(3000);
    registerMsg.setExpires( sipExpires );
    //See interface RegistrationManager::addRegistration() for more details
    </pre>
*/
class RegisterMsg : public SipCommand {
    public:
        //
        enum RegisterMsgForward { registerMsgForward };

        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        RegisterMsg(const string& local_ip);

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        RegisterMsg(const Data& data, const string& local_ip);

        ///
        RegisterMsg(const RegisterMsg&);

        ///
        RegisterMsg(const RegisterMsg& msg, enum RegisterMsgForward);
        ///
        bool operator ==(const RegisterMsg& src);  

        ///
        RegisterMsg& operator =(const RegisterMsg&);

        // Help up-cast safely.
        virtual bool isCancelMsg() const { return false; }

        ///
        virtual Method getType() const;

    private:
        void setRegisterDetails();

        RegisterMsg(); //Not Implemented
};
 
} // namespace Vocal

#endif
