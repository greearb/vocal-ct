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

static const char* const SipMsg_cxx_Version = 
    "$Id: SipMsg.cxx,v 1.3 2004/06/02 20:23:10 greear Exp $";

#include "global.h"
#include <sstream>

#include "AckMsg.hxx"
#include "ByeMsg.hxx"
#include "CancelMsg.hxx"
#include "InfoMsg.hxx"
#include "InviteMsg.hxx"
#include "MessageMsg.hxx"
#include "NotifyMsg.hxx"
#include "OptionsMsg.hxx"
#include "ReferMsg.hxx"
#include "RegisterMsg.hxx"
#include "SipAccept.hxx"
#include "SipAcceptEncoding.hxx"
#include "SipAcceptLanguage.hxx"
#include "SipAllow.hxx"
#include "SipAlso.hxx"
#include "SipContact.hxx"
#include "SipContentDisposition.hxx"
#include "SipContentData.hxx"
#include "SipContentLength.hxx"
#include "SipContentType.hxx"
#include "SipDiversion.hxx"
#include "SipEncryption.hxx"
#include "SipExpires.hxx"
#include "SipMethod.hxx"
#include "SipMimeVersion.hxx"
#include "SipMsg.hxx"
#include "SipRecordRoute.hxx"
#include "SipReplaces.hxx"
#include "SipReferTo.hxx"
#include "SipReferredBy.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SipTimestamp.hxx"
#include "SipTransferTo.hxx"
#include "SipUserAgent.hxx"
#include "SipVia.hxx"
#include "StatusMsg.hxx"
#include "SubscribeMsg.hxx"
#include "TransferMsg.hxx"
#include "UnknownExtensionMsg.hxx"
#include "symbols.hxx"         

using namespace Vocal;

    
SipParserException::SipParserException( 
                              const string& msg,
                              const string& file,
                              const int line,
                              const int error /*Default Arguments*/ )
 : VException( msg, file, line, error )
{
}


string
SipParserException::getName( void ) const
{
    return "SipParserException";
}


SipMsg::SipMsg(const string& _local_ip, const char* class_name)
    :
    local_ip(_local_ip),
    className(class_name),
    myVersion(),
    myReceivedAddress("0.0.0.0"),
    mySendAddress("0.0.0.0"),
    myHeaderList(),
    myMimeList(_local_ip),
    myNextHopIsAProxy(false)
{
}


SipMsg::~SipMsg()
{
}


SipMsg& SipMsg::operator=(const SipMsg& rhs)
{
    if ( &rhs != this) 
    {
	myVersion = rhs.myVersion;
	myReceivedAddress = rhs.myReceivedAddress;
        mySendAddress = rhs.mySendAddress;
	myHeaderList = rhs.myHeaderList;
        myMimeList = rhs.myMimeList;
        myNextHopIsAProxy = rhs.myNextHopIsAProxy;
        local_ip = rhs.local_ip;
        className = rhs.className;
    }
    return *(this);
}


bool
SipMsg::operator<(const SipMsg& src) const
{
    cpLog(LOG_INFO,  "not verified!");
    assert(0);
    return false;
}

   
bool
SipMsg::operator==(const SipMsg& src) const
{
    bool equal = ((myVersion == src.myVersion) &&
                  (myReceivedAddress == src.myReceivedAddress) &&
                  (myHeaderList == src.myHeaderList));
    cpLog(LOG_DEBUG_STACK, "SipMsg::operator== returns %s", 
	  equal ? "true" : "false");
    return equal;
}

    
SipMsg::SipMsg(const SipMsg& src)
    :
    local_ip(src.local_ip),
    className(src.className),
    myVersion(src.myVersion),
    myReceivedAddress(src.myReceivedAddress),
    mySendAddress(src.mySendAddress),
    myHeaderList(src.myHeaderList),
    myMimeList(src.myMimeList),
    myNextHopIsAProxy(src.myNextHopIsAProxy)
{ 
}


void 
SipMsg::copyHeader(const SipMsg& src, SipHeaderType type)
{
    Sptr <SipRawHeader> hdr = src.myHeaderList.getHeader(type);
    Sptr <SipRawHeader> copy;
    if(hdr != 0)
    {
        copy = hdr->duplicate();
    }
    myHeaderList.replaceHeader(type, copy);
    myNextHopIsAProxy = src.myNextHopIsAProxy;
}
    

bool 
SipMsg::containsHeader(SipHeaderType type)
{
    return (myHeaderList.getNumHeaders(type) != 0);
}
    

int SipMsg::getNumAccept() const
{
    return myHeaderList.getNumHeaders(SIP_ACCEPT_HDR);
}
    

const SipAccept&
SipMsg::getAccept( int i /*default argument*/) const
{
    Sptr<SipAccept> x;
    myHeaderList.getParsedHeader(x, SIP_ACCEPT_HDR, getLocalIp(), i);
    return *x;
}

/** set or add another Accept item, if the index is -1, it is appended 
        to the current list */

void 
SipMsg::setAccept(const SipAccept& item, int index /*default argument*/ )
{
    Sptr<SipHeader> x = new SipAccept(item);
    myHeaderList.appendHeader(SIP_ACCEPT_HDR, x, getLocalIp(), index);
}
    
    
/** set or add another Accept item, if the index is -1, it is appended 
        to the current list */

void 
SipMsg::setAccept(const Data&item, int index /*default argument*/ )
{
    Sptr<SipHeader> x = new SipAcceptLanguage(item, getLocalIp());
    myHeaderList.appendHeader(SIP_ACCEPT_LANGUAGE_HDR, x, getLocalIp(), index);
}
    


/** Set number of Accept items.
	If i is less than current number then the extras are deleted. */
void 
SipMsg::setNumAccept(int index)
{
    myHeaderList.setNumHeaders(SIP_ACCEPT_HDR, index);
}

    
int 
SipMsg::getNumAcceptEncoding() const
{ 
    return myHeaderList.getNumHeaders(SIP_ACCEPT_ENCODING_HDR);
}

const SipAcceptEncoding&
SipMsg::getAcceptEncoding( int i /*default argument*/ ) const
{
    Sptr<SipAcceptEncoding> x;
    myHeaderList.getParsedHeader(x, SIP_ACCEPT_ENCODING_HDR, getLocalIp(), i);
    return *x;
}

/** set or add another AcceptEncoding itme, if the index is -1, it is appended 
        to the current list */

void 
SipMsg::setAcceptEncoding(const Data& item, int index /*default argument*/)
{
    Sptr<SipHeader> x = new SipAcceptEncoding(item, getLocalIp());
    myHeaderList.appendHeader(SIP_ACCEPT_ENCODING_HDR, x, getLocalIp(), index); 
}
    

/** set or add another AcceptEncoding itme, if the index is -1, it is appended 
        to the current list */

void 
SipMsg::setAcceptEncoding(const SipAcceptEncoding& item, 
                          int index /*default argument*/)
{
    Sptr<SipHeader> x = new SipAcceptEncoding(item);
    myHeaderList.appendHeader(SIP_ACCEPT_ENCODING_HDR, x, getLocalIp(), index);
}
    


/** Set number of AcceptEncoding items.
	If i is less than current number then the extras are deleted. */
void 
SipMsg::setNumAcceptEncoding(int index)
{
    myHeaderList.setNumHeaders(SIP_ACCEPT_ENCODING_HDR, index);
}


void 
SipMsg::flushAcceptEncodingList()
{
    myHeaderList.setNumHeaders(SIP_ACCEPT_ENCODING_HDR, 0);
}

    
/// get the number of AcceptLanguage items 
int 
SipMsg::getNumAcceptLanguage() const
{
    return myHeaderList.getNumHeaders(SIP_ACCEPT_LANGUAGE_HDR);

}


/// Get the i'th AcceptLanguage item. If i is -1, it gets the last one 
const SipAcceptLanguage&
SipMsg::getAcceptLanguage( int i /*default argument*/ ) const
{
    
    Sptr<SipAcceptLanguage> x;
    myHeaderList.getParsedHeader(x, SIP_ACCEPT_LANGUAGE_HDR, getLocalIp(), i);
    return *x;
}


/** set or add another AcceptLanguage itme, if the index is -1, it is appended 
 to the current list */
void 
SipMsg::setAcceptLanguage(const SipAcceptLanguage& item, 
                          int index /*default argument*/)
{
    Sptr<SipHeader> x = new SipAcceptLanguage(item);
    myHeaderList.appendHeader(SIP_ACCEPT_LANGUAGE_HDR, x, getLocalIp(), index);
}
    
    
void 
SipMsg::setAcceptLanguage(const Data& item, int index /*default argument*/)
{
    Sptr<SipHeader> x = new SipAcceptLanguage(item, getLocalIp());
    myHeaderList.appendHeader(SIP_ACCEPT_LANGUAGE_HDR, x, getLocalIp(), index);
    
}    


/** Set number of AcceptLanguage items.
	If i is less than current number then the extras are deleted. */
void 
SipMsg::setNumAcceptLanguage(int index)
{
    myHeaderList.setNumHeaders(SIP_ACCEPT_LANGUAGE_HDR, index);
}


void 
SipMsg::flushAcceptLanguageList()
{
   myHeaderList.setNumHeaders(SIP_ACCEPT_LANGUAGE_HDR, 0);
}


/*--------------------------- SipSubsNotifyEvent ---------------------------*/


int 
SipMsg::getNumSubsNotifyEvent() const 
{
    return myHeaderList.getNumHeaders(SIP_EVENT_HDR);
}


const SipSubsNotifyEvent&
SipMsg::getSubsNotifyEvent(int i) const
{
    Sptr<SipSubsNotifyEvent> x;
    myHeaderList.getParsedHeader(x, SIP_EVENT_HDR, getLocalIp(), i);
    return *x;
}


void 
SipMsg::setSubsNotifyEvent(const SipSubsNotifyEvent& item, int index)
{
    Sptr<SipHeader> x = new SipSubsNotifyEvent(item);
    myHeaderList.appendHeader(SIP_EVENT_HDR, x, getLocalIp(), index);
}


void 
SipMsg::setSubsNotifyEvent(const Data& item, int index)
{
    Sptr<SipHeader> x = new SipSubsNotifyEvent(item, getLocalIp());
    myHeaderList.appendHeader(SIP_EVENT_HDR, x, getLocalIp(), index);
}


void 
SipMsg::removeSubsNotifyEvent(int index)
{
    myHeaderList.removeHeader(SIP_EVENT_HDR, index);
}


void 
SipMsg::setNumSubsNotifyEvent(int i)
{
    myHeaderList.setNumHeaders(SIP_EVENT_HDR, i);
}


void 
SipMsg::flushSubsNotifyEvent()
{
    myHeaderList.setNumHeaders(SIP_EVENT_HDR, 0);
}


void 
SipMsg::copySubsNotifyEvent(const SipMsg& srcObject)
{
    Sptr <SipRawHeader> hdr 
        = srcObject.myHeaderList.getHeader(SIP_EVENT_HDR);
    Sptr <SipRawHeader> copy;
    if(hdr != 0)
    {
        copy = hdr->duplicate();
    }
    myHeaderList.replaceHeader(SIP_EVENT_HDR, copy);
}


// ----------------- CSeq Header Methods ------------------
/// Get the current CSeq header 
const SipCSeq&
SipMsg::getCSeq() const
{
    Sptr<SipCSeq> cseq;
    myHeaderList.getParsedHeader(cseq, SIP_CSEQ_HDR, getLocalIp());
    return *cseq;
}
    

/// Set the CSeq header 
void 
SipMsg::setCSeq( const SipCSeq& newcseq)
{
    Sptr<SipCSeq> cseq;
    myHeaderList.getParsedHeader(cseq, SIP_CSEQ_HDR, getLocalIp());
    *cseq = newcseq;
}

    
void 
SipMsg::setCSeq( const Data& newcseq)
{
    Sptr<SipCSeq> cseq;
    myHeaderList.getParsedHeader(cseq, SIP_CSEQ_HDR, getLocalIp());
    cseq->decode(newcseq);
}    

void
SipMsg::incrementCSeq()
{
    Sptr<SipCSeq> cseq;
    myHeaderList.getParsedHeader(cseq, SIP_CSEQ_HDR, getLocalIp());
    cseq->incrCSeq();
}


// ----------------- CallId Header Methods ------------------
/// Get the current CallId header 
const SipCallId& 
SipMsg::getCallId() const
{
    Sptr<SipCallId> callId;
    myHeaderList.getParsedHeader(callId, SIP_CALLID_HDR, getLocalIp());
    return *callId;
}


/// Set the CallId header 
void 
SipMsg::setCallId( const SipCallId& newcallId)
{
    Sptr<SipCallId> callId;
    myHeaderList.getParsedHeader(callId, SIP_CALLID_HDR, getLocalIp());
    *callId = newcallId;
}

    
void 
SipMsg::setCallId( const Data& data)
{
    Sptr<SipCallId> callId;
    myHeaderList.getParsedHeader(callId, SIP_CALLID_HDR, getLocalIp());
    callId->decode(data);
}
  
  
// ----------------- Also Header Methods ------------------
/// Get the current Also header 
const SipAlso& 
SipMsg::getAlso() const
{
    Sptr<SipAlso> also;
    myHeaderList.getParsedHeader(also, SIP_ALSO_HDR, getLocalIp());
    return *also;
}


/// Set the Also header 
void 
SipMsg::setAlso( const SipAlso& newalso)
{
    Sptr<SipAlso> also;
    myHeaderList.getParsedHeader(also, SIP_ALSO_HDR, getLocalIp());
    *also = newalso;
}
    

/// Set the Also header 
void 
SipMsg::setAlso( const Data& newalso)
{
    Sptr<SipAlso> also;
    myHeaderList.getParsedHeader(also, SIP_ALSO_HDR, getLocalIp());
    also->decode(newalso);
}

    
// ----------------- CallLeg Header Methods ------------------
/// Get the current CallLeg header 
SipCallLeg
SipMsg::computeCallLeg() const
{
    SipCallLeg callLeg(getFrom(), getTo(), getCallId(), getLocalIp());
    return callLeg;
//form callleg from from, to , callid and return.
}


int 
SipMsg::getNumContact() const
{
    return myHeaderList.getNumHeaders(SIP_CONTACT_HDR);
}


const SipContact&
SipMsg::getContact( int i /*default argument*/) const
{
    Sptr<SipContact> x;
    myHeaderList.getParsedHeader(x, SIP_CONTACT_HDR, getLocalIp(), i);
    return *x;
}


void 
SipMsg::setContact(const SipContact& item, int index /*default argument*/ )
{
    Sptr<SipHeader> x = new SipContact(item);
    myHeaderList.appendHeader(SIP_CONTACT_HDR, x, getLocalIp(), index);
}

void 
SipMsg::setContact(const Data& item, int index /*default argument*/ )
{
    Sptr<SipHeader> x = new SipContact(item, getLocalIp());
    myHeaderList.appendHeader(SIP_CONTACT_HDR, x, getLocalIp(), index);
}
    

void 
SipMsg::setNumContact(int index)
{
    myHeaderList.setNumHeaders(SIP_CONTACT_HDR, index);
}


void
SipMsg::flushContact()
{
    while (getNumContact())
    {
        removeContact(-1);
    }
}


SipContactList 
SipMsg::getContactList() const
{
    SipContactList tmp;
    myHeaderList.getHeaderList(&tmp, SIP_CONTACT_HDR);
    return tmp;
}
    

// ----------------- Date Header Methods ------------------
/// Get the current Date header 
const SipDate&
SipMsg::getDate() const
{
    Sptr<SipDate> date;
    myHeaderList.getParsedHeader(date, SIP_DATE_HDR, getLocalIp());
    return *date;
}
    
    
/// Set the Date header 
void 
SipMsg::setDate( const SipDate& newdate)
{
    Sptr<SipDate> date;
    myHeaderList.getParsedHeader(date, SIP_DATE_HDR, getLocalIp());
    *date = newdate;
}


/// Set the Date header 
void 
SipMsg::setDate( const Data& newdate)
{
    Sptr<SipDate> date;
    myHeaderList.getParsedHeader(date, SIP_DATE_HDR, getLocalIp());
    date->decode(newdate);
}


// ----------------- Encryption Header Methods ------------------
/// Get the current Encryption header 
const SipEncryption&
SipMsg::getEncryption() const
{
    Sptr<SipEncryption> encryption;
    myHeaderList.getParsedHeader(encryption, SIP_ENCRYPTION_HDR, getLocalIp());
    return *encryption;
}


/// Set the Encryption header 
void  
SipMsg::setEncryption(const SipEncryption&  newencryption)
{
    Sptr<SipEncryption> encryption;
    myHeaderList.getParsedHeader(encryption, SIP_ENCRYPTION_HDR, getLocalIp());
    *encryption = newencryption;
}
    

/// Set the Encryption header 
void  
SipMsg::setEncryption(const Data&  newencryption)
{
    Sptr<SipEncryption> encryption;
    myHeaderList.getParsedHeader(encryption, SIP_ENCRYPTION_HDR, getLocalIp());
    encryption->decode(newencryption);
}  
    

// ----------------- Expires Header Methods ------------------
/// Get the current Expires header 
const SipExpires& 
SipMsg::getExpires() const
{
    Sptr<SipExpires> expires;
    myHeaderList.getParsedHeader(expires, SIP_EXPIRES_HDR, getLocalIp());
    return *expires;
}

    
/// Set the Expires header 
void 
SipMsg::setExpires( const SipExpires& newexpires)
{
    Sptr<SipExpires> expires;
    myHeaderList.getParsedHeader(expires, SIP_EXPIRES_HDR, getLocalIp());
    *expires = newexpires;
}
    
    
/// Set the Expires header 
void 
SipMsg::setExpires( const Data& newexpires)
{
    Sptr<SipExpires> expires;
    myHeaderList.getParsedHeader(expires, SIP_EXPIRES_HDR, getLocalIp());
    expires->decode(newexpires);
}  
    

// ----------------- From Header Methods ------------------
/// Get the current From header 
const SipFrom&
SipMsg::getFrom() const
{
    Sptr<SipFrom> from;
    myHeaderList.getParsedHeader(from, SIP_FROM_HDR, getLocalIp());
    return *from;
}
    
    
const SipMimeVersion&
SipMsg::getMimeVersion() const
{
    Sptr<SipMimeVersion> mimeVersion;
    myHeaderList.getParsedHeader(mimeVersion, SIP_MIME_VERSION_HDR, getLocalIp());
    return *mimeVersion;
}
    
    
void 
SipMsg::setMimeVersion( const SipMimeVersion& newmv)
{
    Sptr<SipMimeVersion> mimeVersion;
    myHeaderList.getParsedHeader(mimeVersion, SIP_MIME_VERSION_HDR, getLocalIp());
    *mimeVersion = newmv;
}   
    
    
void 
SipMsg::setMimeVersion( const Data& newmv)
{
    Sptr<SipMimeVersion> mimeVersion;
    myHeaderList.getParsedHeader(mimeVersion, SIP_MIME_VERSION_HDR, getLocalIp());
    mimeVersion->decode(newmv);
}


/// Set the From header 
void 
SipMsg::setFrom( const SipFrom& newfrom)
{
    Sptr<SipFrom> from;
    myHeaderList.getParsedHeader(from, SIP_FROM_HDR, getLocalIp());
    *from = newfrom;
}
    
    
void 
SipMsg::setFrom( const Data& formData)
{
    Sptr<SipFrom> from;
    myHeaderList.getParsedHeader(from, SIP_FROM_HDR, getLocalIp());
    from->decode(formData);
}


// ----------------- RecordRoute Header Methods  -----------------


void 
SipMsg::copyRecordRoute( const SipMsg& srcMsg) 
{
    Sptr <SipRawHeader> hdr 
        = srcMsg.myHeaderList.getHeader(SIP_RECORD_ROUTE_HDR);
    Sptr <SipRawHeader> copy;
    if(hdr != 0)
    {
        copy = hdr->duplicate();
    }
    myHeaderList.replaceHeader(SIP_RECORD_ROUTE_HDR, copy);
}


void 
SipMsg::flushrecordrouteList()
{
    myHeaderList.setNumHeaders(SIP_RECORD_ROUTE_HDR, 0);
}


SipRecordRouteList 
SipMsg::getrecordrouteList() const
{
    SipRecordRouteList tmp;
    myHeaderList.getHeaderList(&tmp, SIP_RECORD_ROUTE_HDR);
    return tmp;
}


int 
SipMsg::getNumRecordRoute() const
{
    return myHeaderList.getNumHeaders(SIP_RECORD_ROUTE_HDR);
}


const SipRecordRoute&
SipMsg::getRecordRoute( int i /*default Arguments*/) const
{
    Sptr<SipRecordRoute> x;
    myHeaderList.getParsedHeader(x, SIP_RECORD_ROUTE_HDR, getLocalIp(), i);
    return *x;
}


void 
SipMsg::setRecordRoute(const SipRecordRoute& item, int index /*Default Arguments*/)
{
    Sptr<SipHeader> x = new SipRecordRoute(item);
    myHeaderList.appendHeader(SIP_RECORD_ROUTE_HDR, x, getLocalIp(), index);
}


void 
SipMsg::setRecordRoute(const Data& item, int index /*Default Arguments*/)
{
    Sptr<SipHeader> x = new SipRecordRoute(item, getLocalIp());
    myHeaderList.appendHeader(SIP_RECORD_ROUTE_HDR, x, getLocalIp(), index);
}


void 
SipMsg::setNumRecordRoute(int index)
{
    myHeaderList.setNumHeaders(SIP_RECORD_ROUTE_HDR, index);
}


bool 
SipMsg::compareRecordRoute(const SipMsg& src) const
{
    int srcRecordRoutesize = src.getNumRecordRoute();

    if ( srcRecordRoutesize != getNumRecordRoute())
    {
        return false;
    }
    else
    {
        for (int i = 0; i < srcRecordRoutesize; i++)
	{
	    if ( !(getRecordRoute(i) == src.getRecordRoute(i)) )
	    {
	        return false;
	    }
	}
	return true;
    }
}



//
void 
SipMsg::recordRoutePushFront(const SipRecordRoute& item)
{
    int n = getNumRecordRoute();
    setRecordRoute(item, 0);
    assert (getNumRecordRoute() == n+1);
}

const SipRecordRoute&
SipMsg::recordRouteFront() const
{
    assert(!recordRouteEmpty());
    return getRecordRoute(0);
}

const SipRecordRoute& 
SipMsg::recordRouteBack() const
{
    assert(!recordRouteEmpty());
    return getRecordRoute(-1);
}

bool
SipMsg::recordRouteEmpty() const
{
    return getNumRecordRoute() == 0;
}


// ----------------- Timestamp Header Methods ------------------
const SipTimestamp&
SipMsg::getTimestamp() const
{
    Sptr<SipTimestamp> timestamp;
    myHeaderList.getParsedHeader(timestamp, SIP_TIMESTAMP_HDR, getLocalIp());
    return *timestamp;    
    
}


void 
SipMsg::setTimestamp( const SipTimestamp& newtimestamp)
{
    Sptr<SipTimestamp> timestamp;
    myHeaderList.getParsedHeader(timestamp, SIP_TIMESTAMP_HDR, getLocalIp());
    *timestamp = newtimestamp;
    
}

    
/// Set the Timestamp header 
void 
SipMsg::setTimestamp( const Data& newtimestamp)
{
    Sptr<SipTimestamp> timestamp;
    myHeaderList.getParsedHeader(timestamp, SIP_TIMESTAMP_HDR, getLocalIp());
    timestamp->decode(newtimestamp);
}



// ----------------- To Header Methods ------------------
/// Get the current To header 
const SipTo&
SipMsg::getTo() const
{
    Sptr<SipTo> to;
    myHeaderList.getParsedHeader(to, SIP_TO_HDR, getLocalIp());
    return *to;
}


/// Set the To header 
void 
SipMsg::setTo( const SipTo& newto)
{
    Sptr<SipTo> to;
    myHeaderList.getParsedHeader(to, SIP_TO_HDR, getLocalIp());
    *to = newto;
}


void 
SipMsg::setTo( const Data& toData)
{
    Sptr<SipTo> to;
    myHeaderList.getParsedHeader(to, SIP_TO_HDR, getLocalIp());
    to->decode(toData);
}


// ----------------- Via Header Methods  -----------------
/// get the number of Via items 
int 
SipMsg::getNumVia() const
{
    return myHeaderList.getNumHeaders(SIP_VIA_HDR);
}


/// Get the i'th Via item. If i is -1, it gets the last one 
const SipVia&
SipMsg::getVia( int i /*default Arguments*/) const
{
    Sptr<SipVia> x;
    myHeaderList.getParsedHeader(x, SIP_VIA_HDR, getLocalIp(), i);
    return *x;
}

void 
SipMsg::removeVia(int index /*deafult Arguments*/)
{
    myHeaderList.removeHeader(SIP_VIA_HDR, index);
}


/** set or add another Via item, if the index is -1, it is appended 
    to the current list */

void 
SipMsg::setVia(const SipVia& item, int index /* Default Arguments*/)
{
    Sptr<SipHeader> x = new SipVia(item);
    myHeaderList.appendHeader(SIP_VIA_HDR, x, local_ip, index);
}

void 
SipMsg::setVia(const Data& item, int index /* Default Arguments*/)
{
    Sptr<SipHeader> x = new SipVia(item, getLocalIp());
    myHeaderList.appendHeader(SIP_VIA_HDR, x, local_ip, index);
}


/** Set number of Via items.
If i is less than current number then the extras are deleted. */
void 
SipMsg::setNumVia(int index)
{
    myHeaderList.setNumHeaders(SIP_VIA_HDR, index);
}


void 
SipMsg::flushViaList()
{
    myHeaderList.setNumHeaders(SIP_VIA_HDR, 0);
}


void 
SipMsg::copyViaList( const SipMsg& srcMsg) 
{
    Sptr <SipRawHeader> hdr = srcMsg.myHeaderList.getHeader(SIP_VIA_HDR);
    Sptr <SipRawHeader> copy;
    if(hdr != 0)
    {
        copy = hdr->duplicate();
    }
    myHeaderList.replaceHeader(SIP_VIA_HDR, copy);
}


SipViaList 
SipMsg::getViaList()  const
{
    SipViaList tmp;
    myHeaderList.getHeaderList(&tmp, SIP_VIA_HDR);
    return tmp;
}

bool
SipMsg::isLoopInViaList(const Data& myHost, int myPort, int limit) const
{
    int count = 0;  // number of times in Via
    SipViaList viaList;
    myHeaderList.getHeaderList(&viaList, SIP_VIA_HDR);
    if ( !viaList.empty() )
    {
        Data hostAddress;
        unsigned short port;
#if defined(__SUNPRO_CC)
        for( SipViaList::iterator iter = viaList.begin();
             !(iter == viaList.end()); iter++ )
#else
        for( SipViaList::iterator iter = viaList.begin();
             iter != viaList.end(); iter++ )
#endif
        {
            hostAddress = ( *iter )->getHost();
            port = ( *iter )->getPort();
            if ( ( port == myPort) &&
	         ( hostAddress == myHost ) )
            {
                // found self in list, ignore to avoid loops
                cpLog( LOG_DEBUG, "found self in Via list" );
                count++;
		if(count >= limit) break;
            }
        }
    }
    else
    {
        cpLog( LOG_DEBUG, "Via list is empty" );
    }

    if(count >= limit) return true;

    return false;
}


void 
SipMsg::viaPushFront(const SipVia& item)
{
    setVia(item, 0);
}

const SipVia& 
SipMsg::viaFront() const
{
    assert(!viaEmpty());
    return getVia(0);
}

void
SipMsg::viaPopFront() 
{
    assert(!viaEmpty());    
    removeVia(0);
}

bool 
SipMsg::viaEmpty() const
{
    return getNumVia() == 0;
}


    
void 
SipMsg::removeContact(int index /*default Arguments*/)
{
    myHeaderList.removeHeader(SIP_CONTACT_HDR, index);
}
    
    
/*-----------------------------------------------------------------------*/
//Diversion header details
//  getNumDiversion
//  removeDiversion
//  addDiversion
//  copyDiversionList
//  getDiversionList
//  flushDiversionList
//  encodeDiversionList
//  ltDiversionList
//  compareDiversionList
//  setDiversion
//  setNumDiversion
    

int 
SipMsg::getNumDiversion() const
{
    return myHeaderList.getNumHeaders(SIP_DIVERSION_HDR);
}


void 
SipMsg::removeDiversion(int index /* Default Arguement */)
{
    myHeaderList.removeHeader(SIP_DIVERSION_HDR, index);
}


SipDiversionList 
SipMsg::getDiversionList() const
{
    SipDiversionList tmp;
    myHeaderList.getHeaderList(&tmp, SIP_DIVERSION_HDR);
    return tmp;
} 


void 
SipMsg::copyDiversionList( const SipMsg& srcMsg) 
{
    myHeaderList.replaceHeader(
        SIP_DIVERSION_HDR, 
        srcMsg.myHeaderList.getHeader(SIP_DIVERSION_HDR));
}


void 
SipMsg::flushDiversionList()
{
    myHeaderList.setNumHeaders(SIP_DIVERSION_HDR, 0);
}


const SipDiversion&
SipMsg::getDiversion( int i /*Default Arguments*/) const
{
    Sptr<SipDiversion> x;
    myHeaderList.getParsedHeader(x, SIP_DIVERSION_HDR, getLocalIp(), i);
    return *x;
} 


void 
SipMsg::setDiversion(const SipDiversion& item, int index /*Default Arguments*/)
{
     Sptr<SipHeader> x = new SipDiversion(item);
     myHeaderList.appendHeader(SIP_DIVERSION_HDR, x, local_ip, index);
}


void 
SipMsg::setNumDiversion(int index)
{
    myHeaderList.setNumHeaders(SIP_DIVERSION_HDR, index);
}  


// ----------------------------------------------------------------------
    
void 
SipMsg::setVersion(const Data& ver)
{
    version = ver;
}


Data 
SipMsg::getVersion() const 
{
    return version;
}


// ----------------------------------------------------------------------


bool 
SipMsg::isRetry() const
{
    return retry;
}


// ----------------------------------------------------------------------

    
const SipRetryAfter&
SipMsg::getRetryAfter() const
{
    Sptr<SipRetryAfter> retryAfter;
    myHeaderList.getParsedHeader(retryAfter, SIP_RETRY_AFTER_HDR, getLocalIp());
    return *retryAfter;
}	

    
void 
SipMsg::setRetryAfter( const SipRetryAfter& newretry)
{
    Sptr<SipRetryAfter> retryAfter;
    myHeaderList.getParsedHeader(retryAfter, SIP_RETRY_AFTER_HDR, getLocalIp());
    *retryAfter  = newretry;
    retry = true;
}
    
    
void 
SipMsg::setRetryAfter( const Data& newretry)
{
    Sptr<SipRetryAfter> retryAfter;
    myHeaderList.getParsedHeader(retryAfter, SIP_RETRY_AFTER_HDR, getLocalIp());
    retryAfter->decode(newretry);
    retry = true;
}


// ----------------------------------------------------------------------
    
void
SipMsg::setSendAddress(NetworkAddress & vaddress)
{
    mySendAddress = vaddress;
}
const NetworkAddress &
SipMsg::getSendAddress() const
{
    return  mySendAddress;
}
const Data& 
SipMsg::getReceivedIPName() const
{
    return recvdIPName;
}


void 
SipMsg::setReceivedIPName(const Data& ipName)
{
    if (ipName.length())
    {
        recvdIPName = ipName;
//        assert(strstr(recvdIPName.c_str(), ":") == 0);
    }
}


const Data&  
SipMsg::getReceivedIPPort()
{
    return  recvdIPPort;
}
    

void  
SipMsg::setReceivedIPPort(const Data& ipPort)
{
    recvdIPPort = ipPort;
}


// ----------------------------------------------------------------------


const SipTransferTo&
SipMsg::getTransferTo() const
{
    Sptr<SipTransferTo> transferTo;
    myHeaderList.getParsedHeader(transferTo, SIP_TRANSFER_TO_HDR, getLocalIp());
    return *transferTo;
}


void 
SipMsg::setTransferTo(const SipTransferTo& transTo)
{
    Sptr<SipTransferTo> transferTo;
    myHeaderList.getParsedHeader(transferTo, SIP_TRANSFER_TO_HDR, getLocalIp());
    *transferTo = transTo;
}
    

void 
SipMsg::setTransferTo(const Data& transTo)
{
    Sptr<SipTransferTo> transferTo;
    myHeaderList.getParsedHeader(transferTo, SIP_TRANSFER_TO_HDR, getLocalIp());
    transferTo->decode(transTo);
} 


// ----------------------------------------------------------------------


void 
SipMsg::setReferredBy(const SipReferredBy& newreferredBy)
{
    Sptr<SipReferredBy> referredBy;
    myHeaderList.getParsedHeader(referredBy, SIP_REFERRED_BY_HDR, getLocalIp());
    *referredBy = newreferredBy;
}

    
void 
SipMsg::setReferredBy(const Data& newreferredBy)
{
    Sptr<SipReferredBy> referredBy;
    myHeaderList.getParsedHeader(referredBy, SIP_REFERRED_BY_HDR, getLocalIp());
    referredBy->decode(newreferredBy);
}

bool
SipMsg::hasReferredBy() const
{
    return myHeaderList.getNumHeaders(SIP_REFERRED_BY_HDR);
}
    
const SipReferredBy&
SipMsg::getReferredBy() const
{
    Sptr<SipReferredBy> referredBy;
    myHeaderList.getParsedHeader(referredBy, SIP_REFERRED_BY_HDR, getLocalIp());
    return *referredBy;
}


// ----------------------------------------------------------------------


void 
SipMsg::setReferTo(const SipReferTo& newreferTo)
{
    Sptr<SipReferTo> referTo;
    myHeaderList.getParsedHeader(referTo, SIP_REFER_TO_HDR, getLocalIp());
    *referTo = newreferTo;
}
    

void 
SipMsg::setReferTo(const Data& newreferTo)
{
    Sptr<SipReferTo> referTo;
    myHeaderList.getParsedHeader(referTo, SIP_REFER_TO_HDR, getLocalIp());
    referTo->decode(newreferTo);
}   
   
    
const SipReferTo&
SipMsg::getReferTo() const
{
    Sptr<SipReferTo> referTo;
    myHeaderList.getParsedHeader(referTo, SIP_REFER_TO_HDR, getLocalIp());
    return *referTo;
}


// ----------------------------------------------------------------------


void 
SipMsg::setReplaces(const SipReplaces& newreplaces)
{
    Sptr<SipReplaces> replaces;
    myHeaderList.getParsedHeader(replaces, SIP_REPLACES_HDR, getLocalIp());
    *replaces = newreplaces;
}
    

void 
SipMsg::setReplaces(const Data& newreplaces)
{
    Sptr<SipReplaces> replaces;
    myHeaderList.getParsedHeader(replaces, SIP_REPLACES_HDR, getLocalIp());
    replaces->decode(newreplaces);
}   
   
    
const SipReplaces&
SipMsg::getReplaces() const
{
    Sptr<SipReplaces> replaces;
    myHeaderList.getParsedHeader(replaces, SIP_REPLACES_HDR, getLocalIp());
    return *replaces;
}



// ----------------------------------------------------------------------


const SipUserAgent&
SipMsg::getUserAgent() const
{
    Sptr<SipUserAgent> userAgent;
    myHeaderList.getParsedHeader(userAgent, SIP_USER_AGENT_HDR, getLocalIp());
    return *userAgent;
}

    
void SipMsg::setUserAgent( const SipUserAgent& newUserAgent)
{
    Sptr<SipUserAgent> userAgent;
    myHeaderList.getParsedHeader(userAgent, SIP_USER_AGENT_HDR, getLocalIp());
    *userAgent = newUserAgent;
}

    
void SipMsg::setUserAgent( const Data& newUserAgent)
{
    Sptr<SipUserAgent> userAgent;
    myHeaderList.getParsedHeader(userAgent, SIP_USER_AGENT_HDR, getLocalIp());
    userAgent->decode(newUserAgent);
}


// ----------------------------------------------------------------------

    
void SipMsg::flushAllowList()
{
    myHeaderList.setNumHeaders(SIP_ALLOW_HDR, 0);
    
}


void 
SipMsg::addAllow(SipAllow* sipallow)
{
    assert(0); // this will not work, but is needed!
}

    
void 
SipMsg::removeAllow(int index /*deafult Arguments*/)
{
    myHeaderList.removeHeader(SIP_ALLOW_HDR, index);
}


const SipAllow&
SipMsg::getAllow( int i /* Default Arguments */) const
{
    Sptr<SipAllow> x;
    myHeaderList.getParsedHeader(x, SIP_ALLOW_HDR, getLocalIp(), i);
    return *x;
}


void 
SipMsg::setAllow(const SipAllow& item, int index /* Default Arguments */ )
{
    Sptr<SipHeader> x = new SipAllow(item);
    myHeaderList.appendHeader(SIP_ALLOW_HDR, x, local_ip, index);
}

    
void 
SipMsg::setAllow(const Data&item, int index /* Default Arguments */ )
{
    Sptr<SipHeader> x = new SipAllow(item, getLocalIp());
    myHeaderList.appendHeader(SIP_ALLOW_HDR, x, local_ip, index);
}


int 
SipMsg::getNumAllow() const
{
     return myHeaderList.getNumHeaders(SIP_ALLOW_HDR);
}


void 
SipMsg::setNumAllow(int index)
{
    myHeaderList.setNumHeaders(SIP_ALLOW_HDR, index);
}


// ----------------------------------------------------------------------


#if 0    
Method
SipMsg::getMethod(const Data& method)
{
    Method type = SIP_UNKNOWN;

    if (method == INVITE_METHOD)
    {
	type = SIP_INVITE;
    }
    else if (method == CANCEL_METHOD)
    {
	type = SIP_CANCEL;
    }
     else if (method == REFER_METHOD)
    {
	type = SIP_REFER;
    }
    else if (method == ACK_METHOD)
    {
	type = SIP_ACK;
    }
    else if (method == BYE_METHOD)
    {
	type = SIP_BYE;
    }
    else if (method == INFO_METHOD)
    {
	type = SIP_INFO;
    }
    else if (method == OPTIONS_METHOD)
    {
	type = SIP_OPTIONS;
    }
    else if (method == REGISTER_METHOD)
    {
	type = SIP_REGISTER;
    }
    else if (method == TRANSFER_METHOD)
    {
        type = SIP_TRANSFER;
    }
    else if (method == SUBSCRIBE_METHOD)
    {
	type = SIP_SUBSCRIBE;
    }
    else if (method == NOTIFY_METHOD)
    {
	type = SIP_NOTIFY;
    }
    else if (method == "SIP/2.0")
    {
        type = SIP_STATUS;
    }
    
    return type;
}
#endif


bool
SipMsg::parse(Data msg)
{
    bool retVal = false;
    retVal = parseHeaders(msg);

    // at this point, msg should only contain the body, if any
    parseMime(msg);
    return retVal;
}
    
    
bool 
SipMsg::parseHeaders(Data& msg)
{
    return myHeaderList.decode(&msg, local_ip);
}
    

void
SipMsg::parseMime(Data msg)
{
    //need to pass contentlength, contenttype, contentdisposition.
    Sptr < SipContentLength> contlen = 
        new SipContentLength(getContentLength());
    Sptr <SipContentType> contType = 
        new SipContentType(getContentType());
    Sptr <SipContentDisposition> contDisp = 
        new SipContentDisposition(getContentDisposition());
    
    myMimeList.decode(msg,contlen, contType, contDisp);
}


Sptr<SipMsg>
SipMsg::decode(Data data, const string& local_ip)
{
    // skip any leading whitespace
    int i = 0;
    int maxlen = data.length();
    bool done = false;
    while(i < maxlen && !done)
    {
        char c = data[i];
        switch(c)
        {
        case ' ':
        case '\r':
        case '\n':
        case '\t':
            ++i;
            break;
        default:
            done = true;
        }
    }

    data = data.substring(i, -1); // this is not implemented, so ....

    Sptr<SipMsg> sipMsg;
    cpLog(LOG_DEBUG_STACK,"Going to decode a sip Msg");
    
    data.removeLWS();
    //new parser hooks.
    data.removeSpaces();
    
    //call expand.
    
    int sipMethod;

    Data tmp = data;
    Data startline;

    bool getLineFailed = false;

    startline = tmp.getLine(&getLineFailed);

    if (getLineFailed) {
	// this is a bad message
	cpLog(LOG_DEBUG_STACK, "Parse failed.  no newlines found in message");
	return sipMsg;
    }

    char matchedChar = '\0';

    Data method = startline.matchChar(" \t", &matchedChar);

    sipMethod  = dataToMethod(method);

    if (sipMethod == SIP_UNKNOWN) {
        cpLog(LOG_DEBUG_STACK, "failed to match method '%s'", 
              method.logData());
    }

    if (sipMethod != SIP_STATUS) {
        // make sure this is really a SIP message
        matchedChar = '\0';
        Data uri = startline.matchChar(" \t", &matchedChar);
        
        if (matchedChar == '\0') {
            cpLog(LOG_DEBUG_STACK, "Could not split off protocol/version");
            return 0;
        }
        Data version = startline;
        matchedChar = '\0';
        Data protocol = version.matchChar("/", &matchedChar);
        if (matchedChar == '\0') {
            cpLog(LOG_DEBUG_STACK, "Could not find protocol version");
            return 0;
        }
        if ( !isEqualNoCase(protocol, "sip")) {
            cpLog(LOG_DEBUG_STACK, "protocol is not SIP");
            return 0;
        }
    }

    switch (sipMethod) {
    case SIP_INVITE:
	try {
	    sipMsg = new InviteMsg(data, local_ip);
	}
	catch (SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the invite msg");
	}
	break;
    case SIP_BYE:
	try {
	    sipMsg = new ByeMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the bye msg");
	}
	break;
    case SIP_CANCEL:
	try {
	    sipMsg = new CancelMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the cancel msg");
	}
	break;
    case SIP_ACK:
	try {
	    sipMsg = new AckMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the ack msg");	
	}
	break;
    case SIP_STATUS:
	try {
	    sipMsg = new StatusMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the status msg");
	}
	break;
    case SIP_INFO:
	try {
	    sipMsg = new InfoMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the info msg");	
	}
	break;
    case SIP_OPTIONS:
	try {
	    sipMsg = new OptionsMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the options msg");
	}
	break;
    case SIP_REGISTER:
	try {
	    sipMsg = new RegisterMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the register msg");
	}
	break;
    case SIP_TRANSFER:
	try {
	    sipMsg = new TransferMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the transfer msg");
	}
	break;
    case SIP_SUBSCRIBE:
	try {
	    sipMsg = new SubscribeMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the subscribe msg");
	}
	break;
    case SIP_REFER:
	try {
	    sipMsg = new ReferMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the refer msg");
	}
	break;
    case SIP_NOTIFY:
	try {
	    sipMsg = new NotifyMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the notify msg");
	}
	break;
    case SIP_UNKNOWN:
	try {
	    sipMsg = new UnknownExtensionMsg(data, local_ip);
	}
	catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, 
                  "Failed in creating the UnknownExtension msg");
	}
        break;
    case SIP_MESSAGE:
        try {
            sipMsg = new MessageMsg(data, local_ip);
        }
        catch(SipParserException&) {
	    cpLog(LOG_DEBUG_STACK, "Failed in creating the MESSAGE msg");
        }
        break;
    default:
        cpLog(LOG_DEBUG_STACK, "NOT A KNOWN SIP MESSAGE");
        cpLog(LOG_INFO, "NOT A KNOWN SIP MESSAGE");

    }//end switch
    
    //print it out here.
    if (sipMsg != 0) {
	cpLog(LOG_DEBUG_STACK, 
	      " In sipmsg:     after decoding: %s", 
	      sipMsg->encode().c_str());
    }
    
    return sipMsg;
}


/** For debugging, can put this in logs, for instance */
string SipMsg::toString() const {
    ostringstream oss;
    oss << "encoding: " << encode().c_str()
        << " brief info: " << briefString() << " retry: "
        << retry << " version: " << version.c_str()
        << " rcvdIpName: " << recvdIPName.c_str() << " rcvdIpPort: "
        << recvdIPPort.c_str() << " nextHopIsProxy: " << myNextHopIsAProxy
        << endl;

    return oss.str();
}



void
SipMsg::encodeBody(Data* msg) const
{
    // need to encode body first to do calculations of length
    Data body;
    body.setBufferSize(800);

    myMimeList.encode(&body);


    Sptr <SipContentLength> contLen = myMimeList.getContentLength();
    if (contLen != 0)
    {
        Sptr<SipContentLength> contentLength;
        myHeaderList.getParsedHeader(contentLength, SIP_CONTENT_LENGTH_HDR, getLocalIp());
        *contentLength = *contLen;
    }
   
    Sptr <SipContentType> contType = myMimeList.getContentType();
    if (contType != 0)
    {
        Sptr<SipContentType> contentType;
        myHeaderList.getParsedHeader(contentType, SIP_CONTENT_TYPE_HDR, getLocalIp());
        *contentType = *(contType);
    }
    
    Sptr <SipContentDisposition> contDisp = myMimeList.getContentDisposition();
    if (contDisp != 0)
    {
        Sptr<SipContentDisposition> contentDisposition;
        myHeaderList.getParsedHeader(contentDisposition, 
                                     SIP_CONTENT_DISPOSITION_HDR, getLocalIp());
        *contentDisposition = *(contDisp);
    }
    
    // encode the body of the message

    myHeaderList.encode(msg);
    *msg += body;
}


Sptr<SipMsg>
SipMsg::clone()
{
    return copyPtrtoSptr(this);
}


Sptr<SipMsg> 
Vocal::copyPtrtoSptr(SipMsg* sipMsg)
{
    assert(sipMsg);
    Sptr<SipMsg> sptrsipMsg;

    switch(sipMsg->getType())
    {
    case SIP_INVITE:
    {
        InviteMsg* inviteMsg = dynamic_cast<InviteMsg*>(sipMsg);
        if (inviteMsg)
        {
            sptrsipMsg = new InviteMsg(*inviteMsg);
        }
        break;
    }

    case SIP_ACK:
    {
        AckMsg* ackMsg = dynamic_cast<AckMsg*>(sipMsg);
        if (ackMsg)
        {
            sptrsipMsg = new AckMsg(*ackMsg);
        }
        break;
    }

    case SIP_BYE:
    {
        ByeMsg* byeMsg = dynamic_cast<ByeMsg*>(sipMsg);
        if (byeMsg)
        {
            sptrsipMsg = new ByeMsg (*byeMsg);
        }
        break;
    }

    case SIP_CANCEL:
    {
        CancelMsg* cancelMsg = dynamic_cast<CancelMsg*>(sipMsg);
        if (cancelMsg)
        {
            sptrsipMsg = new CancelMsg (*cancelMsg);
        }
        break;
    }

    case SIP_STATUS:
    {
        StatusMsg* statusMsg = dynamic_cast<StatusMsg*>(sipMsg);
        if (statusMsg)
        {
            sptrsipMsg = new StatusMsg (*statusMsg);
        }
        break;
    }

    case SIP_REFER:
    {
        ReferMsg* referMsg  = dynamic_cast<ReferMsg*>(sipMsg);
        if (referMsg)
        {
            sptrsipMsg = new ReferMsg (*referMsg);
        }
        break;
    }
    case SIP_REGISTER:
    {
        RegisterMsg* registerMsg  = dynamic_cast<RegisterMsg*>(sipMsg);
        if (registerMsg)
        {
            sptrsipMsg = new RegisterMsg (*registerMsg);
        }
        break;
    }

    case SIP_OPTIONS:
    {
        OptionsMsg* optionsMsg  = dynamic_cast<OptionsMsg*>(sipMsg);
        if (optionsMsg)
        {
            sptrsipMsg = new OptionsMsg (*optionsMsg);
        }
        break;
    }

    case SIP_INFO:
    {
        InfoMsg* infoMsg  = dynamic_cast<InfoMsg*>(sipMsg);
        if (infoMsg)
        {
            sptrsipMsg = new InfoMsg (*infoMsg);
        }
        break;
    }

    case SIP_TRANSFER:
    {
        TransferMsg* transferMsg  = dynamic_cast<TransferMsg*>(sipMsg);
        if (transferMsg)
        {
            sptrsipMsg = new TransferMsg (*transferMsg);
        }
        break;
    }
    case SIP_SUBSCRIBE:
    {
        SubscribeMsg* subsMsg  = dynamic_cast<SubscribeMsg*>(sipMsg);
        if (subsMsg)
        {
            sptrsipMsg = new SubscribeMsg (*subsMsg);
        }
        break;
    }
    case SIP_NOTIFY:
    {
        NotifyMsg* notifyMsg  = dynamic_cast<NotifyMsg*>(sipMsg);
        if (notifyMsg)
        {
            sptrsipMsg = new NotifyMsg (*notifyMsg);
        }
        break;
    }
    case SIP_MESSAGE:
    {
        MessageMsg* msg  = dynamic_cast<MessageMsg*>(sipMsg);
        if (msg)
        {
            sptrsipMsg = new MessageMsg (*msg);
        }
        break;
    }

    default:
    {
        UnknownExtensionMsg* unknownExtensionMsg 
            = dynamic_cast<UnknownExtensionMsg*>(sipMsg);
        if (unknownExtensionMsg)
        {
            sptrsipMsg = new UnknownExtensionMsg (*unknownExtensionMsg);
        }
        break;
    }
    }

    assert (sptrsipMsg != 0);
    return sptrsipMsg;
}


// ----------------- ContentLength Header Methods  -----------------


void 
SipMsg::setContentLength(const SipContentLength& item ) 
{
    Sptr<SipContentLength> contentLength;
    myHeaderList.getParsedHeader(contentLength, SIP_CONTENT_LENGTH_HDR, getLocalIp());
    *contentLength = item;
}


const SipContentLength&
SipMsg::getContentLength( ) const
{
    Sptr<SipContentLength> contentLength;
    myHeaderList.getParsedHeader(contentLength, SIP_CONTENT_LENGTH_HDR, getLocalIp());
    return *contentLength;
}


void 
SipMsg::setContentLength(const Data& item /* Default Arguments*/)
{
    Sptr<SipContentLength> contentLength;
    myHeaderList.getParsedHeader(contentLength, SIP_CONTENT_LENGTH_HDR, getLocalIp());
    contentLength->decode(item);
}


// ----------------------------------------------------------------------


const SipContentDisposition&
SipMsg::getContentDisposition( ) const
{
    Sptr<SipContentDisposition> contentDisposition;
    myHeaderList.getParsedHeader(contentDisposition, 
                                 SIP_CONTENT_DISPOSITION_HDR, getLocalIp());
    return *contentDisposition;
}
    

void 
SipMsg::setContentDisposition(const SipContentDisposition& item 
                              /* Default Arguments*/)
{
    Sptr<SipContentDisposition> contentDisposition;
    myHeaderList.getParsedHeader(contentDisposition, 
                                 SIP_CONTENT_DISPOSITION_HDR, getLocalIp());
    *contentDisposition = item;
}
    
    
void 
SipMsg::setContentDisposition(const Data& item 
                              /* Default Arguments*/)
{
    Sptr<SipContentDisposition> contentDisposition;
    myHeaderList.getParsedHeader(contentDisposition, 
                                 SIP_CONTENT_DISPOSITION_HDR, getLocalIp());
    contentDisposition->decode(item);
}   
    

// ----------------- ContentType Header Methods  -----------------


const  SipContentType&
SipMsg::getContentType( ) const
{
    
    Sptr<SipContentType> contentType;
    myHeaderList.getParsedHeader(contentType, SIP_CONTENT_TYPE_HDR, getLocalIp());
    return *contentType;
    
}

void 
SipMsg::setContentType(const SipContentType& item /*default arguments*/ )
{
    Sptr<SipContentType> contentType;
    myHeaderList.getParsedHeader(contentType, SIP_CONTENT_TYPE_HDR, getLocalIp());
    *contentType = item;
}  

// ----------------- ContentData Header Methods  -----------------


int 
SipMsg::getNumContentData() const
{
    return myMimeList.getNumContentData();
}


Sptr<SipContentData>
SipMsg::getContentData( int i/*default arguments*/) const
{
    return myMimeList.getContentData(i);
}


void SipMsg::setContentData(const SipContentData* item, 
                            int index /*default Arguments*/)
{
    assert(item != 0);
    
    myMimeList.setContentData( item->duplicate(), index);
}


void 
SipMsg::setNumContentData(int index)
{
    assert(index >= 0);

    myMimeList.setNumContentData(index);
}


void 
SipMsg::removeContentData(int index /*Default Agruments*/)
{
    myMimeList.removeContentData(index);
}


void
SipMsg::flushContentData()
{
    myMimeList.setNumContentData(0);
}


// ----------------------------------------------------------------------


void
SipMsg::forceParse()
{
    myHeaderList.forceParse();
    myMimeList.forceParse();
}    


ostream&
SipMsg::brief(ostream& strm) const
{
    assert(this);
    switch (getType())
    {
        case SIP_UNKNOWN:
        strm <<  "UNKNOWN";
        break;
        case SIP_INVITE:
        strm <<  "INVITE";
        break;
        case SIP_ACK:
        strm <<  "ACK";
        break;
        case SIP_OPTIONS:
        strm <<  "OPTIONS";
        break;
        case SIP_BYE:
        strm <<  "BYE";
        break;
        case SIP_CANCEL:
        strm <<  "CANCEL";
        break;
        case SIP_REGISTER:
        strm <<  "REGISTER";
        break;
        case SIP_INFO:
        strm <<  "INFO";
        break;
        case SIP_TRANSFER:
        strm <<  "TRANSFER";
        break;
        case SIP_REFER:
        strm <<  "REFER";
        break;
        case SIP_NOTIFY:
        strm <<  "NOTIFY";
        break;
        case SIP_SUBSCRIBE:
        strm <<  "SUBSCRIBE";
        break;
    case SIP_STATUS:
    {
        const StatusMsg* p = dynamic_cast<const StatusMsg*>(this);
        assert (p != 0);
        strm << "STATUS (" << p->getStatusLine().getStatusCode() << ")";
        break;
    }
    case SIP_MESSAGE:
        strm <<  "MESSAGE ";
        if (getNumContentData())
        {
            Sptr<SipContentData> data = getContentData(0);
            int l;
            strm << "(" << data->encodeBody(l) << ")";
        }
        break;
    default:
        strm << "TYPE NOT PRINTABLE";
        break;
    }
    strm << " ["  << getCallId().getCallIdData() << " : " << getCSeq().getMethod() << " " << getCSeq().getCSeq() << "]";

    return strm;
}

string
SipMsg::briefString() const
{
    ostringstream oss;
    brief(oss);
    oss << endl;
    
    return oss.str();
}

ostream&
Vocal::operator<<(ostream& s, const SipMsg &msg)
{
    s << msg.encode();
    return s;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
