#if !defined(VOCAL_APPLICATION_HXX)
#define VOCAL_APPLICATION_HXX

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


static const char* const Application_hxx_Version = 
    "$Id: Application.hxx,v 1.2 2006/02/24 22:27:52 greear Exp $";


#include "Runnable.hxx"
#include "VocalCommon.hxx"
#include "GarbageCan.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


//using Vocal::GarbageCan;

class Application;

class ApplicationCreator {
public:
   virtual ~ApplicationCreator() { }
   virtual Application * create() = 0;
};

/** A singleton class which should be the entry point for a process.<br><br>
 *  
 *  @see    Vocal::Runnable
 */
class Application : public Vocal::Runnable
{
    public:

        /** Sets the function object to create the application
         */
        static void                 creator(ApplicationCreator *);
        
    	/** To get the singleton. Will create the Application the first
	 *  time called.
    	 */
    	static	Application &	    instance();


    	/** Should be called from the program's main(). It could
	 *  possibly look like:<br><br>
	 *  <code>
	 *  int main(int argc, char ** argv, char ** arge)<br>
	 *  {<ul>
	 *  	return ( Application::main(argc, argv, arge) );
	 *  </ul>
	 *  }</code>
	 *
	 *  This method creates the instance of the Application, 
	 *  initializes it, runs it, uninitializes it and destroys the
	 *  instance.
    	 */
	static	ReturnCode  	    main(int 	    argc, 
	    	    	    	    	 char 	**  argv, 
					 char 	**  arge);
	
    protected:

    	/** Default constructor. Avoid creating objects in the constructor.
	 *  Instead, create them dynamically in init and destroy them in
	 *  uninit.
    	 */
    	Application();
	
    public:

    	/** Virtual destructor
    	 */
	virtual ~Application();

    protected:

    	/** Initialize the application. The command line arguments are
	 *  passed unaltered.
    	 */
    	virtual ReturnCode  	    init(int 	    argc, 
	    	    	    	    	 char 	**  argv, 
					 char 	**  arge);
					 

    	/** Uninitialize the application.
    	 */
	virtual void		    uninit();

    private:

        static ApplicationCreator   *   myCreator;
    	static Application  	    *   myApplication;
        
        GarbageCan                      myGarbageCan;
};


} // namespace Vocal


//#include "deprecated/DeprecatedApplication.hxx"


#endif // !defined(VOCAL_APPLICATION_HXX)
