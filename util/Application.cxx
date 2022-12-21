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
#include "Application.hxx"
#include "VException.hxx"
#include <iostream>
#include <assert.h>

using Vocal::Application;
using Vocal::ApplicationCreator;
using Vocal::Runnable;
using Vocal::ReturnCode;


ApplicationCreator  *   Application::myCreator = 0;
Application  	    *   Application::myApplication = 0;


void
Application::creator(ApplicationCreator * creator_)
{
    myCreator = creator_;
}

Application &	    
Application::instance()
{
    if ( !myApplication )
    {
        if ( myCreator != 0 )
        {
    	    myApplication = myCreator->create();
        }
        else
        {
            throw "No ApplicationCreator";
        }
    }
    return ( *myApplication );
}

ReturnCode  	    
Application::main(int argc, char ** argv, char ** arge)
{
    // Create the application.
    //
    Application &   app = Application::instance();

    //
    // Could install new handler here.
    //
    
    // Initialize the application
    //
    ReturnCode	rc = SUCCESS;
    try
    {
        rc = app.init(argc, argv, arge);
    }
    catch ( VException & )
    {
    	cerr << "Unhandled VException from Application::init(). Exiting." << endl;
    	return ( 1 );
    }
    catch ( ... )
    {
    	cerr << "Unhandled exception from Application::init(). Exiting." << endl;
    	return ( 2 );
    }
    
    if ( rc != SUCCESS )
    {
    	return ( rc );
    }
    
    try
    {
    	rc = app.run();
    }
    catch ( VException & )
    {
    	cerr << "Unhandled VException from Application::run(). Exiting." << endl;
	return ( 1 );
    }
    catch ( ... )
    {
    	cerr << "Unhandled exception from Application::run(). Exiting." << endl;
    	return ( 2 );
    }

    // Cleanup the application
    //
    try
    {
    	app.uninit();
    }
    catch ( VException & )
    {
    	cerr << "Unhandled VException from Application::uninit(). Exiting." << endl;
	return ( 1 );
    }
    catch ( ... )
    {
    	cerr << "Unhandled exception from Application::uninit(). Exiting." << endl;
    	return ( 2 );
    }    

    // Delete the application
    //
    delete myApplication;
    myApplication = 0;
    
    return ( rc );    
}

	
Application::Application()
{
}


Application::~Application()
{
}


ReturnCode
Application::init(int argc, char ** argv, char ** arge)
{
    return ( SUCCESS );
}


void
Application::uninit()
{
}
