
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

static const char* const SipOrganization_cxx_Version =
    "$Id: SipOrganization.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";



#include "global.h"
#include "symbols.hxx"
#include "SipOrganization.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipOrganizationParserException::getName( void ) const
{
    return "SipOrganizationParserException";
}

///
SipOrganization::SipOrganization( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }

    Data ndata = srcData;
    try
    {
        decode(ndata);
    }
    catch (SipOrganizationParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipOrganizationParserException("failed in Decode",
                                                 __FILE__,
                                                 __LINE__,
                                                 DECODE_ORGANIZATION_FAILED);
        }
    }
}



///
SipOrganization& SipOrganization::operator = (const SipOrganization& src)
{
    if (&src != this)
    {
        organization = src.organization;
    }
    return (*this);
}
///
bool 
SipOrganization::operator == (const SipOrganization& src) const
{
  return ( organization == src.organization );
}


void SipOrganization::decode(const Data& data)
{

    Data nData = data;

    try
    {
        scanSipOrganization(nData);
    }
    catch (SipOrganizationParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");

            throw SipOrganizationParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_ORGANIZATION_FAILED );
        }
    }
}

void
SipOrganization::scanSipOrganization(const Data &tmpdata)
{
    string newdata = tmpdata.convertString() ;

    set(newdata);
}


///
SipOrganization::SipOrganization( const SipOrganization& src )
    : SipHeader(src),
      organization(src.organization)
{}

///

Data SipOrganization::get() const
{
    return organization;
}

///
void SipOrganization::set( const Data& newdata )
{
    organization = newdata;
}

///
Data SipOrganization::encode() const
{
    Data data;
    if (organization.length())
    {
        data = ORGANIZATION;
        data += SP;
        data += organization;
        data += CRLF;
    }

    return data;
}



Sptr<SipHeader>
SipOrganization::duplicate() const
{
    return new SipOrganization(*this);
}


bool
SipOrganization::compareSipHeader(SipHeader* msg) const
{
    SipOrganization* otherMsg = dynamic_cast<SipOrganization*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
