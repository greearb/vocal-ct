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

#pragma warning (disable: 4786)

#include "EmbeddedObj.hxx"
#include "cpLog.h"
using namespace Vocal;

EmbeddedObj::reverseMap const EmbeddedObj::_escRMap;
EmbeddedObj::forwardMap const EmbeddedObj::_escFMap;

EmbeddedObj::reverseMap::reverseMap()
{
    this->operator[]("20") = " ";
    this->operator[]("2C") = ",";
    this->operator[]("2c") = ",";
    this->operator[]("3D") = "=";
    this->operator[]("3d") = "=";
    this->operator[]("3B")= ";";
    this->operator[]("3b")= ";";
    this->operator[]("40")= "@";
    this->operator[]("3A")= ":";
    this->operator[]("3a")= ":";
    this->operator[]("3c")= "<";
    this->operator[]("3C")= "<";
    this->operator[]("3e")= ">";
    this->operator[]("3E")= ">";
}
EmbeddedObj::forwardMap::forwardMap()
{
    this->operator[](" ") = "%20";
    this->operator[](",") = "%2C";
    this->operator[]("=") = "%3D";
    this->operator[](";") = "%3B";
    this->operator[]("@") = "%40";
    this->operator[](":") = "%3A";
    this->operator[]("<") = "%3C";
    this->operator[](">") = "%3E";
}

EmbeddedObj::EmbeddedObj(const Data& raw, const string& local_ip)
    :
    _headerList(),
    _rawData(),
    _escFData()
{
    decode(raw, local_ip);
}
EmbeddedObj::EmbeddedObj(const string& raw, const string& local_ip)
    :
    _headerList(),
    _rawData(),
    _escFData()
{
    Data row(raw);
    decode(row, local_ip);
}
EmbeddedObj::EmbeddedObj(const EmbeddedObj& src)
    :
    _headerList(src._headerList),
    _rawData(src._rawData),
    _escFData(src._escFData)
{
}

const EmbeddedObj&
EmbeddedObj::operator=(const EmbeddedObj& src)
{
    if(&src !=this){
	_headerList = src._headerList;
	_rawData = src._rawData;
	_escFData = src._escFData;
    }
    return (*this);
}

void 
EmbeddedObj::decode(const Data& data, const string& local_ip)
{
    _rawData = data;
    if(fastDecode(data, local_ip) == false){
	//throw exception
    }
}

#if 0
const SipHeader& 
EmbeddedObj::getHeader(SipHeaderType type, int index=0) const
{
    assert(0);   
}
#endif

const SipRoute& 
EmbeddedObj::getRoute(const string& local_ip, int i) const
{
    Sptr<SipRoute> E;
    _headerList.getParsedHeader(E, SIP_ROUTE_HDR, local_ip, i);
    return *E;
}

const vector < Sptr<SipRoute> > 
EmbeddedObj::getRouteList(const string& local_ip) const
{
 vector < Sptr<SipRoute> > routes;
    for (int i=0; i<_headerList.getNumHeaders(SIP_ROUTE_HDR); i++)
    {
        Sptr<SipRoute> E;
        _headerList.getParsedHeader(E, SIP_ROUTE_HDR, local_ip, i);
        routes.push_back(E);
    }
    return routes;
}

Data
EmbeddedObj::doForwardEscape(const string& src)
{
    const char *resChar = " :,@;>=<";
    string retStr;
    string::size_type copyStartPos = 0;
    string::size_type keyCharPos;
    forwardMap::const_iterator po;
    while ( (keyCharPos = src.find_first_of(resChar, copyStartPos)) != string::npos )
    {
        retStr += src.substr(copyStartPos, keyCharPos-copyStartPos);
        po = _escFMap.find(src.substr(keyCharPos,1));
        if(po != _escFMap.end()){
            retStr += po->second;
        }
        copyStartPos = keyCharPos + 1;
    }
    retStr += src.substr(copyStartPos);
    return retStr;
    //sprintf based    
}


Data
EmbeddedObj::doReverseEscape(const string & src)
{
    Data ret;
    const char* tmp = src.c_str();
    char buf[512];
    char newbuf[3];
    memset(buf,0,512);
    memset(newbuf,0,3);
    strcpy(buf, tmp);
    char *t1 = 0;
    const char *t2 = 0;
    t1 = strchr(buf, '%');
    for( ; t1 != NULL; t1 = strchr(buf, '%')){
        strncpy(newbuf, t1+1, 2);
	cpLog(LOG_DEBUG_STACK, "Escaped Char found is = %s", newbuf);
        reverseMap::const_iterator pos;
        pos = _escRMap.find(newbuf);
        if(pos != _escRMap.end ())
        {
	    t2 = pos->second;
	    *t1 = *t2;
	    t1++;
	    for( unsigned int i = 0; i <= strlen(t1 - 2); i++ )
	    {
		*(t1+i) = *(t1 + i + 2);
	    }
	    cpLog(LOG_DEBUG_STACK, "replace value read from Static ASCII Map=%s", t2);
        }
    }
    cpLog(LOG_DEBUG_STACK, "The Converted doReverseEscape is =%s", buf);
    ret = buf;
    return ret; 
}

bool
EmbeddedObj::fastDecode(Data str, const string& local_ip)
{
    string src = str.convertString();
    string::size_type keyCharPos;
    if((keyCharPos = src.find_first_of("%")) != string::npos)
    {
	str = doReverseEscape(src);
	str += "\r\n";
        _escFData = src;
	return _headerList.decode(&str, local_ip);
    }
    else
    {
	_escFData = doForwardEscape(src);
        if(_escFData.length()){
	    return true;
	}
    }
    
    return false;
}
