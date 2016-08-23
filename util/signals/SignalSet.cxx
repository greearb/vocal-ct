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



#include "global.h"
#include "SignalSet.hxx"
#include <cassert>


using Vocal::Signals::SignalSet;


SignalSet::SignalSet()
{
    empty();
}


SignalSet::SignalSet(int signum)
{
    empty();
    insert(signum);
}


SignalSet::SignalSet(int * signum, size_t size)
{
    empty();
    insert(signum, size);
}


SignalSet::SignalSet(const vector<int> & signum)
{
    empty();
    insert(signum);
}


SignalSet::SignalSet(const SignalSet & src)
    :   signals(src.signals),
        mySignalNumbers(src.mySignalNumbers)
{
}


SignalSet &
SignalSet::operator=(const SignalSet & src)
{
    if ( this != &src )
    {
        signals = src.signals;
        mySignalNumbers = src.mySignalNumbers;
    }
    return ( *this );
}


void
SignalSet::fill()
{
    #ifndef WIN32
    int rc = sigfillset(&signals);
    assert( rc == 0 );

    mySignalNumbers.clear();

    // Note that this is ridiculously os dependent.
    //
    int signums[] =
    {
        SIGHUP,
        SIGINT,
        SIGQUIT,
        SIGILL,
        SIGTRAP,
        SIGABRT,
        SIGBUS,
        SIGFPE,
        SIGKILL,
        SIGUSR1,
        SIGSEGV,
        SIGUSR2,
        SIGPIPE,
        SIGALRM,
        SIGTERM,
        #ifdef __linux__
        SIGSTKFLT,
        #endif
        SIGCHLD,
        SIGCONT,
        SIGSTOP,
        SIGTSTP,
        SIGTTIN,
        SIGTTOU,
        SIGURG,
        SIGXCPU,
        SIGXFSZ,
        SIGVTALRM,
        SIGPROF,
        SIGWINCH,
        SIGIO,
        #if ! (defined(__FreeBSD__) || defined(__APPLE__) || defined(__OpenBSD__))
        SIGPWR
        #endif

    };

    insert(signums, sizeof(signums)/sizeof(signums[0]));
    #endif
}


void
SignalSet::empty()
{
    #ifndef WIN32
    int rc = sigemptyset(&signals);
    assert( rc == 0 );

    mySignalNumbers.clear();
    #endif
}


void
SignalSet::insert(int signum)
{
    #ifndef WIN32
    if ( !contains(signum) )
    {
        int rc = sigaddset(&signals, signum);
        assert( rc == 0 );

        mySignalNumbers.push_back(signum);
    }
    #endif
}

void
SignalSet::insert(int * signum, size_t size)
{
    assert( signum );

    for ( size_t i = 0; i < size; i++ )
    {
        insert(signum[i]);
    }
}


void
SignalSet::insert(const vector < int > & signum)
{
    size_t size = signum.size();

    for ( size_t i = 0; i < size; i++ )
    {
        insert(signum[i]);
    }
}


void
SignalSet::erase(int signum)
{
    #ifndef WIN32
    int rc = sigdelset(&signals, signum);
    assert( rc == 0 );

    if ( contains(signum) )
    {
        // Note that this is O(n), n is the mySignalNumbers.size().
        // I.e n/2 to find + n/2 to erase. *sigh*
        //
        for ( vector < int > ::iterator it = mySignalNumbers.begin();
                it != mySignalNumbers.end();
                it++
            )
        {
            if ( *it == signum )
            {
                mySignalNumbers.erase(it);
                break;
            }
        }
    }
    #endif
}


void
SignalSet::erase(int * signum, size_t size)
{
    assert( signum );

    for ( size_t i = 0; i < size; i++ )
    {
        // Note this is O(n*m), where m is size.
        //
        erase(signum[i]);
    }
}


void
SignalSet::erase(const vector < int > & signum)
{
    size_t size = signum.size();

    for ( size_t i = 0; i < size; i++ )
    {
        // Note this is O(n*m), where m is size.
        //
        erase(signum[i]);
    }
}


bool
SignalSet::contains(int signum)
{
    int rc = 0;
    #ifndef WIN32
    // NOTE:   On Android, sigismember first arg is not const. -- 
    rc = sigismember(&signals, signum);
    assert ( rc > -1 );
    #endif
    return ( rc == 0 ? false : true );
}


bool
SignalSet::contains(int * signum, size_t size)
{
    assert( signum );

    for ( size_t i = 0; i < size; i++ )
    {
        if ( !contains(signum[i]) )
        {
            return ( false );
        }
    }
    return ( true );
}


bool
SignalSet::contains(const vector < int > & signum)
{
    size_t size = signum.size();

    for ( size_t i = 0; i < size; i++ )
    {
        if ( !contains(signum[i]) )
        {
            return ( false );
        }
    }
    return ( true );
}


const vector < int > &
SignalSet::signalNumbers() const
{
    return ( mySignalNumbers );
}


ostream &
SignalSet::writeTo(ostream & out) const
{
    #ifndef WIN32
    bool first = true;
    for ( vector < int > ::const_iterator it = mySignalNumbers.begin();
            it != mySignalNumbers.end();
            it++
        )
    {
        if ( first )
        {
            first = false;
        }
        else
        {
            out << ", ";
        }

        switch ( *it )
        {
            case SIGHUP:    out << "SIGHUP";	break;
            case SIGINT:    out << "SIGINT";	break;
            case SIGQUIT:   out << "SIGQUIT";	break;
            case SIGILL:    out << "SIGILL";	break;
            case SIGTRAP:   out << "SIGTRAP";	break;
            case SIGABRT:   out << "SIGABRT";	break;
            case SIGBUS:    out << "SIGBUS";	break;
            case SIGFPE:    out << "SIGFPE";	break;
            case SIGKILL:   out << "SIGKILL";	break;
            case SIGUSR1:   out << "SIGUSR1";	break;
            case SIGSEGV:   out << "SIGSEGV";	break;
            case SIGUSR2:   out << "SIGUSR2";	break;
            case SIGPIPE:   out << "SIGPIPE";	break;
            case SIGALRM:   out << "SIGALRM";	break;
            case SIGTERM:   out << "SIGTERM";	break;
            #ifdef __linux__
            case SIGSTKFLT: out << "SIGSTKFLT"; break;
            #endif
            case SIGCHLD:   out << "SIGCHLD";	break;
            case SIGCONT:   out << "SIGCONT";	break;
            case SIGSTOP:   out << "SIGSTOP";	break;
            case SIGTSTP:   out << "SIGTSTP";	break;
            case SIGTTIN:   out << "SIGTTIN";	break;
            case SIGTTOU:   out << "SIGTTOU";	break;
            case SIGURG:    out << "SIGURG";	break;
            case SIGXCPU:   out << "SIGXCPU";	break;
            case SIGXFSZ:   out << "SIGXFSZ";	break;
            case SIGVTALRM: out << "SIGVTALRM"; break;
            case SIGPROF:   out << "SIGPROF";	break;
            case SIGWINCH:  out << "SIGWINCH";	break;
            case SIGIO:     out << "SIGIO"; break;
            #if ! (defined(__FreeBSD__) || defined(__APPLE__) || defined(__OpenBSD__))
            case SIGPWR:    out << "SIGPWR";	break;
            #endif
            default:
            out << "Unknown Signal";	break;	
		}
	}
    #endif // !WIN32
    return ( out );
}
