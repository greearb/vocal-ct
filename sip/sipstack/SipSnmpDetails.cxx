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

static const char* const SipSnmpDetails_cxx_Version =
    "$Id: SipSnmpDetails.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <iostream>
#include <sys/time.h>

#include "SipSnmpDetails.hxx"
#include "SnmpData.hxx"
#include "Data.hxx"

using namespace Vocal;

SipSnmpDetails::SipSnmpDetails(SipAgent* sipagent, const Data& name)
    : stackMap()
{
    currentStatus = rts;
    appStatus = aunknown;

    appName = name;
    //do all the initial settings over here
    setServiceOperStatus(restart);
    setAppOperStatus(aup);
    setServiceStartTime();
    setServiceLastChange();
   

        getWriteLock();
        for (int i = agentApiMibVarFirstEntry; i < agentApiMibVarLastEntry; i++)
        {

            if ( (AgentApiMibVarT)i == sipProtocolVersion)

            {  //done


                snmpData vc(false, false);
                vc.setData("SIP/2.0");
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipServiceOperStatus)
            {  //done

                snmpData vc(false, false);
                vc.setData(Data(getserviceOperStatus()));
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipServiceAdminStatus)
            {
                snmpData vc(false, false);
                vc.setData(Data(getAppOperStatus()));
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipServiceStartTime)
            {

                snmpData vc(false, false);
                vc.setData(Data(int(getServiceStartTime())));
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipServiceLastChange)

            {
                snmpData vc(false, false);
                vc.setData(Data(int(getServiceLastChange())));
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipTransport)

            {
                snmpData vc(false, false) ;
                vc.setData("Not Implemented");
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipPortStatus)

            {
                snmpData vc(false, false);
                vc.setData("Not Implemented");
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipOrganization)

            {
                snmpData vc(false, false);
                vc.setData(getSipOrganization());
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipMaxTransactions)

            {
                snmpData vc(false, false);
                vc.setData(Data(getMaxTransactions()));
                setstackdata( i, vc);

            }
            else if ( (AgentApiMibVarT)i == sipRequestDfltExpires)

            {
                snmpData vc(false, false);
                vc.setData(Data(getSipRequestDfltExpires()));
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipHideOperation)

            {
                snmpData vc(false, false);
                vc.setData("Not Implemented");
                setstackdata( i , vc);

            }
            else if ( (AgentApiMibVarT)i == sipUserLocationServerAddr)

            {
                snmpData vc(false, false);
                vc.setData("Not implemented");
                setstackdata( i , vc);
            }

            else
                //  default:
            {
                snmpData vc(false, false);
                setstackdata( i , vc);

            }
        }
        getUnLock();
        sAgent = sipagent;

	//   } //non empty name
}



SipSnmpDetails::~SipSnmpDetails()
{}



serviceOperStatus
SipSnmpDetails::getserviceOperStatus()
{
    return currentStatus;
}


void
SipSnmpDetails::setServiceOperStatus(serviceOperStatus a)
{
    currentStatus = a;
}


void
SipSnmpDetails::setServiceStartTime()
{
    time_t curtime;
    struct tm *loctime;
    curtime = time (NULL);
    struct tm res;
#if defined(__APPLE__)
    loctime = localtime (&curtime);
#else
    loctime = localtime_r (&curtime, &res);
#endif
    starttime = mktime(loctime);
}


void
SipSnmpDetails::setServiceLastChange()
{
    time_t curtime;
    struct tm *loctime;
    curtime = time (NULL);
    struct tm res;
#if defined(__APPLE__)
    loctime = localtime (&curtime);
#else
    loctime = localtime_r (&curtime, &res);
#endif
    lastchange = mktime(loctime);
}


long
SipSnmpDetails::getServiceLastChange()
{
    return lastchange;
}


long
SipSnmpDetails::getServiceStartTime()
{
    return starttime;
}


appOperStatus
SipSnmpDetails::getAppOperStatus()
{
    return appStatus;
}


void
SipSnmpDetails::setAppOperStatus(appOperStatus a)
{
    appStatus = a;
}


int
SipSnmpDetails::ftrSize()
{
    sipFtrSupportedTable ftrTable;
    return ftrTable.getSize();
}


string
SipSnmpDetails::sipFtrSupported(int index)
{
    sipFtrSupportedTable ftrTable;
    return ftrTable.getEntry(index).getSnmpAdminString();
}


int
SipSnmpDetails::uriSize()
{
    SipUriSupportedTable uriTable;
    return uriTable.getSize();
}


string
SipSnmpDetails::sipUriSupported(int index)
{
    SipUriSupportedTable uriTable;
    return uriTable.getEntry(index).getSnmpAdminString();
}


string
SipSnmpDetails::getSipOrganization()
{
string s;
    return s;
}


unsigned long
SipSnmpDetails::getMaxTransactions()
{
    return 1;
}

void
SipSnmpDetails::setSipOrganization(string s)
{
}


unsigned long
SipSnmpDetails::getSipRequestDfltExpires()
{
    return 32;
}


void
SipSnmpDetails::setSipRequestDfltExpires(unsigned int expires)
{}

/*
   ( This function should be read/write)
    
    uint  SipSnmpDetails::getSipRequestDfltExpires() { return 1;}
    
    void  SipSnmpDetails::setHideOperation(HideOper ) {}
    HideOper  SipSnmpDetails::getHideOperation(){ return h;}
    
    void  SipSnmpDetails::UserLocationServAddr(string) { location =string; }
    string  SipSnmpDetails::getUserLocationServAddr() {return location; }
    


voReturnStatus
SipSnmpDetails::updateSipCounter(AgentApiMibVarT inCounterName)
{
    voReturnStatus x = sAgent->updateSipCounter(inCounterName);
    return x;
}


*/


Data SipSnmpDetails::getappName()
{
    return appName;
}


void SipSnmpDetails::getReadLock(void)
{
    rwlock.readlock();
}


void SipSnmpDetails::getWriteLock(void)
{
    rwlock.writelock();
}


void SipSnmpDetails::getUnLock(void)
{
    rwlock.unlock();
}


void SipSnmpDetails::setstackdata(int index, const snmpData& data)
{
    stackMap[index] = data;

}
snmpData SipSnmpDetails::getstackdata(int index)
{
    snmpData stdata(false, false);
    getReadLock();
    stackDataMap::iterator i = stackMap.find(index);
    if (i != stackMap.end())
    {
        stdata = i->second;


    }
    getUnLock();
    return stdata ;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
