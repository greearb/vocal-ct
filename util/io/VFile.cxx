
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

#include "global.h"
#include <cstdio>
#include <iostream>

#include "VFile.hxx"

VFile::VFile(const string& path, const string& fileName)
        : _fileName(fileName), _path(path)
{

    _type = UNKNOWN;
    _time = -1;

    if (_path.size() && _path[_path.size() - 1] != '/')
    {
        _path += "/";
    }
    //Stip any extra slashes
    while (_path.find("//") != string::npos)
    {
        _path.replace(_path.find("//"), 2, "/");
    }
}

VFile::VFile(const string& fullFilePath)
{

    _type = UNKNOWN;
    _time = -1;

    if (fullFilePath.find("/") == string::npos)
    {
        _path = "./";
        _fileName = fullFilePath;
    }
    else
    {
        string tStr = fullFilePath;
        if (tStr[tStr.size() - 1] == '/')
        {
            _path = tStr;
        }
        else
        {
            string::size_type pos = tStr.find_last_of('/');
            _path = tStr.substr(0, pos);
            _fileName = tStr.substr(pos + 1, tStr.size());
        }
    }
    if (_path.size() && _path[_path.size() - 1] != '/')
    {
        _path += "/";
    }
    //Stip any extra slashes
    while (_path.find("//") != string::npos)
    {
        _path.replace(_path.find("//"), 2, "/");
    }
}

VFile::VFile(const string& fullFilePath, int flag, time_t time)
{

    _type = flag;
    _time = time;

    if (fullFilePath.find("/") == string::npos)
    {
        _path = "./";
        _fileName = fullFilePath;
    }
    else
    {
        string tStr = fullFilePath;
        if (tStr[tStr.size() - 1] == '/')
        {
            _path = tStr;
        }
        else
        {
            string::size_type pos = tStr.find_last_of('/');
            _path = tStr.substr(0, pos);
            _fileName = tStr.substr(pos + 1, tStr.size());
        }
    }
    if (_path.size() && _path[_path.size() - 1] != '/')
    {
        _path += "/";
    }
    //Stip any extra slashes
    while (_path.find("//") != string::npos)
    {
        _path.replace(_path.find("//"), 2, "/");
    }
}


ostream& operator<<(ostream& os, const VFile& file)
{
    os << file.getFullName();
    return os;
}
