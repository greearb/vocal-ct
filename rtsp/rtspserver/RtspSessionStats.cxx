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

static const char* const RtspSessionrStats_cxx_Version =
    "$Id: RtspSessionStats.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspSessionStats.hxx"
#include "RtspConfiguration.hxx"
#include "RtpFileTypes.hxx"
#include <stdio.h>
#include <string>
#include "cpLog.h"


RtspSessionStats::RtspSessionStats()
    : myCodecUsed(-1)
{
}

int 
RtspSessionStats::totalRecRequests()
{
    return myRecordDataList.size();
}

int 
RtspSessionStats::totalPktsRecvd()
{
    int result = 0;
    for (RecordDataList::iterator itr = myRecordDataList.begin();
             itr != myRecordDataList.end(); itr++ )
    {
        result += (*itr).myPktsRecvd;
    }
    return result;
}

int 
RtspSessionStats::totalBytesRecvd()
{
    int result = 0;
    for (RecordDataList::iterator itr = myRecordDataList.begin();
             itr != myRecordDataList.end(); itr++ )
    {
        result += (*itr).myBytesRecvd;
    }
    return result;
}

float
RtspSessionStats::totalRecPSW()
{
    float result = 0.0;
    for (RecordDataList::iterator itr = myRecordDataList.begin();
             itr != myRecordDataList.end(); itr++ )
    {
        result += ((*itr).myPreStreamWaitRecvd).length() * 1.0 /
                   ( TickCount::ticksPerSec * 1.0 );
    }
    return result;
}

float
RtspSessionStats::averageRecPSW()
{
    return (totalRecRequests() == 0) ? 
           0 : ( totalRecPSW() / ( totalRecRequests() * 1.0) );
}

float
RtspSessionStats::totalRecThroughput()
{
    float result = 0.0;
    for (RecordDataList::iterator itr = myRecordDataList.begin();
             itr != myRecordDataList.end(); itr++ )
    {
        if ( ((*itr).myStreamWaitRecvd).length() > 0 )
        {
            result += ((*itr).myBytesRecvd * 1.0) /
                      ( (((*itr).myStreamWaitRecvd).length() * 1.0) /
                        ( TickCount::ticksPerSec * 1.0) );
        }
    }
    return result;
}

float
RtspSessionStats::averageRecThroughput()
{
    return (totalRecRequests() == 0) ?
           0.0 : ( totalRecThroughput() / ( totalRecRequests() * 1.0) );
}

float
RtspSessionStats::totalRecRoughThroughput()
{
    float result = 0.0;
    for (RecordDataList::iterator itr = myRecordDataList.begin();
             itr != myRecordDataList.end(); itr++ )
    {
       if ( (((*itr).myStreamWaitRecvd).length() > 0) ||
            (((*itr).myPreStreamWaitRecvd).length() > 0) )
       {
            result += ((*itr).myBytesRecvd * 1.0) /
                      ( ( ((*itr).myStreamWaitRecvd).length() +
                          ((*itr).myPreStreamWaitRecvd).length() ) * 1.0 /
                        ( TickCount::ticksPerSec * 1.0 ) ) ;
       }
    }
    return result;
}

float
RtspSessionStats::averageRecRoughThroughput()
{
    return (totalRecRequests() == 0) ?
           0.0 : ( totalRecRoughThroughput() / (totalRecRequests() * 1.0) );
}

int 
RtspSessionStats::totalPlyRequests()
{
    return myPlayDataList.size();
}

int 
RtspSessionStats::totalPktsSent()
{
    int result = 0;
    for (PlayDataList::iterator itr = myPlayDataList.begin();
             itr != myPlayDataList.end(); itr++ )
    {
        result += (*itr).myPktsSent;
    }
    return result;
}

int 
RtspSessionStats::totalBytesSent()
{
    int result = 0;
    for (PlayDataList::iterator itr = myPlayDataList.begin();
             itr != myPlayDataList.end(); itr++ )
    {
        result += (*itr).myBytesSent;
    }
    return result;
}

float
RtspSessionStats::totalPlyThroughput()
{
    float result = 0.0;
    for (PlayDataList::iterator itr = myPlayDataList.begin();
             itr != myPlayDataList.end(); itr++ )
    {
        if ( ((*itr).myStreamWaitSent).length() > 0)
        {
            result += ((*itr).myBytesSent * 1.0) /
                      ((((*itr).myStreamWaitSent).length() * 1.0) /
                       (TickCount::ticksPerSec * 1.0));
        }
    }
    return result;
}

float
RtspSessionStats::averagePlyThroughput()
{
    return (totalPlyRequests() == 0) ?
           0.0 : ( totalPlyThroughput() / (totalPlyRequests() * 1.0) );
}

void
RtspSessionStats::report(const char* sessionId)
{
    FILE* f;
    string filename = RtspConfiguration::instance().statisticsLogFile;
    f = fopen(filename.c_str(), "a");
    if (!f)
    {
        cpLog( LOG_ERR, "Fail to open rtsp statistic log file to write" );
        return ;
    }

    RtspConfiguration::instance().logFileMutex.lock();

    fprintf(f, "\n---Per Session Data---\n");
    fprintf(f, "SessionId: %s\n", sessionId);
    if (totalRecRequests() > 0)
    {
        fprintf(f, "TotRecReq: %d\n", totalRecRequests());
        fprintf(f, "TotPktRcv: %d\n", totalPktsRecvd());
        fprintf(f, "TotBytRcv: %d\n", totalBytesRecvd());
        fprintf(f, "AvgRecPSW: %.6f sec\n", averageRecPSW());
        fprintf(f, "AvgRecTh : %.6f\n", averageRecThroughput());
        fprintf(f, "AvgRecRTh: %.6f\n", averageRecRoughThroughput());
    }
    if (totalPlyRequests() > 0)
    {
        fprintf(f, "TotPlyReq: %d\n", totalPlyRequests());
        fprintf(f, "TotPktSnt: %d\n", totalPktsSent());
        fprintf(f, "TotBytSnt: %d\n", totalBytesSent());
        fprintf(f, "AvgPlyTh : %.6f\n", averagePlyThroughput());
    }
    if (myCodecUsed < myNumberOfCodecs)
       fprintf(f, "CodecUsed: %s\n", rtpFileTypeInfo[myCodecUsed].name.c_str());

    fflush(f);
    fclose(f);

    RtspConfiguration::instance().logFileMutex.unlock();

}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
