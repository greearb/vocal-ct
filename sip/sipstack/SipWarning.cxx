
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

static const char* const SipWarning_cxx_Version =
    "$Id: SipWarning.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";



#include "global.h"
#include "SipWarning.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipWarningParserException::getName( void ) const
{
    return "SipWarningParserException";
}


SipWarning::SipWarning( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip),
      code(),
      agent(),
      text()
{

    if (srcData == "") {
        return;
    }

    try
    {
        decode( srcData );
    }
    catch ( SipWarningParserException&)
    {
        if ( SipParserMode::sipParserMode() )
        {
            throw SipWarningParserException(
                "failed to decode the Retry After string",
                __FILE__,
                __LINE__, DECODE_WARNING_FAILED );
        }
    }
}


void
SipWarning::decode( const Data& retryData )
{
    Data data = retryData;

    try
    {
        scanWarning( data );
    }
    catch ( SipWarningParserException& exception)
    {
        if ( SipParserMode::sipParserMode() )
        {
            throw SipWarningParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, SCAN_WARNING_FAILED );
        }
    }
}


void
SipWarning::scanWarning( const Data &tmpData )

{
    Data warnData = tmpData;
    Data warnValue;
    int retn = warnData.match( " ", &warnValue, true );
    if ( retn == FOUND)
    {
        setCode( warnValue.convertInt() );
        Data wAgent = warnData;
        Data wAgentValue;
        int test = wAgent.match( " ", &wAgentValue, true );
        if ( test == FOUND )
        {
            setAgent( wAgentValue );
            setText( wAgent );
        }
        else if ( test == NOT_FOUND )
        {
            if ( SipParserMode::sipParserMode() )
            {
                throw SipWarningParserException(
                    "failed to decode the Retry After string",
                    __FILE__,
                    __LINE__, DECODE_WARNING_FAILED );
            }
        }
        else if ( test == FIRST )
        {

            if ( SipParserMode::sipParserMode() )
            {
                throw SipWarningParserException(
                    "failed to decode the Retry After string",
                    __FILE__,
                    __LINE__, DECODE_WARNING_FAILED );
            }
        }
    }
    else if ( retn == NOT_FOUND )
    {
        if ( SipParserMode::sipParserMode() )
        {
            throw SipWarningParserException(
                "failed to decode the Retry After string",
                __FILE__,
                __LINE__, DECODE_WARNING_FAILED );
        }
    }
    else if ( retn == FIRST )
    {
        if ( SipParserMode::sipParserMode() )
        {
            throw SipWarningParserException(
                "failed to decode the Retry After string",
                __FILE__,
                __LINE__, DECODE_WARNING_FAILED );
        }
    }
}

SipWarning::SipWarning( const SipWarning& src )
    : SipHeader(src)
{
    //copy all member variables
    code = src.code;
    agent = src.agent;
    text = src.text;
}


const SipWarning& SipWarning::operator=( const SipWarning& src )
{
    if ( &src != this )
    {
        code = src.code;
        agent = src.agent;
        text = src.text;
    }
    return *this;
}

/// 
bool SipWarning::operator ==(const SipWarning & src) const
{
  return (
	  ( code == src.code ) &&
	  ( agent == src.agent ) &&
	  ( text == src.text ) 
	  );
}
///
int SipWarning::getCode()
{
    return code.convertInt();
}

///
void SipWarning::setCode( const int& newCode )
{
    code = Data(newCode);
}

///
Data SipWarning::getCodeData()
{
    return code;
}

///
void SipWarning::setCodeData( const Data& newCode )
{
    code = newCode;
}

///
Data SipWarning::getAgent()
{
    return agent;
}

///
void SipWarning::setAgent( const Data& newAgent )
{
    agent = newAgent;
}

///
Data SipWarning::getText()
{
    return text;
}

///
void SipWarning::setText( const Data& newText )
{
    text = newText;
}

/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data SipWarning::encode() const
{
    Data ret;
    ret += SIP_WARNING;
    ret += code;
    ret += SP;
    ret += agent;
    ret += SP;
    ret += text;

    return ret;
}


SipHeader*
SipWarning::duplicate() const
{
    return new SipWarning(*this);
}


bool
SipWarning::compareSipHeader(SipHeader* msg) const
{
    SipWarning* otherMsg = dynamic_cast<SipWarning*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
