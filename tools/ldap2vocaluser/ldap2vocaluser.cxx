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

static const char* const ldap2vocaluser_cxx_Version =
    "$Id: ldap2vocaluser.cxx,v 1.1 2004/05/01 04:15:31 greear Exp $";

using namespace std;

#include <iostream>
#include <string>

#if defined(__svr4__)
#include <stdio.h>
#endif

#if defined(__linux__)
#include <getopt.h>
#endif

#if defined(__FreeBSD__)
#include "getopt_long.h"
#endif

#include "LDAP2Vuser.hxx"

// MAIN 
int
main(int argc, char **argv)
{
    char c;
    int passedhost = 0;
    int passedport = 0;
    int passedoutputdir = 0;
    int passedbasedn = 0;
    int passedbinddn = 0;
    int passedbindpw = 0;
    int passedhashkey = 0;
    string ldaphost;
    int ldapport;
    int hashkey;
    string outputdir;
    string basedn;
    string binddn;
    string bindpw;
    string usage = "Usage: ldap2vocaluser -h ldaphost -b basedn -d output_directory [-k hash_key] [-p ldap_port] [-D binddn -w password]";

    while ((c = getopt( argc, argv, "h:p:b:d:k:D:w:" )) != EOF)
    {
        switch ( c )
        {
            case 'h':   // LDAP host
            ldaphost = optarg;
            passedhost = 1;
            break;
            case 'p':   // LDAP port
            ldapport = atoi(optarg);
            passedport = 1;
            break;
            case 'b':   // LDAP search root
            basedn = optarg;
            passedbasedn = 1;
            break;
            case 'd':   // Output directory
            outputdir = optarg;
            passedoutputdir = 1;
            break;
            case 'k':   // Hash Key
            hashkey = atoi(optarg);
            passedhashkey = 1;
            break;
            case 'D':   // Bind as Distingished Name
            binddn = optarg;
            passedbinddn = 1;
            break;
            case 'w':   // Bind password
            bindpw = optarg;
            passedbindpw = 1;
            break;
            default:
            cerr << usage << endl;
            exit( 1 );
        }
    }

    // Check for all the required arguments
    if (!(passedhost && passedbasedn && passedoutputdir))
    {
        cerr << usage << endl;
        exit( 1 );
    }
    // Check if bind DN passed is coupled with bind PW
    if ((passedbinddn && (!passedbindpw)) || 
       (!passedbinddn && (passedbindpw)))
    {
        cerr << usage << endl;
        exit( 1 );
    }

    // If LDAP Port is not supplied, set to default of 389
    if (!passedport) 
    {
        ldapport = LDAP_PORT;
    }

    // If Hash Key is not supplied, set to default of 6
    if (!passedhashkey) 
    {
        hashkey = 6;
    }

    // If bind DN and PW were passed, login to LDAP server
    // as authenticated user.
    if (passedbinddn)
    {
        LDAP2Vuser connection1(ldaphost, ldapport, basedn, 
                               binddn, bindpw);

        // Authenticated users can set search size limit to infinite
        connection1.setSizelimit(0);
        connection1.exportUsers(hashkey, outputdir);
    }
    else
    {
        LDAP2Vuser connection1(ldaphost, ldapport, basedn);
        connection1.exportUsers(hashkey, outputdir);
    }
    return ( 0 );
}
