#ifndef SUBSTRING_H_
#define SUBSTRING_H_

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





static const char* const substringHeaderVersion =
    "$Id: substring.h,v 1.1 2004/05/01 04:15:33 greear Exp $";




#include <string>

class substring
{
    public:
        substring()
        :
        myString(0),
        start(string::npos),
        myLen(0)
        {}


        substring(const string& str)
        :
        myString(const_cast < string* > (&str)),
        start(0),
        myLen(str.length())
        {}



        substring(const string* str, string::size_type i, string::size_type n)
        :
        myString(const_cast < string* > (str)),
        start(i),
        myLen(n)
        {}


        substring(const substring& x)
        :
        myString(x.myString),
        start(x.start),
        myLen(x.myLen)
        {}


        substring(const substring& x, string::size_type i, string::size_type n)
        :
        myString(x.myString),
        start(x.start + i),
        myLen(n)
        {}


        substring& operator= (const substring& x)
        {
            this->myString = x.myString;
            this->start = x.start;
            this->myLen = x.myLen;
            return *this;
        }

        bool operator== (const string& x) const
        {
            if (!myString)
            {
                return false;
            }
            else
            {
                return myString->substr(start, myLen) == x;
            }
        }

        bool operator== (const char* x) const
        {
            if (!myString)
            {
                return false;
            }
            else
            {
                return myString->substr(start, myLen) == x;
            }
        }

        string str() const
        {
            return myString->substr(start, myLen);
        }

        friend ostream& operator<<(ostream& s, const substring& x)
        {
            return s << x.str();
        }

        char operator[](string::size_type i)
        {
            return (*myString)[i + start];
        }

        char operator[](string::size_type i) const
        {
            return (*myString)[i + start];
        }
#ifndef WIN32
        const char* c_str(void) const
        {
            return myString->substr(start, myLen).c_str();
        }
#endif
        string::size_type length() const
        {
            return myLen;
        }

        string::size_type getStart() const
        {
            return start;
        }

    private:
        string* myString;
        string::size_type start;
        string::size_type myLen;
};

#endif
