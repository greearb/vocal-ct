
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

static const char* const ExpandTest_cxx_Version =
    "$Id: ExpandTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "global.h"
#include <iostream>
#include <string>

static const string RecordRoute( "Record-Route: " );
static const string Route( "Route: " );
static const string Via( "Via: ");
static const string local_CRLF( "\r\n" );
static const string Comma( "," );
static const string ReplaceRecordRouteComma( local_CRLF + RecordRoute );
static const string ReplaceRouteComma( local_CRLF + Route );
static const string ReplaceViaComma( local_CRLF + Via );


static string
expand( string inString )
{
    string::size_type rrPos = inString.find( RecordRoute );
    // cout << "rrPos = " << rrPos << endl;
    if ( rrPos < string::npos )
    {
        string::size_type crlfPos = inString.find( local_CRLF, rrPos );
        //  cout << "crlfPos = " << crlfPos << endl;
        string::size_type commaPos = inString.find( Comma, rrPos );
        // cout << "commaPos = " << commaPos << endl;
        while ( commaPos < crlfPos )
        {
            // found comma
            inString.replace( commaPos, 1, ReplaceRecordRouteComma );

            // find next
            string::size_type newCommaPos = inString.find( Comma, commaPos );
            if ( (commaPos + 1) < inString.length() )
            {
                crlfPos = inString.find( local_CRLF, commaPos + 1 );
            }
            commaPos = newCommaPos;
            //  cout << "commaPos = " << commaPos << endl;
        }

    }

    {
        string::size_type rrPos = inString.find( Route );
        // cout << "rrPos = " << rrPos << endl;
        if ( rrPos < string::npos )
        {
            string::size_type crlfPos = inString.find( local_CRLF, rrPos );
            // cout << "crlfPos = " << crlfPos << endl;
            string::size_type commaPos = inString.find( Comma, rrPos );
            // cout << "commaPos = " << commaPos << endl;
            while ( commaPos < crlfPos )
            {
                // found comma
                inString.replace( commaPos, 1, ReplaceRouteComma );

                // find next
                string::size_type newCommaPos = inString.find( Comma, commaPos );
                if ( (commaPos + 1) < inString.length() )
                {
                    crlfPos = inString.find( local_CRLF, commaPos + 1 );
                }
                commaPos = newCommaPos;
                //  cout << "commaPos = " << commaPos << endl;
            }

        }
    }

    {
        string::size_type rrPos = inString.find( Via );
        // cout << "rrPos = " << rrPos << endl;
        if ( rrPos < string::npos )
        {
            string::size_type crlfPos = inString.find( local_CRLF, rrPos );
            // cout << "crlfPos = " << crlfPos << endl;
            string::size_type commaPos = inString.find( Comma, rrPos );
            // cout << "commaPos = " << commaPos << endl;
            while ( commaPos < crlfPos )
            {
                // found comma
                inString.replace( commaPos, 1, ReplaceViaComma );

                // find next
                string::size_type newCommaPos = inString.find( Comma, commaPos );
                if ( (commaPos + 1) < inString.length() )
                {
                    crlfPos = inString.find( local_CRLF, commaPos + 1 );
                }
                commaPos = newCommaPos;
                //  cout << "commaPos = " << commaPos << endl;
            }

        }

    }

    return inString;
}


int main()
{
    char buf[65000];

    cin.read(buf, 65000);

    string s = expand(buf);
    cout << s << endl;
}
