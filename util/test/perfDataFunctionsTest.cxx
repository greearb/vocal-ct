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
static const char* const DataTest_cxx_Version =
    "$Id: perfDataFunctionsTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include "Data.hxx"
#include "CWBuffer.hxx"
#include "DurationStatistic.hxx"
#include "PerformanceDb.hxx"
   
using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::StatisticsDb;

    
#define NONE 0
#define MATCH 1
#define MATCHREPLACE 11
#define EQUALOPERATOR 2
#define REMOVESPACES 3
#define LENGTH 4
#define EMPTYCONSTRUCTOR 5
#define TRUNCATE 6
#define NOTEQUAL 7
#define REPLACE 8
#define REMOVELWS 9
#define EXPAND 10       

//testing functions of Data, which gets called very frequently.
//initially, considering all functions, called by SipVia, decoding.
    
int main( int argc, char* argv[] )
{
    if ( argc < 2 )
    {
        cerr << "Usage: "
        << argv[0]
	<< " <#timesToRunTest>"
	<< " <FunctionToTest: Enter 1 - match, 2 - operator =, 3 - removeSpaces, 4 - length, 5 - empty Constructor, 6 - Truncate , 7 - operator !=> 8 - Replace , 9 - RemoveLWS, 10 - Expand , 11- MatchReplace" << endl;
        exit( -1 );
    }
    
    int numTimes = 1;
    
    int functionSelect = NONE;
    
    Data line(" SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.1 ");
    
    StatisticsDb db;
    
    DurationStatistic stat(db, Data("perfDataFunctionsTest"));
    
    float sum = 0;
    
    if (argc >= 2)
    {
	numTimes = atoi(argv[1]);
    }
    
    if (argc >= 3)
    {
        functionSelect = atoi(argv[2]); 
    }
    
    stat.start();
    for (int i = 0; i < numTimes; i++)
    {
	switch(functionSelect)
	{
	    case MATCH:
	    {
		Data viadata;
	        line.match("/", &viadata, true);
		//cout << "line after replace is: " << line.getData() << endl;
	    }
	    break;
            case MATCHREPLACE:
            {
                Data viadata;
                Data repl("ABCD");
                line.match("/", &viadata, true, repl);
                //        cout << "line after replace is: " << line.getData() << endl;
            }      
            break;
            case REMOVELWS:
            {
                line.removeLWS();
                //        cout << "line after replace is: " << line.getData() << endl;
            }     
	    case EQUALOPERATOR:
	    {
		Data line2;
    
	        line2 = line;
	    }
	    break;
	    case REMOVESPACES:
	    {
	        line.removeSpaces();
	    }
	    break;
	    case LENGTH:
	    {
	        line.length();
	    }
	    break;
	    case EMPTYCONSTRUCTOR:
	    {
	        Data emptyConstructor;
	    }
	    break;
	    case TRUNCATE:
	    {
		CWBuffer buff("abcdefgh", 8);
		buff.truncate(4, 0);
	    }
	    break;
	    case NOTEQUAL:
	    {
		Data line2("defghi");
		line != line2;
	    }
	    break;
            case REPLACE:
            {
                Data repLine("ABCDE");
//                line.replace(3, 4, repLine);
                //cout << "after replace: " << line.getData() << endl;
            }
           break;  
           case EXPAND:
            {
#if 0
                Data via2("SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.1");
                line+= ",";
                line+= via2;
                line.expand(Data line, Data(",") , Data("\r\n"), Data("\r\n"));
#endif
            }
            break;
	}
    }

    stat.stop();
    stat.record();
    sum+= stat.average();
    
    cout << "average ticks : " << sum/numTimes << endl;
    
    return 0;
}
