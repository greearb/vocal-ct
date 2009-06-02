
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


#include "global.h"
#include <cassert>

#include "Registration.hxx"
#include "StatusMsg.hxx"
#include "SipContact.hxx"
#include "Authenticate.hxx"
#include "SipRequestLine.hxx"
#include "UaConfiguration.hxx"
#include "UaFacade.hxx"

#define LOG_DBG_RM LOG_DEBUG_STACK

using namespace Vocal;
using namespace Vocal::UA;

Registration::Registration(const string& local_ip)
        :
        status(0),
        seqNum(0)
{
    registerMsg = new RegisterMsg(local_ip);
    nextRegisterMs = 0;
}


Registration::Registration(Sptr<RegisterMsg> srcMsg)
        :
        status(0),
        seqNum(0)
{
    registerMsg = new RegisterMsg(*srcMsg);
    nextRegisterMs = 0;
}


Registration::~Registration()
{
}


Registration::Registration(const Registration& src) {
    registerMsg = new RegisterMsg(*src.registerMsg);
    status = src.status;
    seqNum = src.seqNum;
    nextRegisterMs = 0;
}


Sptr<RegisterMsg>
Registration::getNextRegistrationMsg() {
    seqNum++;
    SipCSeq cseq = registerMsg->getCSeq();
    cseq.setCSeq(seqNum);
    registerMsg->setCSeq(cseq);
    return registerMsg;
}


Sptr<RegisterMsg>
Registration::getNextRegistrationCancel() {
    SipExpires expires("", registerMsg->getLocalIp());

    expires.setDelta(Data("0"));
    registerMsg->setExpires(expires);

    SipCSeq cseq = registerMsg->getCSeq();
    cseq.setCSeq(seqNum);
    registerMsg->setCSeq(cseq);

    return new RegisterMsg(*registerMsg);
}


SipContact
Registration::findMyContact(const StatusMsg& msg) const {
    Sptr< BaseUrl > myContactUrl = registerMsg->getContact(0).getUrl();

//    SipUrl myContactUrl = registerMsg.getContact(0).getUrl();
    SipContact myContact("", registerMsg->getLocalIp());
    int numContacts = msg.getNumContact();
    for ( int i = 0; i < numContacts; i++) {
        if ( msg.getContact(i).getUrl()->areEqual(myContactUrl) ) {
            myContact = msg.getContact(i);
            break;
        }
    }
    return myContact;
}


/* Returns delay in seconds. */
int Registration::updateRegistrationMsg(const StatusMsg& msg) {
    int delay = 0;

    status = msg.getStatusLine().getStatusCode();

    //handling 200 status message
    //for now, just extract expires out of the message
    //may not be complete
    if ( 200 == status ) {
        SipContact myContact = findMyContact(msg);
        Data expires = myContact.getExpires().getDelta();
        if ( expires == "" ) {
            //normally we need to get Delta not Date, but
            //for now the parse is not working correctly
            //so we will use this work around for now and
            //fix it later.
            expires == msg.getExpires().getDelta();
            cpLog(LOG_DBG_RM, "Could not get expires for myContact, will use msg's Expires: %s",
                  expires.c_str());
            if ( expires != "" ) {
                SipExpires sipexpires("", registerMsg->getLocalIp());
                sipexpires.setDelta(expires);
                registerMsg->setExpires(sipexpires);
            }
        }
        else {
           cpLog(LOG_DBG_RM, "Found expires in myContact: %s", expires.c_str());
        }

        if ( expires != "" ) {
            SipExpires sipexpires("", registerMsg->getLocalIp());
            sipexpires.setDelta(expires);
            registerMsg->setExpires(sipexpires);
        }

        delay = getDelayMs() / 1000;

        return delay;
    }

    //handle 3xx and 4xx message

    //first thing to do is to get any potential contacts from these messages
    //for now, we simply use the first one in the list.  Eventually we
    //need to maintain a list of unsuccessfully contacted servers and make
    //sure not to contact them again in order to avoid the loop
    if ( status >= 300 && status <= 500 ) {
        int numContact = msg.getNumContact();

        if ( numContact > 0 ) {
            //later on, we need to pick the sipcontact that has not be
            //contacted before
            SipContact sipcontact = msg.getContact(0);
            // for now, we will just copy mos of the info in contact to
            // request line. Later, we will need to handle each of 3xx and
            // 4xx individually
            SipRequestLine requestLine = registerMsg->getRequestLine();
            requestLine.setUrl(sipcontact.getUrl());
            registerMsg->setRequestLine(requestLine);
        }

        if ( status != 401 && status != 407 ) {
            delay = DEFAULT_DELAY_MS;
            cpLog( LOG_ERR, "Register failed, status: %d", status );
            cpLog( LOG_ERR, "Will try again in %d seconds.", delay );
            cpLog(LOG_ERR, "StatusMsg:\n%s\n", msg.toString().c_str());
        }
    }

    // for 401 message, we need to extract the proxy authentication info and
    // add to the register message
    if ( status == 401 || status == 407 ) {
       Data user = UaFacade::getBareUserName();//UaConfiguration::instance().getValue(UserNameTag);
       Data password = UaConfiguration::instance().getValue(PasswordTag);

       cpLog(LOG_WARNING, "Got registration response code: %d.  Will try to authenticate with user -:%s:- passwd -:%s:-\n",
              status, user.c_str(), password.c_str());
       cpLog(LOG_WARNING, "msg -:\n%s\n", msg.toString().c_str());

       if (!authenticateMessage(msg, *registerMsg, user, password)) {
          // i could not find auth information, so delay
          delay = DEFAULT_DELAY_MS;
       }
       
       //int cseq ( msg.getCSeq().getCSeqData().convertInt() );
       //if ( cseq > 1 ) {   
          // This is not the first 401 that we received (we may
          // have supplied a bad username or password). Set the
          // default delay to allow the user time to figure it
          // out.
          // cpLog(LOG_ERR, "Authentication may have failed, check configuration info");
          // delay = DEFAULT_DELAY_MS;
       //}

       cpLog(LOG_WARNING, "Will try Registration again with authentication information");
    }

    return delay / 1000; // Convert to seconds.
}


void Registration::setRegistrationMsg(Sptr<RegisterMsg> msg) {
   registerMsg = new RegisterMsg(*msg);

   SipCSeq cseq = registerMsg->getCSeq();

   //always use the latest seqNum
   cseq.setCSeq(seqNum);
   registerMsg->setCSeq(cseq);
}

void Registration::setNextRegister(uint64 r) {
   int n = vgetCurMs() / 1000;
   int _r = r/1000;
   cpLog(LOG_ERR, "Setting next register to: %d, now: %d  (%d)\n",
         _r, n, _r - n);
   nextRegisterMs = r;
}


bool Registration::operator == (Registration& right) const {
   // Cast away const until someone fixes RegisterMsg's equality
   // operator.
   //
   Registration &  left = const_cast<Registration &>(*this); 
   return ( *(left.registerMsg) == *(right.registerMsg) );
}


int Registration::getDelayMs() {
   return 1000 * registerMsg->getExpires().getDelta().convertInt();   
}

