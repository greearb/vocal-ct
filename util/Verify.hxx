#ifndef VERIFY_HXX_
#define VERIFY_HXX_

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

static const char* const Verify_hxx_Version =
    "$Id: Verify.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "global.h"
#include <iostream>
#include <set>
#include <string>

using std::set;
using std::string;
namespace Vocal {
    namespace Threads {
        class Mutex;
    }
}
using Vocal::Threads::Mutex;

/* Usage:
 *
 * test_verify(test_condition);
 *
 * where test_condition is a boolean condition (like in assert).
 * Unlike assert, test_verify does not stop execution.
 * 
 * 
 *
 * Here is a sample program:
 *
 * #include "Verify.hxx"
 * int main()
 * {
 *     test_verify(0 == 0);  // this should pass
 *     test_verify(1 == 0);  // this should fail
 *     return test_return_code(2);
 * }
 *
 * the output of this program:
 *
 * VerifyTestShort.cxx:5: test case '1 == 0' failed
 * VerifySummary:VerifyTestShort.cxx:2:1:1:0:0
 *
 * ----------------------------------------------------------------------
 * Report for VerifyTestShort.cxx
 * 
 * Summary: FAIL
 * 
 * Total Number of Test Cases: 2
 * Number of Test Cases run: 2
 * 
 * test_verify()
 *     Expected:   2
 *     Passes:     1
 *     Fails:      1
 * 
 * test_bug()
 *     Expected:   0
 *     Bugs:       0
 *     Fixed Bugs: 0
 * 
 * 
 * ======================================================================
 */

/// singleton class used to count the number of test cases that pass / fail
class TestStatistics
{
    public:
	/// called when a test (see below) passes
	static void passed();
	/// called when a test (see below) fails
	static void failed();
	/// called when a test (see below) is broken
	static void broken();
	/// called when a test (see below) is broken
	static void unexpectedPass();
        /// called when a coverage point is called
        static void covered(const char * filename, 
			    int line, 
			    const char * label);

    /** called by a global object that is constructed in Verify.cxx on
        finish to output a summary of results */

	static int finish(const char* filename, int testcase_count);
    protected:
	TestStatistics();
	TestStatistics(const TestStatistics&);
	~TestStatistics();

    private:
	const TestStatistics & operator=(const TestStatistics&);
	static TestStatistics * myInstance;
	static Mutex * myMutex;
	int myPassed;
	int myFailed;
	int myBroken;
	int myUnexpectedPass;

        struct CoverageInfo
        {
            CoverageInfo(const char * pFile, int pLine, const char * pLabel)
                :   file(pFile), line(pLine), label(pLabel ? pLabel : "")
            {}
            
            string      file;
            int         line;
            string      label;
            
            bool operator<(const CoverageInfo & src) const
            {
                return ( file == src.file ? line < src.line : file < src.file );
            }
        };
        set<CoverageInfo>   myCovered;
};

/** the following is a macro which implements the test system.  
 * Usage:
 *     test_verify(condition_to_verify);
 */

#define test_verify(x) \
    do { \
         if(!(x)) \
         { \
               cerr << __FILE__ << ':' << __LINE__ << ": test case '" << \
            #x << "' failed" << endl;  \
            TestStatistics::failed(); \
         } \
         else \
		  TestStatistics::passed(); \
       } while(0)

/** TODO -- replace this with support for the rest of the system to
 * understand the concept of brokenness.
 */

#define test_bug(x) \
    do { \
         if((x)) \
         { \
               cerr << __FILE__ << ':' << __LINE__ << ": test case '" << \
            #x << "' unexpected pass (passed when fail expected)" << endl;  \
            TestStatistics::unexpectedPass(); \
         } \
         else \
		  TestStatistics::broken(); \
       } while(0)

/** the following is a macro which allow a programmer to insert coverage points
 */
 
#if defined(VOCAL_TEST_COVERAGE)
    #define test_covered(x) TestStatistics::covered(__FILE__, __LINE__, (x))
#else
    #define test_covered(x)
#endif

/** the following is a macro which implements the test system.  
 * Usage:
 *     test_return_code(testcase_count) 
 */

#define test_return_code(testcase_count) \
    TestStatistics::finish(__FILE__, (testcase_count))

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
