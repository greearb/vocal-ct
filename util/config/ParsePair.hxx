#if !defined(VOCAL_PARSEPAIR_HXX)
#define VOCAL_PARSEPAIR_HXX

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


static const char* const ParsePair_hxx_Version =
    "$Id: ParsePair.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "VocalCommon.hxx"
#include "NonCopyable.hxx"
#include "NameValue.hxx"
#if defined(VOCAL_USE_XML)
#ifdef WIN32
#include "libxml/parser.h"
#else
#include "parser.h"
#endif
#else
struct xmlDocPtr;
#endif // VOCAL_USE_XML
#include <string>
#include <iostream>


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Vocal 
{


/** 
 */
namespace Configuration
{


using std::string;
using std::istream;


class ParsePair : Vocal::NonCopyable
{
    public:

        ParsePair();
        
        virtual ~ParsePair();
        
        void                    tagName(const string &);
        
        enum Type
        {
            CFG_FILE,
            CFG_STRING,
            CFG_XML_FILE,
            CFG_XML_STRING
        };
        
        /** If type is CFG_FILE, the file will be opened and parsed.
         *  If the file extension is ".xml", it will be parsed as an
         *  XML file. Otherwise is will be parse as name value pairs.
         *  The name and value must be on a single line. The first
         *  word is parsed as the name, and the rest as the value.
         *  You may have one or more separation characters between the
         *  name and value. The separation characters are ":=;,\\/ \t".
         *  You may also include single line comments in the file. A 
         *  comment starts with one of the comment characters "#!".
         *  A line with a name value pair may have a comment on following
         *  the value.<br><br>
         *
         *  If type is CFG_STRING, the string will be parse similar to
         *  a text file.<br><br>
         *
         *  If type is CFG_XML_FILE, the file will be opened and parsed
         *  as an XML file. The general format is:
         *  <pre>
         *  &lt;myTagName&rt;
         *      &lt;name&rt;value&lt;/name&rt;  &lt;!- name value pair -&rt;
         *      &lt;name/&rt;             &lt;!- no value
         *      ...
         *  &lt;myTagName&rt;
         *  </pre><br><br>
         *
         *  If type is CFG_XML_STRING, the string will be parsed as XML.
         *  <br><br>
         *
         *  Calling parse multiple times will add entries to the name 
         *  value map.<br><br>
         */
        ReturnCode              parse(Type, const string &);


        /** Returns the name value map that is populated as a result of
         *  calling parse.
         */
        const NameValueMap &    pairs() const;
        
    private:

        ReturnCode          parseFile();
        ReturnCode          parseStream(istream &);

        ReturnCode          parseXMLFile();
        ReturnCode          parseXMLString(const string &);
        ReturnCode          parseXML(xmlDocPtr &);

        string              myTagName;
                
        string              myFileName;
        NameValueMap        myPairs;
};


} // namespace Configuration
} // namespace Vocal


#endif // !defined(VOCAL_PARSEPAIR_HXX)
