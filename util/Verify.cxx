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

#include "global.h"
#include <cassert>
#include <iomanip>
#include "Verify.hxx"


TestStatistics* TestStatistics::myInstance = 0;

TestStatistics::TestStatistics()
    : myPassed(0),
      myFailed(0),
      myBroken(0),
      myUnexpectedPass(0)
{
}

TestStatistics::~TestStatistics()
{
}

void
TestStatistics::passed()
{
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    myInstance->myPassed++;
}


void
TestStatistics::failed()
{
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    myInstance->myFailed++;
}


void
TestStatistics::broken()
{
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    myInstance->myBroken++;
}


void
TestStatistics::unexpectedPass()
{
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    myInstance->myUnexpectedPass++;
}


void
TestStatistics::covered(const char * filename, int line, const char * label)
{
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    TestStatistics::CoverageInfo coverageInfo(filename, line, label);
    
    set<CoverageInfo>::const_iterator i 
        = myInstance->myCovered.find(coverageInfo);
    if ( i == myInstance->myCovered.end() )
    {
        myInstance->myCovered.insert(coverageInfo);
    }
}


int
TestStatistics::finish(const char* filename, int testcase_count)
{
    bool anyFailed = false;
    if(myInstance)
    {
	assert(filename);
	cerr << "VerifySummary:" << filename 
	     << ':' << testcase_count
	     << ':' << myInstance->myPassed
	     << ':' << myInstance->myFailed
	     << ':' << myInstance->myBroken
	     << ':' << myInstance->myUnexpectedPass
	     << "\n\n";

	cerr << "-------------------------------------------------------------"
	     << "---------\n";
	cerr << "Report for " << filename << "\n\n";

	bool brokenTest = false;

	if(
	    (myInstance->myPassed + myInstance->myFailed + 
	     myInstance->myBroken + myInstance->myUnexpectedPass)
	    != testcase_count)
	{
	    brokenTest = true;
	}

	cerr << "Summary: ";


	if(brokenTest)
	{
	    // the test is broken
	    cerr << "TEST BROKEN\n";
	}
	else if(testcase_count == myInstance->myPassed)
	{
	    // best of all possible -- everything is good
	    cerr << "PASS\n";
	}
	else if (myInstance->myFailed == 0)
	{
	    // everything's ok, but there are some broken entries
	    cerr << "PASS WITH KNOWN BUGS\n";
	}
	else if (myInstance->myFailed > 0)
	{
		cerr << "FAIL\n";
	}
	else 
	{
	    cerr << "VERIFY BUG\n";
	}
	cerr << '\n';

	cerr << "Total Number of Test Cases: " << testcase_count << '\n';
	cerr << "Number of Test Cases run: " 
	     << (myInstance->myPassed +
		 myInstance->myFailed +
		 myInstance->myBroken + 
		 myInstance->myUnexpectedPass) << "\n\n";

	cerr << "test_verify()\n";
	cerr << "    Expected:   " 
	     << myInstance->myPassed + myInstance->myFailed << '\n';
	cerr << "    Passes:     " << myInstance->myPassed << '\n';
	cerr << "    Fails:      " << myInstance->myFailed << '\n';
	cerr << '\n';
	cerr << "test_bug()\n";
	cerr << "    Expected:   " 
	     << myInstance->myBroken + myInstance->myUnexpectedPass
	     << '\n';
	cerr << "    Bugs:       " << myInstance->myBroken << '\n';
	cerr << "    Fixed Bugs: " << myInstance->myUnexpectedPass << '\n';


	cerr << "\n\n";
	if(brokenTest)
	{
	    cerr << "Help With Broken Tests:\n";

	    cerr << "The number of test cases and bugs specified in test_return_code() did not\n";
	    cerr << "match the number of times test_verify() and/or test_bug() were called.\n";
	    cerr << "To fix this, you should probably update the number of times you expect\n";
	    cerr << "test_verify() and/or test_bug() to be called in the arguments to\n";
	    cerr << "test_return_code().\n";
	}

	cerr << "============================================================"
	     << "==========\n";

        for (   set<CoverageInfo>::const_iterator i = myInstance->myCovered.begin();
                i != myInstance->myCovered.end();
                ++i
             )
        {
            cerr << i->file << ':' << i->line 
                 << ": covered label: " << i->label << endl;
        }
	delete myInstance; myInstance = 0;
    }
    if(anyFailed)
    {
	return -1;
    }
    else
    {
	return 0;
    }
}


#if 0
/** this class exists so that TestStatistics::finish() can be called
 * when the program finishes.  To do this, it calls it in the
 * destructor, and a global instance of the class is instantiated.
 */
class TestStatisticDestructorActivation
{
    public:
	TestStatisticDestructorActivation()
	{
	}

	~TestStatisticDestructorActivation()
	{
	    TestStatistics::finish(0);
	}
};

/// here is the global instance
static TestStatisticDestructorActivation junk;
#endif


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
