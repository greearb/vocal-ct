#ifndef  CdrConfig_h
#define CdrConfig_h

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


static const char* const CdrConfig_hxx_Version =
    "$Id: CdrConfig.hxx,v 1.2 2004/06/09 07:19:34 greear Exp $";

#include "global.h"
#include <string>
#include "VCdrException.hxx"

/**
   CdrConfig will use the variable values from the environment
   first and if it doesn't exist in the environment, the config
   file values will be the default. There are also hard-coded
   defaults which are used if the values are not defined in neither
   the environment or config file.
 **/

class CdrConfig {
public:
   ///
   enum CdrConfigTag {
      CdrServerHost = 0,       ///
      CdrServerPort,           ///
      CdrBillingFrequency,     ///
      CdrBillingDirectory,     ///
      CdrBillingFileName,      ///
      CdrLogLevel,             ///
      CdrLogFile,              ///
      CdrRadiusServerHost,     ///
      CdrRadiusRetries,        ///
      CdrRadiusSecretKey,      ///
      CdrUnsentFileExt,        ///
      CdrBillingLockFile,      ///
      CdrRolloverSize,         ///
      CdrRolloverPeriod,       ///
      CdrFileCheckFreq,        ///
      CdrBillRingTime,         ///
      CdrLocalIp,              ///
      CdrConfigTagMax          ///
   };

public:

   ///
   CdrConfig();
    
   /**
    * Copy constructor
    * @param CdrConfig& object to copy
    */
   CdrConfig( const CdrConfig &rhs );
    
   ///
   virtual ~CdrConfig() {}
    
   ///
   void print(const int loglevel);

   ///
   int cdrConfigTag2i( const char *tag );
    
   ///
   void setCdrConfig( const char* tag,
                      const char* type,
                      const char* value);
    
   ///
   void setEnvCdr();

   /**
    * Load data from the config file, environment vars override file
    * @param string& configuration filename
    * @return void
    */
   void getData( const string &fileName ) throw (VCdrException&);

   /**
    * Load data from provisioning, environment vars override ps
    */
   void getPsData( const NetworkAddress& psHost,
                   const NetworkAddress& altHost,
                   const string& readSecret,
                   const string& writeSecret)
      throw (VCdrException&);

public:
   ///
   string m_serverHost;                   ///
   int m_serverPort;                      ///
   int m_billingFrequency;                ///
   string m_billingDirectory;             ///
   string m_billingFileName;              ///
   int m_logLevel;                        ///
   string m_logFile;                      ///
   string m_radiusServerHost;             ///
   int m_radiusRetries;                   ///
   string m_radiusSecretKey;              ///
   string m_unsentFileExt;                ///
   string m_billingLockFile;              ///
   int m_rolloverSize;                    ///
   unsigned long int m_rolloverPeriod;    ///
   unsigned long int m_fileCheckFreq;     ///
   bool m_billRingTime;                   ///
   string m_localIp;
};
#endif
