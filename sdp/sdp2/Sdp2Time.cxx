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

static const char* const SdpTime_cxx_Version =
    "$Id: Sdp2Time.cxx,v 1.1 2004/05/01 04:15:24 greear Exp $";

#include "global.h"
#include <cassert>
#include <time.h>
#include <strstream>

#include "Sdp2Session.hxx"


using Vocal::SDP::SdpTime;
using Vocal::SDP::SdpTimeRepeat;

const unsigned SdpTime::NTP_UX_DIFF = 2208988800U;


SdpTime::SdpTime()
	: repeatList()
{
    char timeStr[256];
    sprintf(timeStr, "%u", static_cast < unsigned int > (time(NULL) + NTP_UX_DIFF) );
    startTime = timeStr;
    ;
    stopTime = "0";
}    // SdpTime::SdpTime


SdpTime::SdpTime(time_t start, time_t end)
	: repeatList()
{
    char timeStr[256];
    sprintf(timeStr, "%u", static_cast < unsigned int > (start + NTP_UX_DIFF) );
    startTime = timeStr;

    sprintf(timeStr, "%u", static_cast < unsigned int > (end + NTP_UX_DIFF) );
    stopTime = timeStr;
}


///
SdpTime::SdpTime (list<Data>& timeDescriptionLines)
{
    list<Data>::iterator linecount = timeDescriptionLines.begin();
    while (linecount != timeDescriptionLines.end())
    {
        if ((*linecount)[0] == 't')
        {
            Data s = (*linecount);
	    s.parse("=");

            list<Data> tList;
	    bool finished = false;

	    while (!finished)
	    {
		Data x = s.parse(" ", &finished);
		if(finished)
		{
		    x = s;
		}
		tList.push_back(x);
	    }


            if (tList.size() != 2)
            {
                throw SdpExceptions (PARAM_NUMERR);
            }
            else
            {
                startTime = *tList.begin();
                stopTime = *(++tList.begin());
            }
        }
        else if ((*linecount)[0] == 'r')
        {
            Data s = (*linecount);
	    s.parse("=");

            list<Data> rList;
	    bool finished = false;

	    while (!finished)
	    {
		Data x = s.parse(" ", &finished);
		if(finished)
		{
		    x = s;
		}
		rList.push_back(x);
	    }

            if (rList.size() < 2)
            {
                throw SdpExceptions (PARAM_NUMERR);
            }
            else
            {
                SdpTimeRepeat stp;
                list<Data>::iterator iter;

                iter = rList.begin();
                stp.setInterval((*iter));
                ++iter;
                stp.setActiveDuration((*iter));
                ++iter;

                while (iter != rList.end())   // at least one offset
                {
                    // Push the offset on to the offsetList
                    stp.addOffset((*iter));
                    ++iter;
                }
                repeatList.push_back(stp);
            }
        }
        ++linecount;
    }
}    // SdpTime::SdpTime

///
void
SdpTime::encode (ostrstream& s)
{
    s << "t=" << startTime << ' ' << stopTime << "\r\n";

    vector < SdpTimeRepeat > ::iterator repeatIter;
    repeatIter = repeatList.begin();

    while (repeatIter != repeatList.end())
    {
        s << "r=" << (*repeatIter).getInterval() << ' '
        << (*repeatIter).getActiveDuration() ;

        if (0 == (*repeatIter).getOffsetListSize())
        {
            s << "\r\n";
        }
        else
        {
            vector < Data > offsetList;
            offsetList = (*repeatIter).getOffsetList();
            vector < Data > ::iterator offsetIter;
            offsetIter = offsetList.begin();

            while (offsetIter != offsetList.end())
            {
                s << ' ' << *offsetIter ;
                ++offsetIter;
            }
            s << "\r\n";
        }
        ++repeatIter;
    }
}    // SdpTime::encode


SdpTimeRepeat::SdpTimeRepeat()
{
    interval = "";
    activeDuration = "";
}
