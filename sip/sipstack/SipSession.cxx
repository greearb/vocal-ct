
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

static const char* const SipSession_cxx_Version =
    "$Id: SipSession.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";


#include "global.h"
#include "SipSession.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipSessionParserException::getName( void ) const
{
    return "SipSessionParserException";
}

SipSession::SipSession( const SipSession& src)
    : SipHeader(src),
      sessionMedia(src.sessionMedia),
      sessionQos(src.sessionQos),
      sessionSecurity(src.sessionSecurity)
{}



bool
SipSession::operator ==(const SipSession& other) const
{
    if ( (sessionMedia == other.sessionMedia) &&
	 (sessionQos == other.sessionQos) &&
	 (sessionSecurity == other.sessionSecurity)
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}



SipSession&
SipSession::operator=( const SipSession& src)
{
    if (&src != this)
    {
        sessionMedia = src.sessionMedia;
        sessionQos = src.sessionQos;
        sessionSecurity = src.sessionSecurity;
    }

    return *this;
}


SipSession::SipSession( const Data& data, const string& local_ip )
    : SipHeader(local_ip),
      sessionMedia(false),
      sessionQos(false),
      sessionSecurity(false)
{
    if (data == "") {
        return;
    }
    try
    {
        decode(data);
    }
    catch (SipSessionParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipSessionParserException("failed in Decode", __FILE__, __LINE__, DECODE_SESSION_FAILED);

        }


    }
}


void
SipSession::decode( const Data& ctypestr )
{
    if (ctypestr == SESSION_MEDIA)
    {
	setSessionMedia(true);
    }
    else if (ctypestr == SESSION_QOS)
    {
	setSessionQos(true);
    }
    else if (ctypestr == SESSION_SECURITY)
    {
	setSessionSecurity(true);
    }
    else
    {
	//throw --
	 cpLog(LOG_ERR, "Not any of Media, Qos, Security ");
            throw SipSessionParserException("failed in Decode", __FILE__, __LINE__, DECODE_SESSION_FAILED);
    }
}


Data SipSession::encode() const
{
    Data session;
    if ( (sessionMedia) || (sessionQos) || (sessionSecurity) )
    {
        session = SESSION;
        session+= SP;
        if (sessionMedia)
        {
	    session+= SESSION_MEDIA;
        }
        if (sessionQos)
        {
	    session+= SESSION_QOS;
        }
        if (sessionSecurity)
        {
	    session+= SESSION_SECURITY;
        }
        session += CRLF;
    }
    return session;
}


void SipSession::setSessionMedia(const bool& newsessionMedia)
{
    sessionMedia = newsessionMedia;
}


void SipSession::setSessionQos(const bool& newsessionQos)
{
    sessionQos = newsessionQos; 
}
    
void SipSession::setSessionSecurity(const bool& newsessionSecurity)
{
    sessionSecurity = newsessionSecurity;
}
    
bool SipSession::isSessionMedia()
{
    return sessionMedia;
}
    
    
bool SipSession::isSessionSecurity()
{
    return sessionSecurity;
}
    
    
bool SipSession::isSessionQos()
{
    return sessionQos;
}    


Sptr<SipHeader>
SipSession::duplicate() const
{
    return new SipSession(*this);
}


bool
SipSession::compareSipHeader(SipHeader* msg) const
{
    SipSession* otherMsg = dynamic_cast<SipSession*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
