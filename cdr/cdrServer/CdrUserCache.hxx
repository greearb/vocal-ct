#ifndef  CdrUserCache_hxx
#define CdrUserCache_hxx

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


static const char* const CdrUserCache_hxx_Version =
    "$Id: CdrUserCache.hxx,v 1.1 2004/05/01 04:14:55 greear Exp $";


#include <string>
#include <map>
#include <utility>

#include "VRWLock.hxx"


#if defined (__linux__) && !defined (__INTEL_COMPILER)

// hash_map is alot faster than regular map

#if defined(__GNUC__) && (__GNUC__ < 3)
#include <hash_map>
#else
#include <ext/hash_map>
#endif

#include "StringHash.hxx"
typedef hash_map < string, pair < string, unsigned long int > , StringHash > UserMap;

#else

// Some STL implementations do not support the hash map eg. Solaris
// so use map instead. Note: map is alot slower
#include <map>
typedef map < string, pair < string, unsigned long int > > UserMap;

#endif

/**
   CdrUserCache, responsible for caching users as they are needed
**/

class CdrUserCache
{
    public:

        ///
        CdrUserCache();
    
        ///
        virtual ~CdrUserCache();
    
        /**
         * Get the primary customer ID (master ID)
         * @param string& alias ID
         * @return string master ID
         */
        string getCustomerId( const string& aliasId );
    
        /**
         * Any change in the user record in provisioning will
         * result in an update callback. The entry in the cache
         * for this ID and all related ones will be deleted.
         * @param string& data contains the ID
         * @param string& filename (not used)
         * @param bool deletedDir no action required if true
         * @return string master ID
         */
        static void updateMasterId( const string& data,
                                    const string& filename,
                                    const bool deletedDir );
    
        /**
         * Any change in the user record in provisioning will
         * result in an update callback. The entry in the cache
         * for this ID will be deleted.
         * @param string& data contains the ID
         * @param string& filename (not used)
         * @param bool deletedDir no action required if true
         * @return string master ID
         */
        static void updateAliases( const string& data,
                                   const string& filename,
                                   const bool deletedDir );
    
        /// destroy the static cache
        static void destroy();

    private:

        ///
        int cleanCache();
    
        /// map< aliasId, pair< masterId, index > >
        static UserMap m_userMap;
    
        /// index of oldest record in the map (used for deletions)
        static unsigned long int m_oldestIdx;

        /// current index for map
        static unsigned long int m_currentIdx;

        ///
        static VRWLock m_lock;
};
#endif
