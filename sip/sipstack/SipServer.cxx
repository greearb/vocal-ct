
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

static const char* const SipServer_cxx_Version =
    "$Id: SipServer.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include "SipServer.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipServerParserException::getName( void ) const
{
    return "SipServerParserException";
}

SipServer::SipServer( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip),
      flgcomment(false),
      flgproduct(false)
{
    if (srcData == "") {
        return;
    }
    data = srcData;
    try
    {
        decode(data);
    }
    catch (SipServerParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipServerParserException(
                "failed to decode the User Agent string",
                __FILE__,
                __LINE__, DECODE_SERVER_FAILED);
        }
    }
}

void SipServer::decode(const Data& uadata)
{

    Data gdata = uadata;

    try
    {

        scanSipServer(gdata);
    }
    catch (SipServerParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipServerParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, SCAN_SERVER_FAILED);

        }
    }

}
void
SipServer::scanSipServer(const Data &tmpdata)
{
    Data agdata = tmpdata;
    Data agvalue;
    int check = agdata.match("(", &agvalue, true);
    if (check == FOUND)
    {
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
                throw SipServerParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_SERVER_FAILED);
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
                throw SipServerParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_SERVER_FAILED);
            }
        }
        else if (test == FIRST)
        {
            //
            if (SipParserMode::sipParserMode())
            {
                throw SipServerParserException(
                    "failed to decode the User Agent string",
                    __FILE__,
                    __LINE__, DECODE_SERVER_FAILED);
            }
        }
    }
}
///
Data SipServer::getProduct()
{
    return product;
}


void SipServer::setProduct( const Data& newdata )
{
    product = newdata;
    flgproduct = true;
}

void SipServer::setComment(const Data& newdata)
{
    comment = newdata;
    flgcomment = true;
}
void SipServer::setVersion(const Data& newdata)
{
    version = newdata;

}
Data SipServer::getVersion()
{
    return version;
}
Data SipServer::getComment()
{
    return comment;
}

Data SipServer::encode() const
{
    Data ret;
    if (product.length() || comment.length())
    {
        ret = SIP_SERVER;
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
            ret += "(";
            ret += comment;
            ret += ")";
        }
        ret += CRLF;
    }
    return ret;
}

SipServer::SipServer( const SipServer& src )
    : SipHeader(src)
{
    data = src.data;
    product = src.product;
    version = src.version;
    comment = src.comment;
    flgcomment = src.flgcomment;
    flgproduct = src.flgproduct;

}

SipServer&
SipServer::operator=( const SipServer& src )
{
    if (&src != this)
    {
        data = src.data;
        product = src.product;
        version = src.version;
        comment = src.comment;
        flgcomment = src.flgcomment;
        flgproduct = src.flgproduct;
    }
    return *this;

}

bool SipServer::operator==( const SipServer& src ) const
{
    
  return 
    (
      (  data == src.data ) &&
      ( product == src.product ) &&
      ( version == src.version ) &&
      ( comment == src.comment ) &&
      ( flgcomment == src.flgcomment ) && 
      ( flgproduct == src.flgproduct ) 
      );
}
///
Data SipServer::get()
{
    return data;
}

///
void SipServer::set( const Data& newdata )
{
    data = newdata;
}



Sptr<SipHeader>
SipServer::duplicate() const
{
    return new SipServer(*this);
}


bool
SipServer::compareSipHeader(SipHeader* msg) const
{
    SipServer* otherMsg = dynamic_cast<SipServer*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
