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

static const char* const RtspUtil_cxx_Version =
    "$Id: RtspUtil.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "CharData.hxx"
#include "RtspUtil.hxx"

CharData RtspUtil::myMethods[] =
{
    CharData(""),
    CharData("ANNOUNCE"),
    CharData("DESCRIBE"),
    CharData("PLAY"),
    CharData("RECORD"),
    CharData("SETUP"),
    CharData("TEARDOWN"),
    CharData("PAUSE"),

    CharData("GET_PARAMETER"),
    CharData("OPTIONS"),
    CharData("REDIRECT"),
    CharData("SET_PARAMETER"),
    CharData("")
};

CharData RtspUtil::myHeaders[] =
{
    CharData("Accept"),
    CharData("Cseq"),
    CharData("Range"),
    CharData("Session"),
    CharData("Transport"),
    CharData("Blocksize"),
    CharData("Content-Base"),
    CharData("Content-length"),
    CharData("Content-Type"),

    CharData("Accept-Encoding"),
    CharData("Accept-Language"),
    CharData("Allow"),
    CharData("Authorization"),
    CharData("Bandwidth"),
    CharData("Cache-Control"),
    CharData("Conference"),
    CharData("Connection"),
    CharData("Content-Encoding"),
    CharData("Content-Language"),
    CharData("Content-Location"),
    CharData("Date"),
    CharData("Expires"),
    CharData("From"),
    CharData("Host"),
    CharData("If-Match"),
    CharData("If-Modified-Since"),
    CharData("Last-Modified"),
    CharData("Location"),
    CharData("Proxy-Authenticate"),
    CharData("Proxy-Require"),
    CharData("Public"),
    CharData("Referer"),
    CharData("Retry-After"),
    CharData("Require"),
    CharData("RTP-Info"),
    CharData("Scale"),
    CharData("Server"),
    CharData("Speed"),
    CharData("Timestamp"),
    CharData("Unsupported"),
    CharData("User-Agent"),
    CharData("Vary"),
    CharData("Via"),
    CharData("WWW-Authenticate"),
    CharData("")

};

u_int32_t RtspUtil::myStatusCodes[] =
{
    0,

    100, //Continue

    200, //OK
    201, //Created
    250, //Low on Storage Space

    300, //Multiple Choices
    301, //Moved Permanently
    302, //Moved Temporarily
    303, //See Other
    305, //Use Proxy

    400, //Bad Request
    401, //Unauthorized
    402, //Payment Required
    403, //Forbidden
    404, //Not Found
    405, //Method Not Allowed
    406, //Not Acceptable
    407, //Proxy Authentication Required
    408, //Request Time-out
    410, //Gone

    411, //Length Required
    412, //Precondition Failed
    413, //Request Entity Too Large
    414, //Request-URI Too Large
    415, //Unsupported Media Type

    451, //Parameter Not Understood
    452, //Conference Not Found
    453, //Not Enough Bandwidth
    454, //Session Not Found
    455, //Method Not Valid in this State
    456, //Header Field Not Valid For Resource
    457, //Invalid Range
    458, //Parameter Is Read-Only
    459, //Aggregate Option Not Allowed
    460, //Only Aggregate Option Allowed
    461, //Unsupported Transport
    462, //Destination Unreachable

    500, //Internal Server Error
    501, //Not Implemented
    502, //Bad Gateway
    503, //Service Unavailable
    504, //Gateway Timeout
    505, //RTSP Version not supported
    551, //Option Not Supported

    0

};

CharData RtspUtil::myStatusCodeStrings[] =
{
    CharData(""),

    CharData("100"), //Continue

    CharData("200"), //OK
    CharData("201"), //Created
    CharData("250"), //Low on Storage Space

    CharData("300"), //Multiple Choices
    CharData("301"), //Moved Permanently
    CharData("302"), //Moved Temporarily
    CharData("303"), //See Other
    CharData("305"), //Use Proxy

    CharData("400"), //Bad Request
    CharData("401"), //Unauthorized
    CharData("402"), //Payment Required
    CharData("403"), //Forbidden
    CharData("404"), //Not Found
    CharData("405"), //Method Not Allowed
    CharData("406"), //Not Acceptable
    CharData("407"), //Proxy Authentication Required
    CharData("408"), //Request Time-out
    CharData("410"), //Gone

    CharData("411"), //Length Required
    CharData("412"), //Precondition Failed
    CharData("413"), //Request Entity Too Large
    CharData("414"), //Request-URI Too Large
    CharData("415"), //Unsupported Media Type

    CharData("451"), //Parameter Not Understood
    CharData("452"), //Conference Not Found
    CharData("453"), //Not Enough Bandwidth
    CharData("454"), //Session Not Found
    CharData("455"), //Method Not Valid in this State
    CharData("456"), //Header Field Not Valid For Resource
    CharData("457"), //Invalid Range
    CharData("458"), //Parameter Is Read-Only
    CharData("459"), //Aggregate Option Not Allowed
    CharData("460"), //Only Aggregate Option Allowed
    CharData("461"), //Unsupported Transport
    CharData("462"), //Destination Unreachable

    CharData("500"), //Internal Server Error
    CharData("501"), //Not Implemented
    CharData("502"), //Bad Gateway
    CharData("503"), //Service Unavailable
    CharData("504"), //Gateway Timeout
    CharData("505"), //RTSP Version not supported
    CharData("551"), //Option Not Supported

    CharData("0")

};

CharData RtspUtil::myStatusStrings[] =
{
    CharData(""),

    CharData("Continue"),                                          //100

    CharData("OK"),                                                //200
    CharData("Created"),                                           //201
    CharData("Low on Storage Space"),                              //250

    CharData("Multiple Choices"),                                  //300
    CharData("Moved Permanently"),                                 //301
    CharData("Moved Temporarily"),                                 //302
    CharData("See Other"),                                         //303
    CharData("Use Proxy"),                                         //305

    CharData("Bad Request"),                                       //400
    CharData("Unauthorized"),                                      //401
    CharData("Payment Required"),                                  //402
    CharData("Forbidden"),                                         //403
    CharData("Not Found"),                                         //404
    CharData("Method Not Allowed"),                                //405
    CharData("Not Acceptable"),                                    //406
    CharData("Proxy Authentication Required"),                     //407
    CharData("Request Time-out"),                                  //408
    CharData("Gone"),                                              //410

    CharData("Length Required"),                                   //411
    CharData("Precondition Failed"),                               //412
    CharData("Request Entity Too Large"),                          //413
    CharData("Request-URI Too Large"),                             //414
    CharData("Unsupported Media Type"),                            //415

    CharData("Parameter Not Understood"),                          //451
    CharData("Conference Not Found"),                              //452
    CharData("Not Enough Bandwidth"),                              //453
    CharData("Session Not Found"),                                 //454
    CharData("Method Not Valid in this State"),                    //455
    CharData("Header Field Not Valid For Resource"),               //456
    CharData("Invalid Range"),                                     //457
    CharData("Parameter Is Read-Only"),                            //458
    CharData("Aggregate Option Not Allowed"),                      //459
    CharData("Only Aggregate Option Allowed"),                     //460
    CharData("Unsupported Transport"),                             //461
    CharData("Destination Unreachable"),                           //462

    CharData("Internal Server Error"),                             //500
    CharData("Not Implemented"),                                   //501
    CharData("Bad Gateway"),                                       //502
    CharData("Service Unavailable"),                               //503
    CharData("Gateway Timeout"),                                   //504
    CharData("RTSP Version not supported"),                        //505
    CharData("Option Not Supported"),                              //551

    CharData("")

};


CharData RtspUtil::myVersion[] =
{
    CharData("RTSP/1.0")
};
    
u_int32_t
RtspUtil::getHeaderInNumber(const CharData& hdrData)
{
    if (hdrData.getLen() == 0)
        return RTSP_UNKNOWN_HDR;

    for (int i = 0; i < RTSP_UNKNOWN_HDR; i++)
    {
        if (hdrData.isEqualNoCase(myHeaders[i].getPtr(),
                                  myHeaders[i].getLen()))
            return i;
    }
    return RTSP_UNKNOWN_HDR; 

}


u_int32_t
RtspUtil::getMethodInNumber(const CharData& methodData)
{
    if (methodData.getLen() == 0)
        return RTSP_UNKNOWN_HDR;

    for (int i = 0; i < RTSP_UNKNOWN_MTHD; i++)
    {
        if (methodData.isEqualNoCase(myMethods[i].getPtr(),
                                     myMethods[i].getLen()))
            return i;
    }
    return RTSP_UNKNOWN_MTHD; 

}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
