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
 * IN EXCESS OF ,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const SipTransactionId_cxx_version =
    "$Id: SipTransactionId.cxx,v 1.3 2004/06/02 20:23:10 greear Exp $";

#include "SipTransactionId.hxx"
#include "SipMsg.hxx"
#include "SipTo.hxx"
#include "SipFrom.hxx"
#include "SipCallId.hxx"
#include "SipCSeq.hxx"
#include "SipVia.hxx"
#include "BaseUrl.hxx"

using namespace Vocal;

SipTransactionId :: SipTransactionId(const SipMsg& sipMsg)
{
    valid = true;
    Sptr<BaseUrl> toUrl = sipMsg.getTo().getUrl();
    if(toUrl != 0)
    {
        level1 = toUrl->getUniqueKey();
    }
    else
    {
        level1 = "invalid-url";
        valid = false;
    }

    Sptr<BaseUrl> fromUrl = sipMsg.getFrom().getUrl();
    if(fromUrl != 0)
    {
        level1+= fromUrl->getUniqueKey();
    }
    else
    {
        level1 += "invalid-url";
        valid = false;
    }
    level1+= sipMsg.getCallId().getCallIdData();
    level2 = sipMsg.getCSeq().getCSeqData() + "V"; // add 'V' to end of CSeq number
    level2+= sipMsg.getVia(0).getBranch();
    level3 = sipMsg.getCSeq().getMethod();
}


SipTransactionId :: SipTransactionId(const SipTransactionId& sipTransactionId)
    : valid(sipTransactionId.valid),
      level1(sipTransactionId.level1),
      level2(sipTransactionId.level2),
      level3(sipTransactionId.level3)
{
}


SipTransactionId :: ~SipTransactionId()
{
}


string SipTransactionId::toString() {
    string rv = "SipTransactionID: valid: ";
    rv += valid ? "true" : "false";
    rv += " level1: ";
    rv += level1.c_str();
    rv += " level2: ";
    rv += level2.c_str();
    rv += " level3: ";
    rv += level3.c_str();
    return rv;
}


void SipTransactionId::clear() {
    valid = false;
    level1 = "";
    level2 = "";
    level3 = "";
}


SipTransactionId&
SipTransactionId :: operator= (const SipTransactionId& sipTransactionId)
{
    if (this != &sipTransactionId)
    {
        valid  = sipTransactionId.valid;
	level1 = sipTransactionId.level1;
	level2 = sipTransactionId.level2;
	level3 = sipTransactionId.level3;
    }
    return *this;
}

bool
SipTransactionId::operator==(const SipTransactionId& sipTransactionId) const
{
    return (valid  == sipTransactionId.valid  &&
            level1 == sipTransactionId.level1 &&
            level2 == sipTransactionId.level2 &&
            level3 == sipTransactionId.level3);
}

bool
SipTransactionId::operator<(const SipTransactionId& sipTransactionId) const
{
    if(this == &sipTransactionId)
    {
        return false;
    }
    else
    {
        if (level1 < sipTransactionId.level1)
            return true;
        else if (level1 > sipTransactionId.level1)
            return false;
        else if (level2 < sipTransactionId.level2)
            return true;
        else if (level2 > sipTransactionId.level2)
            return false;
        else if (level3 < sipTransactionId.level3)
            return true;
        else if (level3 > sipTransactionId.level3)
            return false;
        else if (valid  < sipTransactionId.valid)
            return true;
        else
            return false;
    }
}


SipTransactionId::KeyTypeI&
SipTransactionId :: getLevel1() const
{
    return level1;
}


SipTransactionId::KeyTypeII&
SipTransactionId :: getLevel2() const
{
    return level2;
}


SipTransactionId::KeyTypeIII&
SipTransactionId :: getLevel3() const
{
    return level3;
}


bool
SipTransactionId :: getValid() const
{
    return valid;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
