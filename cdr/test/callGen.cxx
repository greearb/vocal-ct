
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
static const char* const callGen_cxx_Version =
    "$Id: callGen.cxx,v 1.1 2004/05/01 04:14:55 greear Exp $";
#include <unistd.h>
#include <stdio.h>

#include "CdrInterface.hxx"
#include "cpLog.h"

main( int argc, char** argv )
{
    if (argc < 4)
    {
        cerr << "Usage: callGen <cdrServer> <portnum> <# of calls>" << endl;
        exit(0);
    }

    cpLogSetPriority(LOG_ALERT);

    int maxCalls = atoi(argv[3]);
    try
    {
        CdrInterface::instance(argv[1], atoi(argv[2]));
    }
    catch (VException &e)
    {
        cerr << e.getDescription() << endl;
    }

    try
    {
        time_t now = time(0);
        char *timeStr = ctime(&now);
        cout << "Start time: " << timeStr << endl;

        cout << "Generating " << maxCalls << " calls..." << endl;

        CdrInterface &cdr = CdrInterface::instance();

        int count = 0;
        int fromGen = 3210000;
        int toGen = 3830000;
        int idGen = 1;

        while (++count <= maxCalls)
        {
            char callFrom[256];
            char callTo[256];
            char callId[256];

            sprintf(callFrom, "%d", fromGen++);
            sprintf(callTo, "%d", toGen++);
            sprintf(callId, "%d", idGen++);

            cdr.ringStarted(callFrom, callTo, callId);
            cdr.callStarted(callFrom, callTo, callId);
            cdr.callStarted(callFrom, callTo, callId);
            cdr.callEnded(callFrom, callTo, callId);
            cdr.callEnded(callFrom, callTo, callId);

            usleep(24000);
        }

        cout << "Generated " << count - 1 << " calls of " << maxCalls << endl;

        now = time(0);
        timeStr = ctime(&now);
        cout << "End time: " << timeStr << endl;
    }
    catch (VException &e)
    {
        cerr << "Exception caught" << e.getDescription() << endl;
    }
    catch (...)
    {
        cerr << "Caught unknown exception" << endl;
    }
}
