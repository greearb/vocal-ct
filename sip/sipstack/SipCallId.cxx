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

static const char* const SipCallId_cxx_Version =
    "$Id: SipCallId.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipCallId.hxx"
#include "SipMsg.hxx"
#include "symbols.hxx"
#include "HostMatch.hxx"
#include "RandomHex.hxx"
#include "SystemInfo.hxx"

#include "support.hxx"




using namespace Vocal;

#define NUM_RANDOMNESS 16  //128 bits of randomness

string
SipCallIdParserException::getName( void ) const
{
    return "SipCallIdParserException";
}



SipCallId::SipCallId(const SipCallId& src)
    : SipHeader(src),
      localid(src.localid),
      host(src.host)
{}



SipCallId::SipCallId( const Data& data, const string& local_ip )
    : SipHeader(local_ip)
{

    if (data == "") {
        localid = RandomHex::get(NUM_RANDOMNESS);
        host = local_ip;
        return;
    }

    try
    {
        decode(data);
    }
    catch (SipCallIdParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipCallIdParserException("failed in Decode", __FILE__, __LINE__, DECODE_CALLID_FAILED);

        }

    }

}

void
SipCallId::parseLocalId(const Data & ldata)
{
    setLocalId(ldata);
}
void
SipCallId::parseHost(const Data & data)
{
    setHost(data);
}
void
SipCallId::scanSipCallId( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("@", &sipdata, true);
    if (ret == FOUND)
    {
        parseLocalId(sipdata);
        parseHost(data);
    }
    else if (ret == NOT_FOUND)
    {

        parseLocalId(data);


    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Mandatory item @ not present  :o( ");
            throw SipCallIdParserException("Mandatory item @ not presentfailed", __FILE__, __LINE__, DECODE_CALLID_FAILED);

        }


    }
}

void
SipCallId::parse( const Data& calliddata)
{

    Data data = calliddata;

    try
    {

        scanSipCallId(data);
    }
    catch (SipCallIdParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Mandatory item sip not present  :o( ");
            throw SipCallIdParserException("Mandatory item @ not presentfailed", __FILE__, __LINE__, DECODE_CALLID_FAILED);

        }

    }


    //everything allright.


}


void
SipCallId::decode( const Data& callidstr )
{

    try
    {
        parse(callidstr);
    }
    catch (SipCallIdParserException exception)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Mandatory item sip not present  :o( ");
            throw SipCallIdParserException("Mandatory item @ not presentfailed", __FILE__, __LINE__, DECODE_CALLID_FAILED);

        }

    }
}

Data SipCallId::encode() const
{
    if (getData().length())
    {
        return (getData()+ CRLF);
    }
    else
    {
        return (getData());
    }
}

void SipCallId::setData(const Data& data)
{
    decode(data);
}

Data SipCallId::getData() const
{
    Data data;
    if (getLocalId().length())
    {
        data = CALLID;
        data += SP;
        data += getCallIdData();
    }
    return data;
}
Data SipCallId::getCallIdData() const
{
    Data ret;
    if (getLocalId().length())
    {
        ret += getLocalId();
    }
    if (getHost().length())
    {
        ret += "@";
        ret += getHost();
    }
    return ret;
}
const SipCallId&
SipCallId::operator=(const SipCallId& src)
{
    if ( &src != this)
    {
        localid = src.localid;
        host = src.host;
        hash = src.hash;
    }
    return (*this);
}
///
bool
SipCallId::operator < ( const SipCallId& other ) const
{
    Data selfstr = (const_cast < SipCallId* > (this))->encode(); // encode();
    Data otherstr = (const_cast < SipCallId& > (other)).encode(); // other.encode();

    if ( selfstr < otherstr)
    {
        return true;
    }
    return false;
}


///
bool
SipCallId::operator > ( const SipCallId& other ) const
{
    Data selfstr = (const_cast < SipCallId* > (this))->encode(); // encode();
    Data otherstr = (const_cast < SipCallId& > (other)).encode(); // other.encode();

    if ( selfstr > otherstr )
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
bool
SipCallId::operator== ( const SipCallId& other ) const
{
    cpLog(LOG_DEBUG_STACK, "SipCallId == operator");

    bool equal = false;

    equal = ((localid == other.localid) && (host == other.host));

    cpLog(LOG_DEBUG_STACK, "SipCallId == operator, returning %d", equal);

    return equal;
}

#if USE_HASH_MAP
size_t SipCallId::hashfn() const
{

    return localid.hashfn() ^ host.hashfn();
}
#endif


SipHeader*
SipCallId::duplicate() const
{
    return new SipCallId(*this);
}


bool
SipCallId::compareSipHeader(SipHeader* msg) const
{
    SipCallId* otherMsg = dynamic_cast<SipCallId*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


ostream& operator<<(ostream& strm, const SipCallId& id)
{
   strm << id.encode();
   return strm;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
