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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspRangeHdr_cxx_Version =
    "$Id: RtspRangeHdr.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

//#include <stdio.h>
#include "RtspRangeHdr.hxx"
#include "cpLog.h"

RtspRangeHdr::RtspRangeHdr()
    : myStartTime(0.0),
      myEndTime(0.0)
{ }

RtspRangeHdr::RtspRangeHdr(double startTime, double endTime)
    : myStartTime(startTime),
      myEndTime(endTime)
{ }

RtspRangeHdr::RtspRangeHdr(const RtspRangeHdr& x)
    : myStartTime(x.myStartTime),
      myEndTime(x.myEndTime)
{ }

RtspRangeHdr&
RtspRangeHdr::operator=(const RtspRangeHdr& x)
{
    if (&x != this)
    {
        myStartTime = x.myStartTime;
        myEndTime = x.myEndTime;
    }
    return *this;
}

void
RtspRangeHdr::parseRangeHeader(CharDataParser& parser)
{
    CharData specifier;

    parser.getNextWord(&specifier);
    if (specifier.isEqualNoCase("npt", 3))
    {
        parser.getThruLength(NULL, 1); // get thru '='
        if (parser.getNextDouble(myStartTime))
        {
            parser.getThruLength(NULL, 1); // get thru '-'
            if ( ((parser.myMaskDigit)[parser.getCurChar()]) ||
                 (parser.getCurChar() == '.') )
            {
                parser.getNextDouble(myEndTime);
            }
        }
        else
        {
            cpLog(LOG_ERR, "Range hdr: error read start time.");
        }
    }
    else
    {
        cpLog(LOG_ERR, "Range hdr: only support npt time right now.");
    }
            
}

void
RtspRangeHdr::parseRange(const Data& rangeData)
{
    LocalScopeAllocator lo;
    CharData range(rangeData.getData(lo), rangeData.length());
    CharDataParser parser(&range);
    parseRangeHeader(parser);
}

Data
RtspRangeHdr::encodeNPT()
{
    Data nptRange = "npt=";
    char buf1[32], buf2[32];
    doubleToStr(myStartTime, buf1);
    doubleToStr(myEndTime, buf2);
    nptRange += buf1;
    nptRange += "-";
    nptRange += buf2;
    nptRange += "\r\n";

    return nptRange;
}

Data
RtspRangeHdr::encode()
{
    Data nptRange = "Range: ";
    nptRange += encodeNPT();

    return nptRange;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
