
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


static const char* const Datatest_cxx_Version =
    "$Id: Datatest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";


#include "global.h"
#include "mstring.hxx"
#include "cpLog.h"
#include "Data.hxx"
#include <iostream>


int main()
{
    const int MAX_TESTS = 25;

    Data data[MAX_TESTS];
    bool testPassed[MAX_TESTS];

    for (int i = 0; i < MAX_TESTS; i++)
    {
        testPassed[i] = false;
    }


    data[0] = "Testing Data";
    if (strcmp(data[0].getData(), "Testing Data") == 0)
    {
        testPassed[0] = true;
    }

    data[1] = data[0];
    if (strcmp(data[1].getData(), "Testing Data") == 0)
    {
        testPassed[1] = true;
    }

    data[2] = "Testing Data";
    data[2] += "added data";

    if (strcmp(data[2].getData(), "Testing Dataadded data") == 0)
    {
        testPassed[2] = true;
    }
    else
    {
        cerr << "test2: " << data[2].getData() << endl;
    }

    data[3] = data[1] + data[2];

    if (data[3] == "Testing DataTesting Dataadded data")
    {
        testPassed[3] = true;
    }

    string str("This is a string");
    Data data4(str);

    if (data4 == "This is a string")
    {
        testPassed[4] = true;
    }

    mstring mstr("This is a mstring");
    Data data5(mstr);

    if (data5 == "This is a mstring")
    {
        testPassed[5] = true;
    }

    int value = 5;
    Data data6(value);

    if (data6 == "5")
    {
        testPassed[6] = true;
    }

    Data testData;
    testData = "alphb";

    data[7] = "alpha";

    if ((data[7] < testData) &&
            !(data[7] == testData) &&
            !(data[7] > testData) &&
            (data[7] != testData))
    {
        testPassed[7] = true;
    }
    else
    {
        cerr << "test 7: " << endl;
        cerr << "x < testData : " << (data[7] < testData) << endl;
        cerr << "x > testData : " << (data[7] > testData) << endl;
        cerr << "x == testData : " << (data[7] == testData) << endl;
        cerr << "x != testData : " << (data[7] != testData) << endl;
    }

    data[8] = "alphc";
    if ((data[8] > testData) &&
            !(data[8] == testData) &&
            !(data[8] < testData) &&
            (data[8] != testData))
    {
        testPassed[8] = true;
    }
    else
    {
        cerr << "test 8: " << endl;
        cerr << "x < testData : " << (data[8] < testData) << endl;
        cerr << "x > testData : " << (data[8] > testData) << endl;
        cerr << "x == testData : " << (data[8] == testData) << endl;
        cerr << "x != testData : " << (data[8] != testData) << endl;
    }

    data[9] = "alphb";
    if ((data[9] == testData) &&
            !(data[9] < testData) &&
            !(data[9] > testData) &&
            !(data[9] != testData))
    {
        testPassed[9] = true;
    }
    else
    {
        cerr << "test 9: " << endl;
        cerr << "x < testData : " << (data[9] < testData) << endl;
        cerr << "x > testData : " << (data[9] > testData) << endl;
        cerr << "x == testData : " << (data[9] == testData) << endl;
        cerr << "x != testData : " << (data[9] != testData) << endl;
    }

    data[10] = "alphbx";

    if ((data[10] > testData) &&
            !(data[10] < testData) &&
            !(data[10] == testData) &&
            (data[10] != testData))
    {
        testPassed[10] = true;
    }
    else
    {
        cerr << "test 10: " << endl;
        cerr << "x < testData : " << (data[10] < testData) << endl;
        cerr << "x > testData : " << (data[10] > testData) << endl;
        cerr << "x == testData : " << (data[10] == testData) << endl;
        cerr << "x != testData : " << (data[10] != testData) << endl;
    }

    data[11] = "hello, world!";

    char newstr[256];
    strcpy(newstr, data[11].getData());

    if ((strcmp(newstr, "hello, world!") == 0) && (data[11].length() == 13))
    {
        testPassed[11] = true;
    }

    int myMax = 11;

    int allPassed = 0;

    cout << "START test program: " << __FILE__ << endl;

    for (int j = 0; j <= myMax; j++)
    {
        if (testPassed[j])
        {
            cout << "test " << j << " passed." << endl;
        }
        else
        {
            cout << "test " << j << " FAILED." << endl;
            allPassed = -1;
        }
    }

    cout << "END test program: " << __FILE__ << endl;

    return allPassed;
}
