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

static const char* const SipAgent_cxx_Version =
    "$Id: SipAgent.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


// 24/11/03 fpi
// WorkAround Win32
#ifdef WIN32
#include <winsock2.h>
#endif

#include <iostream>
#include "Lock.hxx"
#include "Mutex.hxx"
#include "SipAgent.hxx"
#include "SipSnmpDetails.hxx"
#include "cpLog.h"
#include "SnmpCommon.h"
#include "symbols.hxx"

using namespace Vocal;
using namespace Vocal::Threads;

SipAgent::Table SipAgent::SipCountersTable;

void
SipAgent::setName(const Data& data)
{
    name = data;
}


SipAgent::SipAgent( const Data& inname)
    : AgentApi(SERVER_SipStack, inname.convertString())
{
    name = inname;

    run();
    unsigned long zerop, ulongp;
    for (int i = agentApiMibVarFirstEntry; i < agentApiMibVarLastEntry; i++)
    {
        zerop = 0;
        ulongp = i;
        SipCountersTable.insert(pair < unsigned long, unsigned long > (ulongp, zerop));
    }
       if ( name.length() == 0)
    {   
        snmpdetails = 0;
        cpLog (LOG_ERR, "Application Name needed" );
    } else 
    {
    snmpdetails = new SipSnmpDetails(this, name);
    }
}


SipAgent::~SipAgent()
{
    for (int i = agentApiMibVarFirstEntry; i < agentApiMibVarLastEntry; i++)
    {
        SipCountersTable.erase(i);
    }
    shutdown();
    join();
}


voReturnStatus
SipAgent::updateSipCounter(AgentApiMibVarT inCounterName)
{
    cpLog(LOG_DEBUG_STACK, "updateSipCounter %d", inCounterName);
    TableIter aCounter;
    Lock lockHelper(itsMutex);
    aCounter = SipCountersTable.find((int) inCounterName);
    if ( aCounter != SipCountersTable.end() )
    {
        aCounter->second ++;
        cpLog(LOG_DEBUG_STACK, "new value = %d", aCounter->second);
        return voSuccess;
    }
    else
    {
        cpLog (LOG_DEBUG_STACK, "updatesipcounter failed to find counter");
        return voUnknown;
    }
}


int
SipAgent::getSipCounter(AgentApiMibVarT counter)
{
    int number = -1;

    TableIter aCounter;
    Lock lockHelper(itsMutex);
    aCounter = SipCountersTable.find((int) counter);
    if ( aCounter != SipCountersTable.end() )
    {
        number = aCounter->second ;
        cpLog(LOG_DEBUG_STACK, "returning value = %d", aCounter->second);
    }
    else
    {
        cpLog (LOG_DEBUG_STACK, "getSipCounter failed to find counter");

    }
    return number;
}


voReturnStatus
SipAgent::processMessage(ipcMessage *inmessage, NetworkAddress *sender)
{

    TableIter aCounter;
    int classvalue = 0;
    switch (inmessage->action)
    {
        case Get :
        {
            if (inmessage->mibVariable >= sipSummaryInRequests &&
                    inmessage->mibVariable <= sipNumUnsupportedUris)
            {
                aCounter = SipCountersTable.find(inmessage->mibVariable);
                if (aCounter != SipCountersTable.end() )
                {
                    return sendResponse(aCounter->second, sender);
                }
                else
                {
                    return sendResponse(VO_NA_INT, sender);
                }

            }
            else
            {
                switch (inmessage->mibVariable)
                {
                    case sipProtocolVersion :
                    {
                        snmpData snmdata = getstackData(sipProtocolVersion);
                        string s = snmdata.getData().convertString();
                        return sendResponse( s , sender);
                    }
		    break;
                    case sipServiceOperStatus :
                    {

                        snmpData snmdata = getstackData(sipServiceOperStatus);
                        return sendResponse( static_cast<int>(snmdata.getData().convertInt()), sender);
                    }
		    break;

                    case sipServiceAdminStatus :
                    {
                        snmpData snmdata = getstackData(sipServiceAdminStatus);
                        return sendResponse( VO_NA_INT, sender);
                    }
		    break;
                    case sipServiceStartTime :
                    {
                        snmpData snmdata = getstackData(sipServiceStartTime);
                        return sendResponse( (unsigned long)snmdata.getData().convertInt() , sender);
                    }
		    break;
                    case sipServiceLastChange :
                    {
                        snmpData snmdata = getstackData(sipServiceLastChange);
                        return sendResponse((unsigned long)snmdata.getData().convertInt() , sender);
                    }
		    break;
                    case sipTransport :
		    {
                        return sendResponse(VO_NA_INT , sender);
		    }
		    break;

                    case sipUriSupported :
		    {
                    return sendResponse(VO_NA_STRING , sender);
		    }
		    break;

                    case sipFtrSupported :
		    {
			return sendResponse(VO_NA_STRING , sender);
		    }
		    break;
                    case sipOrganization :
		    {
			return sendResponse(VO_NA_STRING , sender);
		    }
		    break;
                    case sipMaxTransactions :
                    {
                        snmpData snmdata = getstackData(sipMaxTransactions);
                        return sendResponse((unsigned long)snmdata.getData().convertInt() , sender);
                    }
		    break;
                    case sipRequestDfltExpires :
                    {
                        snmpData snmdata = getstackData(sipRequestDfltExpires);
                        return sendResponse((unsigned long)snmdata.getData().convertInt() , sender);
                    }
		    break;
                    case sipHideOperation :
		    {
			return sendResponse(VO_NA_INT , sender);
		    }
		    break;
		    
                    case sipUserLocationServerAddr :
		    {
			return sendResponse(VO_NA_STRING , sender);
		    }
		    break;
		    
                    //handles the high level stack data.
                    case sipStatsInfoClassIns :
		    {
			classvalue += getSipCounter( sipStatsInfoIns);
			classvalue += getSipCounter( sipStatsInfoTryingIns);
			classvalue += getSipCounter( sipStatsInfoRingingIns);
			classvalue += getSipCounter( sipStatsInfoForwardedIns);
			classvalue += getSipCounter( sipStatsInfoQueuedIns);
			classvalue += getSipCounter( sipStatsInfoSessionProgIns);
			return sendResponse(classvalue , sender);
		    }
		    break;
		    
                    case sipStatsInfoClassOuts :
		    {
			classvalue += getSipCounter( sipStatsInfoOuts);
			classvalue += getSipCounter( sipStatsInfoTryingOuts);
			classvalue += getSipCounter( sipStatsInfoRingingOuts);
			classvalue += getSipCounter( sipStatsInfoForwardedOuts);
			classvalue += getSipCounter( sipStatsInfoQueuedOuts);
			classvalue += getSipCounter( sipStatsInfoSessionProgOuts);
			return sendResponse(classvalue , sender);
		    }
		    break;
		    
                    case sipStatsSuccessClassIns :
		    {
			classvalue += getSipCounter( sipStatsSuccessOkIns);
			return sendResponse(classvalue , sender);
		    }
		    break;
		    
                    case sipStatsSuccessClassOuts :
		    {
			classvalue += getSipCounter( sipStatsSuccessOkOuts);
			return sendResponse(classvalue , sender);
		    }
		    break;
		    
                    case sipStatsRedirClassIns :
		    {
			
			classvalue += getSipCounter( sipStatsRedirMultipleChoiceIns);
			classvalue += getSipCounter( sipStatsRedirMovedPermIns);
			classvalue += getSipCounter( sipStatsRedirMovedTempIns);
			classvalue += getSipCounter( sipStatsRedirSeeOtherIns);
			classvalue += getSipCounter( sipStatsRedirUseProxyIns);
			classvalue += getSipCounter( sipStatsRedirAltServiceIns);
			return sendResponse(classvalue , sender);
		    }
		    break;
                    case sipStatsRedirClassOuts :
		    {
			
			classvalue += getSipCounter( sipStatsRedirMultipleChoiceOuts);
			classvalue += getSipCounter( sipStatsRedirMovedPermOuts);
			classvalue += getSipCounter( sipStatsRedirMovedTempOuts);
			classvalue += getSipCounter( sipStatsRedirSeeOtherOuts);
			classvalue += getSipCounter( sipStatsRedirUseProxyOuts);
			classvalue += getSipCounter( sipStatsRedirAltServiceOuts);
			return sendResponse(classvalue , sender);
		    }
		    break;
		    
                    case sipStatsReqFailClassIns :
		    {
			classvalue += getSipCounter( sipStatsClientBadRequestIns);
			classvalue += getSipCounter( sipStatsClientUnauthorizedIns);
			classvalue += getSipCounter( sipStatsClientPaymentReqdIns);
			classvalue += getSipCounter( sipStatsClientForbiddenIns);
			classvalue += getSipCounter( sipStatsClientNotFoundIns);
			classvalue += getSipCounter( sipStatsClientMethNotAllowedIns);
			
			classvalue += getSipCounter( sipStatsClientNotAcceptableIns);
			classvalue += getSipCounter( sipStatsClientProxyAuthReqdIns);
			classvalue += getSipCounter( sipStatsClientReqTimeoutIns);
			classvalue += getSipCounter( sipStatsClientConflictIns);
			classvalue += getSipCounter( sipStatsClientGoneIns);
			classvalue += getSipCounter( sipStatsClientLengthRequiredIns);
			
			classvalue += getSipCounter( sipStatsClientReqEntTooLargeIns);
			classvalue += getSipCounter(sipStatsClientReqURITooLargeIns);
			classvalue += getSipCounter(sipStatsClientNoSupMediaTypeIns);
			classvalue += getSipCounter(sipStatsClientBadExtensionIns);
			classvalue += getSipCounter(sipStatsClientTempNotAvailIns);
			classvalue += getSipCounter(sipStatsClientCallLegNoExistIns);
			
			classvalue += getSipCounter( sipStatsClientLoopDetectedIns);
			classvalue += getSipCounter( sipStatsClientTooManyHopsIns);
			classvalue += getSipCounter( sipStatsClientAddrIncompleteIns);
			classvalue += getSipCounter( sipStatsClientAmbiguousIns);
			classvalue += getSipCounter( sipStatsClientBusyHereIns );
			
			
			
			return sendResponse(classvalue , sender);
		    }
		    break;
                    case sipStatsReqFailClassOuts :
		    {
			classvalue += getSipCounter( sipStatsClientBadRequestOuts);
			classvalue += getSipCounter(sipStatsClientUnauthorizedOuts);
			classvalue += getSipCounter(sipStatsClientPaymentReqdOuts);
			classvalue += getSipCounter(sipStatsClientForbiddenOuts);
			classvalue += getSipCounter(sipStatsClientNotFoundOuts);
			classvalue += getSipCounter( sipStatsClientMethNotAllowedOuts);
			
			classvalue += getSipCounter(sipStatsClientNotAcceptableOuts);
			classvalue += getSipCounter( sipStatsClientProxyAuthReqdOuts);
			classvalue += getSipCounter(sipStatsClientReqTimeoutOuts);
			classvalue += getSipCounter(sipStatsClientConflictOuts);
			classvalue += getSipCounter(sipStatsClientGoneOuts);
			classvalue += getSipCounter(sipStatsClientLengthRequiredOuts);
			
			classvalue += getSipCounter(sipStatsClientReqEntTooLargeOuts);
			classvalue += getSipCounter(sipStatsClientReqURITooLargeOuts);
			classvalue += getSipCounter(sipStatsClientNoSupMediaTypeOuts);
			classvalue += getSipCounter(sipStatsClientBadExtensionOuts);
			classvalue += getSipCounter(sipStatsClientTempNotAvailOuts);
			classvalue += getSipCounter(sipStatsClientCallLegNoExistOuts);
			
			classvalue += getSipCounter(sipStatsClientLoopDetectedOuts);
			classvalue += getSipCounter(sipStatsClientTooManyHopsOuts);
			classvalue += getSipCounter(sipStatsClientAddrIncompleteOuts);
			classvalue += getSipCounter( sipStatsClientAmbiguousOuts);
			classvalue += getSipCounter(sipStatsClientBusyHereOuts);
			
			return sendResponse(classvalue , sender);
		    }
		    break;
                    case sipStatsServerFailClassIns :
		    {
			classvalue += getSipCounter(sipStatsServerIntErrorIns);
			classvalue += getSipCounter(sipStatsServerNotImplementedIns);
			classvalue += getSipCounter(sipStatsServerBadGatewayIns);
			classvalue += getSipCounter(sipStatsServerServiceUnavailIns);
			classvalue += getSipCounter( sipStatsServerGatewayTimeoutIns);
			classvalue += getSipCounter(sipStatsServerBadSipVersionIns);
			
			return sendResponse(classvalue, sender);
		    }
		    break;
		    case sipStatsServerFailClassOuts :
		    {
			classvalue += getSipCounter(sipStatsServerIntErrorOuts);
			classvalue += getSipCounter(sipStatsServerNotImplementedOuts);
			classvalue += getSipCounter(sipStatsServerBadGatewayOuts);
			classvalue += getSipCounter(sipStatsServerServiceUnavailOuts);
			classvalue += getSipCounter( sipStatsServerGatewayTimeoutOuts);
			classvalue += getSipCounter(sipStatsServerBadSipVersionOuts);
		  
		   
			return sendResponse(classvalue, sender);
		    }
		    break;
                    case sipStatsGlobalFailClassIns :
		    {
			classvalue += getSipCounter(sipStatsGlobalBusyEverywhereIns);
			classvalue += getSipCounter( sipStatsGlobalDeclineIns);
			classvalue += getSipCounter( sipStatsGlobalNotAnywhereIns);
			classvalue += getSipCounter(sipStatsGlobalNotAcceptableIns);

			return sendResponse(classvalue , sender);
		    }
		    break;

                    case sipStatsGlobalFailClassOuts :
		    {

			classvalue += getSipCounter(sipStatsGlobalBusyEverywhereOuts);
			classvalue += getSipCounter( sipStatsGlobalDeclineOuts);
			classvalue += getSipCounter( sipStatsGlobalNotAnywhereOuts);
			classvalue += getSipCounter(sipStatsGlobalNotAcceptableOuts);
			
			return sendResponse(classvalue , sender);
		    }
		    break;

                    case DebugLevel :
		    {
			return sendResponse(cpLogGetPriority() , sender);
		    }
		    break;
                    default :
		    {
			return voUnknown;
		    }
		    break;
                }
            }
        }
        case Set :
	{
	    switch (inmessage->mibVariable)
	    {
		case DebugLevel :
		{
		    cpLogSetPriority(*((int *)inmessage->parm1));
		    return voSuccess;
		}
		break;
		default :
		{
		    return voUnknown;
		}
		break;
	    }
	}
	break;
        case Trap :
	{
	}
	break;
        case Response :
	{
	}
	break;
        case Register :
	{
	}
	break;
        default :
	{
	    return voFailure;
	}
	break;
    }

    return voFailure;
}


void
SipAgent::setstackData(int index , const snmpData& data )
{
    snmpdetails->getWriteLock();
    snmpdetails->setstackdata(index, data);
    snmpdetails->getUnLock();

}


snmpData
SipAgent::getstackData(int index)
{

    return ( snmpdetails->getstackdata(index));


}


void
SipAgent::updateStatusSnmpData(int statusCode, SnmpType snmptype)
{
    switch (statusCode)
    {
        case 100:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsInfoTryingIns) :
              updateSipCounter(sipStatsInfoTryingOuts));
        }
        break;
        case 180:
        {
            ( (snmptype == INS ) ?

              updateSipCounter(sipStatsInfoRingingIns) :
              updateSipCounter(sipStatsInfoRingingOuts));
        }
        break;
        case 181:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsInfoForwardedIns) :
              updateSipCounter(sipStatsInfoForwardedOuts));
        }
        break;
        case 182:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsInfoQueuedIns) :
              updateSipCounter(sipStatsInfoQueuedOuts));
        }
        break;
        case 183:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsInfoSessionProgIns) :
              updateSipCounter(sipStatsInfoSessionProgOuts));
        }
        break;
        case 200:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsSuccessOkIns) :
              updateSipCounter(sipStatsSuccessOkOuts));
        }
        break;
        case 300:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsRedirMultipleChoiceIns) :
              updateSipCounter(sipStatsRedirMultipleChoiceOuts));
        }
        break;
        case 301:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsRedirMovedPermIns) :
              updateSipCounter(sipStatsRedirMovedPermOuts));
        }
        break;
        case 302:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsRedirMovedTempIns) :
              updateSipCounter(sipStatsRedirMovedTempOuts));
        }
        break;
        case 303:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsRedirSeeOtherIns) :
              updateSipCounter(sipStatsRedirSeeOtherOuts));
        }
        break;
        case 305:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsRedirUseProxyIns) :
              updateSipCounter(sipStatsRedirUseProxyOuts));
        }
        break;
        case 380:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsRedirAltServiceIns) :
              updateSipCounter(sipStatsRedirAltServiceOuts));
        }
        break;
        case 400:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientBadRequestIns) :
              updateSipCounter(sipStatsClientBadRequestOuts));
        }
        break;
        case 401:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientUnauthorizedIns) :
              updateSipCounter(sipStatsClientUnauthorizedOuts));
        }
        break;
        case 402:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientPaymentReqdIns) :
              updateSipCounter(sipStatsClientPaymentReqdOuts));
        }
        break;
        case 403:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientForbiddenIns) :
              updateSipCounter(sipStatsClientForbiddenOuts));
        }
        break;
        case 404:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientNotFoundIns) :
              updateSipCounter(sipStatsClientNotFoundOuts));
        }
        break;
        case 405:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientMethNotAllowedIns) :
              updateSipCounter(sipStatsClientMethNotAllowedOuts));
        }
        break;
        case 406:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientNotAcceptableIns) :
              updateSipCounter(sipStatsClientNotAcceptableOuts));
        }
        break;
        case 407:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientProxyAuthReqdIns) :
              updateSipCounter(sipStatsClientProxyAuthReqdOuts));
        }
        break;
        case 408:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientReqTimeoutIns) :
              updateSipCounter(sipStatsClientReqTimeoutOuts));
        }
        break;
        case 409:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientConflictIns) :
              updateSipCounter(sipStatsClientConflictOuts));
        }
        break;
        case 410:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientGoneIns) :
              updateSipCounter(sipStatsClientGoneOuts));
        }
        break;
        case 411:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientLengthRequiredIns) :
              updateSipCounter(sipStatsClientLengthRequiredOuts));
        }
        break;
        case 413:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientReqEntTooLargeIns) :
              updateSipCounter(sipStatsClientReqEntTooLargeOuts));
        }
        break;
        case 414:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientReqURITooLargeIns) :
              updateSipCounter(sipStatsClientReqURITooLargeOuts));
        }
        break;
        case 415:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientNoSupMediaTypeIns) :
              updateSipCounter(sipStatsClientNoSupMediaTypeOuts));
        }
        break;
        case 420:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientBadExtensionIns) :
              updateSipCounter(sipStatsClientBadExtensionOuts));
        }
        break;
        case 480:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientTempNotAvailIns) :
              updateSipCounter(sipStatsClientTempNotAvailOuts));
        }
        break;
        case 481:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientCallLegNoExistIns) :
              updateSipCounter(sipStatsClientCallLegNoExistOuts));
        }
        break;
        case 482:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientLoopDetectedIns) :
              updateSipCounter(sipStatsClientLoopDetectedOuts));
        }
        break;
        case 483:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientTooManyHopsIns) :
              updateSipCounter(sipStatsClientTooManyHopsOuts));
        }
        break;
        case 484:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientAddrIncompleteIns) :
              updateSipCounter(sipStatsClientAddrIncompleteOuts));
        }
        break;
        case 485:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientAmbiguousIns) :
              updateSipCounter(sipStatsClientAmbiguousOuts));
        }
        break;
        case 486:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsClientBusyHereIns) :
              updateSipCounter(sipStatsClientBusyHereOuts));
        }
        break;
        case 500:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsServerIntErrorIns) :
              updateSipCounter(sipStatsServerIntErrorOuts));
        }
        break;
        case 501:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsServerNotImplementedIns) :
              updateSipCounter(sipStatsServerNotImplementedOuts));
        }
        break;
        case 502:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsServerBadGatewayIns) :
              updateSipCounter(sipStatsServerBadGatewayOuts));
        }
        break;
        case 503:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsServerServiceUnavailIns) :
              updateSipCounter(sipStatsServerServiceUnavailOuts));
        }
        break;
        case 504:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsServerGatewayTimeoutIns) :
              updateSipCounter(sipStatsServerGatewayTimeoutOuts));
        }
        break;
        case 505:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsServerBadSipVersionIns) :
              updateSipCounter(sipStatsServerBadSipVersionOuts));
        }
        break;
        case 600:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsGlobalBusyEverywhereIns) :
              updateSipCounter(sipStatsGlobalBusyEverywhereOuts));
        }
        break;
        case 603:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsGlobalDeclineIns) :
              updateSipCounter(sipStatsGlobalDeclineOuts));
        }
        break;
        case 604:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsGlobalNotAnywhereIns) :
              updateSipCounter(sipStatsGlobalNotAnywhereOuts));
        }
        break;
        case 606:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsGlobalNotAcceptableIns) :
              updateSipCounter(sipStatsGlobalNotAcceptableOuts));
        }
        break;
        default:
	{
	}
        break;
    }
    //switch here.
}


void
SipAgent::updateCommandSnmpData(Method type, SnmpType snmptype)
{
    //switch here.
    switch (type)
    {
        case SIP_INVITE:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsInviteIns) :
              updateSipCounter(sipStatsInviteOuts));
        }
        break;
        case SIP_ACK:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsAckIns) :
              updateSipCounter(sipStatsAckOuts));
        }
        break;
        case SIP_BYE:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsByeIns) :
              updateSipCounter(sipStatsByeOuts));
        }
        break;
        case SIP_CANCEL:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsCancelIns) :
              updateSipCounter(sipStatsCancelOuts));
        }
        break;
        case SIP_OPTIONS:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsOptionsIns) :
              updateSipCounter(sipStatsOptionsOuts));
        }
        break;
        case SIP_REGISTER:
        {
            ( (snmptype == INS ) ?
              updateSipCounter(sipStatsRegisterIns) :
              updateSipCounter(sipStatsRegisterOuts));
        }
        break;
        case SIP_INFO:
        {
            ( (snmptype == INS ) ? 
	      updateSipCounter(sipStatsInfoIns) :
	      updateSipCounter(sipStatsInfoOuts));
        }
        break;

        default:
	{
	}
        break;
    }//end switch
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
