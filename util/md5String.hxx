
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




#ifndef MD5String_HXX
#define MD5String_HXX


#include "global.h"
#include <assert.h>
#include <string>
#include "vmd5.h"
#include "cpLog.h"

static const char* const md5StringVersion = "$Id: md5String.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


/// C++ wrapper for vmd5.[ch]
//  Usage:
//  #include "md5String.hxx"
//
//   md5String  md5Str;
//   string     key = "E944D9A6-88C509CE-0-2888184C@192.168.6.210";
//   string     digest = md5Str.getMd5String( key );
//
class md5String
{
    public:
        ///
        md5String()
        {
            MD5Init( &_context );
        }

        ///
        ~md5String()
        {}

        // given a string, compute the message digest

        const string& getMd5String( const string& buf )
        {

            unsigned char digest[16];

            MD5Update( &_context,
                       reinterpret_cast < unsigned const char* > (buf.c_str()),
                       buf.length() );

            MD5Final( digest, &_context );

            /// note: to make digest printable, need to encode digest
            ///       in base64 encoding
            _md5String = string(reinterpret_cast < char* > (digest), 16);
            return _md5String;
        }

        ///

    private:

        MD5Context _context;
        string _md5String;

        ///
        md5String( const md5String& rhs )
        {
            assert( 0 );
        }
        ///
        md5String& operator=( const md5String& rhs )
        {
            assert( 0 );
	    return (*this);
        }

};

#endif
