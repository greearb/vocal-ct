#ifndef VXmlParser_hxx
#define VXmlParser_hxx

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

static const char* const VXmlParser_hxx_Version =
    "$Id: VXmlParser.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include <list>
#include <string>
#include "VFile.hxx"
#include "VFileBrowser.hxx"
#include "parser.h"
#include "VBadDataException.hxx"


class VXmlParser
{
    public:

        ///Constructor
        VXmlParser(const VFile& fileName) throw (VBadDataException&);

        ///Constructor
        VXmlParser(const string& data) throw (VBadDataException&);

        ///Destructor
        ~VXmlParser();


        /** Get an attribute for a particular node **/
        string getAttribute(xmlNodePtr node, const string& attr)
        throw (VBadDataException&);


        /**Get a tag content relative to the node, throws exception
         * if tag is not found
         */
        string getContent(xmlNodePtr root, const string& tag)
        throw (VBadDataException&);

        /**Get a subtag content within a tag, throws exception
         * if tag or subtag is not found
         */
        string getContent(const string& tag, const string& subtag)
        throw (VBadDataException&);

        /**Get a tag contents within tree , if tag maches elements in the
           tree, returns the list if contents
           e.g.  <x>
                   <y>
                     <z></z>
                   </y>
                   <y>
                     <z></z>
                   </y>
                  </x>
         */
        void getContent(const string& tag, const string& subTag,
                        list < string > & retList) throw (VBadDataException&);

        /**Get a tag content within a node, if tag maches multiple
           elements in the DOM tree, return a list
           e.g.  <x>
                    <y></y>
                    <y></y>
                 </x>
         */
        void getContent(xmlNodePtr root, const string& tag,
                        const string& subTag, list < string > & retList) throw (VBadDataException&);

        ///Dump the parsed XML tree into a file
        void dump(FILE* file);

        ///Dump the parsed XML tree to stdout
        void dump();

        ///Finds the very first node from the XML tree that matches the name.
        xmlNodePtr findNode(xmlNodePtr root, const string& name) throw (VBadDataException&);

        /**Finds the list of nodes that matches the tag in the tree
           if root=NULL, the top of the tree is taken as root
         */
        void findNode(xmlNodePtr root, const string& name,
                      list < xmlNodePtr > & retList) throw (VBadDataException&);

    private:
        ///Prase the XML contents and create element tree in memory
        void parseContents(const string& contents) throw (VBadDataException&);


        xmlDocPtr _doc;
};

#endif
