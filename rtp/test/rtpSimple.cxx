
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

static const char* const rtpSimple_cxx_Version =
    "$Id: rtpSimple.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";




#include <unistd.h>
#include <fstream>

#include "rtpTypes.h"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "RtpSession.hxx"
#ifdef __vxworks
#include "VThread.hxx"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <deque>
#include "SessionData.hxx"
#include "support.hxx"

class MultiSession
{
    public:
        MultiSession();
        void processControl();
        void periodicProcess();
        int setFds(fd_set* fdSet);
        void processFds(fd_set* fdSet);

    private:
        deque < SessionData* > sessionList;
        string cmdBuf;
};


MultiSession::MultiSession()
{
    SessionData* session;
    session = new SessionData;

    sessionList.push_back(session);
}


bool cmd_verify(const split_t& command_list,
                const char* cmd_name,
                unsigned int args)
{
    if (command_list.size() == args)
    {
        if ((command_list[0] == cmd_name))
        {
            return true;
        }
    }
    return false;
}


void MultiSession::processControl()
{
    cout << "got bytes" << endl;
    // this should do something
    char buf[1024];

    int bytesRead = read(0, buf, 1023);

    buf[bytesRead] = '\0';
    cmdBuf += buf;
    // chomp out to the \n

    SessionData* session = sessionList.front();

    while (1)
    {
        string::size_type pos = cmdBuf.find("\n");
        if (pos != string::npos)
        {
            // this is a command
            string cmd;
            cmd = cmdBuf.substr(0, pos);
            split_t myCmd = split(cmd, " ");
            if (cmd_verify(myCmd, "n", 4))
            {
                cout << "new handle" << endl;
                session->newStack(myCmd[1], myCmd[2], myCmd[3]);
            }
            if (myCmd[0] == "v")
            {
                // voice mail mode:
                // play first filename (greeting)
                // play second filename (beep)
                // record third filename
                split_t::iterator i = myCmd.begin();
                i++;
                while (i != myCmd.end())
                {
                    session->addQueue(*i);
                    i++;
                }
                session->shouldUseQueue(true);
            }
            if (cmd_verify(myCmd, "r", 2))
            {
                session->setRecordFile(myCmd[1]);
            }
            if (cmd_verify(myCmd, "sr", 1))
            {
                session->stopRecord();
            }
            if (cmd_verify(myCmd, "p", 2))
            {
                // play file
                session->setPlayFile(myCmd[1]);
            }
            if (cmd_verify(myCmd, "d", 1))
            {
                // delete stack
                session->closeStack();
            }
            if (cmd_verify(myCmd, "s", 2))
            {
                session->setPktSize(atoi(myCmd[1].c_str()));
            }
            if (myCmd[0] == "q")
            {
                session->closeStack();
                exit(0);
            }

            if ((pos + 1) < cmdBuf.length())
            {
                cmdBuf
                = cmdBuf.substr(pos + 1,
                                cmdBuf.length() - (pos + 1));
            }
            else
            {
                cmdBuf = "";
            }
        }
        else
        {
            // no more cmd -- skip
            break;
        }
    }
}


void MultiSession::periodicProcess()
{
    SessionData* session = sessionList.front();
    session->periodicProcess();
}


int MultiSession::setFds(fd_set* fdSet)
{
    int maxFd = 0;

    FD_SET(0, fdSet);   // stdin -- needed for stuff

    SessionData* session = sessionList.front();
    int sessionFd = session->getFd();
    if (sessionFd >= 0)
    {
        FD_SET(sessionFd, fdSet);
        maxFd = sessionFd;
    }
    return maxFd;
}


void MultiSession::processFds(fd_set* fdSet)
{
    if (FD_ISSET(0, fdSet))
    {
        processControl();
    }

    SessionData* session = sessionList.front();
    int sessionFd = session->getFd();
    if (sessionFd >= 0)
    {
        // handle selects
        if (FD_ISSET(sessionFd, fdSet))
        {
            session->receivePacket();
        }
    }
}


void mainAudioLoop()
{
    MultiSession mySession;

    // in the audio stack, handle appropriate stuff
    while (1)
    {
        fd_set readfds;

        struct timeval tv;
        int retval;
        int maxFd = 0;

        // reset file descriptor
        FD_ZERO(&readfds);

        maxFd = mySession.setFds(&readfds);
        maxFd++;

        tv.tv_sec = 0;
        tv.tv_usec = 1000;

        retval = select(maxFd, &readfds, 0, 0, &tv);

        if (retval < 0)
        {
            cout << "select() returned with an error\n";
            return ;
        }

        if (retval > 0)
        {
            mySession.processFds(&readfds);
        }

        // check for too much time elapsing -- do I need to send a packet?

        mySession.periodicProcess();
    }
}



int main (int argc, char *argv[])
{
    mainAudioLoop();

    //    stack.transmitRTCPBYE();

    //    RtpTransmitter* tran = stack.getRtpTran();

    return 0;
}
