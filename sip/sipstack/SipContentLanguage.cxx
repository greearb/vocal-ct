
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

static const char* const SipContentLanguage_cxx_Version =
    "$Id: SipContentLanguage.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";


#include "global.h"
#include "SipContentLanguage.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipContentLanguageParserException::getName( void ) const
{
    return "SipContentLanguageParserException";
}


SipContentLanguage::SipContentLanguage( const SipContentLanguage & src)
    : SipHeader(src)
{
    primarytag = src.primarytag;
    subtag = src.subtag;
    
}

SipContentLanguage& SipContentLanguage::operator=(const SipContentLanguage& src)
{
    if ( &src != this)
    {
        primarytag = src.primarytag;
	subtag = src.subtag;
    }
    return (*this);
}

SipContentLanguage::SipContentLanguage( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip),
      primarytag(),
      subtag()
{
    if (srcData == "") {
        return;
    }
    Data wdata = srcData;
    try
    {
        decode(wdata);
    }
    catch (SipContentLanguageParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of ContentLanguage :( ");
            throw SipContentLanguageParserException(
                "failed to decode the Subject string :(",
                __FILE__,
                __LINE__, DECODE_CONTENTLANGUAGE_FAILED);
        }


    }

}
void SipContentLanguage::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipContentLanguage(nData);
    }
    catch (SipContentLanguageParserException e)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode method of ContentLanguage  :( ");
            throw SipContentLanguageParserException(
                e.getDescription(),
                __FILE__,
                __LINE__, DECODE_CONTENTLANGUAGE_FAILED
            );
        }
    }
}

void
SipContentLanguage::scanSipContentLanguage(const Data &tmpdata)
{
   Data workdat = tmpdata;
   Data nextdat;
   int test = workdat.match("-", &nextdat,true);
   if (test == FOUND)
     {
       setPrimaryTag(nextdat);
       setSubTag(workdat);
     }
   else if (test == NOT_FOUND)
     {
       setPrimaryTag(workdat);
     }
   else if (test == FIRST)
     {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in method scanSipContentLanguage :( ");
            throw SipContentLanguageParserException("Failed to Decode in method scanSipContentLanguage", __FILE__,__LINE__, DECODE_CONTENTLANGUAGE_FAILED);
        }
     }
}



Data SipContentLanguage::getPrimaryTag()
{
    return primarytag;
}
Data SipContentLanguage::getSubTag()
{
    return subtag;
}

void SipContentLanguage::setPrimaryTag( const Data& newdata)
{
    primarytag = newdata;
}
void SipContentLanguage::setSubTag( const Data& nwdata)
{
    subtag = nwdata;
}

Data SipContentLanguage::encode() const
{
    Data ret;
    if (primarytag.length())
    {
        ret = CONTENTLANGUAGE;
        ret += SP;
	ret += primarytag;
	if (subtag.length())
	  {
	     ret += "-";
	     ret += subtag;
	  }
        ret += CRLF;
    }
    return ret;
}

bool SipContentLanguage::operator==(const SipContentLanguage& src) const
{
    if ( (primarytag == src.primarytag) &&
         (subtag == src.subtag)
       )
      {
	  return true;
      }
      else
      {
	  return false;
      }
}



Sptr<SipHeader>
SipContentLanguage::duplicate() const
{
    return new SipContentLanguage(*this);
}


bool
SipContentLanguage::compareSipHeader(SipHeader* msg) const
{
    SipContentLanguage* otherMsg = dynamic_cast<SipContentLanguage*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
} 

