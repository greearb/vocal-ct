#ifndef RtspFileManager_hxx
#define RtspFileManager_hxx
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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspFileManager_hxx_version =
    "$Id: RtspFileManager.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "Sptr.hxx"
#include "Data.hxx"
#include "cpLog.h"

#include <set>

/** Maintins a list of files for access locking.  Class doesn't check 
    existance of file or acutally lock file on disk.  Class is a singelton.
    This class doesn't use any mutex's so it's not multithread safe.
 */
class RtspFileManager
{
    public:
        /** create a singelton object */
        static RtspFileManager& instance();
        /** destructor */
        ~RtspFileManager();
        /**  frees the memory associated with singelton instance */
        static void destroy();

        /** adds a new file into manager
            @param filename name of new file to lock
            @return false on failure
         */
        bool add( const Data& filename );

        /** delete an existing file from manager
            @param filename name of file to unlock
            @return false on failure
         */
        bool del( const Data& filename );

        /** find if a file exists in manager
            @return true of exists
         */
        bool find( const Data& filename ) const;

    private:
        /** constructor */
        RtspFileManager();

        /** singelton object */
        static RtspFileManager* myInstance;

        /** set of locked filenames */
        set<Data> myFilenameSet;

    protected:
        /** suppress copy constructor */
        RtspFileManager( const RtspFileManager& );
        /** suppress assignment operator */
        RtspFileManager& operator=( const RtspFileManager& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
