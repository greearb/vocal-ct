
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


static const char* const parse3tuple_cxx_Version =
    "$Id: parse3tuple.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include <cstdio>
#include <cassert>
#include <cstring>

#include "parse3tuple.h"
#include "cpLog.h"

/* if you use LDAP you need to link with -lldap -llber */
//#define USE_LDAP

/* if you use XML you need to link with -lxml -lz */
//#define USE_XML


#ifdef USE_LDAP
#include <lber.h>
#include <ldap.h>
#endif

#ifdef USE_XML
#include <gnome-xml/parser.h>
#endif


bool parse3tupleFile (char* fname, void (*parse) (char*, char*, char*));
bool parse3tupleLDAP (char* fname, void (*parse) (char*, char*, char*));
bool parse3tupleXML (char* fname, void (*parse) (char*, char*, char*));

/*
 * Parse some configuration data
 *    if the name ends .xml is assumed to be an XML file 
 *    if the name begins in ldap: it is assumed to be a LDAP url
 *    otherwise it is assumed to be a text file with 3tuples
 */
bool
parse3tuple (char* fname, void (*parse) (char*, char*, char*))
{
    static const char ldap[] = "ldap:";
    static const char xml[] = ".xml";
    bool ret = false;

    if ( !strncmp(fname, ldap, strlen(ldap) ) )
    {
        // it's an LDAP url
        ret = parse3tupleLDAP(fname, parse);
    }
    else if ( strlen(fname) > 4 ? !strncmp(fname + strlen(fname) - strlen(xml), xml, strlen(xml)) : false )
    {
        // it's an XML url
        ret = parse3tupleXML(fname, parse);
    }
    else
    {
        // assume it's a 3tuple file name
        ret = parse3tupleFile(fname, parse);
    }

    return ret;
}


/*
 * This function loads configuration out of a XML file. 
 *
 * The DTD for the file is 
 *      <!ELEMENT config (item)* >
 *      <!ELEMENT item (tag,type,value)>
 *      <!ELEMENT tag (#PCDATA)>
 *      <!ELEMENT type (#PCDATA)>
 *      <!ELEMENT value (#PCDATA)>
 *
 * An example config file would might look like 
 *      <?xml version = "1.0" ?>
 *      <!DOCTYPE config SYSTEM "config.dtd">
 *      <!-- <?xml-stylesheet type="text/xsl" href="test.xsl"?> -->
 *      
 *      <configuration>
 *      
 *      <item>
 *         <tag> myTag1 </tag>
 *         <type> myType1 </type>
 *         <value> myValue1 </value>
 *      </item>
 *      
 *      <item>
 *         <tag> myTag2 </tag>
 *         <type> myType2 </type>
 *         <value> myValue2 </value>
 *      </item>
 *      
 *      </configuration>
 *
 * An xml file that would allow this xml file to be vied in IE 5 is 
 *      <?xml version = "1.0" ?>
 *      <xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
 *      
 *      <xsl:template match="/">
 *      <HTML>
 *      <BODY>
 *      
 *      <xsl:for-each select="configuration/item">
 *      
 *      <xsl:value-of select="tag"/> 
 *      <xsl:value-of select="type"/> 
 *      <xsl:value-of select="value"/> 
 *      <BR/>
 *      
 *      </xsl:for-each>
 *      
 *      </BODY>
 *      </HTML>
 *      </xsl:template>
 *      </xsl:stylesheet>
 */
bool
parse3tupleXML (char* fname, void (*parse) (char*, char*, char*))
{
#ifdef USE_XML // define if XML is supported
    xmlDocPtr doc = NULL;

    doc = xmlParseFile(fname);
    if (!doc)
    {
        cpLog (LOG_ERR, "Cannot process XML file: %s", fname);
        return false;
    }

    xmlNodePtr cur = doc->root;
    assert( cur );

    while ( strcmp( reinterpret_cast < const char * > (cur->name)
                    , "configuration" ) )
    {
        cur = cur->next;
        if ( cur == NULL )
        {
            cpLog (LOG_ERR, "no configuration section in XML file: %s", fname);
            return false;
        }
    }
    assert( cur );

    // step into the configuration data
    cur = cur->childs;

    while (cur != NULL)
    {
        assert( !strcmp( reinterpret_cast < const char * > (cur->name), "item") );

        char tag [TAG_MAX_LENGTH];
        char type [TYPE_MAX_LENGTH];
        char* value;

        xmlNodePtr ptr = cur->childs;

        assert( ptr);
        assert( !strcmp( reinterpret_cast < const char * > (ptr->name), "tag") );
        strncpy( tag ,
                 reinterpret_cast < const char * > (xmlNodeListGetString(doc, ptr->childs, 1)),
                 TAG_MAX_LENGTH );
        ptr = ptr->next;

        assert( ptr);
        assert( !strcmp( reinterpret_cast < const char * > (ptr->name), "type") );
        strncpy( type ,
                 reinterpret_cast < const char * > (xmlNodeListGetString(doc, ptr->childs, 1)),
                 TYPE_MAX_LENGTH );
        ptr = ptr->next;

        assert( ptr);
        assert( !strcmp( reinterpret_cast < const char * > (ptr->name), "value") );

        // make a copy just to be sure no one trashed the main data structure
        value = const_cast < char* > (
                    reinterpret_cast < const char * > (
                        xmlNodeListGetString(doc, ptr->childs, 1)));
        assert(value);

        // call the parse function with the 3 tuple
        (*parse) (tag, type, value);

        // move to next item
        cur = cur->next;
    }

#else
    cpLog (LOG_ERR, "No suuport for XML file: %s", fname);
    return false;
#endif

    return true;
}

/**
 *  This functions parses a 3 tuple objects out of the LDAP 
 *
 *  The passed in url must confom to rfc1959. A example might be
 *     "ldap://localhost/dc=vovida,dc=com"
 * 
 *  You can view the data base from netscape with the url
 *      ldap://localhost/dc=vovida,dc=com?tag,type,value?one?objectclass=3tuple
 *
 *  The ldif for a tuple should look something like
 *     dn: cn=myTag2, dc=vovida, dc=com
 *     cn: myTag2
 *     tag: theTageName
 *     type: someTypeLike_String
 *     value: someValueLike_4
 *     objectclass: 3tuple
 *
 *  This could be added with something like 
 *     ldapadd -D "cn=root,dc=vovida,dc=com" -v -w secret < foobar.ldif
 * 
 *   Note is must have the objectclass of 3tuple
 *
 */
bool
parse3tupleLDAP (char* url, void (*parse) (char*, char*, char*))
{
#ifdef USE_LDAP // define if LDAP is supported
    if ( !ldap_is_ldap_url(url))
    {
        cpLog (LOG_ERR, "Bad LDAP URL: %s", url);
        return false;
    }

    LDAPURLDesc* ldapUrl = NULL;
    int err;
    err = ldap_url_parse( url, &ldapUrl );
    if ( err )
    {
        switch (err)
        {
            case LDAP_URL_ERR_NOTLDAP:
            {
                cpLog (LOG_ERR, "Bad LDAP URL: %s ", url);
                return false;
            }
            break;

            case LDAP_URL_ERR_NODN:
            {
                cpLog (LOG_ERR, "Bad LDAP URL - no DN: %s ", url);
                return false;
            }
            break;

            case LDAP_URL_ERR_BADSCOPE:
            {
                cpLog (LOG_ERR, "Bad LDAP URL - bad scope : error %s ", url);
                return false;
            }
            break;

            case LDAP_URL_ERR_MEM:
            {
                cpLog (LOG_ERR, "Bad LDAP URL: error %d ", err);
                return false;
            }
            break;

            default:
            {
                cpLog (LOG_ERR, "Bad LDAP URL: error %d ", err);
                return false;
            }
        }
    }

    cpLog (LOG_DEBUG, "LDAP host: %s", ldapUrl->lud_host );
    cpLog (LOG_DEBUG, "LDAP port: %d", ldapUrl->lud_port );

    char* host = ldapUrl->lud_host;
    int port = ldapUrl->lud_port;

    if ( port == 0 )
    {
        port = LDAP_PORT;
    }

    LDAP *ld = NULL;
    ld = ldap_init(host, port);
    if (!ld)
    {
        assert(0);
        return false;
    }

    char* who = "";  // set this to the DN of the object you have a passwd for
    char* passwd = "";  // set this to the passwd
    err = ldap_simple_bind_s(ld, who, passwd);
    if ( err )
    {
        cpLog (LOG_ERR, "LDAP problem in simple bind  %s ", ldap_err2string(err));
        return false;
    }

    cpLog (LOG_DEBUG, "LDAP dn: %s", ldapUrl->lud_dn );
    cpLog (LOG_DEBUG, "LDAP scope: %d", ldapUrl->lud_scope );
    cpLog (LOG_DEBUG, "LDAP filter: %s", ldapUrl->lud_filter );
    if ( ldapUrl->lud_attrs )
    {
        cpLog (LOG_DEBUG, "LDAP attr[0]: %s", ldapUrl->lud_attrs[0] );
    }

    char *base = ldapUrl->lud_dn;
    int scope = LDAP_SCOPE_ONELEVEL;  //ldapUrl->lud_scope ;
    char *filter = "objectclass=3tuple";  // ldapUrl->lud_filter;
    char *attrs[4];
    attrs[0] = "tag";
    attrs[1] = "type";
    attrs[2] = "value";
    attrs[3] = NULL;
    int attrsonly = 0 ;

    LDAPMessage *res;
    err = ldap_search_s(ld, base, scope, filter, attrs, attrsonly, &res);
    if ( err )
    {
        cpLog (LOG_ERR, "LDAP problem in search %s ", ldap_err2string(err));
        return false;
    }

    int count = ldap_count_entries(ld, res);
    cpLog (LOG_DEBUG, "LDAP search return %d entries", count );

    for ( LDAPMessage* msg = ldap_first_entry(ld, res);
            msg != NULL;
            msg = ldap_next_entry(ld, msg) )
    {
        cpLog (LOG_DEBUG, "LDAP got messg ");
        if ( msg )
        {
            BerElement* ber = NULL;

            /*
             * This bit of code is good to fine all the attributes types
             * 
             *  char * attr = ldap_first_attribute(ld,msg,&ber);
             *  while ( attr != NULL )
             *  {
             *     cpLog (LOG_DEBUG, "LDAP attr=%s",attr);
             *     attr=ldap_next_attribute(ld,msg,ber);
             *  }
             *  ber = NULL;
             *
             */

            char **data;

            char tag [TAG_MAX_LENGTH];
            char type [TYPE_MAX_LENGTH];

            // get the tag  ----------------------
            data = ldap_get_values(ld, msg, "tag");
            if ( data == NULL )
            {
                cpLog (LOG_ERR, "LDAP in tag data %s ",
                       ldap_err2string(ld->ld_errno));
                return false;
            }
            assert( data[0] );
            cpLog (LOG_DEBUG, "LDAP tag=%s", data[0] );
            strncpy( tag , data[0], TAG_MAX_LENGTH );

            // get the type -----------------------
            data = ldap_get_values(ld, msg, "type");
            if ( data == NULL )
            {
                cpLog (LOG_ERR, "LDAP in type data %s ",
                       ldap_err2string(ld->ld_errno));
                return false;
            }
            assert( data[0] );
            cpLog (LOG_DEBUG, "LDAP type=%s", data[0] );
            strncpy( type , data[0], TYPE_MAX_LENGTH );

            // get the value ---------------------
            data = ldap_get_values(ld, msg, "value");
            if ( data == NULL )
            {
                cpLog (LOG_ERR, "LDAP in value data %s ",
                       ldap_err2string(ld->ld_errno));
                return false;
            }
            assert( data[0] );
            cpLog (LOG_DEBUG, "LDAP value=%s", data[0] );

            // call the parse function with the 3 tuple
            (*parse) (tag, type, data[0]);
        }
    }

    err = ldap_msgfree(res);
    if ( err == -1 )
    {
        cpLog (LOG_ERR, "LDAP problem in msgfree ");
        return false;
    }
    res = NULL;

    err = ldap_unbind_s(ld);
    if ( err )
    {
        cpLog (LOG_ERR, "LDAP problem in unbind %s ", ldap_err2string(err));
        return false;
    }
    ld = NULL;

    ldap_free_urldesc( ldapUrl );
    ldapUrl = NULL;

#else
    cpLog (LOG_ERR, "No support for LDAP data: %s", url);
    return false;
#endif

    return true;
}


/*
 * parse3tuple() parses file fname line by line. Each line is expected
 * to have 3 fields, <tag>-<type>-<value>, separated by blanks or tabs.
 * Lines starting with '#' are comments. Empty lines are ignored.
 * The value field contains a number of characters, even field separators
 * like blanks and tabs, up to the end of the line.
 * User of this utility provides a call back function with 3 arguments
 * (pointers to the tag, type and value strings) to validate and use the
 * data.
 */

bool
parse3tuple (const char* fname, void (*parse) (char*, char*, char*))
{
    return parse3tupleFile (const_cast < char* > (fname), parse);
}

void
removeTrailingBlanks( char* line )
{
    for ( int i = strlen( line ) - 1; i > 0; i-- )
    {
        if ( line[i] == ' ' || line[i] == '\t' )
        {
            line[i] = '\0';
        }
        else
        {
            return ;
        }
    }
}

bool
parse3tupleFile (char* fname, void (*parse) (char*, char*, char*))
{
    char line [TUPLE3_MAX_LENGTH];
    char tag [TAG_MAX_LENGTH];
    char type [TYPE_MAX_LENGTH];
    int lineCnt;
    int lineLen = 0;
    int argCnt;
    int valuePos = 0;
    FILE* fd = fopen (fname , "r");

    if (fd)
    {
        for (lineCnt = 1; fgets (line, TUPLE3_MAX_LENGTH, fd); lineCnt++, valuePos = 0)
        {
            lineLen = strlen(line);
            assert( lineLen > 0 );
            line [lineLen - 1] = 0;    /* remove \n */
            if (lineLen > 1)
            {
                if (line [lineLen - 2] == '\r')
                {
                    line [lineLen - 2] = 0;    /* remove \r too */
                }
            }
            removeTrailingBlanks( line );
            if ((argCnt = sscanf (line, "%s%s%*[ \t]%n", tag, type, &valuePos)) == 2)
            {
                if (line[0] != '#')
                {
                    if (valuePos == 0)
                    {
                        cpLog (LOG_WARNING,
                               "Line %d has too few arguments: %s",
                               lineCnt,
                               line);
                    }
                    else
                    {
                        (*parse) (tag, type, line + valuePos);
                    }
                }
            }
            else
            {
                if (argCnt == 1 && line[0] != '#')
                {
                    cpLog (LOG_WARNING,
                           "Line %d has too few arguments: %s",
                           lineCnt,
                           line);
                }
            }
        }
        fclose (fd);
    }
    else
    {
        cpLog (LOG_ERR, "Cannot open file: %s", fname);
        return false;
    }
    return true;
}    /* parse3tuple */


int
strConst2i (const char* str, const char* strTable[], const int tableSize)
{
    int i;
    for (i = 0; i < tableSize; i++)
    {
        if (strcmp (str, strTable [i]) == 0)
        {
            break;
        }
    }
    return i;
}    // strConst2i
