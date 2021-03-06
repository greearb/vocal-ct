#if !defined(SYSTEM_STATUS_DOT_H)
#define SYSTEM_STATUS_DOT_H

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


static const char* const SystemStatus_hxx_Version = 
    "$Id: SystemStatus.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "VException.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Exception thrown when an unusual, but non-error, condition
 *  happens.<br><br><br>
 *
 *  This other is usually used with EAGAIN.
 */
class SystemStatus : public VException
{
    public:
    

    	/** Create the status.
	 *
	 *  @param  msg     Message to be logged. It's usually a strerror.
	 *  @param  file    File name where exception was created.
	 *  @param  line    Line number of the file where the exception was 
	 *  	    	    created.
	 *  @param  error   Usually an errno, and usually EAGAIN.
	 */
        SystemStatus(  
	    const string	    & 	msg,
    	    const string    	    & 	file,
    	    const int 	    	    	line,
    	    const int 	    	    	status = 0);
    

    	/** Name of this class of exception, "SystemStatus".
    	 */
    	string      	    	    getName() const;


    	/** Returns the error code, usually an errno, usually EAGAIN.
	 */
    	int   	    	    	    getStatus() const;
};


} // namespace Vocal


#endif // !defined(SYSTEM_STATUS_DOT_H)
