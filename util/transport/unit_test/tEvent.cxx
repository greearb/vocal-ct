
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
static const char* const tEvent_cxx_Version = 
"$Id: tEvent.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Application.hxx"
#include "EventObserver.hxx"
#include "EventSubject.hxx"
#include <string>
#include <iostream>


using namespace Vocal;
using namespace Vocal::Behavioral;


class Policy : public EventObserver<int>
{
    public:

    	Policy(
	    EventSubject<int> 	&   es, 
	    int 	    	    interest,
	    const string    	&   name
	) 
	    : 	EventObserver<int>(es),
	    	interest_(interest),
		name_(name)
	{
	}

    	~Policy() 
	{
	}

    	bool	interestedIn(const int & event) 
	{ 
	    return ( event < interest_ ); 
	}

    	void	onEvent(int event)
	{
	    cout << *this << ", Received event: " << event << endl;
	}

	ostream &           writeTo(ostream & out) const
	{
	    return ( out << name_ << ", interest = " << interest_ );
	}
	
    private:
    
    	int 	    	interest_;
	string	    	name_;
	
};

class tEvent : public Application
{
    public:
    	tEvent() 
	{
	}
	
	~tEvent() 
	{
	}

    	ReturnCode  	    run();	
};

class tEventCreator : public ApplicationCreator
{
    Application * create() { return ( new tEvent ); }
};

ReturnCode
tEvent::run()
{
    EventSubject<int>	    subject;
    
    Policy  	    	    policy0(subject, 10, "Policy0"),
    	    	    	    policy1(subject, 20, "Policy1"),
    	    	    	    policy2(subject, 50, "Policy2");
			    
    subject.attach(policy0);
    subject.attach(policy1);
    subject.attach(policy2);

    cout << subject << endl;

    subject.setEvent(1);
    subject.setEvent(11);
    subject.setEvent(21);

    subject.detach(policy2);

    cout << subject << endl;

    subject.setEvent(2);
    subject.setEvent(12);
    subject.setEvent(22);

    subject.detach(policy1);

    cout << subject << endl;

    subject.setEvent(3);
    subject.setEvent(13);
    subject.setEvent(23);

    subject.detach(policy0);

    cout << subject << endl;

    subject.setEvent(4);
    subject.setEvent(14);
    subject.setEvent(24);

    return ( SUCCESS );
}

int main(int argc, char ** argv, char ** arge)
{
    tEventCreator   creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
