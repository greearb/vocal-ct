#ifndef RtspRangeHdr_hxx
#define RtspRangeHdr_hxx
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


static const char* const RtspRangeHdr_hxx_Version =
    "$Id: RtspRangeHdr.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <stdio.h>
#include "CharDataParser.hxx"
#include "Data.hxx"

/** Range Grammer
    Range            = "Range" ":" 1\#ranges-specifier
                          [ ";" "time" "=" utc-time ]
   ranges-specifier = npt-range | utc-range | smpte-range
   npt-range    =   ( npt-time "-" [ npt-time ] ) | ( "-" npt-time )
   npt-time     =   "now" | npt-sec | npt-hhmmss
   npt-sec      =   1*DIGIT [ "." *DIGIT ]
   npt-hhmmss   =   npt-hh ":" npt-mm ":" npt-ss [ "." *DIGIT ]
   npt-hh       =   1*DIGIT     ; any positive number
   npt-mm       =   1*2DIGIT    ; 0-59
   npt-ss       =   1*2DIGIT    ; 0-59

   Examples:
     npt=123.45-125
     npt=12:05:35.3-
     npt=now-

   note: we currently only support npt-range
 */
class RtspRangeHdr
{
    public:
        /** */
        RtspRangeHdr();
        /** */
        RtspRangeHdr(double startTime, double endTime);
        /** */
        RtspRangeHdr(const RtspRangeHdr& x);
        /** */
        ~RtspRangeHdr() {}
        /** */
        RtspRangeHdr& operator= (const RtspRangeHdr& x);
        
        /** */
        double getStartTime() const { return myStartTime; }
        /** */
        void setStartTime(const double  time) { myStartTime = time; }
        /** */
        double  getEndTime() const { return myEndTime; }
        /** */
        void setEndTime(const double  time) { myEndTime = time; }

        /** It parses Range Header and assign myStartTime and myEndTime */
        void parseRangeHeader(CharDataParser& parser);
//            throw (RtspBadDataException&);
        /** It does the same thing by taking Data as input */
        void parseRange(const Data& rangeData);

        /** to convert double  to string */
        void doubleToStr(double  time, char* buf)
        {
            sprintf(buf, "%3.3f", time);
        }

        /** it encodes npt time and return the data of a range */
        Data encodeNPT();
        /** it encodes Range hdr */
        Data encode();

        /** time in ms */
        long getStartTime()
            { return (long)(myStartTime * 1000.0); }
        /** time in ms */
        long getEndTime()
            { return (long)(myEndTime * 1000.0); }

    private:
        /** only support npt and second and fractional format for now 
            TODO support smpte, utc and hhmmss for npt
         */
        double myStartTime;
        /** */
        double myEndTime;

        // TODO support time= parameter
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
