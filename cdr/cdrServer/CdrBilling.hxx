#ifndef  CdrBilling_hxx
#define CdrBilling_hxx

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


static const char* const CdrBilling_hxx_Version =
    "$Id: CdrBilling.hxx,v 1.2 2004/06/09 07:19:34 greear Exp $";


#include "CdrConfig.hxx"
#include "CdrUserCache.hxx"

// Forward Declarations
//
class VCdrException;
class VRadiusException;

class CdrBilling {
protected:

   // Keep a cache of the users so that we don't have to retrieve them
   // from provisioning all the time
   CdrUserCache userAliases;

   // We need to monitor the last time we connected to the billing server,
   // if the time exceeds the time limit for storing billing records, then
   // we start deleting the oldest files 1M at a time to free up space
   //
   uint64 lastConnectTime;

   uint64 billingLockTimeLimitMs;
   string errorFileExt;

public:

   CdrBilling();

   /**
    * Read records from billing files and send to billing server,
    * return false if cannot connect to billing server
    * @param CdrConfig& configuration data
    * @param CdrUserCache&
    * @return bool true if able to connect with billing server
    */
   bool sendBillingRecords( const CdrConfig &cdata,
                                   CdrUserCache &userAliases );

   int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     int& maxdesc, uint64& timeout, uint64 now);
   
   void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                    uint64 now);

   /**
    * Delete files from billing directory
    * @param CdrConfig& configuration data
    * @return void
    */
   static void deleteOldestFiles( const CdrConfig &cdata );
};
#endif
