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


static const char* const tPipe_cxx_Version = 
    "$Id: tPipe.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Application.hxx"
#include "Pipe.hxx"
#include "VLog.hxx"
#include <cstring>
#include <iomanip>


using namespace Vocal;
using namespace Vocal::IO;
using namespace Vocal::Logging;


class TestApplication : public Application
{
    public:
    	ReturnCode  	init(int argc, char ** argv, char ** arge);
	void	    	uninit();
    	ReturnCode  	run();
};

class TestApplicationCreator : public ApplicationCreator
{
    Application * create() { return ( new TestApplication ); }
};


ReturnCode  
TestApplication::init(int argc, char ** argv, char ** arge)
{
    VLog::init(LOG_VERBOSE);
//    VLog::on(LOG_INFO);
//    VLog::on(LOG_DEBUG);
//    VLog::on(LOG_TRACE);
//    VLog::on(LOG_VERBOSE);
    
    return ( SUCCESS );
}


void
TestApplication::uninit()
{
    VLog::uninit();
}


int main(int argc, char ** argv, char ** arge)
{
    TestApplicationCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}


ReturnCode TestApplication::run()
{
    Pipe    pipe;

    pipe.readFD().setNonblocking();
    
    {
	string  outMsg("0123456789");
	string  inMsg(outMsg.size(), '\0');

	int bytesWritten = pipe.writeFD().write(outMsg);

	cout << "bytesWritten = " << bytesWritten << endl;

	assert( bytesWritten == static_cast<int>(outMsg.size()) );

	int bytesRead = pipe.readFD().read(inMsg);

	cout << "bytesRead = " << bytesRead << endl;

	assert( bytesRead == static_cast<int>(bytesWritten) );

	cout << inMsg.c_str() << endl;
    }

    {
	char    outMsg[] = "0123456789";
	
	size_t	size = strlen(outMsg);
	
	char	*   inMsg = new char[size+1];

	int bytesWritten = pipe.writeFD().write(outMsg);

	cout << "bytesWritten = " << bytesWritten << endl;

	assert( bytesWritten == static_cast<int>(size) );

	int bytesRead = pipe.readFD().read(inMsg, size+1);

	cout << "bytesRead = " << bytesRead << endl;

	assert( bytesRead == static_cast<int>(bytesWritten) );

	cout << inMsg << endl;
        
        delete [] inMsg;
    }

    pipe.writeFD().write("This is a test, this is only a test.");
    bool    done = false;

    while ( !done )
    {
    	try
	{
	    char    c;
	    
	    pipe.readFD().read(&c, 1);
	    
	    cout << c << flush;
	}
	catch ( ... )
	{
	    cout << endl;
	    done = true;
	}
    }

    return ( SUCCESS );
}
