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

static const char* const RtspMsgParser_cxx_Version =
    "$Id: RtspMsgParser.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <stdio.h>
#include "RtspMsgParser.hxx"
#include "CharDataParser.hxx"
#include "Data.hxx"
#include "cpLog.h"
#include "RtspDescribeMsg.hxx"
#include "RtspAnnounceMsg.hxx"
#include "RtspPlayMsg.hxx"
#include "RtspRecordMsg.hxx"
#include "RtspSetupMsg.hxx"
#include "RtspPauseMsg.hxx"
#include "RtspTeardownMsg.hxx"
#include "RtspOptionsMsg.hxx"
#include "RtspSetParameterMsg.hxx"
#include "RtspRequest.hxx"
#include "RtspResponse.hxx"

// need to remove all the cpLogs for performance after debuging period

Sptr< RtspMsg >
RtspMsgParser::preParse( const char* data, 
                         int bytesNeedtoDecode,
                         int& bytesDecoded,
                         Sptr< RtspMsg > savedMsg) 
            throw (RtspBadDataException&)
{
//    Data dump(data, bytesNeedtoDecode);
//    cpLog(LOG_DEBUG, "Get data: \n%s", dump.logData());

    Sptr< RtspMsg > msg = 0;

    if (savedMsg == 0)
    {
        cpLog(LOG_DEBUG_HB, "create new msg.");
        msg = createRtspMsg(data, bytesNeedtoDecode);
        assert(msg != 0);
    }
    else
    {
        assert(savedMsg->getCompletionFlag() != RTSP_MSG_COMPLETE);
        msg = savedMsg;
        cpLog(LOG_DEBUG_HB, "append to the saved msg.");
    }
        
    CharData stream(data, bytesNeedtoDecode);

    CharDataParser charParser(&stream);
    CharData aLine;
    int result;
   
    int advance = 0;

    if (msg->myCompletionFlag < RTSP_MSG_STRTLINE_COMPLETE)
    {
        result = charParser.getNextLine(&aLine);
    
        Data startline(aLine.getPtr(), aLine.getLen());
        msg->myStartLine += startline;
        cpLog(LOG_DEBUG_HB, "Startline is:%s", (msg->myStartLine).logData());
        bytesDecoded += aLine.getLen();
        advance += aLine.getLen();
    
        if (result == 0)
        {
            msg->myCompletionFlag = RTSP_MSG_NEW;
            cpLog(LOG_DEBUG_HB, "The startline is not finished.");
            return msg;
        }
        else
            msg->myCompletionFlag = RTSP_MSG_STRTLINE_COMPLETE;
    }

    bool foundEmptyLine = true;
    if (msg->myCompletionFlag < RTSP_MSG_HEADERS_COMPLETE)
        foundEmptyLine = false;

    const char* hdrStart = data + advance;
    int hdrsLen = 0;
    int hdrIndex = msg->myNumHeader;
    int newHdrOffset = (msg->myHeaders).length() - 
                       (msg->myIncompHeader).length();

    while (!foundEmptyLine && (bytesDecoded < bytesNeedtoDecode))
    {
        result = charParser.getNextLine(&aLine);

        if (result == 0)
        {
            Data incompHdr(aLine.getPtr(), aLine.getLen());
            msg->myIncompHeader = incompHdr;
            bytesDecoded += aLine.getLen();
            advance += aLine.getLen();
            hdrsLen += aLine.getLen();
            Data hdrs(hdrStart, hdrsLen);
            msg->myHeaders += hdrs;
            cpLog(LOG_DEBUG_HB, "Incomplete Headers is: %s",
                  (msg->myHeaders).logData());
            msg->myNumHeader = hdrIndex;
//            msg->myCompletionFlag = RTSP_MSG_STRTLINE_COMPLETE;
            return msg;
        }
        else
        {
            bytesDecoded += aLine.getLen();
            advance += aLine.getLen();
            hdrsLen += aLine.getLen();
            if( (msg->myIncompHeader).length() != 0 )
            {
                Data halfHdr(aLine.getPtr(), aLine.getLen());
                msg->myIncompHeader += halfHdr;

                LocalScopeAllocator lo;
                const char* buf = msg->myIncompHeader.getData(lo);

                aLine.set(buf,
                          (msg->myIncompHeader).length());
            }
            else
            {
                hdrIndex++;
            }

            CharDataParser lineParser(&aLine);
        
            if (aLine.isEmptyLine())
            {
                foundEmptyLine = true;
                Data hdrs(hdrStart, hdrsLen);
                msg->myHeaders += hdrs;
                cpLog(LOG_DEBUG_HB, "Complete Headers is: %s", 
                      (msg->myHeaders).logData());
                msg->myNumHeader = hdrIndex;
                msg->myCompletionFlag = RTSP_MSG_HEADERS_COMPLETE;
            }
            else
            {
                cpLog(LOG_DEBUG_HB, "The hdrline len is:%d", aLine.getLen());
       
                CharData aHeader;
                lineParser.getNextWord(&aHeader);
                cpLog(LOG_DEBUG_HB, "aHeader len is: %d", aHeader.getLen());
              
                u_int32_t headerNum;
                headerNum = RtspUtil::getHeaderInNumber(aHeader);
                cpLog(LOG_DEBUG_HB, "headerNum is: %d", headerNum);
                if (headerNum != RTSP_UNKNOWN_HDR)
                {
                    int hdrBdyOffset = newHdrOffset + aHeader.getLen();
                    CharData tmpData;
                    HeaderValueData hv;
      
                    if ( lineParser.parseThru(&tmpData, ':') == 0 )
                    {
                        hv.offset = -1;
                        hv.len = 0;
                    }
                    else
                    {
                        hdrBdyOffset += tmpData.getLen();
     
                        lineParser.getThruSpaces(&tmpData);
                        hdrBdyOffset += tmpData.getLen();
    
                        hv.offset = hdrBdyOffset;
                        hv.len = newHdrOffset + aLine.getLen() - hdrBdyOffset;
                    }
                    msg->setHeadersMap(headerNum, hv); 
                    cpLog(LOG_DEBUG_HB, "** Offset %d, length %d",
                          hv.offset, hv.len);
                }
                else
                {
                    char tmpHeader[32];
                    for (u_int32_t j = 0; j < aHeader.getLen(); j++)
                        tmpHeader[j] = aHeader[j];
                    tmpHeader[aHeader.getLen()] = '\0';
                    cpLog(LOG_INFO, "unknown header: %s", tmpHeader);
                }
            }
            newHdrOffset += aLine.getLen();
            (msg->myIncompHeader).erase();
        }
    }

    int contentLen = msg->getContentLength();
    cpLog(LOG_DEBUG_HB, "Content-Length is: %d", contentLen);

    if (foundEmptyLine)
    {
        if (contentLen > 0)
        {
            if ( (contentLen - (msg->myMsgBody).length()) > 
                 (bytesNeedtoDecode - bytesDecoded) )
            {
                msg->myCompletionFlag = RTSP_MSG_HEADERS_COMPLETE;
                cpLog(LOG_DEBUG_HB, "The msgBody not complete");
                contentLen = bytesNeedtoDecode - bytesDecoded;
            }
            else
            {
                //if the msg already has some msgBody, contenLen need to 
                // subtract that portion since it has been parsed already
                contentLen -= (msg->myMsgBody).length();
                msg->myCompletionFlag = RTSP_MSG_COMPLETE;
                cpLog(LOG_DEBUG_HB, "The msgBody is complete");
            }

            Data msgBody(data + advance, contentLen);
            msg->setHasBody(true);
            msg->myMsgBody += msgBody;
            cpLog(LOG_DEBUG_HB, "Msgbody is: %s",
                  (msg->myMsgBody).logData());
            bytesDecoded += contentLen;
        }
        else
        {
            msg->myCompletionFlag = RTSP_MSG_COMPLETE;
            msg->setHasBody(false);
            cpLog(LOG_DEBUG_HB, "There is no msgbody.");
        }
    }
    else
    {
        // should not come to here
        throw RtspBadDataException( "Bad RTSP message", __FILE__, __LINE__, 0 );
    }

    return msg;

}

Sptr< RtspMsg > 
RtspMsgParser::createRtspMsg(const char* data, int bytesNeedtoDecode)
{
    Sptr< RtspMsg > msg = 0;

    CharData strtLine;
    strtLine.set(data, bytesNeedtoDecode);
    CharDataParser charParser(&strtLine);
    CharData firstWord;
    charParser.getNextWord(&firstWord);

    if (firstWord.isEqualNoCase("RTSP",4))
    {
        msg = new RtspResponse();
    }
    else
    {
        u_int32_t method;
        method = RtspUtil::getMethodInNumber(firstWord);
        switch (method)
        {
            case RTSP_ANNOUNCE_MTHD:
                msg = new RtspAnnounceMsg();
                break;
            case RTSP_DESCRIBE_MTHD:
                msg = new RtspDescribeMsg();
                break;
            case RTSP_PLAY_MTHD:
                msg = new RtspPlayMsg();
                break;
            case RTSP_RECORD_MTHD:
                msg = new RtspRecordMsg();
                break;
            case RTSP_PAUSE_MTHD:
                msg = new RtspPauseMsg();
                break;
            case RTSP_SETUP_MTHD:
                msg = new RtspSetupMsg();
                break;
            case RTSP_TEARDOWN_MTHD:
                msg = new RtspTeardownMsg();
                break;
            case RTSP_GET_PARAMETER_MTHD:
                msg = new RtspRequest();
                break;
            case RTSP_OPTIONS_MTHD:
                msg = new RtspOptionsMsg();
                break;
            case RTSP_REDIRECT_MTHD:
                msg = new RtspRequest();
                break;
            case RTSP_SET_PARAMETER_MTHD:
                msg = new RtspSetParameterMsg();
                break;
            default :
                msg = new RtspMsg();
                if (msg != 0)
                    msg->setFirstWordUnknown(true);
                break;
        }
    }
    return msg;
    
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
