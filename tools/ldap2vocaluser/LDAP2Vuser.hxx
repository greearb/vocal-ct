#ifndef LDAP2VUSER_HXX_
#define LDAP2VUSER_HXX_

/* ====================================================================
 * The Vovida Software License, Version 1.0
 *
 * Copyright (c) 1999, 2000 Vovida Networks, Inc.  All rights reserved.
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
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by Vovida Networks,
 *        Inc. (http://www.vovida.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must 
 *    not be used to endorse or promote products derived from this 
 *    software without prior written permission. For written permission, 
 *    please contact vocal@vovida.org.
 *
 * 5. Products derived from this software may not be called "VOCAL",
 *    nor may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL VOVIDA NETWORKS, INC. OR ITS 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by Vovida 
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see 
 * <http://www.vovida.org/>.
 *
 */

static const char* const LDAP2Vuser_hxx_Version =
    "$Id: LDAP2Vuser.hxx,v 1.1 2004/05/01 04:15:31 greear Exp $";

#include "global.h"
#include <iostream>
#include <lber.h>
#include <ldap.h>

/** Class to export user information from LDAP server into 
    Vocal user format (XML file)
*/
class LDAP2Vuser
{
    public:

        /// Constructor - opens connection (authenticated) with LDAP server
        LDAP2Vuser(string &host, unsigned int port, string &basedn, 
                   string &binddn, string &bindpw);

        /// Constructor - opens connection (as nobody) with LDAP server
        LDAP2Vuser(string &host, unsigned int port, string &basedn);

        /// Destructor - unbinds to LDAP server
        virtual ~LDAP2Vuser();

        /// Set LDAP search size limit
        void setSizelimit(int szlimit);

        /// Get LDAP search size limit
        const int getSizelimit();

        /// export LDAP
        const int exportUsers(const int hashkey, const string& outputdir);

    private:

        /// Initialize LDAP session
        bool initialize();

        /// Authenticate to LDAP as nobody
        bool authenticate();

        /// Strip spaces from telephone number 
        void stripSpaces(string& phoneNumber);

        /// Create XML file for given LDAP entry
        void createXmlbuffer(char* filebuffer, const string& phonenumber, 
                   const string& ip, const string& fna, const string& cfb);

        /// LDAP handle
        LDAP* myLdapHandle;

        /// LDAP host
        string myLdapHost;

        /// LDAP PORT
        unsigned int myLdapPort;

        /// LDAP Base DN (Distinguished Name)
        string myLdapBasedn;

        /// LDAP Bind as DN
        string myLdapBinddn;

        /// LDAP Bind as PW
        string myLdapBindpw;

};
#endif
