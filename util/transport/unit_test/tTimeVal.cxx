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

static const char* const tTimeVal_cxx_Version = 
  "$Id: tTimeVal.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include "Application.hxx"
#include "TimeVal.hxx"
#include "Writer.hxx"
#include "Condition.hxx"

using namespace Vocal;
using namespace Vocal::TimeAndDate;
using namespace Vocal::Threads;

class TestApplication : public Application
{
    public:
    	ReturnCode  	run();
};

class TestApplicationCreator : public ApplicationCreator
{
    Application * create() { return ( new TestApplication ); }
};

int main(int argc, char ** argv, char ** arge)
{
    TestApplicationCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}

ReturnCode TestApplication::run()
{
    TimeVal 	sec0, sec5, sec10, sec15, sec20;

    cout << "sec0:  " << sec0  << ", ms = " << sec0.milliseconds()  << endl
    	 << "sec5:  " << sec5  << ", ms = " << sec5.milliseconds()  << endl
    	 << "sec10: " << sec10 << ", ms = " << sec10.milliseconds() << endl
    	 << "sec15: " << sec15 << ", ms = " << sec15.milliseconds() << endl
    	 << "sec20: " << sec20 << ", ms = " << sec20.milliseconds() << endl;

    sec0.now();
    
    for ( int i = 0; i < 1; i++ ) { usleep(1000000); cout << '.'; } cout << endl;
    sec5.now();

    for ( int i = 0; i < 1; i++ ) { usleep(1000000); cout << '.'; } cout << endl;
    sec10.now();
    
    for ( int i = 0; i < 1; i++ ) { usleep(1000000); cout << '.'; } cout << endl;
    sec15.now();

    for ( int i = 0; i < 1; i++ ) { usleep(1000000); cout << '.'; } cout << endl;
    sec20.now();

    cout << "sec0:  " << sec0  << ", ms = " << sec0.milliseconds()  << endl
    	 << "sec5:  " << sec5  << ", ms = " << sec5.milliseconds()  << endl
    	 << "sec10: " << sec10 << ", ms = " << sec10.milliseconds() << endl
    	 << "sec15: " << sec15 << ", ms = " << sec15.milliseconds() << endl
    	 << "sec20: " << sec20 << ", ms = " << sec20.milliseconds() << endl;

    cout << "sec 5 - sec0:   " << sec5.milliseconds()  - sec0.milliseconds()  << endl
    	 << "sec 10 - sec5:  " << sec10.milliseconds() - sec5.milliseconds()  << endl
    	 << "sec 15 - sec10: " << sec15.milliseconds() - sec10.milliseconds() << endl
    	 << "sec 20 - sec15: " << sec20.milliseconds() - sec15.milliseconds() << endl;

    timeval tv;
    
    tv.tv_sec = 5;
    tv.tv_usec = 5000000;
    
    cout << "tv = " << TimeVal(tv) << endl;

    cout << ( sec0 + tv ) << endl;
    
    sec5 += tv;
    sec10 += 10000;

    TimeVal 	d0, d5, d10, d15;
    
    d0 = sec5 - sec0;
    d5 = sec10 - sec5;
    d10 = sec15 - sec10;
    d15 = sec20 - sec15;

    cout << "sec0:  " << sec0  << ", ms = " << sec0.milliseconds()  << endl
    	 << "sec5:  " << sec5  << ", ms = " << sec5.milliseconds()  << endl
    	 << "sec10: " << sec10 << ", ms = " << sec10.milliseconds() << endl
    	 << "sec15: " << sec15 << ", ms = " << sec15.milliseconds() << endl
    	 << "sec20: " << sec20 << ", ms = " << sec20.milliseconds() << endl;

    cout << "sec5 - sec0:   " << sec5.milliseconds()  - sec0.milliseconds()  << endl
    	 << "sec10 - sec5:  " << sec10.milliseconds() - sec5.milliseconds()  << endl
    	 << "sec15 - sec10: " << sec15.milliseconds() - sec10.milliseconds() << endl
    	 << "sec20 - sec15: " << sec20.milliseconds() - sec15.milliseconds() << endl;

    cout << "d0:  " << d0  << ", ms = " << d0.milliseconds()  << endl
    	 << "d5:  " << d5  << ", ms = " << d5.milliseconds()  << endl
    	 << "d10: " << d10 << ", ms = " << d10.milliseconds() << endl
    	 << "d15: " << d15 << ", ms = " << d15.milliseconds() << endl;

    cout << "sec5 - sec0:   " << d0.milliseconds()  << endl
    	 << "sec10 - sec5:  " << d5.milliseconds()  << endl
    	 << "sec15 - sec10: " << d10.milliseconds() << endl
    	 << "sec20 - sec15: " << d15.milliseconds() << endl;

    cout << "sec0 < sec5 ? " << ( sec0 < sec5 ? "true" : "false" ) << endl;
    cout << "sec0 == sec5 ? " << ( sec0 == sec5 ? "true" : "false" ) << endl;
    cout << "sec20 == sec20 ? " << ( sec20 == sec20 ? "true" : "false" ) << endl;

    timeval tv0;
    tv0.tv_sec = -10;
    tv0.tv_usec = -10000000;
    
    cout << "tv0: " << TimeVal(tv0) << endl;    

    timeval tv1;
    tv1.tv_sec = -9;
    tv1.tv_usec = -10000001;
    
    cout << "tv1: " << TimeVal(tv1) << endl;    

    timeval tv2;
    tv2.tv_sec = -9;
    tv2.tv_usec = -11000000;
    
    cout << "tv2: " << TimeVal(tv2) << endl;    

    cout << "tv1 < tv0 ? " 
    	 << ( (TimeVal(tv1) < TimeVal(tv0)) ? "true" : "false" ) << endl;

    cout << "tv2 == tv0 ? " 
    	 << ( (TimeVal(tv2) == TimeVal(tv0)) ? "true" : "false" ) << endl;


    TimeVal foo, bar;
    foo.now();
    for ( size_t i = 0; i < 1; i++ )
    {
        vusleep(1000);
    }
    bar.now();
    
    TimeVal diff = bar - foo;
    cout << " bar - foo = " << diff.milliseconds() << endl;
        

    return ( SUCCESS );
}
