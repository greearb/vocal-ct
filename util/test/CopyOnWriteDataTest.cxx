
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
static const char* const CopyOnWriteDataTest_cxx_Version =
    "$Id: CopyOnWriteDataTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include "CopyOnWriteData.hxx"
#include "Verify.hxx"


void  testFind()
{
    CopyOnWriteData test("asdf");

    test_verify(test.find("f") == 3);
}
    
    
void  testReplace()
{
    CopyOnWriteData test("asdf");
    CopyOnWriteData test1("asdf");
    CopyOnWriteData test2("asdf");
    CopyOnWriteData test3("asdf");
    CopyOnWriteData test4("asdf");
    CopyOnWriteData test5("asdf");
    CopyOnWriteData test6("asdf");
    CopyOnWriteData test7("asdf");
    CopyOnWriteData test8("asdf");
    CopyOnWriteData test9("asdf");
    
#if 0    
    test.replace(1,2,"e");
    cout << test.getData() << endl;
    test_verify(test == "aef");
    
    test1.replace(0,4,"e");
    cout << test1.getData() << endl;
    test_verify(test1 == "e");

    test2.replace(0,5,"e");
    cout << test2.getData() << endl;
    test_verify(test2 == "e");
    
    test3.replace(1,100,"e");
    cout << test3.getData() << endl;
    test_verify(test3 == "ae");
    
    test4.replace(3,100,"e");
    cout << test4.getData() << endl;
    test_verify(test4 == "asde");
    
    test5.replace(3,100,"lmnopqr");
    cout << test5.getData() << endl;
    test_verify(test5 == "asdlmnopqr");
    
    test6.replace(0,100,"lmnopqr");
    cout << test6.getData() << endl;
    test_verify(test6 == "lmnopqr");
    
    test7.replace(0,1,"lmnopqr");
    cout << test7.getData() << endl;
    test_verify(test7 == "lmnopqrsdf");
    
    test8.replace(1,1,"lmnopqr");
    cout << test8.getData() << endl;
    test_verify(test8 == "almnopqrdf");
    
    test9.replace(1,1,"q");
    cout << test9.getData() << endl;
    test_verify(test9 == "aqdf");
#endif    
#if 0  
    string str("asdf");
    str.replace(0,1,"lmnopqr");
    cout << "string:" << str.c_str() << endl;
#endif

    
}

int main()
{

    testFind();
    testReplace();
    return test_return_code(11);
}
