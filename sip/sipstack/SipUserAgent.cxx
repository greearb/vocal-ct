
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

static const char* const SipUserAgent_cxx_Version =
    "$Id: SipUserAgent.cxx,v 1.2 2004/06/16 06:51:26 greear Exp $";

#include "global.h"
#include "SipUserAgent.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipUserAgentParserException::getName( void ) const
{
    return "SipUserAgentParserException";
}


SipUserAgent::SipUserAgent( const SipUserAgent& src )
    : SipHeader(src)
{
    data = src.data;
    product = src.product;
    version = src.version;
    comment = src.comment;
}


SipUserAgent::SipUserAgent(const Data& newData, const string& local_ip )
    : SipHeader(local_ip),
      flgcomment(false),
      flgproduct(false)

{
    if (newData != "") {
        data = newData;
        try
        {
            decode(data);
        }
        catch (SipUserAgentParserException&)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
    }
}

void SipUserAgent::decode(const Data& uadata)
{
  try
    {

      scanSipUserAgent(uadata);
    }
  catch (SipUserAgentParserException& exception)
    {
      if (SipParserMode::sipParserMode())
        {
	  throw SipUserAgentParserException(
					    exception.getDescription(),
					    __FILE__,
					    __LINE__, SCAN_USERAGENT_FAILED);

        }
    }

}
void
SipUserAgent::scanSipUserAgent(const Data &tmpdata)
{
    Data agdata = tmpdata;
    Data agvalue;
    int check = agdata.match("(", &agvalue, true);
    if (check == FOUND)
    {
      Data j1data = agvalue;
      Data j1value;
      int ret1 = j1data.match("/", &j1value, true);
      if (ret1 == FOUND)
        {
	  setProduct(j1value);
	  setVersion(j1data);
        }
      else if (ret1 == NOT_FOUND)
        {
	  setProduct(j1data);
        }
        else if (ret1 == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
     
        Data avalue;
	int test = agdata.match(")", &avalue,true);
	if (test == FOUND)
	{
	  setComment(avalue);
 	}
	if (test == NOT_FOUND)
	{
	
	}
	if (test == FIRST)
	{
	
	}
    }

    else if (check == NOT_FOUND)
    {
        Data jdata = agdata;
        Data jvalue;
        int ret = jdata.match("/", &jvalue, true);
        if (ret == FOUND)
        {
            setProduct(jvalue);
            setVersion(jdata);
        }
        else if (ret == NOT_FOUND)
        {
            setProduct(jdata);
        }
        else if (ret == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
    }
    else if (check == FIRST)
    {
        Data ndata = agdata;
        Data nvalue;
        int test = ndata.match(")", &nvalue, true);
        if (test == FOUND)
        {
            setComment(nvalue);
        }
        else if (test == NOT_FOUND)
        {
            //
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
        else if (test == FIRST)
        {
            //
            if (SipParserMode::sipParserMode())
            {
                throw SipUserAgentParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_USERAGENT_FAILED);
            }
        }
    }
}
///
const Data& SipUserAgent::getProduct() const
{
    return product;
}


void SipUserAgent::setProduct( const Data& newdata )
{
    product = newdata;
    flgproduct = true;
}

void SipUserAgent::setComment(const Data& newdata)
{
    comment = newdata;
    flgcomment = true;
}
void SipUserAgent::setVersion(const Data& newdata)
{
    version = newdata;

}
const Data& SipUserAgent::getVersion() const
{
    return version;
}
const Data& SipUserAgent::getComment() const
{
    return comment;
}

Data SipUserAgent::encode() const
{
    Data ret;
    if (product.length() || comment.length())
    {
        ret = USERAGENT;
        ret += SP;
        if (product.length())
        {
            ret += product;
        }
        if (version.length())
        {
            ret += "/";
            ret += version;
        }
        if (comment.length())
        {
            ret += " ";
            ret += "(";
            ret += comment;
            ret += ")";
        }
        ret += CRLF;
    }
    return ret;
}



const SipUserAgent&
SipUserAgent::operator=(const SipUserAgent& rhs)
{
    if (&rhs != this)
    {
        data = rhs.data;
        product = rhs.product;
        version = rhs.version;
        comment = rhs.comment;
        flgcomment = rhs.flgcomment;
        flgproduct = rhs.flgproduct;
    }
    return *this;
}


bool
SipUserAgent::operator==(const SipUserAgent& rhs) const
{
    return ((data == rhs.data) &&
	    (product == rhs.product) &&
	    (version == rhs.version) &&
	    (comment == rhs.comment) &&
	    (flgcomment == rhs.flgcomment) &&
	    (flgproduct == rhs.flgproduct));
}


Sptr<SipHeader>
SipUserAgent::duplicate() const
{
    return new SipUserAgent(*this);
}


bool
SipUserAgent::compareSipHeader(SipHeader* msg) const
{
    SipUserAgent* otherMsg = dynamic_cast<SipUserAgent*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
