
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

static const char* const SipUriSupportedTable_cxx_Version =
    "$Id: SipUriSupportedTable.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipUriSupportedTable.hxx"
#include <iostream>
#include <map>

using namespace Vocal;


SipUriSupportedTable::SipUriSupportedTable()
{
    string ss("sip");
    string ss1("http");
    string ss2("ftp");
    string ss3("telnet");
    string ss4("mailto");
    string ss5("mgcp");
    string ss6("h323");

    SipUriSupportedEntry x(0, ss);
    SipUriSupportedEntry x1(1, ss1);
    SipUriSupportedEntry x2(2, ss2);
    SipUriSupportedEntry x3(3, ss3);
    SipUriSupportedEntry x4(4, ss4);
    SipUriSupportedEntry x5(5, ss5);

    uriSupported[0] = x;
    uriSupported[1] = x1;
    uriSupported[2] = x2;
    uriSupported[3] = x3;
    uriSupported[4] = x4;
    uriSupported[5] = x5;
}


SipUriSupportedTable::SipUriSupportedTable(const SipUriSupportedTable& src)
        :
        uriSupported(src.uriSupported),
        size(src.size)
{
}


///

const SipUriSupportedTable&
SipUriSupportedTable::operator=(const SipUriSupportedTable& src)
{
    if (&src != this)
    {
        // do the copy
        uriSupported = src.uriSupported;
        size = src.size;
    }
    return *this;
}


SipUriSupportedTable::~SipUriSupportedTable()
{
}



int
SipUriSupportedTable::getSize()
{
    return uriSupported.size();
}


SipUriSupportedEntry
SipUriSupportedTable::getEntry(int entry)
{
    map < int, SipUriSupportedEntry > ::iterator pos;
    pos = uriSupported.begin();
    SipUriSupportedEntry spe = pos->second;
    return spe;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
