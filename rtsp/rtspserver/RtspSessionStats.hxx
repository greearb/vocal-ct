#ifndef RtspSessionStats_hxx
#define RtspSessionStats_hxx
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


static const char* const RtspSessionStats_hxx_Version =
    "$Id: RtspSessionStats.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "vtypes.h"
#include "Duration.hxx"
#include <list>

using namespace std;
using Vocal::Statistics::TickCount;
using Vocal::Statistics::Duration;

typedef struct PerRecordReqData_t
{
        ///
        int myPktsRecvd; //p1
        ///
        int myBytesRecvd; //p2
        ///
        // need an alternative for SunOS
        Duration myStreamWaitRecvd; //p3
        ///
        // need an alternative for SunOS
        Duration myPreStreamWaitRecvd; //p4
        ///
        bool myFirstPktsFlag;
}
PerRecordReqData;

typedef struct PerPlayReqData_t
{
        ///
        int myPktsSent; //q1
        ///
        int myBytesSent; //q2
        ///
        // need an alternative for SunOS
        Duration myStreamWaitSent; //q3
}
PerPlayReqData;

class RtspSessionStats
{
    public:
        ///
        RtspSessionStats();
        ///
        virtual ~RtspSessionStats() {}

        ///
        int totalPktsRecvd(); //I=sum n of p1
        ///
        int totalBytesRecvd(); //J=sum n of p2
        ///PSW=pre-stream-wait
        float totalRecPSW(); //W=sum n of p4~, (p4~=p4.length()/ticksPerSec)
        ///
        float averageRecPSW(); //=W/n
        ///
        float totalRecThroughput(); //X=sum of n p2/(p3~)
        ///
        float averageRecThroughput(); //=X/n
        ///
        float totalRecRoughThroughput(); //Y=sum of n p2/(p3~ + p4~)
        ///
        float averageRecRoughThroughput(); //=Y/n
        ///
        int totalRecRequests(); //n

        ///
        int totalPktsSent(); //K=sum m of q1
        ///
        int totalBytesSent(); //L=sum m of q2
        ///
        float totalPlyThroughput(); //Z=sum of m q2/(q3~)
        ///
        float averagePlyThroughput(); //=Z/m
        ///
        int totalPlyRequests(); //m
        
        ///
        void report(const char* sessionId);

        ///
        typedef list< PerRecordReqData > RecordDataList;
        RecordDataList myRecordDataList;

        ///
        typedef list< PerPlayReqData > PlayDataList;
        PlayDataList myPlayDataList;

        ///
        int myCodecUsed;
};
        

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
