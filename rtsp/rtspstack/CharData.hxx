#ifndef CharData_Hxx
#define CharData_Hxx
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


static const char* const CharData_hxx_Version =
    "$Id: CharData.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <cassert>
#include <cstddef>
#include <cstring>
#include "vtypes.h"

/** */
class CharData
{
    public:
        /** */
        CharData() : myCharPtr(NULL), myCharLen(0) {}
        /** */
        CharData(const char* buf, u_int32_t len) 
            : myCharPtr(buf), myCharLen(len) {}
        /** */
		CharData(const char* buf) 
            : myCharPtr(buf), myCharLen(buf != NULL ? strlen(buf) : 0) {}
        /** */
        CharData(const CharData& src) 
            : myCharPtr(src.myCharPtr), myCharLen(src.myCharLen) {}
        /** */
        ~CharData() { assert (myCharPtr != 0); }

        /** */
        bool isEqualNoCase(const char* data, const u_int32_t len) const;

        /** check if the whole data is an empty line
         *  it is a very strict checking. It is true when 
         *  the whole data is only CRLF.
         */
        bool isEmptyLine();

        /** */
	    CharData& operator=(const CharData& data) 
            { myCharPtr = data.myCharPtr; 
              myCharLen = data.myCharLen; 
              return *this; }
        /** */
        char operator[](unsigned int i) { assert(i >= 0); return myCharPtr[i]; }
        /** */
		void set(const char* thePtr, u_int32_t theLen) 
            { myCharPtr = thePtr; myCharLen = theLen; }
        /** */
		void setLen(u_int32_t theLen) 
            { myCharLen = theLen; }
        /** */
		const u_int32_t getLen() const
            { return myCharLen; }
        /** */
		const char* getPtr() const
            { return myCharPtr; }

    private:
        /** */
		const char* myCharPtr;
        /** */
		u_int32_t myCharLen;
        /** be used to compare chardata in case insensitive way */
        static u_int8_t myCaseInsensitiveArray[];

};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif 

