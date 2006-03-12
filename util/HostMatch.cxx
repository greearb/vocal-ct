
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

#warning "Dead code..remove this file."

#if 0

#include "global.h"
#include <deque>
#include <string>
#include "HostMatch.hxx"
#include <iostream>
#include <cassert>
#include <set>
#include "NetworkAddress.h"
#include <algorithm>

#ifdef WIN32
#include <ws2tcpip.h>
#endif

class SimpleAddress
{
    public:
        SimpleAddress(const char* addr);
        ostream& print(ostream& s) const;

        friend ostream& operator<< ( ostream& s,
                                     const SimpleAddress& obj );

        friend bool operator<( const SimpleAddress& lhs ,
                               const SimpleAddress& rhs );

        friend bool operator==( const SimpleAddress& lhs ,
                                const SimpleAddress& rhs );


    private:
        char myAddr[4];
};


bool operator<( const SimpleAddress& lhs ,
                const SimpleAddress& rhs )
{
    if (memcmp(lhs.myAddr, rhs.myAddr, 4) < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool operator==( const SimpleAddress& lhs ,
                 const SimpleAddress& rhs )
{
    if (memcmp(lhs.myAddr, rhs.myAddr, 4) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


ostream& operator<< ( ostream& s, const SimpleAddress& obj )
{
    return obj.print(s);
}


SimpleAddress::SimpleAddress(const char* addr)
{
    memcpy(myAddr, addr, 4);
}


ostream& SimpleAddress::print(ostream& s) const
{
    unsigned int x;
    memcpy(&x, myAddr, 4);
    in_addr y;
    y.s_addr = x;
    s << inet_ntoa(y);
    return s;
}

deque < SimpleAddress > getAddrList(const string& host) {
    deque < SimpleAddress > list;

    char buf[2048];
    struct addrinfo *res = 0;

    int retval = NetworkAddress::getHostByName(host.c_str(), res);

    if (retval != NetworkAddress::getHostLookupOK) {
        return list;
    }
    while (res) {
       if (!getnameinfo(res->ai_addr, res->ai_addrlen, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST))
               list.push_back(SimpleAddress(buf));
       res = res->ai_next;
    }
    freeaddrinfo(res);

#if 0
    char** addrPtr;

    addrPtr = ent.h_addr_list;

    while (addrPtr && *addrPtr)
    {
        list.push_back(SimpleAddress(*addrPtr));
        addrPtr++;
    }
#endif

    return list;
}

set < SimpleAddress > getAddrSet(const string& host)
{
    set < SimpleAddress > list;

    char buf[2048];

#ifdef WIN32
    // Handle the case where host is a dotted ip address; getHostByName does
    // not do this properly on WIN32.
    unsigned long addr = inet_addr(host.c_str());
    if (addr != INADDR_NONE) 
    {
	char * c = (char*)&addr;
	list.insert(SimpleAddress(c));
	return list;
    }
#endif
    struct addrinfo *res = 0;
    int retval =
        NetworkAddress::getHostByName(host.c_str(), res);

    if (retval != NetworkAddress::getHostLookupOK)
    {
        return list;
    }
    while (res) {
       if (!getnameinfo(res->ai_addr, res->ai_addrlen, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST))
               list.insert(SimpleAddress(buf));
       res = res->ai_next;
    }
    freeaddrinfo(res);
    return list;
}

bool hostsEqual(const string& first, const string& second) {
    set < SimpleAddress > firstSet;
    set < SimpleAddress > secondSet;

    firstSet = getAddrSet(first);
    secondSet = getAddrSet(second);

    if (firstSet.size() == 0)
    {
        return false;
    }

    return firstSet == secondSet;
}


int hostsCompare(const string& first, const string& second)
{
    set < SimpleAddress > firstSet;
    set < SimpleAddress > secondSet;

    firstSet = getAddrSet(first);
    secondSet = getAddrSet(second);

    if (firstSet < secondSet)
    {
        return -1;
    }
    else if (firstSet > secondSet)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


bool hostsIntersect(const string& first, const string& second)
{
    set < SimpleAddress > firstSet;
    set < SimpleAddress > secondSet;

    firstSet = getAddrSet(first);
    secondSet = getAddrSet(second);

    deque < SimpleAddress > result;

    // set_intersection is part of <algorithm> .  It calculates the
    // intersection between firstSet and secondSet

    set_intersection(firstSet.begin(), firstSet.end(),
                     secondSet.begin(), secondSet.end(),
                     back_inserter(result));

    if (result.size() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }

    assert(0);
    return false;
}

#endif
