#ifndef VFile_hxx
#define VFile_hxx

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

static const char* const VFile_hxx_Version =
    "$Id: VFile.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "global.h"
#include <string>
//#include <ftw.h>
#define UNKNOWN -1
#include <time.h>

class VFile
{
    public:
        ///Constructor
        VFile(const string& path, const string& fileName);

        ///Constructor
        VFile(const string& fullFilePath);
        VFile(const string& fullFilePath, int flag, time_t time);
        VFile(const VFile& other)
        {
            _path = other._path;
            _fileName = other._fileName;
            _time = other._time;
            _type = other._type;
        }

        VFile& operator=(const VFile& other)
        {
            if (&other != this)
            {
                _path = other._path;
                _fileName = other._fileName;
                _time = other._time;
                _type = other._type;
            }
            return *this;
        }

        const string& getName() const
        {
            return _fileName;
        };
        const string& getPath() const
        {
            return _path;
        };
        const string getFullName() const
        {
            return (_path + _fileName);
        };
        int getType() const
        {
            return _type;
        }
        time_t getTime() const
        {
            return _time;
        }

        bool operator<(const VFile& other)
        {
            if (getFullName() < other.getFullName()) return true;
            return false;
        }

        bool operator==(const VFile& other)
        {
            if (getFullName() == other.getFullName()) return true;
            return false;
        }
    private:
        string _fileName;
        string _path;
        int _type;
        time_t _time;
};

#endif
