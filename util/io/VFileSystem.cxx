
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





static const char* const VFileSystem_cxx_Version =
    "$Id: VFileSystem.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";



#include "global.h"
#ifdef __APPLE__
#include <stdint.h>
#endif
/* Change: Apple Mac OS X Support
   types.h needs to be included before dirent.h */
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#include <unistd.h>
#include <utime.h>

#include "VFileSystem.hxx"
#include "VIoException.hxx"


string
VFileSystem::readFile( const string& fileName )
throw(VException&)
{
    FILE* file = fopen( fileName.c_str(), "rb" );
    if ( !file )
    {
        throw VIoException( strerror(errno),
                            __FILE__,
                            __LINE__,
                            errno );
    }

    const int bufSize = 4096;
    char buf[4096];
    string ret;

    while ( !feof(file) )
    {
        int s = fread( buf, sizeof(char), bufSize, file );

        if ( ferror(file) )
        {
            throw VIoException( strerror(errno),
                                __FILE__,
                                __LINE__,
                                errno );
        }

        ret.append(buf, s);
    }

    int err = fclose( file );
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    return ret;
}


void
VFileSystem::writeFile( const string& fileName, const string& data )
throw(VException&)
{
    FILE* file = fopen( fileName.c_str(), "wb" );
    if ( !file )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    const char* buf = data.c_str();
    unsigned int len = data.length();

    while ( len > 0 )
    {
        int e = fwrite( buf, sizeof(char), len, file );
        buf += e;
        len -= e;

        if ( ferror(file) )
        {
            throw VIoException( strerror(errno),
                                __FILE__ ,
                                __LINE__ ,
                                errno );
        }
    }

    int err = fclose( file );
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }
}


void
VFileSystem::setFileTime( const string& fileName, TimeStamp time )
throw(VException&)
{
    struct utimbuf buf;
    buf.actime = time;
    buf.modtime = time;
    int err = utime(fileName.c_str(), &buf);
    if (err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__
                            , __LINE__ ,
                            errno );
    }
}


TimeStamp
VFileSystem::getFileTime( const string& fileName)
throw(VException&)
{
    struct stat buf;
    int err = stat( fileName.c_str(), &buf );
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    time_t t = buf.st_mtime;
    TimeStamp ret = t;

    return ret;
}


int
VFileSystem::getFileSize( const string& fileName)
throw(VException&)
{
    struct stat buf;
    int err = stat( fileName.c_str(), &buf );
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    return buf.st_size;
}


void
VFileSystem::removeFile( const string& fileName )
throw(VException&)
{
    int err = unlink(fileName.c_str());
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }
}


bool
VFileSystem::fileExists( const string& fileName )
throw(VException&)
{
    struct stat buf;
    int err = stat( fileName.c_str(), &buf );
    if ( err != 0 )
    {
        switch ( errno )
        {
            case EBADF:
            case ENOENT:
            case ENOTDIR:
            return false;
            default:
            {
                throw VIoException( strerror(errno),
                                    __FILE__,
                                    __LINE__,
                                    errno );
            }
        }
    }

    int isFile = S_ISREG(buf.st_mode);

    return isFile;
}


void
VFileSystem::createDir( const string& dirName )
throw(VException&)
{
    mode_t mode = 0777;
    int err = mkdir( dirName.c_str(), mode );
    if ( err != 0 )
    {
        switch ( errno )
        {
            case EEXIST:
            break;
            default:
            {
                throw VIoException( strerror(errno),
                                    __FILE__ ,
                                    __LINE__ ,
                                    errno );
            }
        }
    }
}


void VFileSystem::removeDir( const string& dirName )
throw(VException&)
{
    int err = rmdir(dirName.c_str());
    if ( err != 0 )
    {
        switch ( errno )
        {
            default:
            {
                throw VIoException(strerror(errno)
                                   , __FILE__ ,
                                   __LINE__
                                   , errno );
            }
        }
    }
}


bool
VFileSystem::dirExists( const string& dirName )
throw(VException&)
{
    struct stat buf;
    int err = stat( dirName.c_str(), &buf );
    if ( err != 0 )
    {
        switch ( errno )
        {
            case EBADF:
            case ENOENT:
            case ENOTDIR:
            return false;
            default:
            {
                throw VIoException( strerror(errno),
                                    __FILE__ ,
                                    __LINE__ ,
                                    errno );
            }
        }
    }

    int isDir = S_ISDIR(buf.st_mode);

    return isDir;
}


StringList
VFileSystem::readDir( const string& dirName)
throw(VException&)
{
    StringList list;

    DIR* dir = opendir(dirName.c_str());
    if ( dir == NULL )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    struct dirent * dirp = readdir(dir);
    dirp = readdir(dir);  // eat the "."
    dirp = readdir(dir);  // eat the ".."
    while ( dirp )
    {
        list.push_back( string(dirp->d_name) );
        dirp = readdir(dir);
    }

    int err = closedir(dir);
    if ( err != 0 )
    {
        throw VIoException( strerror(errno),
                            __FILE__ ,
                            __LINE__ ,
                            errno );
    }

    return list;
}
