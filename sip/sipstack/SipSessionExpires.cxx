
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

static const char* const SipSessionExpires_cxx_Version =
    "$Id: SipSessionExpires.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";


#include "global.h"
#include "SipSessionExpires.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipSessionExpiresParserException::getName( void ) const
{
    return "SipSessionExpiresParserException";
}

SipSessionExpires::SipSessionExpires( const Data& srcData, const string& local_ip )
    : SipHeader(srcData, local_ip),
      delta(),
      bDelta(false),
      refresher(),
      bRefresher(false)
{

    if (srcData == "") {
        return;
    }

    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipSessionExpiresParserException&)
    {
        parseError("Failed to decode SipSessionExpires in Constructor :(");
    }

}
void SipSessionExpires::decode(const Data& data)
{

    Data nData = data;

    try
    {
        scanSipSessionExpires(nData);
    }
    catch (SipSessionExpiresParserException&)
    {
        parseError("Failed to Decode SipSessionExpires in decode() " );
    }

}

void
SipSessionExpires::scanSipSessionExpires(const Data &tmpdata)

{
    Data newdata = tmpdata ;
    Data newvalue;
    int ret = newdata.match(";", &newvalue, true);
    if (ret == FOUND)
    {
        setDelta(newvalue);
        bDelta = true;
    }
    else if (ret == NOT_FOUND)
    {
        setDelta(newdata);
        bDelta = true;
        //No parameters
        return;
    }
    else if (ret == FIRST)
    {
        parseError("Failed to Decode Session-Expires in scanSipSessionExpire");
    }

    //Search for parameter
    char matchedChar; 
	// look for an equal sign
	Data key = newdata.matchChar("=", &matchedChar);
	Data value;
	if(matchedChar == '=')
	{
	    value = newdata;
	    newdata.erase();

	    // do something with the key-value pair
            key.removeSpaces();
	    if(key == "refresher")
	    {
		// do something here
		bRefresher = true;
		refresher = value;
                //Value is either "uas" or "uac", other values are invalid
                if ((value != "uas") && (value != "uac"))  
                {
                    parseError("Failed to Decode Session-Expires:");
                }
	    }
	    else 
	    {
                parseError("Unknown parameter in Session-Expires:");
	    }
	}
        else
        {
            parseError("Malformed Session-Expires:");
        }
    if(newdata.length())
    {
        //more data then needed
        parseError("Invalid Session-Expires: header");
    }
}




SipSessionExpires::SipSessionExpires( const SipSessionExpires& src )
    : SipHeader(src),
      delta(src.delta),
      bDelta(src.bDelta),
      refresher(src.refresher),
      bRefresher(src.bRefresher)

{}


bool
SipSessionExpires::operator ==(const SipSessionExpires& src) const
{
    cpLog(LOG_DEBUG_STACK, "SessionExpires == Operator ");
    if ( (delta == src.delta) &&
	 (bDelta == src.bDelta) &&
         (refresher == src.refresher) &&
         (bRefresher == src.bRefresher))
    {
        cpLog(LOG_DEBUG_STACK, "SessionExpireS == Operator  returning True :)");
        return true;
    }
    else
    {
        cpLog(LOG_DEBUG_STACK, "SessionExpires == Operator  returning false :(");
        return false;
    }
}

const SipSessionExpires&
SipSessionExpires::operator=( const SipSessionExpires& src )
{
    if ( &src != this )
    {
        delta = src.delta;
        bDelta = src.bDelta;
        refresher = src.refresher;
        bRefresher = src.bRefresher;
    }
    return *this;
}



void SipSessionExpires::setDelta ( const Data& data)
{
    delta = data;
    bDelta = true;

    cpLog(LOG_DEBUG_STACK, "set delta in expires");
}

const Data& SipSessionExpires::getDelta() const
{
    return delta;
}

void SipSessionExpires::setRefreshParam ( const Data& data)
{
    refresher = data;
    bRefresher = true;
}

const Data& SipSessionExpires::getRefreshParam() const
{
    return refresher;
}
///
Data SipSessionExpires::encode() const
{
    Data data;
    if ( (bDelta) )
    {
        data = SESSIONEXPIRES;
        data += SP;
        data += delta;
        if(bRefresher)
        {
            data += SEMICOLON;
            data += SessionTimerParam;
            data += refresher;
        }
	data += CRLF;
    }
    return data;
}


Sptr<SipHeader>
SipSessionExpires::duplicate() const
{
    return new SipSessionExpires(*this);
}


bool
SipSessionExpires::compareSipHeader(SipHeader* msg) const
{
    SipSessionExpires* otherMsg = dynamic_cast<SipSessionExpires*>(msg);
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
SipSessionExpires::parseError(const string& context)
{
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "%s", context.c_str());
            throw SipSessionExpiresParserException(context.c_str(),
                __FILE__,
                __LINE__, DECODE_SESSION_EXPIRE_FAILED);
        }
}
