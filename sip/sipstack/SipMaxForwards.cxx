
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

static const char* const SipMaxForwards_cxx_Version =
    "$Id: SipMaxForwards.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "global.h"
#include "symbols.hxx"
#include "SipMaxForwards.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipMaxForwardsParserException::getName( void ) const
{
    return "SipMaxForwardsParserException";
}

SipMaxForwards::SipMaxForwards( const Data& data, const string& local_ip )
    : SipHeader(local_ip),
      value(70)
{
    if (data == "") {
        return;
    }
    try
    {
        decode(data);
    }
    catch (SipMaxForwardsParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode  SipMaxForwards  : ( ");
            throw SipMaxForwardsParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_MAXFORWARDS_FAILED
            );
        }

    }

}

const SipMaxForwards&
SipMaxForwards::operator=(const SipMaxForwards& src)
{
    if (&src != this)
    {
        value = src.value;
    }
    return (*this);
}

bool
SipMaxForwards::operator==(const SipMaxForwards& src) const
{
  return ( value == src.value );
}
void SipMaxForwards::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipMaxForwards(nData);
    }
    catch (SipMaxForwardsParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() of SipMaxForwards  : ( ");
            throw SipMaxForwardsParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_MAXFORWARDS_FAILED
            );
        }
    }
}

void
SipMaxForwards::scanSipMaxForwards(const Data &tmpdata)
{
    string newdata = tmpdata.convertString() ;

    string DIGIT = "0123456789";
    int test = newdata.find_first_not_of(DIGIT);
    if (test == -1)
    {
        set(newdata);
    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed in parsing token  : ( ");
            throw SipMaxForwardsParserException(
                "Failed in parsing token",
                __FILE__,
                __LINE__, DECODE_MAXFORWARDS_FAILED
            );
        }


    }
}


SipMaxForwards::SipMaxForwards( const SipMaxForwards& src )
    : SipHeader(src),
      value(src.value)
{}

///

int
SipMaxForwards::get() const
{
    return value;
}

///
void
SipMaxForwards::set( const Data& newValue )
{
    if (newValue.length())
    {
        value = newValue.convertInt();
    }
}

void
SipMaxForwards::set( int newValue )
{
    if (newValue >= 0)
    {
        value = newValue;
    }
}

int
SipMaxForwards::decrement()
{
    if (value > 0)
    {
        return --value;
    }
    else
    {
        return -1;
    }
}


Data SipMaxForwards::encode() const
{

    Data data;
    if (value > 0)
    {
        data = MAX_FORWARDS;
        data += SP;
        data += Data(value);
        data += CRLF;
    }

    return data;
}


SipHeader*
SipMaxForwards::duplicate() const
{
    return new SipMaxForwards(*this);
}


bool
SipMaxForwards::compareSipHeader(SipHeader* msg) const
{
    SipMaxForwards* otherMsg = dynamic_cast<SipMaxForwards*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

ostream&
operator<<(ostream& s, const SipMaxForwards &smf)
{
    s << smf.encode();
    return s;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
