#ifndef  CdrCache_hxx
#define CdrCache_hxx

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


static const char* const CdrCache_hxx_Version =
    "$Id: CdrCache.hxx,v 1.2 2004/06/14 00:33:53 greear Exp $";


#include <string>

#include "CdrConfig.hxx"
#include "CdrData.hxx"
#include "CdrFileHandler.hxx"
#include "EventObj.hxx"

#if defined (__linux__) && !defined (__INTEL_COMPILER)

// hash_map is alot faster than regular map
#if defined(__GNUC__) && (__GNUC__ < 3)
#include <hash_map>
#else
#include <ext/hash_map>
#endif

#include "StringHash.hxx"
typedef hash_map < string, CdrRadius, StringHash > CdrMap;

#else

// Some STL implementations do not support the hash map eg. Solaris
// so use map instead. Note: map is alot slower
#include <map>
typedef map < string, CdrRadius > CdrMap;

#endif


class VCdrException;
class CdrFileHandler;


/**
    CdrCache is responsible for keeping the CDR data (up to
    72 hours). It will also correlate CdrClient records with
    the CdrRadius records.
    The completed call records are stored in a text file and
    relayed to the billing server by CdrBilling
**/

class CdrCache : public EventObj {
public:

   /**
    * Must be constructed with configuration data
    * @param CdrConfig& configuration data
    */
   CdrCache( const CdrConfig &cdata );
    
   ///
   virtual ~CdrCache();
   
   /// Event call back
   void onTimer();

   /**
    * Correlate client data with radius records.
    * Each record is written to the cdr billing file.
    * @param CdrClient& data record
    * @return void
    */
   void add( const CdrClient &dat ) throw (VCdrException&);

   /// Purge billed and old records
   void purgeCache();

private:

   ///
   CdrMap m_cdrMap;
   ///
   CdrFileHandler m_fileHandle;
   ///
   CdrConfig m_data;
   ///
   unsigned long int m_lastFileCheck;
};
#endif
