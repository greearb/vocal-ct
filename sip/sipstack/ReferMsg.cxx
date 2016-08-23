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

#include "ReferMsg.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipReferTo.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipUnsupported.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "global.h"
#include "symbols.hxx"

using namespace Vocal;



ReferMsg::ReferMsg(const string& local_ip)
        : SipCommand(local_ip, "ReferMsg")
{
    myRequestLine.setMethod(REFER_METHOD);
    SipCSeq cseq( SIP_REFER, 0, getLocalIp() );
    setCSeq( cseq );
}

ReferMsg::ReferMsg(const ReferMsg& src)
        : SipCommand(src)
{
}


const ReferMsg& 
ReferMsg::operator =(const ReferMsg& src)
{
    //copy details contained in the base class.
    if ( &src != this)
    {
        *static_cast < SipCommand* > (this) = src;
    }
    return (*this);
}

    
bool 
ReferMsg::operator ==(const ReferMsg& src)
{
    return ( *static_cast <SipCommand*> (this) == src );
}
    
    
ReferMsg::ReferMsg( const StatusMsg& statusMsg, Sptr <BaseUrl> referToUrl,
                    const string& local_ip)
    : SipCommand(local_ip, "ReferMsg")
{
    assert( referToUrl != 0 );

    setTo( statusMsg.getTo() );
    setFrom( statusMsg.getFrom() );

    // Construct Route base on Record-Route
    int numRecordRoute = statusMsg.getNumRecordRoute();
    if( numRecordRoute )
    {
        for( int i=numRecordRoute-1; i >= 0; i-- )
        {
            const SipRecordRoute& rr = statusMsg.getRecordRoute( i );
            SipRoute route("", getLocalIp());
            route.setUrl( rr.getUrl() );
            setRoute( route );
        }
    }

    setReferDetails( statusMsg, referToUrl );
}


ReferMsg::ReferMsg( const SipCommand& sipCommand, Sptr <BaseUrl>  referToUrl)
    : SipCommand(sipCommand.getLocalIp(), "ReferMsg")
{
    assert( referToUrl != 0 );

    // Swap To and From
    const SipFrom& srcFrom = sipCommand.getFrom();
    SipTo to( srcFrom.getUrl(), getLocalIp());
    to.setDisplayName( srcFrom.getDisplayName() );
    to.setTag( srcFrom.getTag() );
    to.setToken( srcFrom.getToken() );
    to.setQstring( srcFrom.getQstring() );
    setTo( to );

    const SipTo& srcTo = sipCommand.getTo();
    SipFrom from( srcTo.getUrl(), getLocalIp());
    from.setDisplayName( srcTo.getDisplayName() );
    from.setTag( srcTo.getTag() );
    from.setToken( srcTo.getToken() );
    from.setQstring( srcTo.getQstring() );
    setFrom( from );

    // Construct Route base on Record-Route
    int numRecordRoute = sipCommand.getNumRecordRoute();
    if( numRecordRoute )
    {
        for( int i=0; i < numRecordRoute-1; i++ )
        {
            const SipRecordRoute& rr = sipCommand.getRecordRoute( i );
            SipRoute route("", getLocalIp());
            route.setUrl( rr.getUrl() );
            setRoute( route );
        }
    }

    setReferDetails( sipCommand, referToUrl );
}


ReferMsg::ReferMsg(const ReferMsg& msg, enum ReferMsgForward)
    : SipCommand(msg)
{
    *this = msg;

    Sptr <BaseUrl> dest = getTo().getUrl();

    int numRoute = getNumRoute();
   
    if (numRoute != 0 )
    {
	dest = getRoute(0).getUrl();
	//remove that route.
	removeRoute(0);
    }
    
    SipRequestLine reqLine = getRequestLine();
    reqLine.setUrl( dest );
    setRequestLine( reqLine );
}


void 
ReferMsg::setReferDetails( const SipMsg& sipMsg, Sptr <BaseUrl> referToUrl )
{
    // Put the far end Contact URI to the end of the Route List
    if( sipMsg.getNumContact() == 1 )
    {
        SipRoute route("", getLocalIp());
        route.setUrl( sipMsg.getContact().getUrl() );
        setRoute( route );
    }
    else
    {
        cpLog( LOG_ERR, "Number of Contact header (%d) is not 1",
                        sipMsg.getNumContact() );
    }

    // Remove the Contact header copied from the other message
    setNumContact( 0 );

    // Copy CallId
    setCallId( sipMsg.getCallId() );

    // Set up the Request URI
    Sptr < BaseUrl > dest = getTo().getUrl();
    if( getNumRoute() > 0 )
    {
        dest = getRoute( 0 ).getUrl();
        // Remove the first route from the Route list
        removeRoute( 0 );
    }

    // Construct request line
    SipRequestLine requestLine( SIP_REFER, dest, getLocalIp());
    setRequestLine( requestLine );

    // Set CSeq to the next number
    SipCSeq newcseq( SIP_REFER, Data(sipMsg.getCSeq().getNextCSeq()), getLocalIp() );
    setCSeq( newcseq );

    // Set Via header
    SipVia sipVia("", getLocalIp());
    sipVia.setprotoVersion( "2.0" );

    Sptr < BaseUrl > myUrl = getFrom().getUrl();
    if( myUrl != 0 )
    {
        Sptr<SipUrl> sipUrl((SipUrl*)(myUrl.getPtr()));
        if( sipUrl != 0 )
        {
            sipVia.setHost( sipUrl->getHost() );
            sipVia.setPort( sipUrl->getPort().convertInt() );
            // Set transport parameter in Via
            if( myRequestLine.getTransportParam().length() )
            {
                if( myRequestLine.getTransportParam() == "tcp" )
                {
                    sipVia.setTransport( "TCP" );
                }
            }
        }
    }
    setVia( sipVia, 0 );

    // Clear content data
    setContentLength( ZERO_CONTENT_LENGTH );

    // Add Refer-To header
    SipReferTo referTo( referToUrl, getLocalIp());
    setReferTo( referTo );

    // TODO: Is this supposed to be done in the stack?
    SipRequire require("", getLocalIp());
    require.set( "cc Refer" );
    setRequire( require );
}


ReferMsg::ReferMsg( const Data& data, const string& local_ip )
    : SipCommand(local_ip, "ReferMsg")
{
    try
    {
        SipCommand::decode(data);
    }
    catch (SipParserException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", e.getDescription().c_str());
    }

}    // ReferMsg::ReferMsg( Data& data )


Method 
ReferMsg::getType() const
{
    return SIP_REFER;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
