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

static const char* const DataVerify_cxx_Version =
    "$Id: DataVerify.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include "Data.hxx"
#include "Verify.hxx"
#include <vector>
//#include "CopyOnWriteData.hxx"
//#include "StringData.hxx"

template <class DataClass>
void test()
{
    {
	DataClass xyz("AllCaps");
	
	xyz.uppercase();
	test_verify(xyz == "ALLCAPS");
	
	xyz.lowercase();
	test_verify(xyz == "allcaps");
    }

    {
	DataClass xyz("allcaps");
	test_verify("allcaps" == xyz);
	test_verify("allCaps" != xyz);
    }

    /// test parseOutsideQuotes

    {
	DataClass test("\"Ogawa, Bryan\" <sip:bko@cisco.com,foo,bar,city>,\"Doe, John\" <sip:johnd@test.com,,>,Test Person <sip:test@test.com>");

	bool fail;
	DataClass a = test.parseOutsideQuotes(",", true, true, &fail);
	test_verify(fail == false);

	DataClass b = test.parseOutsideQuotes(",", true, true, &fail);
	test_verify(a == "\"Ogawa, Bryan\" <sip:bko@cisco.com,foo,bar,city>");
	test_verify(b == "\"Doe, John\" <sip:johnd@test.com,,>");
	test_verify(test == "Test Person <sip:test@test.com>");
	test_verify(fail == false);
//	cout << a << endl;
//	cout << b << endl;
    }

    {
	DataClass test("\",,,\" <,,,,>,\",,\" <,,>,\",,,\"<,,,>");

	bool fail;
	DataClass a = test.parseOutsideQuotes(",", true, false, &fail);
	test_verify(a == "\",,,\" <");

	test = "<,,,,> \",,\" <,,>,\",,,\"<,,,>";
	DataClass b = test.parseOutsideQuotes(",", false, true, &fail);
	test_verify(b == "<,,,,> \"");

	test = "<<,<,<,,,<,<,<,   ,,,, , ,  >,>  ";
	DataClass c = test.parseOutsideQuotes(",", false, true, &fail);
	test_verify(c == "<<,<,<,,,<,<,<,   ,,,, , ,  >");
    }

    /// test parse
    {
	bool fail;
	DataClass test = "this is a simple test";
	DataClass a = test.parse(" ", &fail);
	test_verify(fail == false);
	test_verify(a == "this");

	DataClass b = test.parse(" ", &fail);
	test_verify(fail == false);
	test_verify(b == "is");

	DataClass c = test.parse(" ", &fail);
	test_verify(fail == false);
	test_verify(c == "a");

	DataClass d = test.parse(" ", &fail);
	test_verify(fail == false);
	test_verify(d == "simple");

	DataClass e = test.parse(" ", &fail);
	test_verify(fail == true);
	test_verify(test == "test");
    }

    // more tests of parse
    {
	bool fail;
	DataClass test = "this is a simple test";

	{
	    DataClass a = test.parse("sal", &fail);
	    test_verify(fail == false);
	    test_verify(a == "thi");
	}

	{
	    DataClass a = test.parse("sal", &fail);
	    test_verify(fail == false);
	    test_verify(a == " i");
	}

	{
	    DataClass a = test.parse("sal", &fail);
	    test_verify(fail == false);
	    test_verify(a == " ");
	}

	{
	    DataClass a = test.parse("sal", &fail);
	    test_verify(fail == false);
	    test_verify(a == " ");
	}

	{
	    DataClass a = test.parse("sal", &fail);
	    test_verify(fail == false);
	    test_verify(a == "imp");
	}

	{
	    DataClass a = test.parse("sal", &fail);
	    test_verify(fail == false);
	    test_verify(a == "e te");
	}

	{
	    DataClass a = test.parse("sal", &fail);
	    test_verify(fail == true);
	    test_verify(test == "t");
	}
    }

    {
	DataClass test = "From:    testItem";
	bool fail;
	DataClass a = test.parse(": ", &fail);
	test_verify(fail == false);
	test_verify(a == "From");
	test_verify(test == "testItem");
    }


    {
	DataClass test;

	test = DataClass("a") + DataClass("b");
	test_verify(test == "ab");

	DataClass second = "Ogawa";

	test = DataClass("Bryan ") + second;
	test_verify(test == "Bryan Ogawa");

        DataClass number = "7";    
        DataClass result;
            
        result = DataClass("Wigets: ") + number;
	test_verify(result == "Wigets: 7");
    }

    {
	DataClass test0 = "random junk asdf this is a test";
	DataClass test1;
	int retn = test0.match("asdf", &test1, true, "hello");

	test_verify(retn == FOUND);
	
	test_verify (test0 == "hello this is a test");
	test_verify (test1 == "random junk ");
    }

    {
	DataClass test0 = "random junk asdf asdf this is a test";
	DataClass test1;
	int retn = test0.match("asdf", &test1, true, "hello");

	test_verify(retn == FOUND);
	
	test_verify (test0 == "hello asdf this is a test");
	test_verify (test1 == "random junk ");
    }

    {
	DataClass test0 = "random junk asdf asdf this is a test";
	DataClass test1;
	int retn = test0.match("asdf", &test1, false, "");

	test_verify(retn == FOUND);
	
	test_verify (test0 == "random junk asdf asdf this is a test");
	test_verify (test1 == "random junk ");
    }

    {
	DataClass msg1("From: test1@test.com\r\nTo: test2@test2.com\r\nCall-ID: f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com\r\n");
	DataClass msg2("From: test1@test.com\r\nTo: test2@test2.com\r\nCall-ID: e81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com\r\n");

	test_verify( !(msg1 < msg2) );
    }

    {
	DataClass msg1("hello");
	DataClass msg2("Hello");

	test_verify ( isEqualNoCase(msg1, msg2) == true );

	DataClass msg3("HeLLo");
	DataClass msg4("hEllO");

	test_verify ( isEqualNoCase(msg3, msg4) == true ) ;

	DataClass msg5("HeLLo");
	DataClass msg6("hEllOo");

	test_verify ( isEqualNoCase(msg5, msg6) == false );

	DataClass msg7("HeLLoo");
	DataClass msg8("hEllO");

	test_verify ( isEqualNoCase(msg7, msg8) == false );
    }
    {
	DataClass one = "data one";
	DataClass two = "data two";

	DataClass three = one + two;
	test_verify (three == "data onedata two");
    }
    {
	DataClass one = "data one";
	DataClass two = "data two";

	one += two;
	test_verify (one == "data onedata two");
    }
    {
	DataClass one = "abcd";
	DataClass two = one;
	
	two.setchar(0, 'z');
	two.setchar(1, 'y');
	two.setchar(2, 'x');
	two.setchar(3, 'w');
	test_verify ( (one == "abcd") && (two == "zyxw"));
    }
    {
	DataClass junk;
	test_verify(junk == "");
    }
    {
	DataClass junk = Data(7);
	test_verify(junk == "7");
    }
    {
	DataClass junk;
	junk = Data(7);
	test_verify (junk == "7");
    }
    {
	DataClass junk;
	junk = "Content-Length: ";
	junk += Data(7);
	test_verify (junk == "Content-Length: 7");
    }
    {
	DataClass junk = "asnf abcd";
	DataClass rest;
	
	test_verify (junk.match("d", &rest, true) == FOUND);
	test_verify (rest == "asnf abc");
	test_verify (junk == "");
    }
    {
	DataClass test0 = "1024";
	DataClass test1 = "1000x1000";
	DataClass test2 = " 1000";
	DataClass test3 = "1000 1000";
	DataClass test4 = "19238471832758715289725371";
	DataClass test5 = "4000@";

	test_verify(test0.convertInt() == 1024);

	test_verify(test1.convertInt() == 1000);

	test_verify (test2.convertInt() == 1000);

	test_verify(test3.convertInt() == 1000);

	test_verify (test5.convertInt() == 4000);
    }
    {
	DataClass j = "asnf abcd";
	string s = j.convertString();
	test_verify (DataClass(s) == j);
    }

    {
	const int MAX_TESTS = 25;
	
	DataClass data[MAX_TESTS];

	data[0] = "Testing Data";
	test_verify (strcmp(data[0].logData(), "Testing Data") == 0);


	data[1] = data[0];
	test_verify (strcmp(data[1].logData(), "Testing Data") == 0);

	data[2] = "Testing Data";
	data[2] += "added data";

	test_verify (strcmp(data[2].logData(), "Testing Dataadded data") == 0);

	data[3] = data[1] + data[2];

	test_verify (data[3] == "Testing DataTesting Dataadded data");

	string str("This is a string");
	DataClass data4(str);

	test_verify (data4 == "This is a string");

	//	mstring mstr("This is a mstring");
	//	DataClass data5(mstr);

	//	test_verify (data5 == "This is a mstring");

	int value = 5;
	DataClass data6(value);

	test_verify (data6 == "5");

	DataClass testData;
	testData = "alphb";
	
	data[7] = "alpha";
	
	test_verify ((data[7] < testData) &&
		     !(data[7] == testData) &&
		     !(data[7] > testData) &&
		     (data[7] != testData));

	data[8] = "alphc";
	test_verify ((data[8] > testData) &&
		     !(data[8] == testData) &&
		     !(data[8] < testData) &&
		     (data[8] != testData));

	data[9] = "alphb";
	test_verify ((data[9] == testData) &&
		     !(data[9] < testData) &&
		     !(data[9] > testData) &&
		     !(data[9] != testData));

	data[10] = "alphbx";
	
	test_verify ((data[10] > testData) &&
		     !(data[10] < testData) &&
		     !(data[10] == testData) &&
		     (data[10] != testData));

	data[11] = "hello, world!";
	
	char newstr[256];
	strcpy(newstr, data[11].logData());
	
	test_verify (
	    (strcmp(newstr, "hello, world!") == 0) && (data[11].length() == 13));
    }
    {
	DataClass test = "Hello, world!";
	DataClass test2 = "Hello";

	test_verify ( test > DataClass("Hello"));
	test_verify ( test > test2);

	test_verify ( DataClass("Hello") < test);
	test_verify ( test2 < test );
    }
    {
	// test lowercase() and uppercase()

	DataClass test = "Hello, World!";

	test.lowercase();

	test_verify(test == "hello, world!");
	test_verify(test != "Hello, World!");

	DataClass test2 = "Hello, World!";
	test2.uppercase();
	test_verify(test2 == "HELLO, WORLD!");
	test_verify(test2 != "Hello, World!");

    }

    /// test matchChar
    {
	char fail;
	DataClass test = "this is a simple test";

	{
	    DataClass a = test.matchChar("sal", &fail);
	    test_verify(fail == 's');
	    test_verify(a == "thi");
	}

	{
	    DataClass a = test.matchChar("sal", &fail);
	    test_verify(fail == 's');
	    test_verify(a == " i");
	}

	{
	    DataClass a = test.matchChar("sal", &fail);
	    test_verify(fail == 'a');
	    test_verify(a == " ");
	}

	{
	    DataClass a = test.matchChar("sal", &fail);
	    test_verify(fail == 's');
	    test_verify(a == " ");
	}

	{
	    DataClass a = test.matchChar("sal", &fail);
	    test_verify(fail == 'l');
	    test_verify(a == "imp");
	}

	{
	    DataClass a = test.matchChar("sal", &fail);
	    test_verify(fail == 's');
	    test_verify(a == "e te");
	}

	{
	    DataClass a = test.matchChar("sal", &fail);
	    test_verify(fail == '\0');
	    test_verify(test == "t");
	}
    }


    {
	DataClass test = "From:    testItem";
	char fail;
	DataClass a = test.matchChar(": ", &fail);
	test_verify(fail == ':');
	test_verify(a == "From");
	test_verify(test == "    testItem");
    }


    // test getLine
    {
	DataClass test = "test\ntest\r\ntest\r\n";

	{
	    bool fail;
	    DataClass a = test.getLine(&fail);
	    test_verify(fail == false);
	    test_verify(a == "test");
	}

	{
	    bool fail;
	    DataClass a = test.getLine(&fail);
	    test_verify(fail == false);
	    test_verify(a == "test");
	}

	{
	    bool fail;
	    DataClass a = test.getLine(&fail);
	    test_verify(fail == false);
	    test_verify(a == "test");
	}

	{
	    bool fail;
	    DataClass a = test.getLine(&fail);
	    test_verify(fail == true);
	}

	DataClass test2 = "test\r\n\r\ntest\r\n";
	{
	    bool fail;
	    DataClass a = test2.getLine(&fail);
	    test_verify(fail == false);
	    test_verify(a == "test");
	}

	{
	    bool fail;
	    DataClass a = test2.getLine(&fail);
	    test_verify(fail == false);
	    test_verify(a == "");
	}

	{
	    bool fail;
	    DataClass a = test2.getLine(&fail);
	    test_verify(fail == false);
	    test_verify(a == "test");
	}
    }

    // test removeLWS
    {
	DataClass test = "From: test\r\n   continuation\r\n test2\r\n";
	test.removeLWS();
	test_verify(test == "From: test     continuation   test2\r\n");
	cout << test << endl;
    }
    // test substring
    {
	DataClass test = "abcdefg";
	test_verify(test == test.substring(0,-1));
	test_verify(test.substring(0,3) == "abc");
	test_verify(test.substring(0,0) == "");
	test_verify(test.substring(1,-1) == "bcdefg");
	test_verify(test.substring(1,2) == "b");
	test_verify(test.substring(1,1) == "");
	test_verify(test.substring(1,4) == "bcd");
	cout << test.substring(0,test.length() - 1) << endl;
	test_verify(test.substring(0,test.length()) == test);
	test_verify(test.substring(1,test.length() - 1) == "bcdef");
    }

#if 0
    {
      Data buf = "1000,1001,\n";
      std::vector <Data> retList;

      bool matchFail = false;
      Data tok = buf.parse(",\n ", &matchFail);
      
      while (!matchFail)
	{
	  retList.push_back(tok);
	  tok = buf.parse(",\n ", &matchFail);
	}
      
      if(matchFail)
	{
	  retList.push_back(buf);
	  buf.erase();
	}
      test_verify(retList[0] == "1000");
      test_verify(retList[1] == "1001");
    }
#endif
    {
      DataClass buf = "1000,1001";
      bool matchFail;
      DataClass tok = buf.parse(",\n ", &matchFail);

      test_verify(tok == "1000");
      test_verify(buf == "1001");
    }

    {
      DataClass buf = "1000  ";
      bool matchFail;
      Data tok = buf.parse(",\n ", &matchFail);

      test_verify(tok == "1000");
      test_verify(buf == "");
    }

    {
      Data buf = "1000,1001,\n";
      bool matchFail;
      Data tok = buf.parse(",\n ", &matchFail);
      
      test_verify(tok == "1000");
      test_verify(buf == "1001,\n");

      tok = buf.parse(",\n ", &matchFail);
      test_verify(tok == "1001");
      test_verify(buf == "");

      tok = buf.parse(",\n ", &matchFail);
      test_verify(matchFail == true);
      test_verify(buf == "");
    }
}


int main()
{
    cerr << "Data!" << endl;
    test<Data>();

#if 0
    cerr << "CopyOnWriteData!" << endl;
    test<CopyOnWriteData>();
    cerr << "StringData!" << endl;
    test<StringData>();
#endif
    return test_return_code(141);
}

