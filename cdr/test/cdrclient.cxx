
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
static const char* const cdrclient_cxx_Version =
    "$Id: cdrclient.cxx,v 1.1 2004/05/01 04:14:55 greear Exp $";
#include <time.h>
#include <strstream.h>
#include <unistd.h>
#include <string>

#include "VCdrException.hxx"
#include "CdrInterface.hxx"
#include "CdrData.hxx"
#include "mstring.hxx"
#include "cpLog.h"

main( int argc, char** argv )
{
    if (argc < 3)
    {
        cerr << "Usage: cdrclient <primeHost> <primePort> <bkupHost> <backupPort>" << endl;
        exit(0);
    }

    cpLogSetPriority(LOG_DEBUG);

    try
    {
        if (argc > 3)
            CdrInterface::instance(argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));
        else
            CdrInterface::instance(argv[1], atoi(argv[2]));
        cout << "got Cdr instance" << endl;
    }
    catch (VException &e)
    {
        cerr << e.getDescription() << endl;
    }

    try
    {
        CdrInterface &cdr = CdrInterface::instance();
        mstring from("6383");
        mstring to("6388");
        mstring callId("1");

        cdr.ringStarted(from, to, callId);
        sleep(1);
        cdr.callStarted(from, to, callId);
        cdr.callStarted(from, to, callId);
        sleep(2);
        cdr.callEnded(from, to, callId);
        cdr.callEnded(from, to, callId);

        sleep(1);

        to = "2326383";
        from = "2326388";
        callId = "12";
        cdr.ringStarted(from, to, callId);
        sleep(1);
        cdr.callStarted(from, to, callId);
        cdr.callStarted(from, to, callId);
        sleep(2);
        cdr.callEnded(from, to, callId);
        cdr.callEnded(from, to, callId);

        sleep(1);

        callId = "123";
        to = "4082326383";
        from = "92326388";
        cdr.ringStarted(from, to, callId);
        sleep(1);
        cdr.callStarted(from, to, callId);
        cdr.callStarted(from, to, callId);
        sleep(2);
        cdr.callEnded(from, to, callId);
        cdr.callEnded(from, to, callId);

        sleep(1);

        callId = "1234";
        to = "914082326383";
        from = "4082326388";
        cdr.ringStarted(from, to, callId);
        sleep(1);
        cdr.callStarted(from, to, callId);
        cdr.callStarted(from, to, callId);
        sleep(2);
        cdr.callEnded(from, to, callId);
        cdr.callEnded(from, to, callId);

        sleep(1);

        callId = "12345";
        to = "914082326383";
        from = "14082326388";
        cdr.callStarted(from, to, callId);
        cdr.callStarted(from, to, callId);
        sleep(2);
        cdr.callEnded(from, to, callId);
        cdr.callEnded(from, to, callId);

        sleep(1);

        callId = "123456";
        to = "915102326383";
        from = "14082326388";
        cdr.callStarted(from, to, callId);
        cdr.callStarted(from, to, callId);
        sleep(2);
        cdr.callEnded(from, to, callId);
        cdr.callEnded(from, to, callId);

        while (1);
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
