#ifndef VFILTER_HXX_
#define VFILTER_HXX_

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





static const char* const VFilterHeaderVersion =
    "$Id: VFilter.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";
#include "global.h"
#include "VNamedPipeFC.hxx"
#include <string>

// simple filtering class.


// To enable VFilter, you must meet all of these conditions:

// 1.  run unix.  no vxworks support

// 2.  define the environment variable SIP_FILTER !  This is
// important!  use

// setenv SIP_FILTER 1

// in tcsh, or

// SIP_FILTER=1 ; export SIP_FILTER

// in bash to enable.

// VFilter prints out information as it starts running about this.

// 3.  run the appropriate infilter.pl and outfilter.pl code to filter
// the incoming and outgoing sides of the connections.  this code is in
// the vfilter directory under tools .

// There is a readme with more information in tools/vfilter .

#ifdef WIN32
using std::string;
#endif

class VFilter
{
    public:
        static string inFilter(const string& input);
        static string outFilter(const string& input);

    private:
        VFilter();
#ifndef __vxworks
        VNamedPipeFC inPipe;
        VNamedPipeFC outPipe;
        bool useFilter;
        static VFilter* instance_;
#endif
};


#endif
