
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

static const char* const SipMinSE_cxx_Version =
    "$Id: SipMinSE.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "global.h"
#include "SipMinSE.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipMinSEParserException::getName( void ) const
{
    return "SipMinSEParserException";
}

SipMinSE::SipMinSE( const Data& srcData, const string& local_ip )
    :
      SipHeader(srcData, local_ip),
      delta(),
      bDelta(false)
{
    if (srcData == "") {
        return;
    }
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipMinSEParserException&)
    {
        parseError("Failed to decode SipMinSE in Constructor :(");
    }

}
void SipMinSE::decode(const Data& data)
{

    Data nData = data;

    try
    {
        scanSipMinSE(nData);
    }
    catch (SipMinSEParserException exception)
    {
        parseError("Failed to Decode SipMinSE in decode() " );
    }

}

void
SipMinSE::scanSipMinSE(const Data &tmpdata)

{
    Data newdata = tmpdata ;
    Data newvalue;
    //No need to even check for ;, but since it is most common delimiter
    ///check for it anyways
    int ret = newdata.match(";", &newvalue, true);
    if (ret == NOT_FOUND)
    {
        setDelta(newdata);
        bDelta = true;
        return;
    }
    else if ((ret == FIRST) ||(ret == FOUND))
    {
        parseError("Failed to Decode Min-SE in scanSipMinSE");
    }

}




SipMinSE::SipMinSE( const SipMinSE& src )
    : SipHeader(src),
      delta(src.delta),
      bDelta(src.bDelta)

{}


bool
SipMinSE::operator ==(const SipMinSE& src) const
{
    cpLog(LOG_DEBUG_STACK, "MinSE == Operator ");
    if ( (delta == src.delta) &&
	 (bDelta == src.bDelta)) 
    {
        cpLog(LOG_DEBUG_STACK, "MinSE == Operator  returning True :)");
        return true;
    }
    else
    {
        cpLog(LOG_DEBUG_STACK, "MinSE == Operator  returning false :(");
        return false;
    }
}

const SipMinSE&
SipMinSE::operator=( const SipMinSE& src )
{
    if ( &src != this )
    {
        delta = src.delta;
        bDelta = src.bDelta;
    }
    return *this;
}



void SipMinSE::setDelta ( const Data& data)
{
    delta = data;
    bDelta = true;

    cpLog(LOG_DEBUG_STACK, "set delta in expires");
}

const Data& SipMinSE::getDelta() const
{
    return delta;
}

///
Data SipMinSE::encode() const
{
    Data data;
    if ( (bDelta) )
    {
        data = MIN_SE;
        data += SP;
        data += delta;
	data += CRLF;
    }
    return data;
}


SipHeader*
SipMinSE::duplicate() const
{
    return new SipMinSE(*this);
}


bool
SipMinSE::compareSipHeader(SipHeader* msg) const
{
    SipMinSE* otherMsg = dynamic_cast<SipMinSE*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


void
SipMinSE::parseError(const string& context)
{
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "%s", context.c_str());
            throw SipMinSEParserException(context.c_str(),
                __FILE__,
                __LINE__, DECODE_MIN_SE_FAILED);
        }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
