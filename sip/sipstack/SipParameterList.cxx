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

static const char* const SipParameterList_cxx_Version =
    "$Id: SipParameterList.cxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include "global.h"
#include "SipParameterList.hxx"

#include <stdio.h>

using namespace Vocal;

string
SipParameterListParserException::getName( void ) const
{
    return "SipParameterListParserException";
}


SipParameterList::SipParameterList(char delimiter /* default value */)
    : map <Data, Data>(), myDelimiter(delimiter)
{
}


SipParameterList::SipParameterList(const Data& data, 
                                   char delimiter /* default value */) 
    throw(SipParameterListParserException &)
    : map <Data, Data>(), myDelimiter(delimiter)
{
    decode(data, myDelimiter);
}


SipParameterList::SipParameterList(const SipParameterList& src )
    :map<Data, Data>(src), myDelimiter( src.myDelimiter)
{
}


SipParameterList::~SipParameterList()
{
}


Data SipParameterList::encode() const
{
    Data encoded;
    Data delim;
    delim.setchar(0, myDelimiter);
    map<Data,Data>::const_iterator i;
    bool start = true;
    for(i = begin(); i != end(); ++i)
    {
        if(i->first.length() > 0)
        {
            if(!start) encoded += delim;
            start = false;
            encoded += i->first;
            if(i->second.length() > 0)
            {
                int pos;
                Data addQuotes("\"");
                //Check to see if the value already has quotes
                pos = i->second.find("\"");
                if(pos != Data::npos) addQuotes = "";

                encoded += "=";
		if(i->first == "realm"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "nonce") {
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "cnonce"){		//Rajarshi
		    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
		}else if(i->first == "opaque"){		//Rajarshi
		    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
		}else if(i->first == "domain"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "username"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "uri"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "response"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else {
                    encoded += i->second;
		}
            }
        }
    }
    return encoded;
}



void SipParameterList::decode(Data data, char delimiter, bool eatWhitespace)
{
    char matchedChar;
    Data key;
    Data value;
    bool done = false;
    char del1[56], del2[56];
    myDelimiter = delimiter;
    sprintf(del1, "=%c", myDelimiter);
    sprintf(del2, "%c", myDelimiter);

    while(!done)
    {
        key = data.matchChar(del1, &matchedChar);
        if(matchedChar == '=')
        {
            // this is a separator, so stuff before this is a thing
            value = data.matchChar(del2, &matchedChar);
            if(matchedChar != myDelimiter)
            {
                value = data;
                // done
                done = true;
            }
            string tokenstr = value.convertString();
#if 0
            //Leave the parameter value in its original form
            //strip "" off this.
            int pos;
            pos = tokenstr.find("\"");

            if (pos != (int)(string::npos))
            {
                tokenstr = tokenstr.substr(pos + 1, tokenstr.length() - 2);
            }
#endif

            key.removeSpaces();
	    key.lowercase();//comaparison is case sensitive
            Data token = tokenstr;
            if(eatWhitespace)
            {
                token.removeSpaces();
            }
            operator[](key) = token;
        }
        else if(matchedChar == delimiter)
        {
            // no value here, so just stuff nothing into the value
            assert (key != "");
            operator[](key) = "";
        }
        else
        {
            // nothing left, so done
            // done
            if(data.length() != 0)
            {
                assert (data != "");
                operator[](data) = "";
            }
            done = true;
        }
    }
}

Data 
SipParameterList::getValue(const Data& key) const
{
    Data ret;

    map<Data, Data>::const_iterator itr = find(key);
    if(itr != end())
    {
        ret =  itr->second;
    }
    return ret;
}


void 
SipParameterList::setValue(const Data& key, const Data& value)
{
    operator[](key) = value;
}


void 
SipParameterList::clearValue(const Data& key)
{
    erase(key);
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
