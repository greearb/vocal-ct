#ifndef SPTR_REFCOUNT_HXX_
#define SPTR_REFCOUNT_HXX_

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

static const char* const Sptr_RefCount_hxx_Version =
    "$Id: SptrRefCount.hxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include <assert.h>
#include "debug.h"


#define SPTR_DEBUG 1
#define SPTR_THREADSAFE 1
#define SPTR_TRACEBACK 1

#ifdef CODE_OPTIMIZE
// turn off debugging and tracebacks in optimized code

#undef SPTR_DEBUG
#undef SPTR_TRACEBACK
#define SPTR_DEBUG 0
#define SPTR_TRACEBACK 0

#endif

#ifdef WIN32
#define WORKAROUND_VISUAL_C
#endif



/** Template simulates a "smart" pointer which deletes the item it is
    pointing to when no more references to the item exist.  Warning:
    circular references will produce memory leaks. 
    
    <P>Note that only one Sptr should be constructed from the original
    ptr -- Sptr will free twice (and cause havoc) if it is misused like so:
    <P>
    <B> WRONG: </B>
    <PRE>
        T&#42; obj = new T();
        Sptr&lt;T> p;
        Sptr&lt;T> q;
        p = obj;
        q = obj;  
    </PRE>
    <p>
    <I>now both p and q think they are the only ones who will free the 
    memory, so you will get an error.</i>
    </p>
    <p><B>CORRECT:</b></p>
    <PRE> 
       T&#42; obj = new T();
       Sptr&lt;T> p;
       Sptr&lt;T> q;
       p = obj; 
       q = p;   // this keeps things OK.
    </PRE>

    <p>
    When all Sptr&lt;T> objects pointing to the object go out of scope, or are
    assigned to point to some other object, the object is deleted.
    </p>
    <p>
    <b>Examples</b>
    </p>
    <p>
    <PRE>
       Sptr &lt;Data> object = new Data;
       &#42;object = "string1";
       cout << &#42;object << endl;  // prints string1
       
       {
         Sptr &lt;Data> o2 = new Data("string2");
         object = o2;
	 
         // at this point, the string1 object has no references
         // because object points at string2 rather than string1, so
         // string1 is deleted.
	 
         // the string2 object has 2 references to it: object and o2.

         Sptr &lt;Data> o3 = new Data("string3");
         
         // at this point, o3 points to string3.
       }
       
       // at this point, the second string "string2" has only 1
       // pointer to it since o2 was destroyed (it was local to the
       // above section) since the reference count is 1 (object points
       // to it) it is left alone.
       
       // however, since o3 was destoyed, "string3"'s reference count
       // goes to 0 and it is destroyed.
       
       LocalScopeAllocator lo;
       printf("%s\n", object->getData(lo)); // print the object

    </PRE>
    </p>
*/


template < class T >
class Sptr {

private:
   T* ptr;

protected:
   // increment the reference count.
   void increment() {
      if (ptr) {
         ptr->incrementReferenceCount();
      }
   }

   // decrement the reference count
   void decrement() {
      if (ptr) {
         assert(ptr->getReferenceCount() > 0);
         ptr->decrementReferenceCount();
         if (ptr->getReferenceCount() == 0) {
            delete ptr;
            ptr = 0;
         }
      }
   }

public:

#if 0
   /** conversion operator converts pointers of this const class
    * to class const Sptr< T2 >., where T2 is a different base
    * class.  This is most often used when attempting to call a
    * const method of the base class through a derived class
    * pointer.  This is a workaround for SUNPRO .
    */
   template < class T2 >
   operator const Sptr<T2 > () const {
      return Sptr < T2 > (ptr, count);
   }
#endif
   
   // default constructor.  points to NULL.
   Sptr() : ptr(0) { }

   /** constructor used most often as the constructor from a
    * plain pointer.  Do not use this to convert a single pointer
    * to a smart pointer multiple times -- this will result in an
    * error (see class introduction for details).
    */
   Sptr(T* original)
         : ptr(original) {
      increment();
   }

#if 0
#if defined(WORKAROUND_VISUAL_C)
   /** conversion constructor -- this converts Sptrs of one type to
    *  another, but is ONLY used in working around the missing
    *  template conversion operators in Visual C++.
    */ 
   template <class T2>
   Sptr(const Sptr<T2>& x)
         : ptr(x.ptr), count(x.count) {
      increment();
   };
#endif
#endif
      
   /** copy constructor
    */
   Sptr(const Sptr& x)
         : ptr(x.ptr) {
      increment();
   };

   // destructor
   virtual ~Sptr() {
      decrement();
   }

   // dereference operator
   T& operator*() const {
      assert(ptr);
      return *ptr;
   }

   // ! operator .  Returns true if ptr == 0, false otherwise.
   int operator!() const {
      return (ptr == 0);
   }

   /// pointer operator.
   T* operator->() const {
      return ptr;
   }

   /** assignment operator -- this is most often used to assign
    * from a smart pointer to a derived type to a smart pointer
    * of the base type.
    */
   template < class T2 >
   Sptr& operator=(const Sptr < T2 > & x) {
      if (ptr == x.getPtr())
         return * this;
      decrement();
      ptr = x.getPtr();
      increment();
      return *this;
   }


   /** assignment operator from plain pointer.  Do not use this
    * to convert a single pointer to a smart pointer multiple
    * times -- this will result in an error (see class
    * introduction for details).
    */
   Sptr& operator=(T* original) {
      if (ptr == original)
         return * this;
      
      decrement();
      ptr = original;
      increment();
      return *this;
   }

   /// assignment operator
   Sptr& operator=(const Sptr& x) {
      if (ptr == x.ptr)
         return * this;
      decrement();
      ptr = x.ptr;
      increment();
      return *this;
   }

   /// compare whether a pointer and a smart pointer point to different things
   friend bool operator!=(const void* y, const Sptr& x) {
      if (x.ptr != y)
         return true;
      else
         return false;
   }

   /// compare whether a smart pointer and a pointer point to different things
   friend bool operator!=(const Sptr& x, const void* y) {
      return (y != x);
   }

   /// compare whether a pointer and a smart pointer point to the same thing
   friend bool operator==(const void* y, const Sptr& x) {
      if (x.ptr == y)
         return true;
      else
         return false;
   }

   /// compare whether a smart pointer and a pointer point to the same thing
   friend bool operator==(const Sptr& x, const void* y) {
      return (y == x);
   }

   /// compare whether two smart pointers point to the same thing
   bool operator==(const Sptr& x) const {
      if (x.ptr == ptr)
         return true;
      else
         return false;
   }

   /// compare whether two smart pointers point to the same thing
   bool operator!=(const Sptr& x) const {
      return !(operator==(x));
   }

   /**
      get the pointer to which the smart pointer points.
   */
   T* getPtr() const {
      return ptr;
   }

};

#endif
