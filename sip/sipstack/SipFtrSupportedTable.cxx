
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

static const char* const SipFtrSupportedTable_cxx_Version =
    "$Id: SipFtrSupportedTable.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include <iostream>
#include "SipFtrSupportedTable.hxx"
#include <map>

using namespace Vocal;


sipFtrSupportedTable::sipFtrSupportedTable()
{
    string ss("fsip");
    string ss1("fhttp");
    string ss2("fftp");
    string ss3("ftelnet");
    string ss4("fmailto");
    string ss5("fmgcp");
    string ss6("fh323");

    SipFtrSupportedEntry x(0, ss);
    SipFtrSupportedEntry x1(1, ss1);
    SipFtrSupportedEntry x2(2, ss2);
    SipFtrSupportedEntry x3(3, ss3);
    SipFtrSupportedEntry x4(4, ss4);
    SipFtrSupportedEntry x5(5, ss5);

    ftrsupported[0] = x;
    ftrsupported[1] = x1;
    ftrsupported[2] = x2;
    ftrsupported[3] = x3;
    ftrsupported[4] = x4;
    ftrsupported[5] = x5;
}


sipFtrSupportedTable::sipFtrSupportedTable(const sipFtrSupportedTable& src)
        :
        ftrsupported(src.ftrsupported),
        size(src.size)
{}



const sipFtrSupportedTable&
sipFtrSupportedTable::operator=(const sipFtrSupportedTable& src)
{
    if (this != &src)
    {
        ftrsupported = src.ftrsupported;
        size = src.size;
    }
    return *this;
}


sipFtrSupportedTable::~sipFtrSupportedTable()
{
}


int sipFtrSupportedTable::getSize()
{
    return ftrsupported.size();
}

SipFtrSupportedEntry sipFtrSupportedTable::getEntry(int entry)
{
    map < int, SipFtrSupportedEntry > ::iterator pos;
    pos = ftrsupported.begin();
    SipFtrSupportedEntry spe = pos->second;
    return spe;
}

/*
static SipUriSupportedEntry uriEntry;
int size;
}
 
*/
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
