#ifndef CIRCULARBUFFER_HXX_
#define CIRCULARBUFFER_HXX_

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

static const char* const CircularBuffer_hxx_version =
    "$Id: CircularBuffer.hxx,v 1.2 2004/06/15 06:20:35 greear Exp $";

#include <cassert>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{

/** Abstract data types
 */

namespace ADT
{

/** Circular buffer class.  used to keep a queue of bytes for the user
 */
template < class T >
class CircularBuffer {
    public:
        /** construct a buffer with maximum size
            @param  size   maximum size
        */
        CircularBuffer(int size);
        ~CircularBuffer();

        /// report number of bytes free
        int bytesAvailable() const;
        /// report number of bytes used (available for data)
        int bytesUsed() const;

        /** get bytes from the buffer
            @param maxSize    maximum number of bytes to get
            @param T          place to store bytes
            @return  the number of bytes
        */
        int get(T* data, int maxSize);
        int put(const T* data, int size);

    private:

        int used() const;
        int available() const;

        T* myBuffer;
        int myStartUsed;
        int myEndUsed;
        int mySize;
};


template < class T >
CircularBuffer<T>::CircularBuffer(int size)
    :myBuffer(),
     myStartUsed(0),
     myEndUsed(0),
     mySize(size)
{
    myBuffer = new T[size];
}


template < class T >
CircularBuffer<T>::~CircularBuffer()
{
    delete[] myBuffer;
}


template < class T >
int
CircularBuffer<T>::used() const
{
    int used = myEndUsed - myStartUsed;
    if(used < 0)
    {
	used = mySize + used;
    }
    return used;
}


template < class T >
int
CircularBuffer<T>::available() const
{
    return mySize - used();
}


template < class T >
int 
CircularBuffer<T>::bytesAvailable() const
{
    return available();
}


template < class T >
int
CircularBuffer<T>::bytesUsed() const
{
    return used();
}


template < class T >
int
CircularBuffer<T>::get(T* data, int maxSize)
{
    // true for this input
    assert(maxSize <= mySize);

    // always true
    assert(myStartUsed >= 0);
    assert(myEndUsed >= 0);
    assert(myStartUsed < mySize);
    assert(myEndUsed < mySize);
    int copySize = 0;

    while((myStartUsed  != myEndUsed) && maxSize )
    {
	*data = myBuffer[myStartUsed];
	++data;
	myStartUsed = (myStartUsed + 1) % mySize;
	--maxSize;
	++copySize;
    }

    // always true
    assert(myStartUsed >= 0);
    assert(myEndUsed >= 0);
    assert(myStartUsed < mySize);
    assert(myEndUsed < mySize);

    return copySize;
}


template < class T >
int
CircularBuffer<T>::put(const T* data, int size)
{
    // always true
    assert(myStartUsed >= 0);
    assert(myEndUsed >= 0);
    assert(myStartUsed < mySize);
    assert(myEndUsed < mySize);

    int next;
    while( ( (next = ((myEndUsed + 1) % mySize)) != myStartUsed) &&
	   size )
    {
	myBuffer[myEndUsed] = *data;
	++data;
	--size;
	myEndUsed = next;
    }

    // always true
    assert(myStartUsed >= 0);
    assert(myEndUsed >= 0);
    assert(myStartUsed < mySize);
    assert(myEndUsed < mySize);

    return size;
}


} // namespace ADT
} // namespace Vocal

#endif
