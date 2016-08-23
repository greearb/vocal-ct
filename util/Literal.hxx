#ifndef LITERAL_HXX_
#define LITERAL_HXX_

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

#include "Data.hxx"

class Literal: public Data
{
    public:
        /// Default constructor
        Literal( );
        /** constructor for C style strings
	 ** @param str      null-terminated (C style) character array
	 */
        Literal( const char* str );
        /** constructor for character arrays with length 
	 **  @param buffer character array 
	 **  @param len    size of buffer
	 */
        Literal( const char* buffer, int len );
        /// copy constructor
        Literal( const Data& data );
        /** constructor for C++ strings
	 */
        Literal( const string& str);
        /** constructor for mstring (a specialization of C++ strings)
	 */
        //Literal( const mstring& mstr);
        /// constructor that converts an int to a Literal
        Literal( const int value);

        /** OVERLOADED for caseinsensitive equality operator
	 ** @param data   Literal to compare to
	 */
        bool operator==( const Data& data ) const;
        bool operator==( const char* data ) const;

        /// OVERLOADED for caseinsensitive inequality operator
        bool operator!=( const Data& data ) const;
        bool operator!=( const char* data ) const;

        /// friend to compare a c-style string to a data (avoids conversion)
        friend bool operator==( const char* data, const Literal& d );
        friend bool operator==( const Data& data, const Literal& d );

        /// friend to compare a c-style string to a data (avoids conversion)
        friend bool operator!=( const char* str, const Literal& d );
        friend bool operator!=( const Data& str, const Literal& d );

};


#endif


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

