
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

static const char* const VFileBrowser_cxx_Version =
    "$Id: VFileBrowser.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "global.h"
#include <cerrno>
#include <cstdio>

// Add: Apple Mac OS X Support
#if !defined(__FreeBSD__) && !defined(WIN32) && !defined(__APPLE__)
#include <ftw.h>
#else
#include "ftw.h"
#endif


#include <sys/stat.h>
#include <unistd.h>
#include <strstream>
#include <fstream>

#include "VFileBrowser.hxx"
#include "VIoException.hxx"
#include "cpLog.h"

pthread_mutex_t VFileBrowser::_lock = PTHREAD_MUTEX_INITIALIZER;


VFileBrowser::DirList VFileBrowser::_dirList;
list < string > VFileBrowser::_toDel;

VFileBrowser::VFileBrowser(const string& root)
        : _root(root)
{}


void
VFileBrowser::removeAll(const string& dirPath) throw (VIoException&)
{

    VFileBrowser::_toDel.clear();

    string dPath(_root);
    if (dirPath.size() && dirPath[0] != '/') dPath += "/";

    // path should always be something like /usr/local/vocal or more
    // to be safe, abort if too short

    dPath += dirPath;

    if (dPath.size() < 16)
    {
        cpLog(LOG_ERR, "Cowardly refusing to delete tree with path",
              " name less than 16 characters : %s", dPath.c_str());
        return ;
    }

    pthread_mutex_lock(&VFileBrowser::_lock);
    VFileBrowser::_dirList.clear();
    if (ftw(dPath.c_str(), VFileBrowser::addnode, 5) , 0)
    {
        char buf[256];
        sprintf(buf, "Failed to get dir listing for %s, reason %s",
                dPath.c_str(), strerror(errno));
        cpLog(LOG_ALERT, buf);
        pthread_mutex_unlock(&_lock);
        throw VIoException(buf, __FILE__, __LINE__, errno);
    }

    list < string > ::iterator itr;
    for (itr = _toDel.begin(); itr != _toDel.end(); itr++)
    {
        if (::remove((*itr).c_str()) == -1)
        {
            char buf[256];
            sprintf(buf, "Failed to remove file %s, reason %s",
                    (*itr).c_str(),
                    strerror(errno));
            throw VIoException (buf, __FILE__, __LINE__, errno);
        }

    }



    pthread_mutex_unlock(&VFileBrowser::_lock);
}



VFileBrowser::DirList
VFileBrowser::directory(const string& dirPath) throw (VIoException&)
{
    string dPath(_root);
    if (dirPath.size() && dirPath[0] != '/') dPath += "/";
    dPath += dirPath;
    pthread_mutex_lock(&VFileBrowser::_lock);
    VFileBrowser::_dirList.clear();
    if (ftw(dPath.c_str(), VFileBrowser::pftw, 5) , 0)
    {
        char buf[256];
        sprintf(buf, "Failed to get dir listing for %s, reason %s",
                dPath.c_str(), strerror(errno));
        cpLog(LOG_ALERT, buf);
        pthread_mutex_unlock(&_lock);
        throw VIoException(buf, __FILE__, __LINE__, errno);
    }
    DirList dirList = VFileBrowser::_dirList;
    pthread_mutex_unlock(&VFileBrowser::_lock);
    return (dirList);
}

int
VFileBrowser::pftw(const char* fName, const struct stat* sb, int flag)
{
    if (flag == FTW_F)
    {
        VFile aFile(fName, flag, sb->st_mtime);
        _dirList.push_back(aFile);
    }
    return 0;
}


int
VFileBrowser::addnode(const char* fName, const struct stat* sb, int flag)
{
    _toDel.push_front(fName);
    return 0;
}


void
VFileBrowser::mkDir(const string& dirPath) throw (VIoException&)
{
    string reconPath(_root);
    reconPath += dirPath;
    cpLog(LOG_DEBUG, "Creating dir %s", reconPath.c_str());
    if (!dirExists(dirPath))
    {
        if (dirPath.find("/") != string::npos)
        {
            //Directory has subdir in it
            //use mkdir -p which would create all
            //the directories in path
            string cmdStr("mkdir -p ");
            cmdStr += reconPath;
            if (system(cmdStr.c_str()) < 0)
            {
                char buf[256];
                ostrstream ostr(buf, 256);
                ostr << "Failed to create dir :"
                << reconPath
                << ", reason:"
                << strerror(errno)
                << ends;
                cpLog(LOG_ALERT, ostr.str());
                throw VIoException(ostr.str(), __FILE__, __LINE__, errno);
            }
        }
    }
}

void
VFileBrowser::writeFile(const string& filePath, const string& content) throw (VIoException)
{
    string path;
    string::size_type pos = filePath.find_last_of("/");
    if (pos != string::npos)
        path = filePath.substr(0, filePath.find_last_of("/"));
    mkDir(path);
    string reconPath(_root);
    reconPath += filePath;
    ofstream fout(reconPath.c_str());
    if (fout.bad())
    {
        char buf[256];
        ostrstream ostr(buf, 256);
        ostr << "Failed to open file :"
        << reconPath
        << ", reason:"
        << strerror(errno)
        << ends;
        cpLog(LOG_DEBUG, ostr.str());
        throw VIoException(ostr.str(), __FILE__, __LINE__, errno);
    }
    //Write data to file
    fout << content.c_str();
    fout.flush();
}

void
VFileBrowser::writeFile(const string& filePath, const string& content,
                        time_t timestamp)
throw (VIoException)
{

    struct stat statBuf;
    struct utimbuf timeBuf;
    string path;
    string::size_type pos = filePath.find_last_of("/");
    if (pos != string::npos)
        path = filePath.substr(0, filePath.find_last_of("/"));
    mkDir(path);
    string reconPath(_root);
    reconPath += filePath;
    ofstream fout(reconPath.c_str());
    if (fout.bad())
    {
        char buf[256];
        ostrstream ostr(buf, 256);
        ostr << "Failed to open file :"
        << reconPath
        << ", reason:"
        << strerror(errno)
        << ends;
        cpLog(LOG_DEBUG, ostr.str());
        throw VIoException(ostr.str(), __FILE__, __LINE__, errno);
        return ;
    }
    //Write data to file
    fout << content.c_str();
    fout.flush();


    if (stat(reconPath.c_str(), &statBuf) < 0)
    {
        cpLog(LOG_ERR, "Failed to stat file that was just created!");
        throw VIoException(
            "Failed to stat file that was just created!",
            __FILE__, __LINE__, errno);
        return ;
    }

    timeBuf.actime = statBuf.st_atime;
    timeBuf.modtime = timestamp;

    if (utime(reconPath.c_str(), &timeBuf) < 0)
    {
        cpLog(LOG_ERR,
              "Failed to set timestamp for file that was just created!");
        throw VIoException(
            "Failed to set timestamp for file that was just created!",
            __FILE__, __LINE__, errno);
        return ;
    }

}

void
VFileBrowser::writeFile(const string& filePath, const string& content,
                        time_t *timestamp)
throw (VIoException)
{

    struct stat statBuf;
    string path;
    string::size_type pos = filePath.find_last_of("/");
    if (pos != string::npos)
        path = filePath.substr(0, filePath.find_last_of("/"));
    mkDir(path);
    string reconPath(_root);
    reconPath += filePath;
    ofstream fout(reconPath.c_str());
    if (fout.bad())
    {
        char buf[256];
        ostrstream ostr(buf, 256);
        ostr << "Failed to open file :"
        << reconPath
        << ", reason:"
        << strerror(errno)
        << ends;
        cpLog(LOG_DEBUG, ostr.str());
        throw VIoException(ostr.str(), __FILE__, __LINE__, errno);
        return ;
    }

    //Write data to file
    fout << content.c_str();
    fout.flush();


    if (stat(reconPath.c_str(), &statBuf) < 0)
    {
        cpLog(LOG_ERR, "Failed to stat file that was just created!");
        throw VIoException(
            "Failed to stat file that was just created!",
            __FILE__, __LINE__, errno);
        return ;
    }

    *timestamp = statBuf.st_mtime;

}



bool
VFileBrowser::dirExists(const string& dirName)
{
    struct stat stBuf;
    if (stat(dirName.c_str(), &stBuf) < 0)
    {
        return false;
    }
    return true;
}

void
VFileBrowser::rename(const string& oldPath, const string& newPath)
throw (VIoException&)
{
    string rOldPath(_root);
    rOldPath += oldPath;

    string rNewPath(_root);
    rNewPath += newPath;
    cpLog(LOG_DEBUG, "Renaming frm %s to %s", rOldPath.c_str(), rNewPath.c_str());
    if (::rename(rOldPath.c_str(), rNewPath.c_str()) < 0)
    {
        char buf[256];
        ostrstream ostr(buf, 256);
        ostr << "Failed to rename file from :"
        << oldPath
        << " to "
        << newPath
        << ", reason:"
        << strerror(errno)
        << ends;
        cpLog(LOG_DEBUG, ostr.str());
        throw VIoException(ostr.str(), __FILE__, __LINE__, errno);
    }
}

void
VFileBrowser::atomicWrite(const string& fileName, const string& contents)
{
    //First write file to modify dir and then rename it to its
    //original locaion
    string modFileName;
    string::size_type pos = fileName.find_last_of("/");
    modFileName = fileName;
    if (pos != string::npos)
    {
        modFileName.insert(pos, "/modify/");
    }
    else
    {
        modFileName = "/modify/";
        modFileName += fileName;
    }
    writeFile(modFileName, contents);
    //Now move the file to its original location
    rename(modFileName, fileName);

}

///Remove a file
void
VFileBrowser::remove(const string& filePath) throw (VIoException&)
{
    string reconPath(_root);
    if (filePath.size() && filePath[0] != '/') reconPath += "/";
    reconPath += filePath;
    if (::remove(reconPath.c_str()) == -1)
    {
        char buf[256];
        sprintf(buf, "Failed to remove file %s, reason %s", reconPath.c_str(),
                strerror(errno));
        throw VIoException (buf, __FILE__, __LINE__, errno);
    }
}

bool
VFileBrowser::fileExists(const VFile& file)
{
    struct stat statBuf;
    if (stat(file.getFullName().c_str(), &statBuf) < 0)
    {
        if (errno == ENOENT) return false;
        cpLog(LOG_ALERT, "Failed to get stat for file %s", file.getFullName().c_str());
        return false;
    }
    return true;
}

int
VFileBrowser::fileSize(const VFile& file)
{
    struct stat statBuf;
    if (stat(file.getFullName().c_str(), &statBuf) < 0)
    {
        cpLog(LOG_ALERT, "Failed to get stat for file %s", file.getFullName().c_str());
        return 0;
    }
    return statBuf.st_size;
}
