
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

static const char* const VFilter_cxx_Version =
    "$Id: VFilter.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "global.h"
#include "VFilter.hxx"
#include "cpLog.h"

#ifndef __vxworks

VFilter* VFilter::instance_ = 0;

VFilter::VFilter()
        :
        inPipe("sip-inpipe"),
        outPipe("sip-outpipe"),
        useFilter(false)
{
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    inPipe.setTimeout(timeout);
    outPipe.setTimeout(timeout);
    if (getenv("SIP_FILTER"))
    {
        cpLog( LOG_INFO, "Environment variable SIP_FILTER is set." );
        cpLog( LOG_INFO, "Using VFilter to modify SIP messages" );
        useFilter = true;
    }
    else
    {
        cpLog( LOG_INFO, "Environment variable SIP_FILTER is Not set." );
        cpLog( LOG_INFO, "Not using VFilter to modify SIP messages" );
    }
}


string VFilter::inFilter(const string& input)
{
    if (!instance_)
    {
        instance_ = new VFilter;
    }

    if (instance_->useFilter)
    {
        instance_->inPipe.clearCtl();

        string output;
        if (instance_->inPipe.sendMsg(input))
        {
            if (instance_->inPipe.recMsg(&output))
            {
                return output;
            }
        }
    }

    // in this case, there was an error or the environment variable
    // is not set, so no filtering is done
    return input;
}


string VFilter::outFilter(const string& input)
{
    if (!instance_)
    {
        instance_ = new VFilter;
    }

    if (instance_->useFilter)
    {
        instance_->outPipe.clearCtl();

        string output;
        if (instance_->outPipe.sendMsg(input))
        {
            if (instance_->outPipe.recMsg(&output))
            {
                return output;
            }
        }
    }

    // in this case, there was an error or the environment variable
    // is not set, so no filtering is done
    return input;
}

#else
// the vxworks

VFilter::VFilter()
{}


string VFilter::inFilter(const string& input)
{
    return input;
}

string VFilter::outFilter(const string& input)
{
    return input;
}

#endif
