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

static const char* const SipContentType_cxx_Version =
    "$Id: SipContentType.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include "SipContentType.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipContentTypeParserException::getName( void ) const
{
    return "SipToParserException";
}
    
SipContentType::SipContentType( const SipContentType& src )
    : SipHeader(src),
      type(src.type),
      subtype(src.subtype),
      myParamList(src.myParamList)
{
   type.lowercase();
   subtype.lowercase();
}


bool
SipContentType::operator ==(const SipContentType& other) const 
{
    if (    (type == other.type) &&
            (subtype == other.subtype) &&
            (myParamList == other.myParamList) 
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}


const SipContentType&
SipContentType::operator=( const SipContentType& src) 
{
    if (&src != this)
    {
        type = src.type;
        subtype = src.subtype;
        myParamList = src.myParamList;
    }

    return *this;
}

/*
SipContentType::SipContentType(const string& local_ip)
    : SipHeader(local_ip),
      type(),
      subtype(),
      myParamList(';')
{
    
}
*/

SipContentType::SipContentType(const Data& gtype, const Data& gsubType,
                               const string& local_ip)
    : SipHeader(local_ip),
      myParamList(';')
{
    type = gtype;
    subtype = gsubType;
    type.lowercase();
    subtype.lowercase();
}


SipContentType::SipContentType( const Data& data, const string& local_ip, int dummy )
    : SipHeader(local_ip),
      myParamList(';')
{
    if (data != "") {
        try
        {
            decode(data);
        }
        catch (SipContentTypeParserException&)
        {
            
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipContentTypeParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTTYPE_FAILED);
                
            }
        }
    }
}


void
SipContentType::scanSipContentType( const Data & adata)
{
    Data sipdata;
    Data data = adata;
    Data appl;
    data.match( ":", &sipdata, true);
  
    int ret = data.match("/", &sipdata, true);
    if (ret == NOT_FOUND)
    {   
	if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR, "Failed to Decode in Constructor, data: %s ", adata.c_str());
            assert(0); //TODO: Remove
	    throw SipContentTypeParserException("failed in Decode", __FILE__, __LINE__,
                                                DECODE_CONTENTTYPE_FAILED);
	    
	}	  
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR, "Failed to Decode in Constructor, data: %s ", adata.c_str());
	    throw SipContentTypeParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTTYPE_FAILED);
	    
	}
    }
    else if (ret == FOUND)
    {
	sipdata.removeSpaces();
	setType(sipdata);
    
	Data unparsedData = data;
	Data lsubtype;
	int retn = unparsedData.match(";", &lsubtype, true);
	if (retn == NOT_FOUND)
	{
	    setSubType(unparsedData);
	}
	if (retn == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR, "Failed to parse subtype :o( ");
		throw SipContentTypeParserException("failed in parse'n subtype ", __FILE__, __LINE__, DECODE_CONTENTTYPE_FAILED);
	    }
	}
	
	if (retn == FOUND)
	{
	    setSubType(lsubtype);
	    Data parms = unparsedData;
            myParamList.decode(parms, ';');
	}
    }
}

    
void
SipContentType::parse( const Data& ctypedata)
{
    Data data = ctypedata;

    try
    {
        Data ndat;
        int remcrlf = data.match ( CRLF, &ndat, true);
        if ( remcrlf == NOT_FOUND)
	{
            scanSipContentType(data);
        }
        else
	{
            scanSipContentType(ndat);
	}
    }
    catch (SipContentTypeParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to parse'n Parms, ctypedata: %s ", ctypedata.c_str());
            assert(0); // TODO: Remove
            throw SipContentTypeParserException(
                "failed in parse'n Parms ", 
                __FILE__, __LINE__, 
                DECODE_CONTENTTYPE_FAILED);
        }
    }
    //everything allright.
    type.lowercase();
    subtype.lowercase();
}


void
SipContentType::decode( const Data& ctypestr )
{
    try
    {
        parse(ctypestr);
    }
    catch (SipContentTypeParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to parse'n Parms :o( ");
            throw SipContentTypeParserException(
                "failed in parse'n Parms ", __FILE__, __LINE__, 
                DECODE_CONTENTTYPE_FAILED);
        }
    }
}


Data SipContentType::encode() const
{
    Data content;

    if(type.length() > 0)
    {
	content = CONTENT_TYPE;
	content += SP;
	content += type;
	content += "/";
	if (subtype.length() > 0)
	{
	    content += subtype;
	}

        if(myParamList.size())
        {	
            content+= "; ";
	    content+= myParamList.encode();
	}

	Data scon;
	int rcrlf = content.match(CRLF, &scon, false);
	if ( rcrlf == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
		throw SipContentTypeParserException("failed in Decode", 
                                                    __FILE__, __LINE__, 
                                                    DECODE_CONTENTTYPE_FAILED);
		
	    }
	}
	if (rcrlf == NOT_FOUND)
	{
	    content += CRLF;
	}
    }
    return content;
}


void 
SipContentType::setTokenDetails(const Data& token, const Data& tokenValu)
{
    myParamList.setValue(token, tokenValu);
}



void
SipContentType::clearToken(const Data& token)
{
    myParamList.clearValue(token);
}


Data
SipContentType::getType() const 
{ 
    return type; 
}


Data 
SipContentType::getSubType() const 
{ 
    return subtype; 
}


Data SipContentType::getAttributeValue(const Data& token) const
{
    Data ret = myParamList.getValue(token);
    return ret;
}


Data 
SipContentType::getMediatype() 
{ 
    if ( media.length()!=0) 
    {
	return media; 
    }
    else 
    {
	media = type;
	media += "/";
	media += subtype;
	return media;
    }
} 


void 
SipContentType::setMediatype(const Data& data)
{
    Data line1;
    Data cdata = data;
    int value1 = cdata.match("/", &line1, true);
    if (value1 == FOUND)
    {
    type = line1;
    subtype = cdata;
    media = data;
    }
    else
    {
	//throw exception
    }
}


Sptr<SipHeader>
SipContentType::duplicate() const
{
    return new SipContentType(*this);
}


bool
SipContentType::compareSipHeader(SipHeader* msg) const
{
    SipContentType* otherMsg = dynamic_cast<SipContentType*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


