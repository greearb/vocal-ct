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

static const char* const SipSdp_cxx_Version =
    "$Id: SipSdp.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipSdp.hxx"
#include <cassert>
#include "cpLog.h"

using namespace Vocal;

using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;
using Vocal::SDP::SdpMedia;


SipSdp::SipSdp(const string& local_ip)
    : SipContentData(local_ip),
      sdpDescriptor()
{
    setContentType(new SipContentType("application", "sdp", local_ip));
}


SipSdp::SipSdp(const SdpSession& descriptor, const string& local_ip)
    : SipContentData(local_ip),
     sdpDescriptor(descriptor)
{
    setContentType(new SipContentType("application", "sdp", local_ip));
}

SipSdp::~SipSdp()
{
}


SipSdp&
SipSdp::operator =(const SipSdp& src)
{
   
    if (&src != this )
    {
	SipContentData::operator=(src);
        sdpDescriptor = src.sdpDescriptor;
    }
    return (*this);
}


#if 0
bool
SipSdp::operator ==(const SipSdp& rhs) const
{
    // xxx not implemented
    assert(0);
    if ( &rhs == this )
    {
        return true;
    }
    else if (rhs.sdpDescriptor == sdpDescriptor)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif


SipSdp::SipSdp(const SipSdp& src)
    : SipContentData(src),
      sdpDescriptor(src.sdpDescriptor)
{
    setContentType(new SipContentType("application", "sdp", src.getLocalIp()));
}


SipSdp::SipSdp( const Data& data, const string& local_ip )
    : SipContentData(local_ip)
{
    sdpDescriptor.decode(data);
    setContentType(new SipContentType("application", "sdp", local_ip));
}

SdpSession&
SipSdp::getSdpDescriptor() 
{
    return sdpDescriptor;
}

    
void
SipSdp::setSdpDescriptor(const SdpSession& sdp)
{
    sdpDescriptor = sdp;
}

bool
SipSdp::doCodecsMatch() const // logic for this will be in sdp stack
{
    // What's up with TODO?  -ctam

    //TODO
    return true;
}

    
void
SipSdp::setConnAddress(const SdpConnection& connAddr)
{
    sdpDescriptor.setConnection(connAddr);
}


Data
SipSdp::getConnAddress()
{
    Data data;
    SdpConnection* conn = sdpDescriptor.getConnection();
    if (conn)
    {
        data = conn->getUnicast();
    }

    return data;
}


void
SipSdp::setUserName(const Data& name)
{
    LocalScopeAllocator lo;
    sdpDescriptor.setUserName(name.getData(lo));
}


Data
SipSdp::getUserName()
{
    return Data(sdpDescriptor.getUserName());
}

void
SipSdp::setAddressType(SDP::AddressType addrType)
{
    sdpDescriptor.setAddressType(addrType);   
}

SDP::AddressType
SipSdp::getAddressType()
{
    return sdpDescriptor.getAddressType();   
}

void
SipSdp::setAddress(const Data& address)
{
    LocalScopeAllocator lo;
    sdpDescriptor.setAddress(address.getData(lo));   
}


Data
SipSdp::getAddress()
{
    return Data(sdpDescriptor.getAddress());
}



void
SipSdp::setRtpPort(int port)
{
    list < SdpMedia* > mediaList = sdpDescriptor.getMediaList();
    if (mediaList.size() > 0)
    {
        (*mediaList.begin())->setPort(port);
    }
    else
    {
        SdpMedia* media = new SdpMedia;
        media->setPort(port);
        sdpDescriptor.addMedia (media);
    }
}


int
SipSdp::getRtpPort() const
{
    list < SdpMedia* > mediaList = sdpDescriptor.getMediaList();
    if (mediaList.size() > 0)
    {
        return (*mediaList.begin())->getPort();
    }
    else
    {
        //TODO Throw an exception
        return -1;
    }
}


void
SipSdp::setSessionName(const Data& session)
{ 
    LocalScopeAllocator lo;
    sdpDescriptor.setSessionName(session.getData(lo));
}


Data
SipSdp::getSessionName()
{
    return Data(sdpDescriptor.getSessionName());
}

void
SipSdp::setSessionId(unsigned int sessionId)
{
    sdpDescriptor.setSessionId(sessionId);
}
 
 
unsigned int
SipSdp::getSessionId()
{
    return (sdpDescriptor.getSessionId());
}


Data
SipSdp::encodeBody(int& len)
{
    Data encsdp = sdpDescriptor.encode();
    
    len = encsdp.length();

    if (getContentLength() != 0)
    {
	getContentLength()->setLength(len);
    }
    return encsdp;
}


SipContentData* 
SipSdp::duplicate() const
{
    return new SipSdp(*this);
}

std::ostream&
Vocal::operator<<(std::ostream& s, SipSdp& msg)
{
    int l=0;
    s << msg.encodeBody(l);
    return s;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
