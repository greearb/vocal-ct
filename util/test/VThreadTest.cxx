
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
static const char* const VThreadTest_cxx_Version =
    "$Id: VThreadTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";


#include <signal.h>
#include <sys/types.h>
#include <typeinfo>
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include "VTime.hxx"
#include "VThread.hxx"
#include "VMutex.h"
#include "VCondition.h"
#include "cpLog.h"
#include "VThreadException.hxx"
#include <iostream>

VMutex *theMutex;
VCondition *theCondition;

/// displays 'name' strlen( name ) times
void *
display( void* name )
{
    static int first = 1;
    int length = strlen( ( char* ) name );

    cerr << "see if this works" << endl;
    theMutex->lock();
    if ( first )
    {
        first = 0;
        assert( theCondition->wait( theMutex ) == 0 );
        while ( length-- > 0 )
        {
            cpLog( LOG_ERR, "%s\n", ( char* ) name );
            sleep( 1 );
        }
    }
    else
    {
        while ( length-- > 0 )
        {
            cpLog( LOG_DEBUG, "%s\n", ( char* ) name );
            sleep( 1 );
        }
        assert( theCondition->signal() == 0 );
    }
    theMutex->unlock();
    return name;
}


void*
startThreads( void* name )
{
    // setup logfile
    cpLogSetLabel( "VThreadTest" );
    cpLogSetPriority( LOG_DEBUG );

    VThread* thread1;
    thread1 = new VThread();
    VThread thread2;

    char arg1[] = "first";
    char arg2[] = "second";

    theMutex = new VMutex;
    theCondition = new VCondition;

    // make sure cpLog() functionality is present
    cpLog( LOG_DEBUG, "Is cpLog() working???" );

    try
    {
        thread1->spawn( display, ( void* ) arg1 );
        // this should result in a cpLog warning
        //thread1->spawn( display, ( void* ) arg1 );
    }

    catch ( VThreadExceptionInvalidAttributes& ex )
    {
        cpLog( LOG_DEBUG, "handle VThreadExceptionInvalidAttributes" );
    }

    try
    {
        // for linux this should throw an exception which gets caught
        //thread2.spawn(display, ( void * ) arg2, 0, 175 );
        thread2.spawn(display, ( void * ) arg2 );
    }
    catch ( VThreadExceptionInvalidAttributes& ex )
    {
        cpLog( LOG_DEBUG, "handle VThreadExceptionInvalidAttributes" );
    }
    catch ( VThreadExceptionInvalidPriority& ex )
    {
        cpLog( LOG_DEBUG, "handle VThreadExceptionInvalidPriority" );
        thread2.spawn( display, ( void * ) arg2 );
    }

    printf( "thread1 priority: %d\n", thread1->getPriority() );
    printf( "thread2 priority: %d\n", thread2.getPriority() );
#ifdef __vxworks
    // include this if you want to change the order of execution
    //thread2.setPriority( 75 );
    //printf("thread2 new priority: %d\n", thread2.getPriority() );
#endif // __vxworks

    //sleep(10);
    //thread1->exit();
    //thread2.exit();
    thread1->join();
    printf("joined with thread1\n" );
    thread2.join();
    printf("joined with thread2\n" );

    delete theMutex; theMutex = 0;
    delete theCondition; theCondition = 0;
    delete thread1; thread1 = 0;
    return 0;
} // end main()


int
main( int argc, char* argv[] )
{
    VThread* mainThread;
    mainThread = new VThread();

    try
    {
        mainThread->spawn( startThreads );
    }
    catch ( VThreadExceptionInvalidAttributes& ex )
    {
        cpLog( LOG_DEBUG, "handle VThreadExceptionInvalidAttributes" );
    }

    mainThread->join();
    delete mainThread; mainThread = 0;
    return 0;
} // end main()
