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

static const char* const Random_cxx_Version =
    "$Id: Random.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include "Random.hxx"
#include "MACAddress.hxx"
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>



using Vocal::Random;
using Vocal::Transport::MACAddress;


bool            Random::myInit = false;
unsigned int    Random::mySeed = 0;


Random::Random()
{
    init();
}    
        

Random::~Random()
{
}


int     
Random::operator()()
{
    return ( get() );
}
        

int        
Random::get()
{
    return ( rand() );
}


void            
Random::init(unsigned int seed)
{
    if ( myInit == true )
    {
        return;
    }
    
    MACAddress	    macAddr;
    #ifndef WIN32
    tms     	    emptyTms;
    #endif

    if ( seed == 0 )
    {
        mySeed  = 	macAddr.high() 
                ^   macAddr.low() 
                ^   static_cast<unsigned>(getpid())
                #ifndef WIN32
                ^   static_cast<unsigned>(times(&emptyTms))
                #endif
                ^   time(0);
    }
    else
    {
        mySeed = seed;
    }

    srand(mySeed);

    myInit = true;
};


unsigned int
Random::seed()
{
    return ( mySeed );
}
