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


static const char* const SipCallLegData_cxx_Version =
    "$Id: SipCallLegData.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include "ContactData.hxx" 
#include "SipCallLegData.hxx" 
#include "cpLog.h" 

using namespace Vocal;
using namespace Vocal::UA;

SipCallLegData::SipCallLegData( const Sptr<SipMsg>& reqMsg )
          : myRequestMsg(reqMsg),
            myCallLegState(CLS_NONE)
{
    mySipCallLeg = new SipCallLeg(reqMsg->getFrom(), reqMsg->getTo(),
                                  reqMsg->getCallId(), reqMsg->getLocalIp());
    myLocalSdpData.dynamicCast(reqMsg->getContentData(0));
};


Sptr<ContactData>
SipCallLegData::getContactData(int index) const
{
    assert(index < static_cast<int>(myContactVector.size()));
    unsigned int sz = myContactVector.size();
    if(sz == 0) return 0;;
    if(index == -1)
    {
        //return the entry from the bottom of the list
        return (myContactVector[sz-1]);
    }
    return (myContactVector[index]);
}

void 
SipCallLegData::addContact(const Sptr<ContactData>& contact, bool top)
{
    if(top) myContactVector.insert(myContactVector.begin(), contact);
    else myContactVector.insert(myContactVector.end(), contact);
}

void
SipCallLegData::pushContact(const Sptr<ContactData>& contact)
{
    addContact(contact, false);
}

Sptr<ContactData> 
SipCallLegData::popContact()
{
    Sptr<ContactData> retVal;
    if(myContactVector.size())
    {
        retVal = myContactVector.back();
        myContactVector.pop_back();
    }
    return retVal;
}



void 
SipCallLegData::setRequest(const Sptr<SipMsg>& sipMsg) 
{ 
    assert(sipMsg->getType() == SIP_INVITE);
    myRequestMsg = sipMsg; 
};

void 
SipCallLegData::setResponse(const Sptr<SipMsg>& sipMsg) 
{ 
    assert(sipMsg->getType() == SIP_STATUS);
    myResponseMsg = sipMsg; 
};

void 
SipCallLegData::setAck(const Sptr<SipMsg>& sipMsg) 
{ 
    assert(sipMsg->getType() == SIP_ACK);
    myAckMsg = sipMsg; 
};
