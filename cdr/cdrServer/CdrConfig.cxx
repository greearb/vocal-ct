
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


static const char* const CdrConfig_cxx_Version =
    "$Id: CdrConfig.cxx,v 1.5 2004/08/18 22:39:14 greear Exp $";


#include <fstream.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <ctype.h>

#include "CdrConfig.hxx"
#include "ProvisionInterface.hxx"
#include "VCdrException.hxx"
#include "NetworkAddress.h"
#include "cpLog.h"


// Valid tag strings for CDR configuration file
static const char* cdrConfigTagString[CdrConfig::CdrConfigTagMax] =
{
   "CdrServerHost",
   "CdrServerPort",
   "CdrBillingFrequency",
   "CdrBillingDirectory",
   "CdrBillingFileName",
   "CdrLogLevel",
   "CdrLogFile",
   "CdrRadiusServerHost",
   "CdrRadiusRetries",
   "CdrRadiusSecretKey",
   "CdrUnsentFileExt",
   "CdrBillingLockFile",
   "CdrRolloverSize",
   "CdrRolloverPeriod",
   "CdrFileCheckFreq",
   "CdrBillRingTime",
   "CdrLocalIp"
};

CdrConfig::CdrConfig()
      : m_serverHost("localhost"),
        m_serverPort(10000),
        m_billingFrequency(86400),
        m_billingDirectory("/usr/local/billing/"),
        m_billingFileName("billing.dat"),
        m_logLevel(LOG_DEBUG),
        m_radiusServerHost("192.168.1.1"),
        m_radiusRetries(2),
        m_radiusSecretKey("testing123"),
        m_unsentFileExt(".unsent"),
        m_billingLockFile(".billinglock"),
        m_rolloverSize(5000000),
        m_rolloverPeriod(86400),
        m_fileCheckFreq(1800),
        m_billRingTime(false),
        m_localIp("")
{}

CdrConfig::CdrConfig( const CdrConfig &rhs )
      : m_serverHost( rhs.m_serverHost ),
        m_serverPort( rhs.m_serverPort ),
        m_billingFrequency( rhs.m_billingFrequency ),
        m_billingDirectory( rhs.m_billingDirectory ),
        m_billingFileName( rhs.m_billingFileName ),
        m_logLevel( rhs.m_logLevel ),
        m_radiusServerHost( rhs.m_radiusServerHost ),
        m_radiusRetries( rhs.m_radiusRetries ),
        m_radiusSecretKey( rhs.m_radiusSecretKey ),
        m_unsentFileExt( rhs.m_unsentFileExt ),
        m_billingLockFile( rhs.m_billingLockFile ),
        m_rolloverSize( rhs.m_rolloverSize ),
        m_rolloverPeriod( rhs.m_rolloverPeriod ),
        m_fileCheckFreq( rhs.m_fileCheckFreq ),
        m_billRingTime( rhs.m_billRingTime ),
        m_localIp( rhs.m_localIp )
{}

int
CdrConfig::cdrConfigTag2i( const char *tag ) {
   int i;

   // search until matching tag is found
   for (i = 0; i < CdrConfigTagMax; i++) {
      if (strcmp(tag, cdrConfigTagString[i]) == 0) {
         cpLog (LOG_DEBUG, "Matched tag = %s", tag);
         return i;
      }
   }

   // no match found, report error
   cpLog (LOG_ERR, "Unknown tag = %s", tag);
    
   return CdrConfigTagMax;
}


void
CdrConfig::setEnvCdr() {
   // The environment variables take priority, so set them last
   char *envptr;

   if ((envptr = getenv("CDRSERVERHOST")) != 0) {
      m_serverHost = envptr;
      cpLog(LOG_DEBUG, "Using CDRSERVERHOST defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRSERVERPORT")) != 0) {
      m_serverPort = atoi(envptr);
      cpLog(LOG_DEBUG, "Using CDRSERVERPORT defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRBILLINGFREQUENCY")) != 0) {
      m_billingFrequency = atoi(envptr);
      cpLog(LOG_DEBUG, "Using CDRBILLINGFREQUENCY defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRUNSENTFILEEXT")) != 0) {
      m_unsentFileExt = envptr;
      cpLog(LOG_DEBUG, "Using CDRUNSENTFILEEXT defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRBILLINGLOCKFILE")) != 0) {
      m_billingLockFile = envptr;
      cpLog(LOG_DEBUG, "Using CDRBILLINGLOCKFILE defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRBILLINGDIRECTORY")) != 0) {
      m_billingDirectory = envptr;
      cpLog(LOG_DEBUG, "Using CDRBILLINGDIRECTORY defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRBILLINGFILENAME")) != 0) {
      m_billingFileName = envptr;
      cpLog(LOG_DEBUG, "Using CDRBILLINGFILENAME defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRLOGLEVEL")) != 0) {
      m_logLevel = atoi(envptr);
      cpLog(LOG_DEBUG, "Using CDRLOGLEVEL defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRLOGFILE")) != 0) {
      m_logFile = envptr;
      cpLog(LOG_DEBUG, "Using CDRLOGFILE defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRRADIUSSERVERHOST")) != 0) {
      m_radiusServerHost = envptr;
      cpLog(LOG_DEBUG, "Using CDRRADIUSSERVERHOST defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRRADIUSRETRIES")) != 0) {
      m_radiusRetries = atoi(envptr);
      cpLog(LOG_DEBUG, "Using CDRRADIUSRETRIES defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRRADIUSSECRETKEY")) != 0) {
      m_radiusSecretKey = envptr;
      cpLog(LOG_DEBUG, "Using CDRRADIUSSECRETKEY defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRROLLOVERSIZE")) != 0) {
      m_rolloverSize = atoi(envptr);
      cpLog(LOG_DEBUG, "Using CDRROLLOVERSIZE defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRROLLOVERPERIOD")) != 0) {
      m_rolloverPeriod = atoi(envptr);
      cpLog(LOG_DEBUG, "Using CDRROLLOVERPERIOD defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRFILECHECKFREQ")) != 0) {
      m_fileCheckFreq = atoi(envptr);
      cpLog(LOG_DEBUG, "Using CDRFILECHECKFREQ defined in environment: %s", envptr);
   }

   if ((envptr = getenv("CDRBILLRINGTIME")) != 0) {
      m_billRingTime = true;
      cpLog(LOG_DEBUG, "Using CDRBILLRINGTIME defined in environment: Ring time will be billed");
   }

   if ((envptr = getenv("CDRLOCALIP")) != 0) {
      m_localIp = envptr;
      cpLog(LOG_DEBUG, "Using CDRLOCALIP defined in environment: %s", envptr);
   }
}

#warning "Implement getting CDR info from ProvisionInterface"
#if 0
// TODO:  Get this from the provisioning server
   m_billingDirectory = psCdrData.getBillingDir();
   m_billingFileName = psCdrData.getBillingDataFile();
   m_billingFrequency = psCdrData.getBillingFreq();
   m_unsentFileExt = psCdrData.getBillingUnsentExt();
   m_billingLockFile = psCdrData.getBillingLockFile();
   m_serverPort = psCdrData.getPort();
   m_radiusServerHost = psCdrData.getRadiusHost();
   m_radiusSecretKey = psCdrData.getRadiusKey();
   m_radiusRetries = psCdrData.getRadiusRetries();
   m_rolloverSize = psCdrData.getRolloverSize();
   m_rolloverPeriod = psCdrData.getRolloverPeriod();
   m_billRingTime = psCdrData.getBillForRingtime();
   
   // Logically, the file check frequency should not be more than
   // the rolloverPeriod
   if (m_fileCheckFreq > m_rolloverPeriod) {
      m_fileCheckFreq = m_rolloverPeriod;
   }

   // since the env vars are used for debugging, they override
   // the config and ps values
   setEnvCdr();
}
#endif

void
CdrConfig::print(const int loglevel) {
   cpLog(loglevel, "CDR Server Host:     %s", m_serverHost.c_str());
   cpLog(loglevel, "CDR Server Port:     %d", m_serverPort);
   cpLog(loglevel, "Billing Directory:   %s", m_billingDirectory.c_str());
   cpLog(loglevel, "Billing File:        %s", m_billingFileName.c_str());
   cpLog(loglevel, "Billing Frequency:   %d", m_billingFrequency);
   cpLog(loglevel, "Billing Unsent ext:  %s", m_unsentFileExt.c_str());
   cpLog(loglevel, "Billing Lock File:   %s", m_billingLockFile.c_str());
   if (m_billRingTime) {
      cpLog(loglevel, "Billing Ring Time:   Yes");
   }
   else {
      cpLog(loglevel, "Billing Ring Time:   No");
   }
   cpLog(loglevel, "File Rollover Size:  %d", m_rolloverSize);
   cpLog(loglevel, "File Rollover Period:%d", m_rolloverPeriod);
   cpLog(loglevel, "File Check Frequency:%d", m_fileCheckFreq);
   cpLog(loglevel, "Radius Hostname:     %s", m_radiusServerHost.c_str());
   cpLog(loglevel, "Radius Secret Key:   %s", m_radiusSecretKey.c_str());
   cpLog(loglevel, "Radius Retries:      %d", m_radiusRetries);
   cpLog(loglevel, "Local IP:            %s", m_localIp.c_str());
   
   cout << "Log file information" << endl;
   cout << "--------------------" << endl;
   cpLogShow();
}
