
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
static const char* const main_cxx_Version =
    "$Id: main.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "sdp.hxx"
#include <string>


void
testEncode (SDPDescriptor& sd)
{
    cout << "------ Encode begins ------" << endl;
    // Session Description
    // v=0 by default - no other value as of now
    // o= owner/creator and session identifier
    // s= session name
    // i=* session information
    // u=* URI of description
    // e=* email address
    // p=* phone number
    // c=* connection information (not required if included in all media
    // b=* bandwidth information
    // Time Description
    // t= time the session is active
    // r=* zero or more repeated times
    // z=* time zone adjustment
    // k=* encryption key
    // Verify
    cout << sd.encode();

    // tbd: Return the string and plug it into decode()
    cout << "------- Encode ends -------" << endl;
}    // testEncode


/** This tests out the decoding of the SDP message, copy C'tor, and
= operator for the SDP class */
void
testDecode(SDPDescriptor& sdpDesc)
{
    //form a SDP string, and call decode, , and call writeDetails to check if the values have been assigned properly.
    string sdp;
    sdp.erase();

    sdp = "v=0\n";
    sdp += "o=mhandley 28908 23456 IN IP4 126.16.64.4\n";
    sdp += "s=SDP seminar\n";
    sdp += "i=information\n";
    sdp += "c=IN IP4 224.2.17.12/127\n";
    sdp += "a=recvonly\n";

    sdp += "m=audio 49230 RTP/AVP 96 97 98\n";
    sdp += "a=rtpmap:96 L8/8000\n";
    sdp += "a=rtpmap:97 L16/8000\n";
    sdp += "a=rtpmap:98 X-myrtpmap/8000\n";
    sdp += "c=IN IP4 128.2.124.5\n";

    sdp += "m=video 51372 RTP/AVP 31\n";
    sdp += "a=orient:portrait\n";
    sdp += "a=X-myAttrib:myValue\n";
    sdp += "a=ptime:100\n";
    sdp += "b=AS:64\n";

    sdpDesc.decode(sdp);

    cout << sdpDesc;


    cout << "Testing the copy operator";
    SDPDescriptor sdpDest;
    sdpDest = sdpDesc;

    cout << sdpDest;


    //test copy constructor.
    SDPDescriptor sdpCopyCtor(sdpDest);

    cout << sdpCopyCtor;

    cout << "Test Decode done" << endl;
}


void
main()
{
    cpLogSetPriority(LOG_DEBUG);

    SDPDescriptor sd;
    testDecode (sd);
    testEncode (sd);
}
