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


static const char* const RWMutexTest_cxx_Version = 
    "$Id: tRWMutex.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Application.hxx"
#include "Runnable.hxx"
#include "VRWLock.hxx"
#include "VCondition.h"
#include "Thread.hxx"
#include "SignalHandler.hxx"
#include "VLog.hxx"


using namespace Vocal;
using namespace Vocal::Threads;
using namespace Vocal::Logging;

class DataReader : public Runnable
{
    public:
    
        DataReader(VRWLock & lock, int & data, int & readcount)
            :   myLock(lock), myData(data), myReadCount(readcount)
        {
        }
        
        ReturnCode  run();
    
    private:
    
        VRWLock &   myLock;
        int     &   myData;
        int     &   myReadCount;
};


ReturnCode
DataReader::run()
{
    while ( true )
    {
        myLock.ReadLock();

        myReadCount++;

        cout << Thread::selfId() << ": read locked" << endl;

        if ( myData != 0 )
        {
            assert( 0 );
        }
        
        
        while ( myReadCount != 4 )
        {
            vusleep(1000000);
        }
        
        if ( myReadCount == 4 )
        {
            vusleep(1000000);
        }

        myReadCount--;
        
        cout << Thread::selfId() << ": read unlocked" << endl;
        myLock.Unlock();
    }
    return ( SUCCESS );
}


class DataWriter : public Runnable
{
    public:
    
        DataWriter(VRWLock & lock, int & data)
            :   myLock(lock), myData(data)
        {
        }

        ReturnCode  run();
        
    private:
    
        VRWLock &   myLock;
        int     &   myData;
};

ReturnCode
DataWriter::run()
{
    while ( true )
    {
        myLock.WriteLock();
        cout << Thread::selfId() << ": write locked" << endl;

        for ( int i = 0; i < 5; i++ )
        {
            myData = ((myData + 1) % 5);

            vusleep(200000);    

            assert( myData == ((i+1)%5) );
            
            cout << myData << endl;
        }

        cout << Thread::selfId() << ": write unlocked" << endl;
        myLock.Unlock();    
        
        vusleep(1500000);
    }
    return ( SUCCESS );
}


class RWMutexTest : public Application
{
    public:
    
        RWMutexTest();
                     
        virtual ~RWMutexTest() {}
        
        ReturnCode  init(int, char **, char **);
        void        uninit();
        
        ReturnCode  run();

    private:
    
        VRWLock         myLock;
        int             myData;
        int             myReadCount;
        
        DataReader      r1, r2, r3, r4;
        DataWriter      w1, w2;
        
        Thread      * read1, * read2, *read3, *read4,
                    * write1, * write2;
};


RWMutexTest::RWMutexTest()
    :   myLock(),
        myData(0),
        myReadCount(0),
        r1(myLock, myData, myReadCount),
        r2(myLock, myData, myReadCount),
        r3(myLock, myData, myReadCount),
        r4(myLock, myData, myReadCount),
        w1(myLock, myData),
        w2(myLock, myData),
        read1(0),
        read2(0),
        read3(0),
        read4(0),
        write1(0),
        write2(0)
{
}


ReturnCode
RWMutexTest::init(int, char **, char **)
{
    Thread::init();
    SignalHandler::init();
    VLog::init(LOG_DEBUG);
    
    read1 = new Thread(r1);
    read2 = new Thread(r2);
    read3 = new Thread(r3);
    read4 = new Thread(r4);
    write1 = new Thread(w1);
    write2 = new Thread(w2);
    
    return ( SUCCESS );
}


ReturnCode
RWMutexTest::run()
{
    return ( SUCCESS );
}


void
RWMutexTest::uninit()
{
    write2->join();
    write1->join();
    read4->join();
    read3->join();
    read2->join();
    read1->join();
}


class RWMutexTestCreator : public ApplicationCreator
{
    Application * create() { return ( new RWMutexTest ); }
};


int main(int argc, char ** argv, char ** arge)
{
    RWMutexTestCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}

