#ifndef AGENT_API_MIB_VARS_H_
#define AGENT_API_MIB_VARS_H_

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





static const char* const AgentApiMibVarsHeaderVersion =
    "$Id: AgentApiMibVars.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

/* from all mibs for now it's here, later read from config file */
typedef enum
{
    agentApiMibVarFirstEntry = 0,   /* make sure this is the first entry */
    /* debug level of all processes on a machine */
    DebugLevel,
    /* trap notifications */
    ServerUp,
    ServerDown ,
    /* RS specific Mib objects
     * not in mib. may be used to speed up processing between snmpd and RsAgent.  Get a bulk
     * transfer from the RS of the server info
     */
    AllServerInfo ,
    SysStatsTotalServers ,
    SysStatsTotalSubscribers,
    SysStatsTotalProcessDeaths,
    SysStatsTotalProcessRestarts,
    /* MS specific Data */

    /* FS Specific Data */

    /* Sip Specific Mib Objects */
    sipSummaryInRequests,
    sipSummaryOutRequests,
    sipSummaryInResponses,
    sipSummaryOutResponses,
    sipSummaryTotalTransactions,
    sipStatsInviteIns,
    sipStatsInviteOuts,
    sipStatsAckIns,
    sipStatsAckOuts,
    sipStatsByeIns,
    sipStatsByeOuts,
    sipStatsCancelIns,
    sipStatsCancelOuts,
    sipStatsOptionsIns,
    sipStatsOptionsOuts,
    sipStatsRegisterIns,
    sipStatsRegisterOuts,
    /* the following have been grouped into SIPSTATSINFOCLASSINS/SIPSTATSINFOCLASSOUTS */
    /* in the 7/2000 sip mib. */
    sipStatsInfoIns,
    sipStatsInfoOuts,
    sipStatsInfoTryingIns,
    sipStatsInfoTryingOuts,
    sipStatsInfoRingingIns,
    sipStatsInfoRingingOuts,
    sipStatsInfoForwardedIns,
    sipStatsInfoForwardedOuts,
    sipStatsInfoQueuedIns,
    sipStatsInfoQueuedOuts,
    sipStatsInfoSessionProgIns,
    sipStatsInfoSessionProgOuts,
    /* the following have been renamed to sipStatssuccessClassIns / sipStatssuccessClassOuts */
    sipStatsSuccessOkIns,
    sipStatsSuccessOkOuts,
    /* the following have been grouped into sipStatsRedirClassIns / sipStatsRedirClassOuts */
    sipStatsRedirMultipleChoiceIns,
    sipStatsRedirMultipleChoiceOuts,
    sipStatsRedirMovedPermIns,
    sipStatsRedirMovedPermOuts,
    sipStatsRedirMovedTempIns,
    sipStatsRedirMovedTempOuts,
    sipStatsRedirSeeOtherIns,
    sipStatsRedirSeeOtherOuts,
    sipStatsRedirUseProxyIns,
    sipStatsRedirUseProxyOuts,
    sipStatsRedirAltServiceIns,
    sipStatsRedirAltServiceOuts,
    /* the following have been grouped into sipStatsReqFailClassIns / sipStatsReqFailClassOuts */
    sipStatsClientBadRequestIns,
    sipStatsClientBadRequestOuts,
    sipStatsClientUnauthorizedIns,
    sipStatsClientUnauthorizedOuts,
    sipStatsClientPaymentReqdIns,
    sipStatsClientPaymentReqdOuts,
    sipStatsClientForbiddenIns,
    sipStatsClientForbiddenOuts,
    sipStatsClientNotFoundIns ,
    sipStatsClientNotFoundOuts,
    sipStatsClientMethNotAllowedIns,
    sipStatsClientMethNotAllowedOuts,
    sipStatsClientNotAcceptableIns,
    sipStatsClientNotAcceptableOuts,
    sipStatsClientProxyAuthReqdIns,
    sipStatsClientProxyAuthReqdOuts,
    sipStatsClientReqTimeoutIns,
    sipStatsClientReqTimeoutOuts,
    sipStatsClientConflictIns,
    sipStatsClientConflictOuts,
    sipStatsClientGoneIns,
    sipStatsClientGoneOuts,
    sipStatsClientLengthRequiredIns,
    sipStatsClientLengthRequiredOuts,
    sipStatsClientReqEntTooLargeIns,
    sipStatsClientReqEntTooLargeOuts,
    sipStatsClientReqURITooLargeIns,
    sipStatsClientReqURITooLargeOuts,
    sipStatsClientNoSupMediaTypeIns,
    sipStatsClientNoSupMediaTypeOuts,
    sipStatsClientBadExtensionIns,
    sipStatsClientBadExtensionOuts,
    sipStatsClientTempNotAvailIns,
    sipStatsClientTempNotAvailOuts,
    sipStatsClientCallLegNoExistIns,
    sipStatsClientCallLegNoExistOuts,
    sipStatsClientLoopDetectedIns,
    sipStatsClientLoopDetectedOuts,
    sipStatsClientTooManyHopsIns,
    sipStatsClientTooManyHopsOuts,
    sipStatsClientAddrIncompleteIns,
    sipStatsClientAddrIncompleteOuts,
    sipStatsClientAmbiguousIns,
    sipStatsClientAmbiguousOuts,
    sipStatsClientBusyHereIns,
    sipStatsClientBusyHereOuts,
    /* the following have been grouped into sipStatsServerFailClassIns / sipStatsServerFailClassOuts */
    sipStatsServerIntErrorIns,
    sipStatsServerIntErrorOuts,
    sipStatsServerNotImplementedIns,
    sipStatsServerNotImplementedOuts,
    sipStatsServerBadGatewayIns,
    sipStatsServerBadGatewayOuts,
    sipStatsServerServiceUnavailIns,
    sipStatsServerServiceUnavailOuts,
    sipStatsServerGatewayTimeoutIns,
    sipStatsServerGatewayTimeoutOuts,
    sipStatsServerBadSipVersionIns,
    sipStatsServerBadSipVersionOuts ,
    /* the following have been grouped into sipStatsGlobalFailClassIns / sipStatsGlobalFailClassOuts */
    sipStatsGlobalBusyEverywhereIns,
    sipStatsGlobalBusyEverywhereOuts,
    sipStatsGlobalDeclineIns,
    sipStatsGlobalDeclineOuts,
    sipStatsGlobalNotAnywhereIns,
    sipStatsGlobalNotAnywhereOuts,
    sipStatsGlobalNotAcceptableIns,
    sipStatsGlobalNotAcceptableOuts,
    /* the new higher level statistics are shown below: */
    sipStatsInfoClassIns,
    sipStatsInfoClassOuts,
    sipStatsSuccessClassIns,
    sipStatsSuccessClassOuts,
    sipStatsRedirClassIns,
    sipStatsRedirClassOuts,
    sipStatsReqFailClassIns,
    sipStatsReqFailClassOuts,
    sipStatsServerFailClassIns,
    sipStatsServerFailClassOuts,
    sipStatsGlobalFailClassIns,
    sipStatsGlobalFailClassOuts,
    /* done with the grouping of statistics messages */
    sipCurrentTransactions,
    sipTransTo,
    sipTransFrom,
    sipTransCallId,
    sipTransCSeq,
    sipTransState,
    sipTransOutstandingBranches,
    sipTransExpiry,
    sipTransCallingPartyContentType,
    sipTransCalledPartyContentType,
    sipNumUnsupportedUris,
    /* this next group is the sipCommonConfigGroup */
    sipProtocolVersion,
    sipServiceOperStatus,
    sipServiceAdminStatus,
    sipServiceStartTime,
    sipServiceLastChange,
    sipTransport,
    sipPortStatus,
    sipUriSupported,
    sipFtrSupported,
    sipOrganization,
    sipMaxTransactions,
    sipRequestDfltExpires,
    sipHideOperation,
    sipUserLocationServerAddr,

    /* Network Application MIB */
    applIndex,
    applName,
    applDirectoryName,
    applVersion,
    applUpTime,
    applOperStatus,
    applLastChange,

    agentApiMibVarLastEntry   /* make sure this is the last entry */
} AgentApiMibVarT;

#endif /*AGENT_API_MIB_VARS_H */
