
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





static const char* const VNamedPipeFC_cxx_Version =
    "$Id: VNamedPipeFC.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";
#ifndef __vxworks
#include "global.h"
#include "VNamedPipeFC.hxx"
#include "Sptr.hxx"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

const int maxNamedPipeFds = 128;

static int make_and_open_named_pipe(const char* pipeName)
{
    struct stat buffer;

    int err = stat(pipeName, &buffer);

    if ((err == -1) || !S_ISFIFO(buffer.st_mode))
    {
        unlink(pipeName);
        mknod(pipeName, S_IFIFO | 0644, 0);
    }

    int fd = open(pipeName, O_RDWR | O_NONBLOCK);

    if (fd <= 0)
    {
        cerr << "error, error, can't open: " << pipeName << endl;
    }

    return fd;
}


VNamedPipeFC::~VNamedPipeFC()
{
    close(inFd);
    close(inCtlFd);
    close(outFd);
    close(outCtlFd);

    string inName = pipeName_ + string(".in");
    string inCtlName = pipeName_ + string(".in.ctl");
    string outName = pipeName_ + string(".out");
    string outCtlName = pipeName_ + string(".out.ctl");

    //    unlink(inName.c_str());
    //    unlink(inCtlName.c_str());
    //    unlink(outName.c_str());
    //    unlink(outCtlName.c_str());
}



VNamedPipeFC::VNamedPipeFC(const char* pipeName)
        : timeoutValid(false),
        pipeName_(pipeName)
{

    string inName = pipeName_ + string(".in");
    string inCtlName = pipeName_ + string(".in.ctl");
    string outName = pipeName_ + string(".out");
    string outCtlName = pipeName_ + string(".out.ctl");

    // create the nodes

    inFd = make_and_open_named_pipe(inName.c_str());
    inCtlFd = make_and_open_named_pipe(inCtlName.c_str());
    outFd = make_and_open_named_pipe(outName.c_str());
    outCtlFd = make_and_open_named_pipe(outCtlName.c_str());
}


void VNamedPipeFC::clearTimeout()
{
    timeoutValid = false;
}

void VNamedPipeFC::setTimeout(const struct timeval& timeout)
{
    localTimeout.tv_sec = timeout.tv_sec;
    localTimeout.tv_usec = timeout.tv_usec;
    timeoutValid = true;
}


string VNamedPipeFC::encodeMsg(const string& msg)
{
    char header[20];

    sprintf(header, "msg %10.10d\n", msg.length());

    string o_buf;

    o_buf = header;
    o_buf += msg;

    return o_buf;
}


VNamedPipeFC::VNamedPipeFC()
{
    assert(0);
}

bool VNamedPipeFC::handleInCtl()
{
    int bufSize = 5000;  
#if defined (__SUNPRO_CC)
    char buf[5000];
#else    
    char buf[bufSize];
#endif
    fd_set myReadSet;

    struct timeval timeout;

    bool noMoreData = false;

    while (!noMoreData)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        FD_ZERO(&myReadSet);
        FD_SET(inCtlFd, &myReadSet);

        int selVal = select(maxNamedPipeFds, &myReadSet, 0, 0, &timeout);

        if (selVal == 0)
        {
            noMoreData = true;
        }

        if (selVal < 0)
        {
            // bad news!
            noMoreData = true;
        }

        if (FD_ISSET(inCtlFd, &myReadSet))
        {
            // read
            read(inCtlFd, buf, bufSize);
            // go again
        }

    }
    return false;
}


bool VNamedPipeFC::writeOkBlockOrTimeout()
{
    if (timeoutValid)
    {
        struct timeval timeout;

        timeout.tv_sec = localTimeout.tv_sec;
        timeout.tv_usec = localTimeout.tv_usec;

        return writeOk(&timeout);
    }
    else
    {
        return writeOk(0);
    }
}


bool VNamedPipeFC::writeOkNoBlock()
{
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return writeOk(&timeout);
}

bool VNamedPipeFC::writeOk(struct timeval* timeout)
{
    fd_set myReadSet;
    fd_set myWriteSet;

    FD_ZERO(&myReadSet);
    FD_SET(inCtlFd, &myReadSet);

    FD_ZERO(&myWriteSet);
    FD_SET(outFd, &myWriteSet);

    int selVal = select(maxNamedPipeFds, &myReadSet, &myWriteSet, 0, timeout);

    if (selVal < 0)
    {
        // bad news!
        return false;
    }

    if (FD_ISSET(inCtlFd, &myReadSet))
    {
        handleInCtl();
        cerr << "ng" << endl;
        return false;
    }

    if (FD_ISSET(outFd, &myWriteSet))
    {
        //	cerr << "good" << endl;
        return true;
    }

    //    cerr << "other" << endl;

    return false;
}


bool VNamedPipeFC::readOkBlockOrTimeout()
{
    if (timeoutValid)
    {
        struct timeval timeout;

        timeout.tv_sec = localTimeout.tv_sec;
        timeout.tv_usec = localTimeout.tv_usec;

        return readOk(&timeout);
    }
    else
    {
        return readOk(0);
    }
}


bool VNamedPipeFC::readOkNoBlock()
{
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return readOk(&timeout);
}

bool VNamedPipeFC::readOk(struct timeval* timeout)
{
    fd_set myReadSet;

    FD_ZERO(&myReadSet);
    FD_SET(inCtlFd, &myReadSet);
    FD_SET(inFd, &myReadSet);

    int selVal = select(maxNamedPipeFds, &myReadSet, 0, 0, timeout);

    if (selVal < 0)
    {
        // bad news!
        return false;
    }
    if (FD_ISSET(inCtlFd, &myReadSet))
    {
        handleInCtl();
        cerr << "ng" << endl;
        return false;
    }


    if (FD_ISSET(inFd, &myReadSet))
    {
        //	cerr << "go" << endl;
        return true;
    }

    //    cerr << "other" << endl;

    return false;
}



bool VNamedPipeFC::sendMsg(const string& msg)
{
    string encodedMsg = encodeMsg(msg);

    unsigned int total_bytes_written = 0;
    //	int bytes_written = 0;
    while (total_bytes_written < encodedMsg.length())
    {
        // select for both write and the control channel
        if (writeOkBlockOrTimeout() == false)
        {
            fsync(outFd);
            return false;
        }

        int bytes_written
        = write(outFd,
                encodedMsg.c_str() + total_bytes_written,
                encodedMsg.length() - total_bytes_written);
        total_bytes_written += bytes_written;
    }
    fsync(outFd);
    return true;
}


bool VNamedPipeFC::recMsg(string* msg)
{
    char header_buffer[20];

    const int bytes_to_read = 15;
    int total_bytes_read;

    total_bytes_read = 0;

    while (total_bytes_read < bytes_to_read)
    {
        if (readOkBlockOrTimeout() == false)
        {
            return false;
        }
        int bytes_read = read(inFd, header_buffer + total_bytes_read,
                              bytes_to_read - total_bytes_read);
        total_bytes_read += bytes_read;
    }

    header_buffer[15] = '\0';

    // now, parse the buffer for the data

    //    cout << "buf: " << header_buffer << endl;

    //bytes_to_read = atoi(header_buffer + 4);

    int nbytes_to_read =  atoi(header_buffer + 4) ;

    //    cout << "reading (" << bytes_to_read << " bytes)" << endl;
    char* buffer = new char[nbytes_to_read+1];

    total_bytes_read = 0;
    while (total_bytes_read < nbytes_to_read)
    {
        if (readOkBlockOrTimeout() == false)
        {
            return false;
        }
        int bytes_read = read(inFd, buffer + total_bytes_read,
                              nbytes_to_read - total_bytes_read);
        total_bytes_read += bytes_read;
    }
    buffer[nbytes_to_read] = '\0';
    *msg = string(buffer);
    delete [] buffer;
    return true;
}


void VNamedPipeFC::clearCtl()
{
    handleInCtl();
}


#else

// vxworks stuff

static int unused;

#endif
