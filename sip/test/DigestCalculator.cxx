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

static const char* const DigestCalculator_cxx_Version =
    "$Id: DigestCalculator.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipDigest.hxx"
#include <string>

using namespace Vocal;

char* trim(char* s)
{
    int len = strlen(s);
    if(len > 0 && s[len - 1] == '\n')
    {
	s[len - 1] = '\0';
    }
    return s;
}

int main()
{
    char s[1024];

    cout << "Username: ";
    fgets(s, 1023, stdin);

    Data username(trim(s));
    cout << "Password: ";
    fgets(s, 1023, stdin);
    Data passwd(trim(s));
    cout << "Method: ";
    fgets(s, 1023, stdin);
    // make method all caps
    Data method(trim(s));

    cout << "URI: ";
    fgets(s, 1023, stdin);
    Data uri(trim(s));

    cout << "Realm: ";
    fgets(s, 1023, stdin);
    Data realm(trim(s));

    cout << "QOP: ";
    fgets(s, 1023, stdin);

    Data qop(trim(s));

    cout << "Nonce: ";
    fgets(s, 1023, stdin);
    Data nonce(trim(s));

    cout << "CNonce: ";
    fgets(s, 1023, stdin);

    Data cnonce(trim(s));

    cout << "Algorithm: ";
    fgets(s, 1023, stdin);

    Data algorithm(trim(s));

    cout << "Noncecount: ";
    fgets(s, 1023, stdin);

    Data noncecount(trim(s));

    SipDigest first;
    Data firstSum = first.form_SIPdigest(nonce, username, passwd, method, uri,
	                                 realm, qop, cnonce, algorithm, 
					 noncecount);

    cout << firstSum << endl;
    return 0;
}
