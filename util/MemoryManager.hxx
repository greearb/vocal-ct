#ifndef MEMORYMANAGER_HXX_
#define MEMORYMANAGER_HXX_

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
    
static const char* const MemoryManager_hxx_version =
    "";
    
#include <cstddef>
#include <iostream>
#include <cstdlib>
    
#define ALLOC_BLOCK 128

typedef struct Header
{
        Header* next;
        int size;
}HEADER;    

    
class MemoryManager
{
    public:
        /// Default constructor
        static MemoryManager* getInstance();
        static void*  mynew(size_t size);
        static void*  mynew(size_t size, void* p);
        //static void*  mynew[](size_t size, void* p);
        static void   mydelete(void* p);
        static void   mydelete(void* p, void*);
        //static void   mydelete[](void* p, void*);
        
        static void displayAllocList();
        static void displayFreeList();
    protected:
        MemoryManager();
    private:
        static MemoryManager* mInstance;
        
       
    
        HEADER* mAllocList;
        HEADER* mFreeList;
        bool mAvail; //if the first block is available or not.
    
    
        ///memory management functions
        void* alloc(size_t size);
        bool dealloc(void* ptr);
    
        HEADER* moreCore(int nBytes);
        HEADER* getFromFreeList(int nBlocks);
};
    
    
#if 0
void* MemoryManager::mynew(size_t size)
{
    
    MemoryManager* manager = MemoryManager::getInstance();
    void* p = manager->alloc(size);
    //void* p = alloc(size);
    cout << "in my new" << endl;
    return p;
    
    
    
#if 0
    void* p = malloc(size);
    cout << "in my new" << endl;
    return p;
#endif
    
    
}
    

void MemoryManager::mydelete(void* p) 
{

    MemoryManager* manager = MemoryManager::getInstance();
    if (p != NULL)
    {
        manager->dealloc(p);
     //dealloc(p);
    }
    
    
#if 0
    if (p != NULL)
    {
        free (p);
    }
#endif
}
#endif

#endif
