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


static const char* const Sptr_cxx_Version =
    "$Id: Sptr.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include "Sptr.hxx"
#ifdef __linux__
#include <execinfo.h>
#endif
#include <stdio.h>

#define SPTR_TRACEBACK_RUNNING 0
/// this C file is only used for debugging, so it is totally ifdef'ed

#define SPTR_DEBUG_ACTIVE 0

#if SPTR_DEBUG

// static item
SptrDebugger* SptrDebugger::impl_ = 0;

SptrDebugger::SptrDebugger()
        : ptrsInUse(),
        mutex()
{}


SptrDebugger* SptrDebugger::getInstance()
{
    if (!impl_)
    {
        impl_ = new SptrDebugger;
    }
    assert(impl_);
    return impl_;
}


void SptrDebugger::newPtr(void* ptr)
{
#if SPTR_DEBUG_ACTIVE
    if (ptr == 0)
    {
        // do nothing for the null ptr.
        return ;
    }

    PtrMap::iterator i;

    getInstance()->mutex.lock();
    i = getInstance()->ptrsInUse.find(ptr);
    if (i != getInstance()->ptrsInUse.end())
    {
        // check to see if it's set to inUse

        if (i->second)
        {
            cerr << "pointer " << ptr
            << " already in a different Sptr" << endl;
            assert(0);
        }
    }
    getInstance()->ptrsInUse[ptr] = true;
    getInstance()->mutex.unlock();
#endif
}


void SptrDebugger::deletePtr(void* ptr)
{
#if SPTR_DEBUG_ACTIVE
    PtrMap::iterator i;

    getInstance()->mutex.lock();
    i = getInstance()->ptrsInUse.find(ptr);
    if (i == getInstance()->ptrsInUse.end())
    {
        cerr << "pointer " << ptr
        << " is not in any Sptr" << endl;
        assert(0);
    }
    else
    {
        if (i->second == false)
        {
            cerr << "pointer " << ptr
            << " was already deleted by another Sptr" << endl;
            assert(0);
        }
        else
        {
            getInstance()->ptrsInUse[ptr] = false;
        }
    }
    getInstance()->mutex.unlock();
#endif
}

#endif


#if SPTR_TRACEBACK

#if SPTR_TRACEBACK_RUNNING


#ifdef __linux__

class SptrTraceback
{
    public:
        SptrTraceback();
        SptrTraceback(const SptrTraceback& x);
        SptrTraceback(void* sptr);

        bool operator==(const SptrTraceback& x) const;
        SptrTraceback& operator=(const SptrTraceback& x);

        void dump(FILE* f);

    private:

        void* sptr;
        void *my_backtrace[50];
        int backtrace_size;
};

SptrTraceback::SptrTraceback()
        : sptr(0),
        backtrace_size(0)
{
}



SptrTraceback::SptrTraceback(const SptrTraceback& x)
        : sptr(x.sptr),
        backtrace_size(x.backtrace_size)
{
    for (int i = 0; i < backtrace_size; i++)
    {
        my_backtrace[i] = x.my_backtrace[i];
    }
}


SptrTraceback::SptrTraceback(void* sptr)
        : sptr(sptr),
        backtrace_size(0)
{
    backtrace_size = backtrace(my_backtrace, 50);
}


bool SptrTraceback::operator==(const SptrTraceback& x) const
{
    return sptr == x.sptr;
}


SptrTraceback& SptrTraceback::operator=(const SptrTraceback& x)
{
    sptr = x.sptr;
    backtrace_size = x.backtrace_size;
    for (int i = 0; i < backtrace_size; i++)
    {
        my_backtrace[i] = x.my_backtrace[i];
    }
    return *this;
}


void SptrTraceback::dump(FILE* f)
{
    fprintf(f, "  %p:", sptr);
    for (int i = 0; i < backtrace_size; i++)
    {
        fprintf(f, " %p", my_backtrace[i]);
    }
    fprintf(f, "\n");
}


struct ptr_less : public binary_function < void*, void*, bool >
{
    bool operator()(void* const & x, void* const & y) const
    {
        return ((unsigned int) x) < ((unsigned int) y);
    }
};


//////////////////////////////////////////////////////////////////////

class SptrTracebackPack
{
    public:
        SptrTracebackPack();
        SptrTracebackPack(const SptrTracebackPack& x);
        SptrTracebackPack(void* myPtr, void* sptr);

        void insert(void* sptr);
        void clear(void* sptr);

        void dump(FILE* f);

    private:
        void* ptr;
        map < void*, SptrTraceback, ptr_less > tracebacks;
};


SptrTracebackPack::SptrTracebackPack(void* myPtr, void* sptr)
        : ptr(myPtr),
        tracebacks()
{
    insert(sptr);
}


SptrTracebackPack::SptrTracebackPack()
        : ptr(0),
        tracebacks()
{
}



SptrTracebackPack::SptrTracebackPack(const SptrTracebackPack& x)
        : ptr(x.ptr),
        tracebacks(x.tracebacks)
{
}



void SptrTracebackPack::insert(void* sptr)
{
    SptrTraceback t(sptr);
    tracebacks[sptr] = t;
}


void SptrTracebackPack::clear(void* sptr)
{
    tracebacks.erase(tracebacks.find(sptr));
}


void SptrTracebackPack::dump(FILE* f)
{
    map < void*, SptrTraceback, ptr_less > ::iterator i = tracebacks.begin();

    fprintf(f, "%p>\n", ptr);

    while (i != tracebacks.end())
    {
        i->second.dump(f);
        ++i;
    }
}


typedef map < void*, SptrTracebackPack > TraceMap;

static TraceMap tracemap;


void sptrDebugMarkTraceback(void* sptr, void* ptr)
{
    TraceMap::iterator i;
    if(ptr)
    {
	i = tracemap.find(ptr);
	if (i != tracemap.end())
	{
	    i->second.insert(sptr);
	}
	else
	{
	    tracemap[ptr] = SptrTracebackPack(ptr, sptr);
	}
    }
}


void sptrDebugClearTraceback(void* sptr, void* ptr)
{
    TraceMap::iterator i;
    if(ptr)
    {
	i = tracemap.find(ptr);
	if (i != tracemap.end())
	{
	    i->second.clear(sptr);
	}
	else
	{
	    assert(0);
	}
    }
}


void sptrDebugDumpTraceback(char* filename)
{
    // open file, and dump
    FILE* f;
    f = fopen(filename, "w");
    if (!f)
    {
        return ;
    }

    TraceMap::iterator i = tracemap.begin();
    while (i != tracemap.end())
    {
        // dump the record
        i->second.dump(f);
        ++i;
    }
    fclose(f);
}

#else

// no linux -- these functions do nothing

void sptrDebugMarkTraceback(void* sptr, void* ptr)
{
}



void sptrDebugClearTraceback(void* sptr, void* ptr)
{
}



void sptrDebugDumpTraceback(char* filename)
{
}

// #if __linux__

#endif
// #if __linux__

// #if SPTR_TRACEBACK_RUNNING
#else
// #if SPTR_TRACEBACK_RUNNING


void sptrDebugMarkTraceback(void* sptr, void* ptr)
{
}



void sptrDebugClearTraceback(void* sptr, void* ptr)
{
}



void sptrDebugDumpTraceback(char* filename)
{
}


// #if SPTR_TRACEBACK_RUNNING

#endif
// #if SPTR_TRACEBACK_RUNNING

// #if SPTR_TRACEBACK
#endif
// #if SPTR_TRACEBACK
