
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


static const char* const CdrFileHandler_cxx_Version =
    "$Id: CdrFileHandler.cxx,v 1.1 2004/05/01 04:14:55 greear Exp $";


#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream.h>
#include <dirent.h>

#include "CdrFileHandler.hxx"
#include "VCdrException.hxx"
#include "Sptr.hxx"
#include "cpLog.h"

CdrFileHandler::CdrFileHandler(const string &directory,
                               const string &filename)
    throw(VCdrException&) :
        m_directory(directory),
        m_fileName(filename),
        m_dirPathFileName(directory + '/' + filename),
        m_fileCheckFreq(false),
        m_rolloverSize(5000000),
        m_rolloverPeriod(86400),
        m_fileDesc(0),
        m_eof(true),
        m_bptr(m_readBuffer),
        m_bytesRead(0),
        m_timeOpened(0),
        m_fileSize(0),
        m_lastModified(0)
{}


CdrFileHandler::CdrFileHandler(const CdrFileHandler& obj)
{
    m_directory = obj.m_directory;
    m_fileName = obj.m_fileName;
    m_dirPathFileName = obj.m_dirPathFileName;
    m_renameSuffix = obj.m_renameSuffix;
    m_fileCheckFreq = obj.m_fileCheckFreq;
    m_rolloverSize = obj.m_rolloverSize;
    m_rolloverPeriod = obj.m_rolloverPeriod;
    m_fileDesc = obj.m_fileDesc;
    m_eof = obj.m_eof;
    m_bptr = obj.m_bptr;
    m_bytesRead = obj.m_bytesRead;
    m_openFlags = obj.m_openFlags;
    m_openMode = obj.m_openMode;
    m_timeOpened = obj.m_timeOpened;
    m_fileSize = obj.m_fileSize;
    m_lastModified = obj.m_lastModified;

    memcpy(m_readBuffer, obj.m_readBuffer, sizeof(m_readBuffer));
}

CdrFileHandler::~CdrFileHandler()
{
    close();
}

void
CdrFileHandler::setDirectory(const string &dir)
{
    m_directory = dir;
    m_dirPathFileName = m_directory + '/' + m_fileName;
}

void
CdrFileHandler::setFileName(const string &fn)
{
    m_fileName = fn;
    m_dirPathFileName = m_directory + '/' + m_fileName;
}

void
CdrFileHandler::setCheckFileSizeOn()
{
    m_fileCheckFreq = true;
}

int
CdrFileHandler::writeCdr(const CdrRadius &ref)
{
    char buffer[2048];
    int len = ref.dumpFlds(buffer, sizeof(buffer));
    buffer[len++] = '\n';

    int num = ::write(m_fileDesc, buffer, len);
    if (num < 0)
    {
        m_eof = true;
        cpLog(LOG_ERR, "Warning::Failed to write to file %s, records will be lost",
              m_dirPathFileName.c_str());
    }
    return num;
}

char
CdrFileHandler::readChar()
{
    if (m_bptr < m_readBuffer + m_bytesRead)
    {
        return (*m_bptr++);
    }
    else
    {
        m_bptr = m_readBuffer;

        while (1)
        {
            m_bytesRead = ::read(m_fileDesc, m_readBuffer, sizeof(m_readBuffer));
            if (m_bytesRead < 0)
            {
                if (errno == EINTR)
                    continue;
                m_eof = true;
                return (0);
            }
            else if (m_bytesRead == 0)
            {
                m_eof = true;
                return (0);
            }
            break;
        }
    }

    return (*m_bptr++);
}

int
CdrFileHandler::readTokens( list < string > &tokenList,
                            const char delimiter )
    throw(VCdrException&)
{
    char c = 0;
    string token;

    // read until the eol char

    while (c != '\n')
    {
        c = readChar();
        if (c == 0)
        {
            break;
        }
        else if (c == delimiter)
        {
            tokenList.push_back(token);
            token.erase();
        }
        else
            token += c;
    }
    if (token.size() > 0)
    {
        tokenList.push_back(token);
    }

    return tokenList.size();
}

void
CdrFileHandler::open( const int flags, const mode_t mode )
    throw(VCdrException&)
{
    m_openFlags = flags;
    m_openMode = mode;

    close();

    m_fileDesc = ::open(m_dirPathFileName.c_str(), flags, m_openMode);
    if (m_fileDesc < 0)
    {
        string msg("Failed to open file ");
        msg += m_dirPathFileName;
        cpLog(LOG_ERR, msg.c_str());
        throw VCdrException(msg, __FILE__, __LINE__);
    }

    m_timeOpened = (long)time(0);

    if (m_fileCheckFreq)
    {
        checkFileSize();
    }

    m_eof = false;

    updateDynamicVars();
}

void
CdrFileHandler::close()
{
    if (m_fileDesc != 0)
    {
        ::close(m_fileDesc);
        m_fileDesc = 0;
        m_eof = true;
    }
}

void
CdrFileHandler::updateDynamicVars()
    throw (VCdrException&)
{
    if (m_eof)
    {
        return ;
    }

    struct stat buf;

    if (fstat(m_fileDesc, &buf) < 0)
    {
        string msg("Failed to access file ");
        msg += m_dirPathFileName;

        msg += " error: ";
        msg += strerror(errno);

        cpLog(LOG_ERR, msg.c_str());
        throw VCdrException(msg, __FILE__, __LINE__);
    }
    m_fileSize = buf.st_size;
    m_lastModified = buf.st_ctime;
}

void
CdrFileHandler::checkFileSize() throw (VCdrException&)
{
    cpLog(LOG_DEBUG, "Performing checkFileSize");

    // check the size of the file, if too large create a new one

    time_t now = (long)time(0);
    struct stat buf;

    if (stat(m_dirPathFileName.c_str(), &buf) < 0)
    {
        string msg("Failed to access file ");
        msg += m_dirPathFileName;
        cpLog(LOG_ERR, msg.c_str());
        throw VCdrException(msg, __FILE__, __LINE__);
    }
    else if (buf.st_size > m_rolloverSize ||
             (now - buf.st_ctime) > (long)m_rolloverPeriod ||
             (now - m_timeOpened) > (long)m_rolloverPeriod)
    {
        close();

        time_t tt = time(0);
        struct tm *t = localtime(&tt);
        char newName[512];
        sprintf(newName, "%s.%02d%02d%02d-%02d%02d%02d%s",
                m_dirPathFileName.c_str(),
                t->tm_mon + 1,
                t->tm_mday,
                (t->tm_year - 100),
                t->tm_hour,
                t->tm_min,
                t->tm_sec,
                m_renameSuffix.c_str());

        cpLog(LOG_DEBUG, "File size has hit size or age limit, renaming file %s to %s",
              m_dirPathFileName.c_str(),
              newName);

        ::rename(m_dirPathFileName.c_str(), newName);

        open(m_openFlags, m_openMode);
    }
}

int
CdrFileHandler::directoryList( list < string > &fileList,
                               const string &dirPath,
                               const string &fileNameSubstr,
                               const string &suffixSubstr )
{
    struct dirent *dcontents;
    DIR *dptr = opendir(dirPath.c_str());

    if (dptr == 0)
    {
        return 0;
    }

    while ((dcontents = readdir(dptr)) != 0)
    {
        // check the filenameSubstr is at the beginning of the filename

        char * sptr = strstr(dcontents->d_name, fileNameSubstr.c_str());

        if (sptr != 0 && sptr == dcontents->d_name)
        {
            // make sure the suffix is at the end of the filename

            sptr = strstr(dcontents->d_name, suffixSubstr.c_str());

            if (sptr && strlen(sptr) == suffixSubstr.size())
            {
                fileList.push_back(dcontents->d_name);
            }
        }
    }
    closedir(dptr);
    return fileList.size();
}
