#ifndef RtspMsg_hxx
#define RtspMsg_hxx
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


static const char* const RtspMsg_hxx_Version =
    "$Id: RtspMsg.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "Data.hxx"
#include "RtspUtil.hxx"
#include "RtspTransportHdr.hxx"
#include "RtspRangeHdr.hxx"
#include "RtspBadDataException.hxx"
#include "Sptr.hxx"
#include "RtspTcpBuffer.hxx"

typedef struct RtspHeaderValueData_t
{
    int offset;
    u_int32_t len;
}
HeaderValueData;

enum RtspTransportType
{
    RTSP_TCP,
    RTSP_UDP
};

enum RtspMsgCompletionType
{
    RTSP_MSG_NEW, // a newly created msg or startline not complete msg
    RTSP_MSG_STRTLINE_COMPLETE, // The headers and on are not completed
    RTSP_MSG_HEADERS_COMPLETE, // Found emptyline but msgbody is not completed
    RTSP_MSG_COMPLETE,    // found emptyline, may or maynot have msgbody
};

/** */
class RtspMsg
{
    public:
        /** */
        RtspMsg();
        /** */
        RtspMsg(const RtspMsg& src);
        /** */
        RtspMsg& operator= (const RtspMsg& src);
        /** */
        virtual ~RtspMsg() {}

        /** transport connection */
        Sptr< RtspTcpBuffer > getTransConnPtr() const { return myTransConnPtr; }
        /** */
        void setTransConnPtr(Sptr< RtspTcpBuffer > ptr) 
            { myTransConnPtr = ptr; }

        /** */
        void setStartLine(const Data& stline) { myStartLine = stline; }
        /** */
        void setNumHeader(u_int32_t num) { myNumHeader = num; }
        /** */
        void setHeaders(const Data& hdr) { myHeaders = hdr; }
        /** */
        const Data& getHeaders() const { return myHeaders; }
        /** */
        void setHeadersMap(u_int32_t hdrType, const HeaderValueData& hvd) 
             { myHeaderMap[hdrType].offset = hvd.offset; 
               myHeaderMap[hdrType].len = hvd.len; }
        /** if it's the first time call, it will parse content-length header
            and set myContentLength, otherwise it directly return it
         */
        const int getContentLength() ; 
        /** */                       
        void setHasBody(bool has) { myHasBody = has; }
        /** */                       
        bool hasBody() const { return myHasBody; }
        /** */
        void setMsgBody(const Data& body) { myMsgBody = body; }
        /** */
        const Data& getMsgBody() const { return myMsgBody; }
        /** */
        const RtspMsgCompletionType getCompletionFlag() const 
            { return myCompletionFlag; }
        /** */
        void setCompletionFlag(RtspMsgCompletionType flag) 
            { myCompletionFlag = flag; }
        /** */                       
        void setFirstWordUnknown(bool flag) { myFirstWordUnknown = flag; }
        /** */                       
        bool getFirstWordUnknown() const { return myFirstWordUnknown; }

        /** check if there is Cseq number */
        bool isValid();

        /** It will look at the first word of the startLine
         *  if it is *not* RTSP, return true; otherwise false.
         */
        virtual bool isRequest();
        /** It will look at the first word of the startLine
         *  if it *is* RTSP, return true; otherwise false.
         */
        virtual bool isResponse();

        /** It parse Session header for the first time and assign mySessionId */
        const Data& getSessionId() ;
        /** It parse Cseq header for the first time and assign myCSeq */
        const Data& getCSeq() ;

        /** It parse Content-Base header for the first time and 
            return a reference of the data
         */
        const Data& getContentBase();

        /** It parse Content-Type header and return a copy of the data
         *  Return the whole string of the header body for now
         *  Application just need to see if the returned Data has
         *  substring of "application/sdp" to know if it support sdp
         *
         *  TODO: try parse the detail of Content-Type and Accept
         */
        const Data& getContentType();

        /** It returns an object when parser found a suitable transport-spec in
         *  the transport hdr, and it assigns myTransportSpec; 
         *  otherwise it returns 0
         */
        Sptr< RtspTransportSpec > getTransport();
        /** It returns an object when parser found and understood the Range hdr,
         *  and it assigns myRange; otherwise return 0
         */
        Sptr< RtspRangeHdr > getRange();
        /** */
        Data& getStartLine() { return myStartLine; }

        /** not finished */
        virtual Data encode() { return ""; }

    protected:
        friend class RtspMsgParser;

        /** return a Data copy of the header body
         * if full is true, the data stops at the CRLF
         * if full is not tru, the data stops at the first space or CRLF
         */
        const Data getHdrBodyData(RtspHeadersType header, bool full) const;
        /** return a value of the number header body */
        const u_int32_t getHdrBodyValue(RtspHeadersType header) const;
        
        /** saves the connection where the msg comming from 
         * Curretnly it only support TCP, if support UDP later may need to
         * create the higher level class and points to that
         */
        Sptr< RtspTcpBuffer > myTransConnPtr;
        
        /** This is the beginning of the msg.
         * because not whole msg being decoded at once, so need to 
         * hold the orig data
         *
         * The following are filled in by preParse() function:
         * myStartLine, myHeaders, myNumHeader, myHeaderMap, myContentLength, 
         * myHasBody, myMsgBody, myCompletionFlag
         */
        Data myStartLine;
        /** Holds all the header data for this msg */
        Data myHeaders;

        /** TODO if no one use it, remove it */
        u_int32_t myNumHeader;
        /**
         * Holds all kind of the headers' body offset, and only to parse 
         * when needed. The offset points to the beginning of the first 
         * non-space after ":", each item initialzed with -1.
         */
        HeaderValueData myHeaderMap[RTSP_UNKNOWN_HDR];

        /** */
        u_int32_t myContentLength;
        /** */
        bool myHasBody;
        /** */
        Data myMsgBody;
        /** A RtspMsg can be incomplete because when reading from TCP 
         *  connection we read a fixed size of bytes, and there can be 
         *  incomplete msg at the end. So use this flag to indicate the
         *  completion status of this msg.
         *
         */
        RtspMsgCompletionType myCompletionFlag;
        /** To save an incomplete header */
        Data myIncompHeader;
        /** This is to indicate the msg has firstword incomplete or unknown */
        bool myFirstWordUnknown;

        /** following headers are parsed after preParse(), whenever
         *  the server trys to get it
         */
        /** */
        Data mySessionId;
        /** */
        Data myCSeq;
        /** */
        Data myContentBase;
        /** */
        Data myContentType;
        /** */
        Sptr< RtspTransportSpec > myTransportSpec;
        /** */
        Sptr< RtspRangeHdr > myRange;

    private:

};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
