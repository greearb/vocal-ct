
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

static const char* const VXmlParser_cxx_Version =
    "$Id: VXmlParser.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "global.h"
#include <cassert>
#include <cstdio>
#include <fstream>

#include "VXmlParser.hxx"
#include "VBadDataException.hxx"
#include "parser.h"
#include "tree.h"
#include "debugXML.h"


VXmlParser::VXmlParser(const VFile& file) throw (VBadDataException&)
        : _doc(NULL)
{
    char buf[256];

	 // 10/12/03 fpi
	 // Win32 WorkAround
	 // avoid compiling util/io with VFileBrowser.cxx
/*
    if (!VFileBrowser::fileExists(file))
    {
        sprintf(buf, "File %s does not exist." , file.getFullName().c_str());
        throw VBadDataException(buf, __FILE__, __LINE__, 0);
    }
	 ifstream inRead(file.getFullName().c_str());
*/
	 ifstream inRead;
	 try {
		 inRead.open(file.getFullName().c_str());
	 }
	 catch(...) {
        sprintf(buf, "File %s does not exist." , file.getFullName().c_str());
        throw VBadDataException(buf, __FILE__, __LINE__, 0);
	 }
	 //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    string dRead;
    while (inRead.getline(buf, 256))
    {
        dRead += buf;
    }
    if (dRead.size()) parseContents(dRead);
}


VXmlParser::VXmlParser(const string& contents) throw (VBadDataException&)
        : _doc(NULL)
{
    if (contents.size()) parseContents(contents);
}

VXmlParser::~VXmlParser()
{
    xmlFreeDoc(_doc);
}

void
VXmlParser::parseContents(const string& contents) throw (VBadDataException&)
{
    _doc = xmlParseMemory((char*)contents.c_str(), contents.size());
    if (!_doc)
    {
        throw VBadDataException("Failed to parse XML", __FILE__, __LINE__, 0);
    }
}

string
VXmlParser::getAttribute(xmlNodePtr node, const string& attr)
throw (VBadDataException&)
{
    xmlChar *result = xmlGetProp(node, (xmlChar*)attr.c_str());

    if (result == NULL)
    {
        char buf[256];
        sprintf(buf, "Failed to get attribute (%s)",
                attr.c_str());
        throw VBadDataException(buf, __FILE__, __LINE__, 0);
    }
    else
    {

        string returnVal((char*)result);
        free(result);
        return returnVal;
    }

}




string
VXmlParser::getContent(xmlNodePtr root, const string& tag)
throw (VBadDataException&)
{
    string retString;
    xmlNodePtr node = findNode(root, tag);
    if (node)
    {
        char* c = (char*)xmlNodeGetContent(node);
        if (c)
        {
            retString = c;
            free(c);
        }
    }
    if (!retString.size())
    {
        char buf[256];
        sprintf(buf, "Failed to get contents for tag (%s)",
                tag.c_str());
        throw VBadDataException(buf, __FILE__, __LINE__, 0);
    }
    return retString;
}

string
VXmlParser::getContent(const string& tag, const string& subTag)
throw (VBadDataException&)
{
    string retString;
    if (!_doc)
    {
        throw VBadDataException("Failed to parse XML", __FILE__, __LINE__, 0);
    }

    xmlNodePtr node = findNode( xmlDocGetRootElement( _doc ), tag);
    if (node)
    {
        node = findNode(node, subTag);
        if (node)
        {
            char* c = (char*)xmlNodeGetContent(node);
            if (c)
            {
                retString = c;
                free(c);
            }
        }
    }
    if (!retString.size())
    {
        char buf[256];
        sprintf(buf, "Failed to get contents for tag (%s) and subtag (%s)",
                tag.c_str(), subTag.c_str());
        throw VBadDataException(buf, __FILE__, __LINE__, 0);
    }
    return retString;
}

void
VXmlParser::getContent(const string& tag, const string& subTag, list < string > & retList)
throw (VBadDataException&)
{
    char* c = NULL;
    list < xmlNodePtr > nodeList;

    if (!_doc)
    {
        throw VBadDataException("Failed to parse XML", __FILE__, __LINE__, 0);
    }

    findNode( xmlDocGetRootElement( _doc ), tag, nodeList);
    for (list < xmlNodePtr > ::iterator itr = nodeList.begin();
            itr != nodeList.end(); itr++)
    {
        xmlNodePtr node = (*itr);
        if (subTag.size())
        {
            c = (char*)xmlNodeGetContent(findNode(node, subTag));
        }
        else
        {
            c = (char*)xmlNodeGetContent(node);
        }
        if (c)
        {
            retList.push_back(c);
            free(c);
        }
    }
}

void
VXmlParser::getContent(xmlNodePtr root, const string& tag, const string& subTag, list < string > & retList)
throw (VBadDataException&)
{
    char* c = NULL;
    list < xmlNodePtr > nodeList;
    findNode( root, tag, nodeList);
    for (list < xmlNodePtr > ::iterator itr = nodeList.begin();
            itr != nodeList.end(); itr++)
    {
        xmlNodePtr node = (*itr);
        if (subTag.size())
        {
            c = (char*)xmlNodeGetContent(findNode(node, subTag));
        }
        else
        {
            c = (char*)xmlNodeGetContent(node);
        }
        if (c)
        {
            retList.push_back(c);
            free(c);
        }
    }
}

xmlNodePtr
VXmlParser::findNode(xmlNodePtr root, const string& name) throw (VBadDataException&)
{
    xmlNodePtr retNode = NULL;
    xmlNodePtr startNode = NULL;


    if (root == NULL)
    {
        if (!_doc)
        {
            throw VBadDataException("Failed to parse XML", __FILE__, __LINE__, 0);
        }
        startNode = xmlDocGetRootElement( _doc );
    }
    else startNode = root;

    assert(startNode);

    if (strcmp((char*)startNode->name, name.c_str()) == 0) return startNode ;

    xmlNodePtr node = startNode->children;
    while (node)
    {
        if (strcmp((char*)node->name, name.c_str()) == 0) return node;
        xmlNodePtr cNode = node->children;
        if (cNode)
        {
            retNode = findNode(node, name);
            if (retNode) break;
        }
        node = node->next;
    }
    return retNode;
}

void
VXmlParser::findNode(xmlNodePtr root, const string& name, list < xmlNodePtr > & retList)
throw (VBadDataException&)
{
    xmlNodePtr startNode;

    if (root == NULL)
    {
        if (!_doc)
        {
            throw VBadDataException("Failed to parse XML", __FILE__, __LINE__, 0);
        }
        startNode = xmlDocGetRootElement( _doc );
    }
    else startNode = root;

    if (strcmp((char*)startNode->name, name.c_str()) == 0)
    {
        retList.push_back(startNode);
    }
    xmlNodePtr node = startNode->children;
    while (node)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            findNode(node, name, retList);
        }
        node = node->next;
    }
}

void
VXmlParser::dump(FILE* file)
{
    xmlDocDump(file, _doc);
}

void
VXmlParser::dump()
{
    xmlDocDump(NULL, _doc);
}
