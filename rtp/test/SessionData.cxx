
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

static const char* const SessionData_cxx_Version =
    "$Id: SessionData.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "SessionData.hxx"


SessionData::SessionData()
        : stack(0),
        playFileFd( -1),
        recFileFd( -1),
        state(0),
        pktSize(40)
{}



void
SessionData::receivePacket()
{
    RtpPacket* packet = 0;
    if (stack)
    {
        cout << "-";
        packet = stack->receive();
    }
    if (packet)
    {
        if (recFileFd >= 0)
        {
            cout << "x";
            if (packet->getPayloadUsage() != (8 * pktSize))
            {
                cout << "not the right size!" << endl;
            }
            write (recFileFd,
                   packet->getPayloadLoc(), packet->getPayloadUsage());
        }
        delete packet;
        packet = 0;
    }
    else
    {
        cout << "no packet!" << endl;
    }
}

void
SessionData::transmitPacket()
{
    RtpPacket* packet = 0;

    if (stack)
    {
//        packet = stack->createPacket(pktSize * 8);
        packet = stack->createPacket();

        if (playFileFd >= 0)
        {
            int bytecount =
                read(playFileFd,
                     packet->getPayloadLoc(), packet->getPayloadSize());
            if (bytecount)
            {
                packet->setPayloadUsage (bytecount);
                //      outRtpPkt->setRtpTime(audioStack->getPrevRtpTime() + NETWORK_RTP_RATE);
                stack->transmit (packet);
            }
            else
            {
                // this file is now empty -- close and quit
                close(playFileFd);
                playFileFd = -1;
                delete packet;
                packet = 0;
                if (state == 1)
                {
                    // i'm done!
                    state = 2;
                }
            }
        }
    }
}


void SessionData::newStack(
    const string& localPort,
    const string& remoteHost,
    const string& remotePort )
{
    int localPortNumber = atoi(localPort.c_str());
    int remotePortNumber = atoi(remotePort.c_str());

    if ((localPortNumber < 1024) || (remotePortNumber < 1024))
    {
        return ;
    }

    closeStack();

    stack = new RtpSession(remoteHost.c_str(), remotePortNumber,
                           localPortNumber, remotePortNumber + 1,
                           localPortNumber + 1, rtpPayloadPCMU,
                           rtpPayloadPCMU, 5);

    stack->setApiPktSize (pktSize * 8);
    stack->setNetworkPktSize (pktSize * 8);

    // set SDES information - optional
    RtcpTransmitter* rtcpTran = stack->getRtcpTran();
    rtcpTran->setSdesEmail("development@vovida.com");

    state = 0;
}


void
SessionData::addQueue(const string& str)
{
    queueList.push_back(str);
}


void
SessionData::shouldUseQueue(bool val)
{
    if (val)
    {
        if (state == 0)
        {
            state = 2;
        }
    }
    else
    {
        state = 0;
    }
}

void
SessionData::closeStack()
{
    close(playFileFd);
    close(recFileFd);
    playFileFd = -1;
    recFileFd = -1;
    if (stack)
    {
        stack->transmitRTCPBYE();
        delete stack;
    }
    state = 0;
    stack = 0;
}


void
SessionData::setPktSize(int size)
{
    pktSize = size;
}

void
SessionData::setPlayFile(const string& filename)
{
    cout << "playing file: " << filename << endl;

    if (playFileFd >= 0)
    {
        close(playFileFd);
    }

    // do something about this
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0)
    {
        cout << "error opening file for reading: " << filename << endl;
        cout << "reason: " << strerror(errno) << endl;
        // this is an error
    }

    playFileFd = fd;
}


void SessionData::stopRecord()
{
    // stop recording
    close(recFileFd);
    recFileFd = -1;
}

void SessionData::setRecordFile(const string& filename)
{
    cout << "record file: " << filename << endl;

    if (recFileFd >= 0)
    {
        close(recFileFd);
    }

    // do something about this
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        cout << "error opening file for writing!" << endl;
        cout << "reason: " << strerror(errno) << endl;
        // this is an error
    }

    recFileFd = fd;
}


void
SessionData::handleQueue()
{
    // do something about the queue items

    if (state == 2)
    {
        if (queueList.size())
        {
            string cmd = (queueList.front());
            queueList.pop_front();
            state = 1;
            // now, figure out what this means
            if (cmd.length() >= 2)
            {
                if (cmd[0] == 'p')
                {
                    // set the play buffer
                    setPlayFile(cmd.substr(2, cmd.length() - 2));
                }
                if (cmd[0] == 'r')
                {
                    // set the record buffer
                    setRecordFile(cmd.substr(2, cmd.length() - 2));
                }
            }
        }
        else
        {
            // all done!
            state = 0;
        }
    }
}


int SessionData::getFd()
{
    if (stack)
    {
        return stack->getRtpRecv()->getSocketFD();
    }
    else
    {
        return -1;
    }
}

void SessionData::periodicProcess()
{
    VTime currentTime;
    int deltaMs =
        currentTime - lastTimePktSent;
    if (deltaMs > (pktSize - 2))
    {
        // send a packet
        transmitPacket();
        lastTimePktSent = lastTimePktSent + pktSize;
    }

    if (state == 2)
    {
        handleQueue();
    }

    if (stack)
    {
        stack->processRTCP();
    }
}
