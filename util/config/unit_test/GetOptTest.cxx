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


static const char* const GetOptTest_cxx_Version = 
    "$Id: GetOptTest.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "Application.hxx"
#include "GetOpt.hxx"
#include <cassert>
#include <iostream>


using Vocal::Application;
using Vocal::Configuration::GetOpt;
using Vocal::Configuration::NameValueMap;
using Vocal::Configuration::NO_VALUE;
using Vocal::Configuration::VALUE_PRESENT;
using Vocal::Configuration::VALUE_OPTIONAL;
using Vocal::ReturnCode;
using Vocal::SUCCESS;
using std::cout;
using std::endl;
using std::list;
using std::string;

class TestApplication : public Application
{
    public:
        ReturnCode      init(int, char**, char**);
    	ReturnCode  	run();
};

Application *	Application::create()
{
    return ( new TestApplication );
}


ReturnCode  
TestApplication::init(int argc, char ** argv, char ** arge)
{
    GetOpt  options;
    
    ReturnCode  rc = SUCCESS;
    
    options.add(0, 'a');
    options.add(0, 'b');
    options.add("create", 'c', VALUE_PRESENT);
    options.add(0, 'd', VALUE_PRESENT);
    options.add(0, '0');
    options.add(0, '1');
    options.add(0, '2');
    options.add("add", 0, VALUE_PRESENT);
    options.add("append");
    options.add("delete", 0, VALUE_PRESENT);
    options.add("verbose");
    options.add("file", 0, VALUE_PRESENT);
    options.add("close", 0, VALUE_OPTIONAL);
    options.add(0, 'x', VALUE_OPTIONAL);
 
    rc = options.add("clo");
    
    assert( rc != SUCCESS );

    options.printError(true);
    
    rc = options.parse(argc, argv);
    
    cout << "Remaining argc: " << options.remainingArgc() << endl;
    
    const list<string> & remaining = options.remaining();
    
    for (   list<string>::const_iterator i = remaining.begin();
            i != remaining.end();
            ++i
        )
    {
        cout << *i << " , ";
    }
    cout << endl;

    NameValueMap   opts = options.options();
    
    for (   NameValueMap::const_iterator   j = opts.begin();
            j != opts.end();
            ++j
        )
    {
        cout << "name: " << j->first;
        
        if ( j->second.type != NO_VALUE )
        {
            cout << " value: " << j->second.value.back();
        }
        cout << endl;
    }

    return ( rc );
}


int main(int argc, char ** argv, char ** arge)
{
    return ( Application::main(argc, argv, arge) );
}


ReturnCode TestApplication::run()
{
    return ( SUCCESS );
}
