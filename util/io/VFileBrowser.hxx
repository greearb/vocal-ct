#ifndef VFileBrowser_hxx
#define VFileBrowser_hxx

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

static const char* const VFileBrowser_hxx_Version =
    "$Id: VFileBrowser.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include <pthread.h>
#include <list>
#include <string>

#ifndef WIN32
#include <utime.h>
#endif

#include "VFile.hxx"
#include "VIoException.hxx"

class VFileBrowser
{
    public:
        typedef list < VFile > DirList;

        ///Constructor initialises to the root directory
        VFileBrowser(const string& root);

        ///Constructor initialises to the root directory
        VFileBrowser(): _root("/")
        { }
        ;

        ///Returns list of files in directory, relative to the root directory
        DirList directory(const string& dirName) throw (VIoException&);

        ///Makes a directory defined by the path, NOP when dir exits
        void mkDir(const string& dirPath) throw (VIoException&);

        ///Writes a file in the given path, relative to _root
        void writeFile(const string& filePath, const string& contents)
        throw (VIoException);

        ///Writes a file in the given path, relative to _root
        // sets modification time to be timestamp
        void writeFile(const string& filePath, const string& contents,
                       time_t timestamp)
        throw (VIoException);

        ///Writes a file in the given path, relative to _root
        // retunrs modification time as timestamp
        void writeFile(const string& filePath, const string& contents,
                       time_t *timestamp)
        throw (VIoException);

        //Atomic write, write the contents to a temporary location first
        //and then move the file to the actual location
        void atomicWrite(const string& fileName, const string& contents);


        ///Checks if a directory exists
        bool dirExists(const string& dirName);

        ///Rename a file
        void rename(const string& oldPath, const string& newPath)
        throw (VIoException&);

        ///Remove a file
        void remove(const string& filePath) throw (VIoException&);

        ///Remove a file/directory and anything under it
        void removeAll(const string& filePath) throw (VIoException&);

        static bool fileExists(const VFile& file);
        static int fileSize(const VFile& file);

    private:

        static int pftw(const char* fName, const struct stat* sb, int flag);
        static int addnode(const char* fName, const struct stat* sb, int flag);

        string _root;
        static DirList _dirList;
        static pthread_mutex_t _lock;

        static list < string > _toDel;

};
#endif
