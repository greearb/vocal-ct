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

static const char* const Daemon_cxx_Version =
    "$Id: Daemon.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "global.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include "cpLog.h"
#include "Daemon.hxx"
#include <assert.h>
#include <stdio.h>
#include <errno.h>

int Daemon(bool stdout_to_file)
{
#if !defined(__vxworks) && !defined(WIN32)

    pid_t pid;

    // start as daemon as default, but let environment variable override
    char* envValString = getenv( "NO_DAEMON" );
    if ( envValString )
    {
        if ( atoi( envValString ) != 0 )
        {
            cerr << "NO_DAEMON environment variable set" << endl;
            return 0 ;
        }
    }

    if ( ( pid = fork() ) < 0 )
    {
        cerr << "first fork failed!" << endl;
        return -1 ;
    }
    else if ( pid != 0 )
    {
        exit ( 0 );
    }

    setsid();

    if ( ( pid = fork() ) < 0 )
    {
        cerr << "second fork failed!" << endl;
        return -1 ;
    }
    else if ( pid != 0 )
    {
	// output this to the appropriate place ?

        exit( 0 );
    }

    setsid();

    umask( 0 );

    close( 0 );
    int fd;
    fd = open( "/dev/null", O_RDONLY );

    if ( fd == -1 )
    {
        cerr << "reopen stdin failed!" << endl;
        return -1;
    }

    close( 1 );
    if ( stdout_to_file )
    {
        fd = open( "stdout.log", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR );
    }
    else
    {
        fd = open( "/dev/null", O_WRONLY );
    }

    if ( fd == -1 )
    {
        perror("reopen stdout failed");
        return -1;
    }

    close( 2 );
    
    if ( stdout_to_file )
    {
        fd = dup( 1 );
    }
    else
    {
        fd = open( "/dev/null", O_WRONLY );
    }

    if ( fd == -1 )
    {
        perror("reopen stderr failed");
	cout << "reopen stderr failed: " << strerror(errno) << endl;
        return -1;
    }
    return 0;

#else
    cerr << "Daemon not defined in vxworks" << endl;
    assert(0);

    return 0;
#endif
}
