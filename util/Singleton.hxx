#if !defined(VOCAL_SINGLETON_HXX)
#define VOCAL_SINGLETON_HXX

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


static const char* const Singleton_hxx_Version = 
    "$Id: Singleton.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "NonCopyable.hxx"
#include "GarbageCan.hxx"
#include "Garbage.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Singleton pattern. See Gang of Four.
 */
template <class Object>
class Singleton : public Garbage, public NonCopyable
{
    protected:
    
        /** Protected constructor to force specialization.
         */
        Singleton();
        
        
        /** Protected virtual destructor. This should only be deleted
         *  via Garbage's virtual destructor.
         */
        virtual ~Singleton();


    public:


        /**  Create an instance of this class.
         */
        static Object &         instance();

        
    protected:


        /** Initialize the instance.
         */    
        static void             init();

        
    private:


        /** The instance.
         */    
        static  Object      *   myInstance;
        static  unsigned int    myCount;
};


template <class Object>
Object  *   Singleton<Object>::myInstance = 0;


template <class Object>
unsigned int    Singleton<Object>::myCount = 0;


template <class Object>
Singleton<Object>::Singleton()
{
    ++myCount;
    
    if ( myCount > 1 )
    {
        throw ( "There can be only one.");
    }
}


template <class Object>
Singleton<Object>::~Singleton()
{
}


template <class Object>
Object &
Singleton<Object>::instance()
{
    init();
    return ( *myInstance );
}


template <class Object>
void
Singleton<Object>::init()
{
    if ( myInstance == 0 )
    {
        myInstance = new Object;

        GarbageCan::add(myInstance);
    }
}


} // namespace Vocal


#endif // !defined(VOCAL_SINGLETON_HXX)
