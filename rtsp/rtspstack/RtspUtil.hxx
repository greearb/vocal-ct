#ifndef RtspUtil_Hxx
#define RtspUtil_Hxx
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


static const char* const RtspUtil_hxx_Version =
    "$Id: RtspUtil.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "CharData.hxx"

enum RtspMethodsType
{
    RTSP_NULL_MTHD = 0, // first several are most common ones
    RTSP_ANNOUNCE_MTHD,
    RTSP_DESCRIBE_MTHD,
    RTSP_PLAY_MTHD,
    RTSP_RECORD_MTHD,
    RTSP_SETUP_MTHD,
    RTSP_TEARDOWN_MTHD,
    RTSP_PAUSE_MTHD,

    RTSP_GET_PARAMETER_MTHD,
    RTSP_OPTIONS_MTHD,
    RTSP_REDIRECT_MTHD,
    RTSP_SET_PARAMETER_MTHD,
    RTSP_UNKNOWN_MTHD        // 12

};
    
enum RtspHeadersType
{
    RTSP_ACCEPT_HDR = 0, // first several are most common ones 
    RTSP_CSEQ_HDR,
    RTSP_RANGE_HDR,
    RTSP_SESSION_HDR,
    RTSP_TRANSPORT_HDR,
    RTSP_BLOCKSIZE_HDR,
    RTSP_CONTENT_BASE_HDR,
    RTSP_CONTENT_LENGTH_HDR,
    RTSP_CONTENT_TYPE_HDR,

    RTSP_ACCEPT_ENCODING_HDR,
    RTSP_ACCEPT_LANGUAGE_HDR,
    RTSP_ALLOW_HDR,
    RTSP_AUTHORIZATION_HDR,
    RTSP_BANDWIDTH_HDR,
    RTSP_CACHE_CONTROL_HDR,
    RTSP_CONFERENCE_HDR,
    RTSP_CONNECTION_HDR,
    RTSP_CONTENT_ENCODING_HDR,
    RTSP_CONTENT_LANGUAGE_HDR,
    RTSP_CONTENT_LOCATION_HDR,
    RTSP_DATE_HDR,
    RTSP_EXPIRES_HDR,
    RTSP_FROM_HDR,
    RTSP_HOST_HDR,
    RTSP_IF_MATCH_HDR,
    RTSP_IF_MODIFIED_SINCE_HDR,
    RTSP_LAST_MODIFIED_HDR,
    RTSP_LOCATION_HDR,
    RTSP_PROXY_AUTHENTICATE_HDR,
    RTSP_PROXY_REQUIRE_HDR,
    RTSP_PUBLIC_HDR,
    RTSP_REFERER_HDR,
    RTSP_RETRY_AFTER_HDR,
    RTSP_REQUIRE_HDR,
    RTSP_RTP_INFO_HDR,
    RTSP_SCALE_HDR,
    RTSP_SERVER_HDR,
    RTSP_SPEED_HDR,
    RTSP_TIMESTAMP_HDR,
    RTSP_UNSUPPORTED_HDR,
    RTSP_USER_AGENT_HDR,
    RTSP_VARY_HDR,
    RTSP_VIA_HDR,
    RTSP_WWW_AUTHENTICATE_HDR,
    RTSP_UNKNOWN_HDR        // 44
};

enum RtspStatusCodesType
{
    RTSP_NULL_STATUS = 0,

    RTSP_100_STATUS,

    RTSP_200_STATUS,
    RTSP_201_STATUS,
    RTSP_250_STATUS,

    RTSP_300_STATUS,
    RTSP_301_STATUS,
    RTSP_302_STATUS,
    RTSP_303_STATUS,
    RTSP_305_STATUS,

    RTSP_400_STATUS,
    RTSP_401_STATUS,
    RTSP_402_STATUS,
    RTSP_403_STATUS,
    RTSP_404_STATUS,
    RTSP_405_STATUS,
    RTSP_406_STATUS,
    RTSP_407_STATUS,
    RTSP_408_STATUS,
    RTSP_410_STATUS,

    RTSP_411_STATUS,
    RTSP_412_STATUS,
    RTSP_413_STATUS,
    RTSP_414_STATUS,
    RTSP_415_STATUS,

    RTSP_451_STATUS,
    RTSP_452_STATUS,
    RTSP_453_STATUS,
    RTSP_454_STATUS,
    RTSP_455_STATUS,
    RTSP_456_STATUS,
    RTSP_457_STATUS,
    RTSP_458_STATUS,
    RTSP_459_STATUS,
    RTSP_460_STATUS,
    RTSP_461_STATUS,
    RTSP_462_STATUS,

    RTSP_500_STATUS,
    RTSP_501_STATUS,
    RTSP_502_STATUS,
    RTSP_503_STATUS,
    RTSP_504_STATUS,
    RTSP_505_STATUS,
    RTSP_551_STATUS,

    RTSP_UNKNOWN_STATUS //45

};


/** */
class RtspUtil
{
    public:
        /** */
        static u_int32_t getMethodInNumber(const CharData& methodInStr);

        /** */
        static u_int32_t getHeaderInNumber(const CharData& headerInStr);

        /** */
        static u_int32_t getStatusCodeInNumber(RtspStatusCodesType code)
            { return myStatusCodes[code]; }

        /** */
        static CharData& getStatusCodeInString(RtspStatusCodesType code)
            { return myStatusCodeStrings[code]; }

        /** */
        static CharData& getStatusInString(RtspStatusCodesType code)
            { return myStatusStrings[code]; }

        /** */
        static CharData& getVersion()
            { return myVersion[0]; }

    private:
        /** */
        static CharData myMethods[];
        /** */
        static CharData myHeaders[];
        /** */
        static u_int32_t myStatusCodes[];
        /** */
        static CharData myStatusCodeStrings[];
        /** */
        static CharData myStatusStrings[];
        /** */
        static CharData myVersion[];
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
