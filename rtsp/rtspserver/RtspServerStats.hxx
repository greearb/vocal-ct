#ifndef RtspServerStats_hxx
#define RtspServerStats_hxx
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


static const char* const RtspServerStats_hxx_Version =
    "$Id: RtspServerStats.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "vtypes.h"

#define FileTypeIndex 32 // should be equal to myNumberOfCodecs

class RtspServerStats
{
    public:
        ///
        RtspServerStats();
        ///
        virtual ~RtspServerStats() {}

        int totalSuccessReqRecvd() 
            { return (myTotalReqRecvd - myTotalNonSuccessReqRecvd); }

        int totalFinishedSessions()
            { return (myTotalFinRecSessions + myTotalFinPlySessions); }

        int totalActiveSessions()
            { return (myTotalActRecSessions + myTotalActPlySessions); }

        float averageRecordingThroughput() //=D/C
            { return (myTotalRecReqProcessed == 0) ?
                     0 : (myTotalRecThroughput / (myTotalRecReqProcessed*1.0)); }

        float averageRecordingRoughThroughput() //=E/C
            { return (myTotalRecReqProcessed == 0) ?
                     0 : 
                     (myTotalRecRoughThroughput / (myTotalRecReqProcessed*1.0)); }

        float averageRecordingPSW() //=F/C
            { return (myTotalRecReqProcessed == 0) ?
                     0 : (myTotalRecPSW / (myTotalRecReqProcessed*1.0)); }

        float averagePlayingThroughput() //=B/A
            { return (myTotalPlyReqProcessed == 0) ?
                     0 : (myTotalPlyThroughput / (myTotalPlyReqProcessed*1.0)); }

        bool report(); 

        /// counters
        ///
        int myTotalReqRecvd;
        ///
        int myTotalNonSuccessReqRecvd;
        ///
        int myTotalRespSent;
        ///
        int myTotalFinRecSessions;
        ///
        int myTotalFinPlySessions;
        ///
        int myTotalActRecSessions;
        ///
        int myTotalActPlySessions;

        //The notations in comments refer to RtspSessionStats.hxx
        // for calc averages throughput for finished recording sessions
        ///
        int myTotalRecReqProcessed; //C=sum of n
        /// 
        float myTotalRecThroughput; //D=sum of X
        /// 
        float myTotalRecRoughThroughput; //E=sum of Y
        ///
        float myTotalRecPSW; //F=sum of W
        
        // for calc averages throughput for finished playing sessions
        ///
        int myTotalPlyReqProcessed; //A=sum of m
        ///
        float myTotalPlyThroughput; //B=sum of Z
        
        ///
        unsigned int myMinPktsStreamedPerSession; //min of I's, K's
        ///
        unsigned int myMaxPktsStreamedPerSession; //max of I's, K's
        ///
        unsigned int myMinBytesStreamedPerSession; //min of J's, L's
        ///
        unsigned int myMaxBytesStreamedPerSession; //max of J's, L's

        /// codec usage, array size > myNumberOfCodecs
        int myCodecUsage[FileTypeIndex];

};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
