
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

static const char* const ParseTest_cxx_Version =
    "$Id: ParseTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "global.h"
#include <symbols.hxx>

static const char* version UNUSED_VARIABLE =
    "";


#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <iostream.h>
#include "MpParseItem.h"
#include "SipMsg.hxx"
#include "MsgParser.h"

#include "ParseTest.hxx"


int ParseTest::testParse(Data inString)
{
    LocalScopeAllocator lo;
    SipMsg* sipMsg = SipMsg::decode(inString);
    if (!sipMsg)
    {
        cpLog(LOG_ERR, "Ignored some recieved message in SipMsg.decode");
        test( -1, false , seqNumber.getData(lo));
        return -1;
    }

    Method method = sipMsg->getType();
    switch (method)
    {
        case SIP_INVITE:
        {
            cpLog(LOG_DEBUG, "INVITE MSG");
            test(0, true , seqNumber.getData(lo));
            return 0;
        }
        break;
        case SIP_INFO:
        {
            cpLog(LOG_DEBUG, "INFO MSG");
            test(0, true , seqNumber.getData(lo));
            return 0;
        }
        break;
        case SIP_CANCEL:
        {
            cpLog(LOG_DEBUG, "CANCEL MSG");
            test(0, true , seqNumber.getData(lo));
            return 0;
        }
        break;
        case SIP_BYE:
        {
            cpLog(LOG_DEBUG, "BYE MSG");
            test(0, true , seqNumber.getData(lo));
            return 0;
        }
        break;
        case SIP_STATUS:
        {
            cpLog(LOG_DEBUG, "STATUS MSG");
            test(0, true , seqNumber.getData(lo));
            return 0;
        }
        break;
        case SIP_ACK:
        {
            cpLog(LOG_DEBUG, "ACK MSG");
            test(0, true , seqNumber.getData(lo));
            return 0;
        }
        break;
        case SIP_OPTIONS:
        {
            cpLog(LOG_DEBUG, "OPTIONS MSG");
            test(0, true , seqNumber.getData(lo));
            return 0;
        }
        break;
        default:
        {
            cpLog(LOG_DEBUG, "UNKNOWN MSG");
            test(0, false , seqNumber.getData(lo));
            return -1;
        }
        break;
    }
}

bool ParseTest::readAndparse(std::ifstream& file)
{
    //    std::ifstream file(fileName);
    bool pass = true;

    if (!file)
    {
        cpLog(LOG_ERR, "CANNOT OPEN FILE");
        test( -1, false, "parseTest:cannot open file");
        return false;
    }
    else
    {
        char line[256];
        char msg[4096];
        char seqNum[256];
        char strLen[256];

        while (!file.eof())
        {
            memset(line, 0, 256);
            file.getline(line, 256);

            memset(seqNum, 0, 256);
            memset(strLen, 0, 256);

            sscanf(line, "%s%s", seqNum, strLen);
            seqNumber = seqNum;

            int len = atoi(strLen);

            memset(msg, 0, 4096);
            file.read(msg, len );

            pass = (pass && testParse(msg));

        } //while 1
        return pass;
    }//end else
}




int main(int argc, char *argv[])
{

    ParseTest parseTest;
#if 0
    char* msg_invite = "INVITE sip:watson@boston.bell-telephone.com\r\nSIP/2.0\r\nVia: SIP/2.0/UDP 169.130.12.5\r\nFrom: <sip:a.g.bell@bell-telephone.com>\r\nTo: T. A. Watson <sip:watson@bell-telephone.com>\r\nCall-ID: 187602141351@worcester.bell-telephone.com\r\n";


    char* msg_info = "INFO sip:watson@boston.bell-telephone.com SIP/2.0\nVia: SIP/2.0/UDP 169.130.12.5\nFrom: <sip:a.g.bell@bell-telephone.com>\nTo: T. A. Watson <sip:watson@bell-telephone.com>\nCall-ID: 187602141351@worcester.bell-telephone.com\n";
#endif

    cpLogSetPriority( LOG_DEBUG );

    if (argc < 2)
    {
        cerr << "Usage: ParseTest <filename> \n";
        exit (1);
    }

    std::ifstream file(argv[1]);

    //read the file, and call testParse
    if (parseTest.readAndparse(file))
    {
        parseTest.test(0, true, "parseTest");
        return 0;
    }
    else
    {
        parseTest.test( -1, false, "parseTest");
        return -1;
    }
}
