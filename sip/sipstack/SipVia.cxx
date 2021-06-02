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

#include "global.h"
#include "Data.hxx"
#include "cpLog.h"
#include "SipVia.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "NetworkAddress.h"

using namespace Vocal;
static Literal RPORT = "rport";


string
SipViaParserException::getName( void ) const
{
    return "SipViaParserException";
}


SipVia::SipVia( const SipVia& src )
    : SipHeader(src)
{
    if (&src != this)
    {
        host = src.host;
        port = src.port;
        protocolName = src.protocolName;
        transport = src.transport;
        protocolVersion = src. protocolVersion;
        receivedHost = src.receivedHost;
        branchToken = src.branchToken;
        comment = src.comment;

	ttl = src.ttl;
	extparm = src.extparm;
	extvalue = src.extvalue;
	maddr = src.maddr;

        viaBranch = src.viaBranch;
        viaComment = src.viaComment;
        viaReceived = src.viaReceived;
        viaHidden = src.viaHidden;
        viaTtl = src.viaTtl;
	viaMaddr = src.viaMaddr;
	rport = src.rport;
	rportValue = src.rportValue;
    }
}

const
SipVia& SipVia::operator =(const SipVia& src)
{
    if (&src != this)
    {
        host = src.host;
        port = src.port;
        protocolName = src.protocolName;
        transport = src.transport;
        protocolVersion = src.protocolVersion;
        receivedHost = src.receivedHost;
        branchToken = src.branchToken;
        comment = src.comment;
	
	ttl = src.ttl;
	extparm = src.extparm;
	extvalue = src.extvalue;
	maddr = src.maddr;

        viaBranch = src.viaBranch;
        viaComment = src.viaComment;
        viaReceived = src.viaReceived;
        viaHidden = src.viaHidden;
        viaTtl = src.viaTtl;
        viaMaddr = src.viaMaddr;
	rport = src.rport;
	rportValue = src.rportValue;
    }
    return (*this);
}


bool
SipVia::operator< (const SipVia& src) const
{
    if (
//	(hostsCompare(host, src.host) < 0) ||
	(host < src.host) ||
	(port < src.port) ||
	(protocolName < src.protocolName) ||
	(transport < src.transport) ||
	(protocolVersion < src.protocolVersion) ||
	(receivedHost < src.receivedHost) ||
	(branchToken < src.branchToken) ||
	(comment < src.comment) ||
	
	(extparm < src.extparm) ||
	(extvalue < src.extvalue) ||
	(ttl < src.ttl) ||
	(maddr < src.maddr) ||
	
	(viaBranch < src.viaBranch) ||
	(viaComment < src.viaComment) ||
	(viaReceived < src.viaReceived) ||
	(viaHidden < src.viaHidden) ||
	(viaTtl < src.viaTtl) ||
	(viaMaddr < src.viaMaddr)
       )
    {
	return true;
    }
    return false;
}    


bool
SipVia::operator> (const SipVia& src) const
{
    return ( src < *this );
}    


bool SipVia::operator == (const SipVia& src) const
{
    if ( (host == src.host) &&
	 (port == src.port) &&
         (protocolName == src.protocolName) &&
         (transport == src.transport) &&
         (protocolVersion == src.protocolVersion) &&
         (receivedHost == src.receivedHost) &&
         (branchToken == src.branchToken) &&
         (comment == src.comment) &&

	 (extparm == src.extparm) &&
	 (extvalue == src.extvalue) &&
	 (ttl == src.ttl) &&
	 (maddr == src.maddr) &&

         (viaBranch == src.viaBranch) &&
         (viaComment == src.viaComment) &&
         (viaReceived == src.viaReceived) &&
         (viaHidden == src.viaHidden) &&
         (viaTtl == src.viaTtl) &&
	 (viaMaddr == src.viaMaddr) &&
	 (rport == src.rport && rportValue == src.rportValue)
       )
	 {
	     return true;
	     
	 }
	 else
	 {
	     return false;
	 }
}


/*** decode data , and store in data members */
SipVia::SipVia( const Data& data, const string& local_ip)
    : SipHeader(local_ip),
    host(),
    port(DEFAULT_SIP_PORT),
    protocolName(DEFAULT_PROTOCOL),
    protocolVersion(DEFAULT_VERSION),
    transport(DEFAULT_TRANSPORT),
    viaBranch(false),
    viaComment(false),
    viaReceived(false),
    viaHidden(false),
    viaTtl(false),
    viaMaddr(false),
    rport(false)
{

    if (data == "") {
        return;
    }

    try
    {
        decode(data);
    }
    catch (SipViaParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
	    throw SipViaParserException(
                "failed to decode the Via string",
                __FILE__,
                __LINE__,DECODE_VIA_FAILED);
        }
    }
}


void SipVia::decode(const Data& viadata)
{
    try
    {
	scanSipVia(viadata);
    }
    catch (SipViaParserException& exception)
    {
	if (SipParserMode::sipParserMode())
	{
	    throw SipViaParserException(
		exception.getDescription(),
		__FILE__,
		__LINE__,SCAN_VIA_FAILED);
	}
    }
}

void 
SipVia::scanSipVia(const Data &tmpdata)
{
    Data viadata;
    Data data = tmpdata;
    // This is Check for Mandatory Item / in Sent Protocol
    int ret = data.match("/", &viadata, true);
    
    if (ret == FOUND)
    {
        // trim extra whitespace if needed
        viadata.removeSpaces();
	setprotoName(viadata);
	Data newdata = data;
	Data newviadata ;
	// This is Check for Mandatory Item / in Sent Protocol
	
	int retn = newdata.match("/", &newviadata, true);
	if (retn == NOT_FOUND)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"Mandatory Item / is not present <%s>", tmpdata.logData());
                throw SipViaParserException("Mandatory Item / is not present ",
                                            __FILE__,__LINE__,
                                            MANDATORY_VIA_ITEM_MISSING);
	    } 
	}
	else if (retn == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"Mandatory Item / is not present <%s>", tmpdata.logData());
		throw SipViaParserException("Mandatory Item / is not present ",
					    __FILE__,__LINE__,
					    MANDATORY_VIA_ITEM_MISSING);
	    }
	}
	else if (retn == FOUND)
	{
	    setprotoVersion(newviadata);
	    parseRestVia(newdata);
	}
    }
    else if (ret == NOT_FOUND)
    {
	if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR,"Mandatory Item / is not present <%s>", tmpdata.logData());
	    throw SipViaParserException("Mandatory Item / is not present ",
					__FILE__,__LINE__,
					MANDATORY_VIA_ITEM_MISSING);
	}
    }
    else  if (ret == FIRST)
    { 
	if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR,"Mandatory Item / is not present <%s>", tmpdata.logData());
	    throw SipViaParserException("Mandatory Item / is not present ",
					__FILE__,__LINE__,
					MANDATORY_VIA_ITEM_MISSING);
	}
    }
}


void 
SipVia::parseRestVia(const Data& restdata)
{
    Data transdata;
    Data viarestdata = restdata;
    viarestdata.removeSpaces();
    // This is Check for SP bitween Sent Protocol & Sent By
    int ret =viarestdata.match(" ",&transdata,true);
    if (ret == NOT_FOUND)
    {   
	if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR,"Mandatory Item SP is not present <%s>", restdata.logData());
	    throw SipViaParserException("Mandatory Item <-SP-> is not present ",
					__FILE__,__LINE__,
					MANDATORY_VIA_ITEM_MISSING);
	}
    }
    else if (ret == FIRST)
    { 
	if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR,"Mandatory Item SP is not present <%s>", restdata.logData());
	    
	    throw SipViaParserException("Mandatory Item <-SP-> is not present ",
					__FILE__,__LINE__,
					MANDATORY_VIA_ITEM_MISSING);
	}
    }
    else if (ret == FOUND)
    {
        transdata.removeSpaces();
	parseTransport(transdata);
	
	Data hostviadata = viarestdata ;
	Data hostdata;
	// Check for VIA Parms ?
	int retn =hostviadata.match(";",&hostdata,true);
	if (retn == NOT_FOUND)
	{   
	    Data commentdata=hostviadata;
	    Data commentvalue;

	    // In case Via parms are not there since *(via-parms) is
	    // The Condition, this Checks for [Comment]

	    int retrnm =commentdata.match("(",&commentvalue,true);
	    if (retrnm == NOT_FOUND)
	    {
		// Which is Fine Since Comment is Optional, Then SetHost :
		parseHost(commentdata);
	    }
	    else if (retrnm == FIRST)
	    {
		if (SipParserMode::sipParserMode())
		{
		    cpLog(LOG_ERR,"The ) closing of Pranthess is not Done <%s>", restdata.logData());

		    throw SipViaParserException(
			"The ) closing of Pranthess is not Done :(",
			__FILE__,
			__LINE__,COMMENT_ERR
			);
		}
	    }
	    else if (retrnm == FOUND)
	    {
		Data cvalue;
		parseHost(commentvalue);
		Data ddata = commentdata; 
		// This checks that optional [comment] has the right syntax?
		int r =ddata.match(")",&cvalue,true);
		if (r == NOT_FOUND)
		{ 
		    if (SipParserMode::sipParserMode())
		    {
			cpLog(LOG_ERR,
			      "The ) closing of Paranthesis is not Done <%s>", restdata.logData());
			throw SipViaParserException("The ) closing of Paranthesis is not Done :(",
                                                    __FILE__,
                                                    __LINE__,COMMENT_ERR);
		    }
		    
		}
		else if (r == FIRST)
		{ 
		    if (SipParserMode::sipParserMode())
		    {
			cpLog(LOG_ERR,"The  Comment Contains NULL <%s>", restdata.logData());
			throw SipViaParserException("The Comment Contains NULL :(",
                                                    __FILE__,
                                                    __LINE__,COMMENT_ERR);
		    }
		    
		}
		else if (r == FOUND)
		{ //  In case it has the Right Stuff Set it!!
		    setComment(cvalue);
		}
	    }
	}
	else if (retn == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"The ; can't be first <%s>", restdata.logData());
		throw SipViaParserException("The ; can't be first :(",
                                            __FILE__,__LINE__,PARSE_VIAPRAM_ERR);
	    }
	}
	else if (retn == FOUND)
	{ // if ";" Check was Successful then Set the Host & Parse Via Parms
	    parseHost(hostdata);
	    Data parmsviadata=hostviadata;
	    
	    parseViaParms(parmsviadata); 
	}
    }
}


void 
SipVia::parseComment(const Data& cdata)
{
    Data commentdata=cdata;
    Data commentvalue;
    int ret =commentdata.match("(",&commentvalue,true);
    if (ret == NOT_FOUND)
    {
	if (SipParserMode::sipParserMode())
	{
	    cpLog(LOG_ERR,"The  Paranthesis is not Present <%s>", cdata.logData());
	    throw SipViaParserException("The  Paranthesis is not present",
                                        __FILE__, __LINE__,COMMENT_ERR);
	}
    }
    else if (ret == FIRST)
    {
	// if The Comment (is the Only thing Left to Parse!! Then "("
	// this will be FIRST)
	Data cvalue;
	Data ddata = commentdata; 
	int retn =ddata.match(")",&cvalue,true);
	if (retn == NOT_FOUND)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"The ) closing of Paranthesis is not Done <%s>", cdata.logData());
		throw SipViaParserException("The ) closing of Paranthesis is not Done :(",
		    __FILE__,__LINE__,COMMENT_ERR);
	    }
	}
	else  if (retn == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"The  Comment Contains NULL <%s>", cdata.logData());
		throw SipViaParserException("The Comment Contains NULL :(",
		    __FILE__,__LINE__,COMMENT_ERR );
	    }
		    
	}
	else if (retn == FOUND)
	{
	    setComment(cvalue);
	}
    }
    else if (ret == FOUND)
    {
	/* 
	   This is for the Condition Like for Example: "SIP/2.0/UDP
	   exmaple.vovida.com:3254;hidden(comment)" Since it will pass
	   the Test ";" But it will Fail while Check'n for Hidden in
	   ParseViaHiddenParm Method Bcos there the Value by which we
	   Check Const Value "hidden" is obtain by sepration of
	   ";"(which will work fine in this type of Message Flow
	   ""SIP/2.0/UDP
	   exmaple.vovida.com:3254;hidden;branch=215321(comment)"...)
	   Is this Comment Confused You? 
	*/

	if (commentvalue == HIDDEN)
	{
	    viaHidden=true;
	}
	Data cvalue;
	Data ddata = commentdata; 
	int retn =ddata.match(")",&cvalue,true);
	if (retn == NOT_FOUND)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"The ) closing of Paranthesis is not Done <%s>", cdata.logData());
		throw SipViaParserException("The ) closing of Paranthesis is not Done :(",
                                            __FILE__,__LINE__,COMMENT_ERR );
	    }
	}
	else if (retn == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"The  Comment Contains NULL <%s>", cdata.logData());
		throw SipViaParserException("The Comment Contains NULL :(",
                                            __FILE__, __LINE__,COMMENT_ERR );
	    }   
	}
	else if (retn == FOUND)
	{
	    setComment(cvalue);
	}
    }
}


void 
SipVia::parseViaParms( const Data& data)
{
    Data viaparms = data;
    Data cvalue;
    // Loop for Parse'n all the ViaParms Which are separated by SemiColon
    while (viaparms.length())
    {
	Data parm;
	int ret = viaparms.match(";", &parm, true);

	if (ret == NOT_FOUND)
	{
	    // which is OK one One Parm could be There 
	    parseViaParmHidden( viaparms,parm);
	    cvalue = viaparms;
	    break;
	    
	}
	else if (ret == FIRST)
	{
	    //NOT A valid via-parm. throw exception
		    
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"The ; can't be first <%s>", data.logData());
		throw SipViaParserException("The ; can't be first ",
                                            __FILE__, __LINE__,PARSE_VIAPRAM_ERR );
	    }

	}
	else if (ret == FOUND)
	{ 
	    Data value;
	    value = viaparms;
		   
	    try 
	    { 
		parseViaParmHidden(parm,value);
	    }
	    catch (SipViaParserException& exception)
	    {
		if (SipParserMode::sipParserMode())
		{
		    cpLog(LOG_ERR,"The Via String Contains Null  <%s>", data.logData());
		    throw SipViaParserException (exception.getDescription(),
                                                 __FILE__,__LINE__,PARSE_VIAPRAM_ERR); 
		}
	    }
	}

		    
    }

    /*if after finishing parseViaParms anything is left that is a
     * comment parseComment(cvalue);*/
}


void 
SipVia::parseViaParmHidden( const Data& data, const Data& pvalue)
{
    Data viaparm;
    Data parmvalue;
    viaparm = data;
    parmvalue = pvalue;
    viaparm.removeSpaces();
    parmvalue.removeSpaces();
    //Check for Hidden Parm First since this the Only Param Without "="
    if (viaparm == HIDDEN)
    {
	viaHidden=true;
    }
    else
    {
	Data value;
	// Check for the Parms With Constant "=" sign
	int check = viaparm.match("=", &value , true);
	  	
	if ( check == NOT_FOUND)
	{
		viaparm.removeSpaces(); 
    		if (viaparm != RPORT)
		{
		   if (SipParserMode::sipParserMode())
	    	   {
			cpLog(LOG_ERR,"not a valid Viaparm syntax  <%s>", pvalue.logData());
			throw SipViaParserException( "not a valid Viaparm syntax",
			    __FILE__,__LINE__,PARSE_VIAPRAM_ERR);
	    	    }
		}
		else rport = true;
	}
	else if (check == FIRST)
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"not a valid Viaparm syntax  <%s>", pvalue.logData());
		throw SipViaParserException("not a valid Viaparm syntax",
		    __FILE__,__LINE__,PARSE_VIAPRAM_ERR);
	    }
	}
	else if (check == FOUND)
	{	
	    //value has the correct 
	    
	    if (viaparm.length())
	    {
		try 
		{
		    value.removeSpaces();
		    viaparm.removeSpaces(); 
		    parseViaParm(value,viaparm);
		}
		catch (SipViaParserException& exception)
		{ 
		    if (SipParserMode::sipParserMode())
		    {
			cpLog(LOG_ERR,"The Via String Contains Null  <%s>", pvalue.logData());
			throw SipViaParserException (exception.getDescription(),
			    __FILE__,__LINE__,PARSE_VIAPRAM_ERR); 
		    }
				
		}
	    }
	}
    }
}


void
SipVia::parseViaParm(const Data & data, const Data & value)
{
    Data viaparm;
    Data parmvalue;
    viaparm = data;
    parmvalue = value;
    if (viaparm == MADDR_PARM)
    {
	setMaddr(parmvalue);
    }
    else if (viaparm == RECEIVED)
    {
	parseRecevied(parmvalue);
    }
    else if (viaparm == TTL)
    {   
	int phvalue1 = parmvalue.convertInt();
	if (phvalue1>=0 && phvalue1<=255) 
	{
	    setTtl(parmvalue);
	}
	else
	{
	    if (SipParserMode::sipParserMode())
	    {
		cpLog(LOG_ERR,"not a valid Viaparm ttl syntax  <%s>", value.logData());
		throw SipViaParserException("not a valid ttl syntax",
		    __FILE__,__LINE__,PARSE_VIAPRAM_ERR);
	    }
	}
    }
    else if (viaparm == BRANCH)
    {
	parseBranch(parmvalue);
    }
    else if (viaparm == RPORT)
    {
	setRPort(parmvalue);
    }
    else
    {
	// This introduced with New SIpDraft not Offical(2543b)
	parseExtension(viaparm,parmvalue);
	       
    }
}


void 
SipVia::parseRecevied(const Data& recevieddata)
{
    setReceivedhost(recevieddata);
}


void 
SipVia::parseBranch(const Data& branchdata)
{
    Data branchdat =  branchdata;
    branchdat.removeSpaces();
    Data branchval;
    int test = branchdat.match("(",&branchval,true);
    if (test == FOUND)
    {
	setBranch(branchval);
	Data brdat=branchdat;
	Data brval;
	int ret = brdat.match(")",&brval,true);
	if (ret == FOUND)
	{
	    setComment(brval);
	}
	else if (ret == NOT_FOUND)
	{
	}
	else if (ret == FIRST)
	{
	}
    }
    else if (test == NOT_FOUND)
    {
	setBranch(branchdat);
    }
    else if (test == FIRST)
    {
    }
}


void 
SipVia::parseExtension(const Data& extensiondata, const Data& extensionvalue)
{
	Data tokenparm = extensiondata ;
	Data tokenvalue = extensionvalue;
	setExtensionParm(tokenparm);
       	setExtensionValue(tokenvalue);
	
    
}


void 
SipVia::parseTransport(const Data& tdata)
{
    Data transdata=tdata;
    setTransport(transdata);
}


void
 SipVia::setprotoName(const Data& protoName)
{
    protocolName = protoName;
}

void
SipVia::setRPort(const Data& port)
{
	rportValue = port;
	rport = true;
}

void
SipVia::setRPort(const bool flag)
{
	rport = flag;
	rportValue = "";
}

Data
SipVia::getRPort() const
{
	return rportValue;
}

Data
SipVia::getprotoName() const
{
        return protocolName;
}


void
 SipVia::setprotoVersion(const Data& protoVer)
{
    protocolVersion = protoVer;
}


Data
 SipVia::getprotoVersion() const
{
    return protocolVersion;
}


void 
SipVia::setTransport(const Data& trans)
{
    transport = trans;
}


/* This is the transport which appears in sent-protocol, and not in
 * transport-param. Hence it is stored here */
Data
SipVia::getTransport() const
{
    return transport;
}


void 
SipVia::parseHost(const Data& newHost)
{
    Data hostport = newHost;
    Data hostdata;

    // v6 Address
    int ret = hostport.match("]",&hostdata,true);
    if (ret == FOUND) {
	hostdata += "]";
	char tmp = ':';
	hostport.matchChar(&tmp);
    }
    else if (ret == NOT_FOUND) {
	ret = hostport.match(":",&hostdata,true);
    }

    if (ret == FOUND)
    {
	Data testo;
	int retu = hostdata.match(" ",&testo,true);
	if (retu == FIRST)
	{
	    setHost(hostdata);
	}
	else  if(retu == NOT_FOUND)
	{
	    setHost(hostdata);
	}
	else  if (retu == FOUND)
	{
	    cpLog(LOG_ERR, "Spaces between Net Address <%s>", newHost.logData());
	}
	setPort(hostport.convertInt());
    }
    else if (ret == FIRST)
    {
	    
    }
    else if (ret == NOT_FOUND)
    {
        hostport.removeSpaces();
	setHost(hostport);
    }

}


void 
SipVia::setHost( const Data&  newHost)
{
    host = newHost;
}


const Data&
SipVia::getHost() const
{
    return host;
}


void 
SipVia::setPort(int newport)
{
    port = Data(newport);
}


void 
SipVia::setPortData( const Data&  newport)
{
    port = newport;
}


int SipVia::getPort() const
{
    return port.convertInt();
}


void 
SipVia::setTtl(const Data& newttl)
{
    ttl = newttl;
    if (ttl.length()>0)
    {
	viaTtl=true;
    }
}


void 
SipVia::setMaddr(const Data& newmaddr)
{
    maddr=newmaddr;
    if (maddr.length()>0)
    {
	viaMaddr=true;
    }
}


Data
SipVia::getMaddr() const
{
    return maddr;
}


/***This belongs to the other-parms. So, maintain here? */
void
SipVia::setReceivedhost(const Data& recvdhost)
{
    if(NetworkAddress::is_valid_ip6_addr(recvdhost))
    {
        receivedHost = "[";
        receivedHost += recvdhost;
        receivedHost += "]";
    }
    else
    {
        receivedHost = recvdhost;
    }
    if (receivedHost.length())
    {
        viaReceived = true;
    }
}


Data
SipVia::getReceivedhost() const
{
    return receivedHost;
}


/** This belongs to the other-parms. So, maintain here? */
void
SipVia::setHidden(const Data& hidden)
{
    if (hidden == HIDDEN)
    {
        viaHidden = true;
    }
}


bool
SipVia::getHidden() const
{
    return viaHidden;
}


void 
SipVia::setBranch(const Data& newtoken)
{
    branchToken = newtoken;
    if (branchToken.length())
    {
        viaBranch = true;
    }
    else
    {
        viaBranch = false;
    }
}


Data
SipVia::getBranch() const
{
    return branchToken;
}


/** This belongs to the other-parms. So, maintain here? */
void 
SipVia::setComment(const Data& newcomment)
{

    comment = newcomment;
    if (comment.length())
    {
        viaComment = true;
    }
}


Data
SipVia::getComment() const
{
    return comment;
}
  
  
/*** return the encoded string version of this. This call should only be
         used inside the stack and is not part of the API */
Data
SipVia::encode() const
{
    Data ret;
    
    ret = VIA;
    
    ret+= SP;

    
    //construct the sent-protocol
    Data sentprotocol = getprotoName();
    sentprotocol+= "/";
    sentprotocol+= DEFAULT_VERSION;
    sentprotocol+= "/";
    sentprotocol+= getTransport();
    sentprotocol+= SP; 
   
    ret += sentprotocol;
    ret +=getHost();
    Data pport = Data(getPort());
    if (pport.length() > 0)
    {
        ret+= ":";
        ret+= pport;
    }
    ret+= formOtherparms(); 
    ret+= CRLF;
      
    return ret;
}


Data 
SipVia::formOtherparms() const
{
    Data otherparms;

    //construct via-parms.
    if ( (viaHidden) ||(viaTtl)||(viaMaddr)||(viaReceived) ||
	 (viaBranch) || (viaComment) || (rport))
    {
        if (viaHidden)
	{
	    otherparms+=";";
	    otherparms+=HIDDEN;
	}
	if (viaTtl)
	{
	    otherparms += ";";
	    otherparms +=TTL;
	    otherparms +="=";
	    otherparms +=getTtl();
	}
	if (rport)
	{
	    otherparms= otherparms+";"+RPORT;
	    if (rportValue.length())
		otherparms = otherparms+"="+rportValue;
	}
	if (viaMaddr)
	{
	    otherparms += ";";
	    otherparms += MADDR_PARM;
	    otherparms += "=";
	    otherparms += getMaddr();

	}
	if (viaReceived)
	{
	    otherparms+=";";
	    otherparms+=RECEIVED;
	    otherparms+="=";
	    otherparms+= getReceivedhost();

	}
	if (viaBranch)
	{
	    otherparms+=";";
 	    otherparms+=BRANCH;
	    otherparms+="=";
	    otherparms+= getBranch();

	}
	
	if (viaComment)
	{
	    otherparms+= "(";
	    otherparms+= getComment();
	    otherparms+= ")";
	}
    }
    
    return otherparms;
}


Data
SipVia::getExtensionValue() const 
{
  return extvalue;
}


Data
SipVia:: getExtensionParm() const 
{
  return extparm;
}


void 
SipVia::setExtensionParm(const Data & newextparm)
{
  extparm = newextparm;
}


void
SipVia:: setExtensionValue(const Data& newextvalue)
{
  extvalue = newextvalue;
}


Data 
SipVia::getTtl()const
{ 
  return ttl;
}

     
void
SipVia::setReceivedport(const Data& recvdport)
{
    setRPort(recvdport);
}


Data 
SipVia::getReceivedport() const
{
    return getRPort();
}


bool 
SipVia::isViaReceived() const
{
    return viaReceived;
    
}


Sptr<SipHeader>
SipVia::duplicate() const
{
    return new SipVia(*this);
}


bool
SipVia::compareSipHeader(SipHeader* msg) const
{
    SipVia* otherMsg = dynamic_cast<SipVia*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


