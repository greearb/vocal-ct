
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

static const char* const SipMimeVersion_cxx_Version =
    "$Id: SipMimeVersion.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";


#include "global.h"
#include "symbols.hxx"
#include "SipMimeVersion.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipMimeVersionParserException::getName( void ) const
{
    return "SipMimeVersionParserException";
}


SipMimeVersion::SipMimeVersion( const Data& data, const string& local_ip ) //TODO
    : SipHeader(local_ip)
{
    if (data == "") {
        return;
    }
    try
    {
        decode(data);
    }
    catch (SipMimeVersionParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode  SipMimeVersion  : ( ");
            throw SipMimeVersionParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_MIMEVERSION_FAILED
            );
        }

    }

}

const SipMimeVersion&
SipMimeVersion::operator=(const SipMimeVersion& src)
{
    if (&src != this)
    {
        value = src.value;
    }
    return (*this);
}

void SipMimeVersion::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipMimeVersion(nData);
    }
    catch (SipMimeVersionParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() of SipMimeVersion  : ( ");
            throw SipMimeVersionParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_MIMEVERSION_FAILED
            );
        }
    }
}

void
SipMimeVersion::scanSipMimeVersion(const Data &tmpdata)
{
    Data newdata = tmpdata ;
    Data rdata;
    int test = newdata.match(".",&rdata,true);
    if (test == FOUND)
      {
	string DIGIT = "0123456789";
	string rsdata = rdata.convertString();
	string newsdata = newdata.convertString();
	int test1 = rsdata.find_first_not_of(DIGIT);
	if (test1 == -1)
	  {
	    int test2 = newsdata.find_first_not_of(DIGIT);
	    if (test2 == -1)
	      {

		set(tmpdata);
	      }
	    else
	      {
		if (SipParserMode::sipParserMode())
		  {
		    cpLog(LOG_ERR, "Failed in parsing DIGITS for MIME-VERSION  : ( ");
		    throw SipMimeVersionParserException(
							"Failed in parsing DIGITS for MIME-VERSION  ",
							__FILE__,
							__LINE__, DECODE_MIMEVERSION_FAILED
							);
		  }
	      }
	  }
	else
	  {
	    if (SipParserMode::sipParserMode())
	      {
		cpLog(LOG_ERR, "Failed in parsing DIGITS for MIME-VERSION  : ( ");
		throw SipMimeVersionParserException(
						    "Failed in parsing  DIGITS for MIME-VERSION  ",
						    __FILE__,
						    __LINE__,DECODE_MIMEVERSION_FAILED
						    );
	      }
	    
	    
	  }
      }
    if (test == NOT_FOUND)
      {
	if (SipParserMode::sipParserMode())
	      {
		cpLog(LOG_ERR, "Failed in parsing DIGITS for MIME-VERSION  : ( ");
		throw SipMimeVersionParserException(
						    "Failed in parsing  DIGITS for MIME-VERSION  ",
						    __FILE__,
						    __LINE__, DECODE_MIMEVERSION_FAILED
						    );
	      }
      }
    if (test == FIRST)
      {
	if (SipParserMode::sipParserMode())
	  {
	    cpLog(LOG_ERR, "Failed in parsing DIGITS for MIME-VERSION  : ( ");
	    throw SipMimeVersionParserException(
						"Failed in parsing  DIGITS for MIME-VERSION  ",
						__FILE__,
						__LINE__, DECODE_MIMEVERSION_FAILED
						);
	  }
	
      }
}
    

SipMimeVersion::SipMimeVersion( const SipMimeVersion& src )
    : SipHeader(src),
      value(src.value)
{}

///

Data
SipMimeVersion::get() const
{
    return value;
}

///
void
SipMimeVersion::set( Data newValue )
{
    if (value.length())
    {
        value = newValue;
    }
}

bool SipMimeVersion::operator ==(const SipMimeVersion& srcmv) const
{
    bool equal = false;

    equal = (value == srcmv.value);
    return equal;
}

Data SipMimeVersion::encode() const
{

    Data data;
    if (value.length())
    {
        data = "Mime-Version:";
        data += SP;
        data += value;
        data += CRLF;
    }

    return data;
}



Sptr<SipHeader>
SipMimeVersion::duplicate() const
{
    return new SipMimeVersion(*this);
}


bool
SipMimeVersion::compareSipHeader(SipHeader* msg) const
{
    SipMimeVersion* otherMsg = dynamic_cast<SipMimeVersion*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
