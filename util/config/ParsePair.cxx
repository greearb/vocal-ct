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


static const char* const ParsePair_cxx_Version =
    "$Id: ParsePair.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include "ParsePair.hxx"
#include "vstring.hxx"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <fstream>
#include <strstream>
#include <cctype>


using Vocal::Configuration::ParsePair;
using Vocal::Configuration::Value;
using Vocal::Configuration::NameValueMap;
using Vocal::ReturnCode;
using Vocal::SUCCESS;
using Vocal::getnext;
using Vocal::getline;
using Vocal::remove_quotes;
using std::string;
using std::ifstream;
using std::istrstream;


ParsePair::ParsePair()
    :   myTagName("data")
{
}


ParsePair::~ParsePair()
{
}


void
ParsePair::tagName(const string & tag_name)
{
    myTagName = tag_name;
}


ReturnCode              
ParsePair::parse(Type type, const string & s)
{
    ReturnCode rc = !SUCCESS;

    switch ( type )
    {
        case CFG_XML_FILE:
        {
            myFileName = s;
            return ( parseXMLFile() );
        }
        case CFG_XML_STRING:
        {
            return ( parseXMLString(s) );
        }
        case CFG_STRING:
        {
            istrstream  str(s.c_str(), s.size());
            return ( parseStream(str) );
        }
        case CFG_FILE:
        default:
        {
            myFileName = s;
            
            string  xml(".xml");

            size_t  file_name_size = myFileName.size();
            size_t  xml_size = xml.size();

            if (    file_name_size > 4 
                &&  myFileName.substr(file_name_size - xml_size) == xml
                )
            {
                rc = parseXMLFile();
            }
            else
            {
                rc = parseFile();
            }
        }
    }

    return ( rc );
}


const NameValueMap &
ParsePair::pairs() const
{
    return ( myPairs );
}


ReturnCode          
ParsePair::parseFile()
{
    ifstream     file(myFileName.c_str());
    
    if ( !file.is_open() )
    {
        return ( !SUCCESS );
    }

    ReturnCode rc = parseStream(file);

    file.close();
    
    return ( rc );
}

ReturnCode
ParsePair::parseStream(istream & in)
{
    string line;
    size_t line_num = 1;
    
    bool read_line = false;

    for (   read_line = getline(in, line, line_num); 
            read_line; 
            read_line = getline(in, line, line_num)
        )
    {
        string new_name = getnext(line);
        assert( new_name.size() );
        
        if ( line.size() == 0 )
        {
            Value & val = myPairs[new_name];
            ++val.count;
            val.type = NO_VALUE;
            continue;
        }
        
        Value & val = myPairs[new_name];
        ++val.count;
        val.type = VALUE_PRESENT;
        val.value.push_back(remove_quotes(line));
    }
    
    return ( SUCCESS );
}


ReturnCode          
ParsePair::parseXMLFile()
{
#if defined(VOCAL_USE_XML)
    xmlDocPtr doc = 0;

    doc = xmlParseFile(myFileName.c_str());

    if ( !doc )
    {
        return ( !SUCCESS );
    }

    ReturnCode rc = parseXML(doc);
    
    xmlFreeDoc(doc);
    
    return ( rc );

#else

    assert(0);
    return ( !SUCCESS );

#endif // VOCAL_USE_XML
}


ReturnCode
ParsePair::parseXMLString(const string & s)
{
#if defined(VOCAL_USE_XML)
    xmlDocPtr doc = 0;
    
    doc = xmlParseMemory(const_cast<char *>(s.c_str()), s.size());
    
    if ( !doc )
    {
        return ( !SUCCESS );
    }
    
    ReturnCode rc = parseXML(doc);
    
    xmlFreeDoc(doc);
    
    return ( rc );
#else
    
    assert(0);
    return ( !SUCCESS );

#endif // VOCAL_USE_XML    
}

ReturnCode
ParsePair::parseXML(xmlDocPtr & doc)
{
#if defined(VOCAL_USE_XML)

    xmlNodePtr currentNode = xmlDocGetRootElement(doc);
    
    assert( currentNode != 0 );

    string  name(reinterpret_cast<const char *>(currentNode->name));
    
    while ( name != myTagName )
    {
        currentNode = currentNode->next;

        if ( currentNode == 0 )
        {
            return ( !SUCCESS );
        }

        name = reinterpret_cast<const char *>(currentNode->name);
    }
    assert( currentNode != 0 );

    // step into the configuration data
    //

    for (   currentNode = currentNode->children;
            currentNode != 0; 
            currentNode = currentNode->next 
        )
    {
        if ( currentNode->type != XML_ELEMENT_NODE )
        {
            continue;
        }
        
        name = reinterpret_cast<const char *>(currentNode->name);
        
        const char *  pvalue 
            = reinterpret_cast<const char *>(xmlNodeGetContent(currentNode));

        string value;
        
        if ( pvalue == 0 )
        {
            // We have a name only.
            //
            Value & val = myPairs[name];
            ++val.count;
            val.type = NO_VALUE;
            
            continue;
        }
        
        value = pvalue;
        
        remove_ws(value);

        if ( value.size() == 0 )
        {
            // We have a name only.
            //
            Value & val = myPairs[name];
            ++val.count;
            val.type = NO_VALUE;
            
            continue;
        }
                
        Value & val = myPairs[name];
        ++val.count;
        val.type = VALUE_PRESENT;
        val.value.push_back(value);
        
        currentNode = currentNode->next;
    }
    
    return ( SUCCESS );
    
#else
    
    assert(0);
    return ( !SUCCESS );

#endif
    
}





