#ifndef CWBUFFER_HXX_
#define CWBUFFER_HXX_

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


static const char* const CWBuffer_hxx_Version =
    "$Id: CWBuffer.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "CountSemaphore.hxx"
#include <string.h>
#include <ctype.h>
#include "Sptr.hxx"
#include "fastmemcpy.hxx"
#include "Lock.hxx"
#include <iostream>
#include <cstdlib>
#include <cctype>

#define INLINE_ inline


using namespace Vocal::Threads;


#define BUFFER_STATISTICS 0

class CWCharArray;



INLINE_ int
mystrlen(const char* s)
{
    int i = 0;
    while (*s++ != '\0')
    {
        i++;
    }
    return i;
}


#if 1
INLINE_ void*
mmemcpy(char* dest, const char* s, int len)
{
    char *d = dest;
    while (len--)
    {
        *d++ = *s++;
    }
    return dest;
}
#else
#define mmemcpy memcpy
#endif

extern char* emptyStr;

/** character array structure used for copy on write data.  Do not
 * derive from this class */
class CWCharArray
{
    public:
	/// construct an array of length len
	CWCharArray(int len);
	
	/// construct an array of size allocsize, copying len bytes from p
	CWCharArray(const char* p, int len, int allocsize);
	/// destructor
	~CWCharArray();

	CountSemaphore mutex;
	char* ptr;
};


#if (BUFFER_STATISTICS)

struct BufferStats
{
    public:
	BufferStats() : refCount(0), realCount(0)
	{
	}
	int refCount;
	int realCount;
};

class BufferStatistic
{
    public:
	static void add(int size, CWCharArray* ptr)
	{
	    create();
	    Lock lock_(instance->bufferMutex);
	    instance->buffer[ptr] = size;
	    if(instance->rawSizes.find(size) == instance->rawSizes.end())
	    {
		instance->rawSizes[size] = 1;
	    }
	    else
	    {
		instance->rawSizes[size]++;
	    }
	}

	static void remove(CWCharArray* ptr)
	{
	    create();
	    Lock lock_(instance->bufferMutex);
	    map <CWCharArray*, int>::iterator i = instance->buffer.find(ptr);
	    if(i != instance->buffer.end())
	    {
		instance->buffer.erase(i);
	    }
	}

	static void print(ostream& s)
	{
	    create();
	    Lock lock_(instance->bufferMutex);
	    s << "\n\t\tBuffer Report\n\n";
	    map <CWCharArray*, int>::iterator i;
	    map <int, BufferStats> myStats;
	    map <int, BufferStats>::iterator j;
	    for(i = instance->buffer.begin(); i != instance->buffer.end(); ++i)
	    {
		j = myStats.find(i->second);
		if(j == myStats.end())
		{
		    BufferStats x;
		    x.refCount = (i->first)->mutex.getCount();
		    x.realCount = 1;
		    myStats[i->second] = x;
		}
		else
		{
		    j->second.refCount += (i->first)->mutex.getCount();
		    j->second.realCount ++;
		}
#if 0
		s << i->first
		  << ":"
		  << i->second
		  << ":"
		  << (i->first)->mutex.getCount()
		  << "\n";
#endif
	    }

	    s << "current:\n\n";

	    for(j = myStats.begin(); j != myStats.end(); ++j)
	    {
		s << j->first
		  << ":"
		  << j->second.realCount
		  << ":"
		  << j->second.refCount
		  << "\n";
	    }

	    s << "allocs:\n\n";
	    for(map <int, int>::iterator k = instance->rawSizes.begin();
		k != instance->rawSizes.end(); 
		++k)
	    {
		s << k->first
		  << ":"
		  << k->second
		  << "\n";
	    }


	}

    protected:
	BufferStatistic()
	    : bufferMutex(),
	      buffer()
	{
	}

	static void create()
	{
	    if(!instance)
	    {
		instance = new BufferStatistic;
	    }
	}

	static BufferStatistic* instance;
	Mutex bufferMutex;
	map <CWCharArray*, int> buffer;
	map <int, int> rawSizes;
};


INLINE_
void printBufferStatistics(ostream& s)
{
    BufferStatistic::print(s);    
}

#else

INLINE_
void printBufferStatistics(ostream& s)
{
    // do nothing
    return;
}

#endif

extern "C" {
void dumpBufferStatistics();
};


INLINE_
CWCharArray::CWCharArray(int length)
        : mutex()
{
    ptr = new char[length];
    memset(ptr, 0, length);
#if (BUFFER_STATISTICS)
    BufferStatistic::add(length, this);
#endif
}


INLINE_
CWCharArray::CWCharArray(const char* p, int len, int allocsize)
        : mutex()
{
    assert(allocsize >= len);

    ptr = new char[allocsize];
    memset(ptr, 0, allocsize);
    mmemcpy(ptr, p, len);
#if (BUFFER_STATISTICS)
    BufferStatistic::add(len, this);
#endif
}


INLINE_
CWCharArray::~CWCharArray()
{
    assert(mutex.compare(0));
    delete[] ptr;
#if (BUFFER_STATISTICS)
    BufferStatistic::remove(this);
#endif
}


INLINE_ void decrementCA(CWCharArray* ptr)
{
    if (ptr)
    {
        if (ptr->mutex.compare(0))
        {
            delete (ptr);
            ptr = 0;
        }
        else if (ptr->mutex.decrement())
        {
            delete (ptr);
            ptr = 0;
        }
    }
}

/** copy on write buffer, used in conjunction with CopyOnWriteData.
 * Do not derive from this class */
class CWBuffer
{
    public:
	/// default constructor (constructs an empty buffer of length len)
	CWBuffer(int len = 0);
	/// construct a buffer of size hint, copying length bytes from p
	CWBuffer(const char* src, int length, int hint = 0);
	/// copy constructor
	CWBuffer(const CWBuffer& src);

	/// destructor
	~CWBuffer();

	/// assignment operator
	const CWBuffer& operator=(const CWBuffer& src);

	// these functions don't make modifications to the underlying
	// buffer, so they ought to be cheap

	/// clear the string
	void clear();
	/** compare this buffer with a character array
	    @param str  character array to compare against
	    @param rightSize  size of array to compare against
	*/
	int compare(const char* str, int rightSize) const;

	/** compare this buffer with a character array
	    @param str  character array to compare against
	    @param rightSize  size of array to compare against
	*/
	int compareCstr(const char* str) const;

	/** compare this buffer with a character array case-insensitvely
	    @param str  character array to compare against
	    @param rightSize  size of array to compare against
	*/
	int compareNoCase(const char* str, int rightSize) const;

	/** split the string into two.  This object becomes the front,
	    which has all the characters w/ positions < p, while ptr
	    has the positions >= p.  Note that this is special in that
	    it does NOT write to any of the string, so the underlying
	    buffer is never written.
	    @param p    number of bytes to split at.
	    @param ptr  returned buffer, containing the positions >= p
	*/
	
	void split(int p, CWBuffer* ptr);

	/** truncate the current  buffer.
	    @param front   number of characters to remove from the beginning 
	                   of the string
	    @param back    number of characters to remove from the end 
	                   of the string
	*/
	void truncate(int front, int back);


	/** truncate the current buffer.
	    @param first    the first character of the string to keep
	    @param last     the last character of the string to keep
	*/
	void substr(int first, int last);

	// these functions (potentially) modify the underlying buffer, so
	// they are (potentially) expensive

	/** verify that the current string can be written to, up to
	    length-1 characters.
	    @param length   number of characters in bitsPtr which are writable.
	*/
	void makeWritable(int length);
	/** convert this string into a C string (a NUL terminated one) */
	void makeCString();


	CWCharArray* bufPtr;
	char* bitsPtr;
	int size;
	int allocSpace;
};


INLINE_
CWBuffer::CWBuffer(int length)
        :
        bufPtr(0),
        bitsPtr(emptyStr),
        size(0),
        allocSpace(length)
{
    if (allocSpace)
    {
        bufPtr = new CWCharArray(allocSpace);
        bufPtr->mutex.increment();
        bitsPtr = bufPtr->ptr;
    }
}


INLINE_
CWBuffer::CWBuffer(const char* src, int length, int hint)
        :
        bufPtr(0),
        bitsPtr(emptyStr),
        size(length),
        allocSpace(length)
{
    if (hint > length)
    {
        allocSpace = hint;
    }

    if (allocSpace)
    {
        bufPtr = new CWCharArray(src, length, allocSpace);
        bufPtr->mutex.increment();
        bitsPtr = bufPtr->ptr;
    }
}


INLINE_
CWBuffer::CWBuffer(const CWBuffer& src)
        :
        bufPtr(src.bufPtr),
        bitsPtr(src.bitsPtr),
        size(src.size),
        allocSpace(src.allocSpace)
{
    if (bufPtr)
    {
        bufPtr->mutex.increment();
    }
}


INLINE_ const CWBuffer&
CWBuffer::operator=(const CWBuffer& src)
{
    bufPtr = src.bufPtr;
    bitsPtr = src.bitsPtr;
    size = src.size;
    allocSpace = src.allocSpace;

    if (bufPtr)
    {
        bufPtr->mutex.increment();
    }
    return *this;
}


INLINE_ void
CWBuffer::clear()
{
    if (bufPtr)
    {
        decrementCA(bufPtr);
    }
    bitsPtr = emptyStr;
    size = 0;
    allocSpace = 0;
    bufPtr = 0;
}


INLINE_
CWBuffer::~CWBuffer()
{
    decrementCA(bufPtr);
}


INLINE_ int
CWBuffer::compare(const char* str, int rightSize) const
{
    char* ptr = bitsPtr;
    int leftSize = size;

    while (leftSize && rightSize)
    {
        if (*ptr < *str)
        {
            return -1;
        }
        if (*ptr > *str)
        {
            return 1;
        }
        leftSize--;
        rightSize--;
        ptr++;
        str++;
    }

    if (leftSize < rightSize)
    {
        return -1;
    }
    else if (leftSize > rightSize)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


INLINE_ int
CWBuffer::compareCstr(const char* str) const
{
    char* ptr = bitsPtr;
    int leftSize = size;

    while (leftSize && (*str != '\0'))
    {
        if (*ptr < *str)
        {
            return -1;
        }
        if (*ptr > *str)
        {
            return 1;
        }
        leftSize--;
        ptr++;
        str++;
    }

    if ((leftSize == 0) && (*str != '\0'))
    {
        return -1;
    }
    else if ((leftSize > 0) && (*str == '\0'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


INLINE_ int
CWBuffer::compareNoCase(const char* str, int rightSize) const
{
    char* ptr = bitsPtr;
    int leftSize = size;

    while (leftSize && rightSize)
    {
        if (::toupper(*ptr) < ::toupper(*str))
        {
            return -1;
        }
        if (::toupper(*ptr) > ::toupper(*str))
        {
            return 1;
        }
        leftSize--;
        rightSize--;
        ptr++;
        str++;
    }

    if (leftSize < rightSize)
    {
        return -1;
    }
    else if (leftSize > rightSize)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


INLINE_ void
CWBuffer::split(int p, CWBuffer* ptr)
{
    assert((p >= 0) && (p <= size));

    if (p == size)
    {
        // do not actually split, but return an empty CWBuffer.  p ==
        // size is a border case where we do nothing, and return an
        // empty string, since nothing belongs in the second half of
        // the split.

        ptr->clear();
    }
    else
    {
        // do the split

        *ptr = *this;

        // correct the new ptr
        ptr->bitsPtr = ptr->bitsPtr + p;
        ptr->size = ptr->size - p;
        ptr->allocSpace = ptr->allocSpace - p;

        // correct the old ptr
        allocSpace = p;
        size = p;
    }
}


INLINE_ void
CWBuffer::truncate(int front, int back)
{
    assert((front + back) <= size);

    bitsPtr = bitsPtr + front;
    size = size - (front + back);
}



INLINE_ void
CWBuffer::substr(int first, int last)
{
    assert((first <= last) &&
           (first <= size) &&
           (last <= size) &&
           (first >= 0) &&
           (last >= 0));

    bitsPtr = bitsPtr + first;
    size = (last - first);
}


INLINE_ void
CWBuffer::makeWritable(int length)
{
    if (bufPtr)
    {
        if ( (length > allocSpace) || !bufPtr->mutex.compare(1))
        {
            // if we need more space than we have in this buffer, OR
            // if this is not the only CWBuffer pointing to this
            // CharArray, we need to copy it

#if 0
	    // this is the policy
            if (length > allocSpace)
            {
                allocSpace = length + 128;
            }
#endif
	    // replace above policy with an exponential backoff
	    // policy.  See: http://www.gotw.ca/gotw/043.htm -- the
	    // comment is exponential increase of about 1.5 is best
	    // (allegedly from Andrew Koenig's column in the September
	    // 1998 issue of JOOP (Journal of Object-Oriented
	    // Programming).

	    if (length > allocSpace)
	    {
		allocSpace = length + length / 2;
	    }


            CWCharArray* tmp = new CWCharArray(bitsPtr, size, allocSpace);

            // this must be done first or it will cause problems
            tmp->mutex.increment();
            bitsPtr = tmp->ptr;

	    tmp->mutex.exchange(reinterpret_cast<void**>(&bufPtr), 
				reinterpret_cast<void**>(&tmp));

            // get rid of the old pointer, now in tmp
            decrementCA(tmp);
        }
    }
    else if (length > 0)
    {
        assert(size == 0);
        // allocate a buffer
//        allocSpace = length + 128;
	// see above for the rationale for this
	allocSpace = length + length / 2;

        bufPtr = new CWCharArray(allocSpace);
        bitsPtr = bufPtr->ptr;

        bufPtr->mutex.increment();
    }
}


INLINE_ void
CWBuffer::makeCString()
{
    if (bitsPtr != emptyStr)
    {
        makeWritable(size + 1);
        bitsPtr[size] = '\0';
    }
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
