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

static const char* const SipCSeq_cxx_Version =
    "$Id: SipCSeq.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "global.h"

#include <cmath>
#include <cassert>

#include "SipCSeq.hxx"
#include "SipRequestLine.hxx"
#include "SipMethod.hxx"

using namespace Vocal;

string
SipCSeqParserException::getName( void ) const
{
    return "SipCSeqParserException";
}


SipCSeq::SipCSeq(Method newMethod, const Data& cseqvalue, const string& local_ip)
    : SipHeader(local_ip),
        flag(false)
{
    method = methodToData(newMethod);
    setCSeqData(cseqvalue);
}


SipCSeq::SipCSeq(const Data& newMethod, const Data& cseqvalue, const string& local_ip)
    : SipHeader(local_ip),
        flag(false)
{
    setMethod(newMethod);
    setCSeqData(cseqvalue);
}


SipCSeq::SipCSeq(const Data& data, const string& local_ip)
    : SipHeader(local_ip),
        flag(false)
{
    if (data == "") {
        return;
    }
    try
    {
        decode(data);
    }
    catch (SipCSeqParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Constructor :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }
    }

}


SipCSeq::SipCSeq( const SipCSeq& src)
    : SipHeader(src),
        cseq(src.cseq),
        method(src.method),
        flag(src.flag)
{
}


const SipCSeq& 
SipCSeq::operator =(const SipCSeq& src)
{
    if ( &src != this)
    {
        cseq = src.cseq;
        method = src.method;
        flag = src.flag;
    }
    return *this;
}


void
SipCSeq::parseCSeq(const Data & ldata)
{
    if (flag)
    {

        setCSeqData(ldata);
    }
    else
    {
        //Exceptions!
    }
}


void
SipCSeq::parseMethod(const Data & data)
{
    flag = true;
    Data tmp(data);
    tmp.removeSpaces();
    setMethod(tmp);
}


void
SipCSeq::decode( const Data& cseqstr )
{

    try
    {
        parse(cseqstr);
    }
    catch (SipCSeqParserException exception)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Constructor :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }

    }
}


void
SipCSeq::scanSipCSeq( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match(" ", &sipdata, true);
    if (ret == FOUND)
    {

        parseMethod(data);
        parseCSeq(sipdata);
    }
    else if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Constructor scanSipCSeq :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }

    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Constructor scanSipCSeq :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }

    }
}


void
SipCSeq::parse( const Data& cseqdata)
{
    Data data = cseqdata;

    try
    {
        scanSipCSeq(data);
    }
    catch (SipCSeqParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in CSeq Pares() :( ");
            throw SipCSeqParserException(
                "failed to decode the CallId string",
                __FILE__,
                __LINE__, DECODE_CSeq_FAILED);
        }
    }
    //everything allright.
}


/**it is up to the user to set the correct value of the CSeq.
when incrementing it, the user should convert to uint and increment */
void 
SipCSeq::setCSeqData(const Data& data)
{
    cseq = data;
}


void 
SipCSeq::setCSeq(unsigned int seqnum)
{
    if( seqnum < static_cast<unsigned int>(0x80000000) )   // 2**31
    {
        cseq = Data(int(seqnum));
    }
    else
    {
        cpLog( LOG_ERR, "seqnum is out of range, defaulting to 2**31 - 1" );
        cseq = Data(0x7fffffff);
    }
}


Data 
SipCSeq::encode() const
{
    Data data;
    data = CSEQ;
    data += SP;

    data += cseq;
    data += SP;

    data += method;
    data += CRLF;
    return data;
}


int
SipCSeq::getCSeq() const
{
    unsigned int val = cseq.convertInt();
    return val;
}

int
SipCSeq::getNextCSeq() const
{
    unsigned int val = cseq.convertInt();
    if( val < static_cast<unsigned int>(0x7fffffff) )   // 2**31
    {
        ++val;
    }
    else
    {
        cpLog( LOG_ERR, "next seqnum is out of range, defaulting to 2**31 - 1" );
        val = 0x7fffffff;
    }
    return val;
}


void
SipCSeq::incrCSeq()
{
    cseq = Data(getNextCSeq());
}


bool 
SipCSeq::operator == (const SipCSeq& other) const
{
    cpLog(LOG_DEBUG_STACK, "SipCSeq == operator");

    if ((cseq == other.cseq) && (method == other.method))
    {
        cpLog(LOG_DEBUG_STACK, "SipCSeq == operator returning true");

        return true;
    }
    else
    {
        cpLog(LOG_DEBUG_STACK, "SipCSeq == operator returning false");

        return false;
    }
}


bool 
SipCSeq::operator < (const SipCSeq& other) const
{
    if (cseq < other.cseq)
    {
        return true;
    }
    else if (cseq > other.cseq)
    {
        return false;
    }

    else if (method < other.method)
    {
        return true;
    }
    else if (method > other.method)
    {
        return false;
    }
    else
    {
        return false;
    }
}


bool 
SipCSeq::operator > (const SipCSeq& other) const
{
    if (cseq > other.cseq)
    {
        return true;
    }
    else if (cseq < other.cseq)
    {
        return false;
    }

    else if (method > other.method)
    {
        return true;
    }
    else if (method < other.method)
    {
        return false;
    }
    else
    {
        return false;
    }
}


void
SipCSeq::setMethod(const Data & newMethod)
{
    method = newMethod;
}


#if USE_HASH_MAP
size_t
SipCSeq::hashfn() const
{
    return cseq.hashfn() ^ method.hashfn();
}
#endif


SipHeader*
SipCSeq::duplicate() const
{
    return new SipCSeq(*this);
}


bool
SipCSeq::compareSipHeader(SipHeader* msg) const
{
    SipCSeq* otherMsg = dynamic_cast<SipCSeq*>(msg);
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
