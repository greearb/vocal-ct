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

static const char* const LDAP2Vuser_cxx_Version =
    "$Id: LDAP2Vuser.cxx,v 1.1 2004/05/01 04:15:31 greear Exp $";

#include "global.h"
#include <strstream>
#include "FileDataStore.hxx"
#include "VIoException.hxx"
#include "LDAP2Vuser.hxx"
#include <cassert>

// Attribute not found in LDAP
static const char* const PHONE_NOT_IN_LDAP = "nonumber";
static const char* const IP_NOT_IN_LDAP = "xxx.xxx.xxx.xxx";
static const char* const FNA_NOT_IN_LDAP = "1111";
static const char* const CFB_NOT_IN_LDAP = "2222";

// Max user record file size is 1Mb
static const unsigned int MAX_FILE_SIZE = 1048576;

// Constructor -- opens connection with LDAP server as authenticated user
LDAP2Vuser::LDAP2Vuser(string &host, unsigned int port, string &basedn, 
                       string &binddn, string &bindpw):
myLdapHost(host),
myLdapPort(port),
myLdapBasedn(basedn),
myLdapBinddn(binddn),
myLdapBindpw(bindpw)
{
    myLdapHandle = 0;
    assert(initialize());
    assert(authenticate());
    setSizelimit(500);
}

// Constructor -- opens connection with LDAP server as nobody
LDAP2Vuser::LDAP2Vuser(string &host, unsigned int port, string &basedn):
myLdapHost(host),
myLdapPort(port),
myLdapBasedn(basedn)
{
    myLdapHandle = 0;
    assert(initialize());
}

// Destructor -- unbinds from LDAP
LDAP2Vuser::~LDAP2Vuser()
{
    ldap_unbind(myLdapHandle);
}

// Initialize LDAP session
bool 
LDAP2Vuser::initialize()
{
    char host_buff[128];
    char ldapperrormsg[20] = "ldap_init";

    // Pass non-consts char* to LDAP API functions
    strncpy(host_buff, myLdapHost.c_str(), myLdapHost.length()+1);
    
    if ((myLdapHandle = ldap_init(host_buff, myLdapPort) ) == NULL )
    {
        ldap_perror( myLdapHandle, ldapperrormsg );
        return(false);
    }
    return(true);
}

//  Authenticate to LDAP server as nobody
bool 
LDAP2Vuser::authenticate()
{
    char binddn_buff[128];
    char bindpw_buff[128];
    char ldapperrormsg[20] = "ldap_simple_bind_s";

    // Pass non-consts char* to LDAP API functions
    strncpy(binddn_buff, myLdapBinddn.c_str(), myLdapBinddn.length()+1);
    strncpy(bindpw_buff, myLdapBindpw.c_str(), myLdapBindpw.length()+1);
    
    if (ldap_simple_bind_s(myLdapHandle, binddn_buff, bindpw_buff)
        != LDAP_SUCCESS)
    {
        ldap_perror( myLdapHandle, ldapperrormsg );
        return(false);
    }
    return(true);
}

// Set new search size limit
void
LDAP2Vuser::setSizelimit(int szlimit)
{
#if defined(LDAP_OPT_SIZELIMIT)
    ldap_set_option(myLdapHandle, LDAP_OPT_SIZELIMIT, &szlimit);
#else
    myLdapHandle->ld_sizelimit = szlimit;
#endif
}

// Get search size limit
const int
LDAP2Vuser::getSizelimit()
{
#if defined(LDAP_OPT_SIZELIMIT)
    int limit;
    ldap_get_option(myLdapHandle, LDAP_OPT_SIZELIMIT, &limit);
    return ( limit );
#else
    return myLdapHandle->ld_sizelimit;
#endif
}

//  Create XML file for given entry
void 
LDAP2Vuser::createXmlbuffer(char* filebuffer, const string& phonenumber,
       const string& ip, const string& fna, const string& cfb)
{
    strstream buf(filebuffer, MAX_FILE_SIZE);

    buf << "<user><name>" << phonenumber << "</name>" << endl
    << "<password>6f7d5634f65192a3b9c8479cee3b655</password>" << endl
    << "<ipaddr>" << ip << "</ipaddr>" << endl
    << "<marshalgroup>UserAgentGroup</marshalgroup>" << endl
    << "<staticRegistration value=\"false\"><terminatingContactList><contact value=\"UserAgentGroup\"></contact>" << endl
    << "<contact value=\"xxx.xxx.xxx.xxx:yyyy\"></contact>" << endl
    << "</terminatingContactList>" << endl
    << "</staticRegistration>" << endl
    << "<authenticationType value=\"Access list\"><authenticationPassword value=\"password\"></authenticationPassword>" << endl
    << "</authenticationType>" << endl
    << "<failurecase>unknown</failurecase>" << endl
    << "<cfa enabled=\"false\"><setfeat>OFF</setfeat>" << endl
    << "<featuregroup>ForwardAllCallsGroup</featuregroup>" << endl
    << "<forwardto>none</forwardto>" << endl
    << "</cfa>" << endl
    << "<fnab enabled=\"false\"><setfeat>OFF</setfeat>" << endl
    << "<featuregroup>ForwardNoAnswerBusyGroup</featuregroup>" << endl
    << "<fna set=\"false\"><forwardto>" << fna << "</forwardto>" << endl
    << "</fna>" << endl
    << "<cfb set=\"false\"><forwardto>" << cfb << "</forwardto>" << endl
    << "</cfb>" << endl
    << "</fnab>" << endl
    << "<cs enabled=\"false\"><setfeat>OFF</setfeat>" << endl
    << "<featuregroup>CallScreeningGroup</featuregroup>" << endl
    << "</cs>" << endl
    << "<clbl enabled=\"false\"><setfeat>OFF</setfeat>" << endl
    << "<featuregroup>CallBlockingGroup</featuregroup>" << endl
    << "<nineHundredBlocked adminSet=\"false\" userSet=\"false\"></nineHundredBlocked>" << endl
    << "<longDistanceBlocked adminSet=\"false\" userSet=\"false\"></longDistanceBlocked>" << endl
    << "</clbl>" << endl
    << "<jtapi enabled=\"true\"><setfeat>ON</setfeat>" << endl
    << "<featuregroup>JtapiGroup</featuregroup>" << endl
    << "</jtapi>" << endl
    << "<callReturn enabled=\"false\"><setfeat>OFF</setfeat>" << endl
    << "<featuregroup>CallReturnGroup</featuregroup>" << endl
    << "</callReturn>" << endl
    << "<callerIdBlocking enabled=\"false\"><setfeat>OFF</setfeat>" << endl
    << "<featuregroup>CallerIdBlockingGroup</featuregroup>" << endl
    << "</callerIdBlocking>" << endl
    << "<voicemail enabled=\"false\"><setfeat>OFF</setfeat>" << endl
    << "<featuregroup>VoiceMailGroup</featuregroup>" << endl
    << "</voicemail>" << endl
    << "<aliases></aliases>" << endl
    << "</user>" << endl

    << ends;

//    return(void);
}

// Strip spaces from telephone number 
void
LDAP2Vuser::stripSpaces(string& phoneNumber)
{
    string buff = phoneNumber;
    string::iterator buff_i = buff.begin();
    string::iterator phoneNumber_i = phoneNumber.begin();
    int spaceCount = 0;

    while (buff_i != buff.end())
    {
        if (*buff_i != ' ')
        {
            *phoneNumber_i++ = *buff_i++;
        }
        else
        {
            buff_i++;
            spaceCount++;
        }
    }
    phoneNumber.resize(phoneNumber.length() - spaceCount);
}

// Find matches in LDAP server
const int
LDAP2Vuser::exportUsers(const int hashkey, const string &outputdir)
{
    LDAPMessage* result;
    LDAPMessage* entry;
    int retCode;
    int msgid;
    int numEntries = 0;
    bool finished;
    char filebuffer[MAX_FILE_SIZE];
    char basedn_buff[128];
    string phonenumber;
    string ip;
    string fna;
    string cfb;
    char** attr_telephonenumber;
    char** attr_vocalPhonenumber;
    char** attr_vocalIP;
    char** attr_vocalFNA;
    char** attr_vocalCFB;
    char ldapperrormsg1[] = "ldap_search";
    char ldapperrormsg2[] = "ldap_result";
    char search_filter[] = "(objectclass=*)";
    char attr_key1[] = "telephonenumber";
    char attr_key2[] = "vocalphonenumber";
    char attr_key3[] = "vocalip";
    char attr_key4[] = "vocalfna";
    char attr_key5[] = "vocalcfb";

    FileDataStore userdb(hashkey, outputdir);

    // Pass non-consts char* to LDAP API functions
    strncpy(basedn_buff, myLdapBasedn.c_str(), myLdapBasedn.length()+1);

    // Search 
    if ((msgid = ldap_search(myLdapHandle, basedn_buff, LDAP_SCOPE_SUBTREE, search_filter, NULL, 0 )) < 0 )
    {
        ldap_perror( myLdapHandle, ldapperrormsg1 );
        return ( -1 );
    }

    // Loop, polling for results until finished
    finished = 0;
    while ( !finished )
    {
        result = NULL;
        retCode = ldap_result( myLdapHandle, msgid, 0, NULL, &result );
        if (retCode == -1) 
        {
            // some error occurred
            ldap_perror( myLdapHandle, ldapperrormsg2 );
            return ( -1 );
        }
        else 
        if (retCode == 0) 
        {
            // Timeout was exceeded.  No entries are ready for retrieval. 
            if ( result != NULL )
            {
                ldap_msgfree( result );
            }
        }
        /*
         * Either an entry is ready for retrieval, or all entries have
         * been retrieved.
         */
        else
        if (( entry = ldap_first_entry( myLdapHandle, result )) == NULL )
        {
            // All done 
            finished = true;
            if ( result != NULL )
            {
                ldap_msgfree( result );
            }
            continue;
        }
        // for each entry ...
        if ((attr_telephonenumber =
             ldap_get_values(myLdapHandle, entry, attr_key1))
             != NULL)
        {
            numEntries++;

            if ((attr_vocalPhonenumber = ldap_get_values(myLdapHandle, 
                entry, attr_key2)) != NULL)
            {
                phonenumber = attr_vocalPhonenumber[0];
                // Strip any spaces from LDAP phone number 
                stripSpaces(phonenumber);
            }
            else
            {
                phonenumber = PHONE_NOT_IN_LDAP;
            }

            if ((attr_vocalIP = ldap_get_values(myLdapHandle, entry,
                 attr_key3)) != NULL)
            {
                ip = attr_vocalIP[0];
            }
            else
            {
                ip = IP_NOT_IN_LDAP;
            }

            if ((attr_vocalFNA = ldap_get_values(myLdapHandle, entry, 
                 attr_key4)) != NULL)
            {
                fna = attr_vocalFNA[0];
            }
            else
            {
                fna = FNA_NOT_IN_LDAP;
            }

            if ((attr_vocalCFB = ldap_get_values(myLdapHandle, entry, 
                 attr_key5)) != NULL)
            {
                cfb = attr_vocalCFB[0];
            }
            else
            {
                cfb = CFB_NOT_IN_LDAP;
            }

            // Set filebuffer to xml output
            createXmlbuffer(filebuffer, phonenumber, ip, fna, cfb);

            // Create a vocal user file for each entry
            cout << "Exporting: " << phonenumber << endl;
            try 
            {
    
                userdb.putItem("Accounts", phonenumber, filebuffer);
            }
            catch (VException& e)
            {
                cerr << "putItem failed, reason: " <<
                   e.getDescription() << endl;
                exit(-1);
            }

            // Free each entry attributes
            ldap_value_free(attr_vocalPhonenumber);
            ldap_value_free(attr_vocalIP);
            ldap_value_free(attr_vocalFNA);
            ldap_value_free(attr_vocalCFB);
        }
        ldap_msgfree( result );
    }
    cout << numEntries << " Entries exported" << endl; 
    return (numEntries);
}
