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

static const char* const RtspServerStats_cxx_Version =
    "$Id: RtspServerStats.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspServerStats.hxx"
#include "RtspConfiguration.hxx"
#include "RtpFileTypes.hxx"
#include <stdio.h>
#include <string>
#include <errno.h>
#include "cpLog.h"


RtspServerStats::RtspServerStats()
    : myTotalReqRecvd( 0 ),
      myTotalNonSuccessReqRecvd(0),
      myTotalRespSent(0),
      myTotalFinRecSessions(0),
      myTotalFinPlySessions(0),
      myTotalActRecSessions(0),
      myTotalActPlySessions(0),
      myTotalRecReqProcessed(0),
      myTotalRecThroughput(0.0),
      myTotalRecRoughThroughput(0.0),
      myTotalRecPSW(0.0),
      myTotalPlyReqProcessed(0),
      myTotalPlyThroughput(0.0),
      myMinPktsStreamedPerSession(2147483647),
      myMaxPktsStreamedPerSession(0),
      myMinBytesStreamedPerSession(2147483647),
      myMaxBytesStreamedPerSession(0)
{
//    for(int i=0; i<myNumberOfCodecs; i++)
    for(int i=0; i<FileTypeIndex; i++)
    {
        myCodecUsage[i] = 0;
    }
}

bool
RtspServerStats::report()
{
    FILE* f;
    string filename = RtspConfiguration::instance().statisticsLogFile;
    f = fopen(filename.c_str(), "a");
    if (!f)
    {
        cpLog( LOG_ERR, "Fail to open rtsp statistic log file to write: %s",
               strerror( errno ) );
        return false;
    }

    RtspConfiguration::instance().logFileMutex.lock();

    fprintf(f, "\n---Per Server  Data---\n");
    fprintf(f, "TotReqRcv:  %d\n", myTotalReqRecvd);
    fprintf(f, "TotSucReq:  %d\n", totalSuccessReqRecvd());
    fprintf(f, "TotRecReq:  %d\n", myTotalRecReqProcessed);
    fprintf(f, "TotPlyReq:  %d\n", myTotalPlyReqProcessed);
    fprintf(f, "TotResSnt:  %d\n", myTotalRespSent);
    fprintf(f, "TotFinSes:  %d\n", totalFinishedSessions());
    fprintf(f, "TotActSes:  %d\n", totalActiveSessions());
    fprintf(f, "AvgRecTh :  %.6f\n", averageRecordingThroughput());
    fprintf(f, "AvgRecRTh:  %.6f\n", averageRecordingRoughThroughput());
    fprintf(f, "AvgRecPSW:  %.6f sec\n", averageRecordingPSW());
    fprintf(f, "AvgPlyTh :  %.6f\n", averagePlayingThroughput());
    fprintf(f, "MinPktStrm: %d\n", myMinPktsStreamedPerSession);
    fprintf(f, "MaxPktStrm: %d\n", myMaxPktsStreamedPerSession);
    fprintf(f, "MinBytStrm: %d\n", myMinBytesStreamedPerSession);
    fprintf(f, "MaxBytStrm: %d\n", myMaxBytesStreamedPerSession);

    int totalSess = totalFinishedSessions() + totalActiveSessions();
    for(int i=0; i < myNumberOfCodecs; i++)
    {
        if (myCodecUsage[i] > 0)
        {
            fprintf(f, "CodecUsage: %s %.2f\n", 
                    rtpFileTypeInfo[i].name.c_str(),
                    (myCodecUsage[i]*100.0) / (totalSess * 1.0));
        }
    }

    fflush(f);
    fclose(f);

    RtspConfiguration::instance().logFileMutex.unlock();

    return true;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
