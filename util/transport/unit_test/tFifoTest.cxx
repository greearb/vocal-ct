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


static const char* const tFifoTest_cxx_Version = 
  "$Id: tFifoTest.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Application.hxx"
#include "Runnable.hxx"
#include "Thread.hxx"
#include "Fifo.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <cassert>

using namespace Vocal;
using namespace Vocal::Threads;


static u_int32_t NUM_ITERATIONS = 1000;
static int MAX_TIMER_ERRORS = 10;

class FifoAdd : public Runnable
{
    private:
    
    	Fifo<TimeVal>	&   fifo_;
    	u_int32_t   	    uAddDelay_;
	milliseconds_t	    timerDelay_;
	u_int32_t   	    numIterations_;

    public:

    	FifoAdd(
	    Fifo<TimeVal>   	&   fifo, 
	    u_int32_t 	    	    uAddDelay, 
	    milliseconds_t  	    timerDelay,
	    u_int32_t	    	    numIterations = NUM_ITERATIONS
	)
	    : 	fifo_(fifo), uAddDelay_(uAddDelay), timerDelay_(timerDelay),
	    	numIterations_(numIterations)
	{
    	}
	
	~FifoAdd()
	{
    	}

    	ReturnCode  	    run();
};


ReturnCode
FifoAdd::run()
{
    TimeVal     timeNow;
    
    u_int32_t	count = 0;
    
    while ( count++ < numIterations_ ) 
    {
    	if ( uAddDelay_ > 0 )
	{
	    usleep(uAddDelay_);
	}

	fifo_.addDelayMs(timeNow.now(), timerDelay_);
    }

    return ( SUCCESS );    
}


class FifoRemove : public Runnable
{
    private:
    
    	Fifo<TimeVal>	&   fifo_;
	milliseconds_t	    timerDelay_;
	u_int32_t   	    numIterations_;

    public:

    	FifoRemove(
	    Fifo<TimeVal>   	&   fifo, 
	    milliseconds_t	    timerDelay,
	    u_int32_t	    	    numIterations = NUM_ITERATIONS
	)
	    : 	fifo_(fifo), timerDelay_(timerDelay), 
	    	numIterations_(numIterations)
	{
    	}
	
	~FifoRemove()
	{
    	}

    	ReturnCode  	    run();
};


ReturnCode
FifoRemove::run()
{
    TimeVal 	timeNow, timeThen, timeDiff;
    int     	numErrors = 0;

    u_int32_t	count = 0;
    
    while ( count++ < numIterations_ ) 
    {
    	timeThen = fifo_.getNext();

	timeNow.now();

	timeDiff = timeNow - timeThen;

	milliseconds_t	msDiff = timeDiff.milliseconds() - timerDelay_;
	
	if ( msDiff < 0 || msDiff > 30 )
	{
	    cout << "count: " << count << ", error: msDiff = " << msDiff << endl;
	    
	    if ( ++numErrors >= MAX_TIMER_ERRORS )
	    {
	    	exit(1);
	    }
	}
        
        if ( count % 1000 == 0 && numErrors > 0 )
        {
	    cout << "count: " << count << ", error discounted" << endl;

            numErrors--;
        }
    }
    return ( SUCCESS );
}


class TestApplication : public Application
{
    public:
    	ReturnCode  	init(int argc, char ** argv, char ** arge);
    	ReturnCode  	run();
	
    private:
    	u_int32_t   	    uAddDelay_;
	milliseconds_t	    timerDelay_;
    	u_int32_t   	    numIterations_;
};


class TestApplicationCreator : public ApplicationCreator
{
    Application * create() { return ( new TestApplication ); }
};


int main(int argc, char ** argv, char ** arge)
{
    TestApplicationCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}


ReturnCode  	
TestApplication::init(int argc, char ** argv, char ** arge)
{
    if ( argc > 2 )
    {
        uAddDelay_ = atoi(argv[1]);
        timerDelay_ = atoi(argv[2]);

        if ( argc > 3 )
        {
    	    numIterations_ = atoi(argv[3]);

	    if ( numIterations_ == 0 )
	    {
	        numIterations_ = NUM_ITERATIONS;
	    }
        }
        else
        {
            numIterations_ = NUM_ITERATIONS;
        }
    }
    else
    {
        uAddDelay_ = 1000;
        timerDelay_ = 20;
        numIterations_ = NUM_ITERATIONS;
    }

    
    cout << argv[0] << " initalized:"
    	 << "\n\tuAddDelay:     " << uAddDelay_ 
    	 << "\n\tmTimerDelay:   " << timerDelay_ 
	 << "\n\tnumIterations: " << numIterations_ << endl;
	 
    return ( SUCCESS );
}


ReturnCode TestApplication::run()
{
    Fifo<TimeVal>   	fifo;

    TimeVal             refTime, delay;
    
    refTime.now();

    cout << "refTime: " << refTime << endl;
    
    milliseconds_t delta = 1000;
    
    for ( int i = 0; i < 20; i++ )
    {
        delay = refTime + (delta * i);

        cout << "delay: " << delay << endl;
        
        fifo.addUntil(delay, delay);
    }

    size_t  size = fifo.size();
            
    for ( size_t i = 0; i < size; i++ )
    {
        TimeVal     rightNow;
        
        TimeVal     msg = fifo.getNext();
        
        cout << "Now: " << rightNow.now() << " Expiry: " << msg 
             << " : " << setw(2) << fifo.size() 
             << " (" << setw(2) << fifo.sizeAvailable()
             << "," << setw(2) << fifo.sizePending() << ")" << endl;
    }
    
    assert( fifo.size() == 0 );


    FifoAdd 	    	fifoAdd(fifo, uAddDelay_, timerDelay_, numIterations_);
    FifoRemove	    	fifoRemove(fifo, timerDelay_, numIterations_);
    Thread  	    	fifoRemoveThread(fifoRemove);
    Thread  	    	fifoAddThread(fifoAdd);
    fifoAddThread.join();
    fifoRemoveThread.join();

    return ( SUCCESS );
}
