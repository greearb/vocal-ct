#ifndef SIPSNMPDETAILS_HXX
#define SIPSNMPDETAILS_HXX

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

#if 0


#include <cassert>

#include "SipFtrSupportedTable.hxx"
#include "SipUriSupportedTable.hxx"
#include "SipFtrSupportedEntry.hxx"
#include "SipUriSupportedEntry.hxx"
#include "symbols.hxx"
#include "SnmpData.hxx"
#include "SipAgent.hxx"


namespace Vocal
{
    
/* Operation Status of the service     */

typedef enum {

    shutdwn = 0,
    reset,
    restart,
    stop,
    rts
} serviceOperStatus;

/* Operation Status of the application */
typedef enum {

    aup = 0,
    adown,
    atesting,
    aunknown,
    ahalted,
    arestarting

} appOperStatus;


///
class SipSnmpDetails: public BugCatcher
{

    public:
        ///
        friend class SipAgent;
        ///
        typedef map < int , snmpData > stackDataMap;
        ///
        SipSnmpDetails(SipAgent*, const Data& name);
        ///
        virtual ~SipSnmpDetails();
        ///
        serviceOperStatus getserviceOperStatus();
        ///
        void setServiceOperStatus(serviceOperStatus);
        ///
        void setAppOperStatus(appOperStatus);
        ///
        void setServiceStartTime();
        ///
        void setServiceLastChange();
        ///
        long getServiceLastChange();
        ///
        long getServiceStartTime();
        ///
        appOperStatus getAppOperStatus();
        ///
        int ftrSize();
        ///
        string sipFtrSupported(int);
        ///
        int uriSize();
        ///
        string sipUriSupported(int);
        ///
        string getSipOrganization();
        ///
        unsigned long getMaxTransactions();
        ///
        void setSipOrganization(string );
        ///
        unsigned long getSipRequestDfltExpires();
        ///
        void setSipRequestDfltExpires(unsigned int);
        ///
        Data getappName();
        ///
        void setstackdata(int index, const snmpData&);
        ///
        snmpData getstackdata(int index);


    private:
        ///
        long starttime;
        ///
        long lastchange;
        ///
        serviceOperStatus currentStatus;
        ///
        appOperStatus appStatus;
        ///
        SipAgent* sAgent;
        ///

        stackDataMap stackMap;

        Data appName;
};
 
} // namespace Vocal

#endif // AGENT_API_MIB_VARS_HXX

#endif
