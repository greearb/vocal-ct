#ifndef SDP2TIME_HXX_
#define SDP2TIME_HXX_

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

static const char* const Sdp2Time_hxx_Version =
    "$Id: Sdp2Time.hxx,v 1.1 2004/05/01 04:15:24 greear Exp $";

#include <vector>
#include <strstream>

#include "Data.hxx"
#include "support.hxx"
#include "Data.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


/// r=<repeat-interval> <active-duration> <list-of-offsets-from-start-time>
class SdpTimeRepeat
{
    public:
        ///
        SdpTimeRepeat();
        ///
        Data getInterval ()
        {
            return interval;
        }
        ///
        void setInterval (const Data& inter)
        {
            interval = inter;
        }

        ///
        Data getActiveDuration()
        {
            return activeDuration;
        }
        ///
        void setActiveDuration (const Data& duration)
        {
            activeDuration = duration;
        }

        ///
        vector < Data > getOffsetList()
        {
            return offsetList;
        }
        ///
        void addOffset (const Data& offset)
        {
            offsetList.push_back(offset);
        }
        ///
        int getOffsetListSize()
        {
            return offsetList.size();
        }

    private:
        Data interval;
        Data activeDuration;
        vector < Data > offsetList;
};


/** t=<start-time> <stop-time>
 *  zero or more SdpTimeRepeat
 */
class SdpTime
{
    public:
        static const unsigned NTP_UX_DIFF;
        ///
        SdpTime();
	///
	SdpTime(time_t start, time_t end);
        ///
        SdpTime (list<Data>& timeDescriptionLines);

        ///
        Data getStartTime()
        {
            return startTime;
        }
        ///
        void setStartTime (const Data& start)
        {
            startTime = start;
        }

        ///
        Data getStopTime()
        {
            return stopTime;
        }
        ///
        void setStopTime (const Data& stop)
        {
            stopTime = stop;
        }

        ///
        vector < SdpTimeRepeat > getTimeRepeat()
        {
            return repeatList;
        }
        ///
        void addTimeRepeat (SdpTimeRepeat& repeat)
        {
            repeatList.push_back(repeat);
        }

        ///
        void encode (ostrstream& s);

    private:
        Data startTime;
        Data stopTime;
        vector < SdpTimeRepeat > repeatList;
};


}


}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */


#endif
