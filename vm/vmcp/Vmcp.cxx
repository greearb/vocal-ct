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

#include"Vmcp.h"
#include "cpLog.h"
#include <stdio.h>
#include <errno.h>

Vmcp::Vmcp()
{
}

Vmcp::~Vmcp()
{
}

int Vmcp::assign(int ifd,int ofd)
{
    m_xIo.assign(ifd,ofd);
    return 1;
}

int Vmcp::wait(int port)
{
    sockaddr_in sin;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    sin.sin_addr.s_addr=INADDR_ANY;
    m_xSfd=socket(AF_INET,SOCK_STREAM,0);
    if( m_xSfd == - 1 )
    {
	cpLog(LOG_ERR,"Error 1");
	return -1;
    }
    int one = 1;			// option value of 1
    ::setsockopt (m_xSfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof (one));
    if( ::bind(m_xSfd,(sockaddr *)&sin,sizeof(sin)) == - 1) 
    {
	cpLog(LOG_ERR,"Error 2");
	return -1;
    }
    if( ::listen(m_xSfd,4) == -1 ) 
    {
	cpLog(LOG_ERR,"Error 3");
	return -1;
    }
    return m_xSfd;
}

int Vmcp::accept()
{
    int fd;
    sockaddr_in sin;
#if defined(__APPLE__)
    int sinsize=0;
#else
    unsigned int sinsize=0;
#endif

    bool again = true;

    while( again ) 
    { 
	fd = ::accept(m_xSfd,(sockaddr *)&sin,&sinsize);
	if( fd == -1  )
	{
	    if( errno != EINTR )
	    {
		::perror("");
		return -1;
	    }
	}
	else 
	{
	    again = false;
	}
    }



    m_xIo.assign(fd,fd);
    return fd;
}

int Vmcp::connect(unsigned int port)
{
    return connect(INADDR_ANY,port);
}

int Vmcp::connect(const char *server,unsigned int port)
{
    hostent *ent=gethostbyname(server);
    return connect(*(unsigned long *)ent->h_addr,port);
}

int Vmcp::connect(unsigned int addr,unsigned int port)
{
    sockaddr_in in;

    in.sin_family = AF_INET;
    in.sin_port = htons (port);
    in.sin_addr.s_addr = addr;
    int fd;

    fd=::socket(AF_INET,SOCK_STREAM,0);
    if( fd==-1 ) return 0;
    if (::connect (fd, (sockaddr *) & in, sizeof (in)) == -1) return 0;
    m_xIo.assign(fd,fd);
    return fd;
}

int Vmcp::getMsg()
{
    while( m_xIo.isReady() )
    {
	char c=m_xIo.read();
	if( c == -1 ) 
	{
	    cpLog(LOG_DEBUG, "got error, interpreting as close\n");
	    m_xLineBuffer.erase();
	    return Close;
	}
	if( c == 0 )
	{
	    cpLog(LOG_DEBUG, "got NUL, ignoring\n");

	} 
	else if( m_xLineBuffer.push(c) )
	{
	    cpLog(LOG_DEBUG, "VMCP got line: %s", m_xLineBuffer.c_str());
	    Lineparser parser;
	    parser << m_xLineBuffer;
	    m_xLineBuffer.erase();
	    int rc=processMsg(&parser);
	    parser.freeze(0);
	    return rc;
	}
    }
    return OnEmpty();
}

int Vmcp::processMsg(Lineparser *parser)
{
    string cmd;
    *parser >> cmd;

    cpLog(LOG_DEBUG,"Vmcp: %s",cmd.c_str());	
    if( cmd=="CLOSE" ) return OnClose(parser);
    else if( cmd=="SESSIONINFO" ) return OnSessionInfo(parser);
    else if( cmd=="REQSESSIONINFO" ) return OnReqSessionInfo(parser);
    else if( cmd=="DTMF" ) return OnDtmf(parser);
    else if( cmd=="PLAYFILE" ) return OnPlayFile(parser);
    else if( cmd=="STARTPLAY" ) return OnStartPlay(parser);
    else if( cmd=="STOPPLAY" ) return OnStopPlay(parser);
    else if( cmd=="PLAYSTOPPED" ) return OnPlayStopped(parser);
    else if( cmd=="RECORDFILE" ) return OnRecordFile(parser);
    else if( cmd=="STARTRECORD" ) return OnStartRecord(parser);
    else if( cmd=="STOPRECORD" ) return OnStopRecord(parser);
    else if( cmd=="RECORDSTOPPED" ) return OnRecordStopped(parser);
    else if( cmd=="ACK" ) return Ack;
    else if( cmd=="NACK" ) return Nack;
    else if( cmd=="INVALID COMMAND") return Ack;
    else { cpLog(LOG_ERR,"Invalid command :%s",cmd.c_str()); return OnInvalidCommand(); }
}

int Vmcp::sendDtmf(int code)
{
    int rc;
    strstream cmd;
    cmd << "DTMF|" << code << "\r\n";
    cmd.put(0);
    rc=m_xIo.write(cmd.str());
    cmd.freeze(0);
    return rc;
}

int Vmcp::getDtmf()
{
    return m_iDtmf;
}

void Vmcp::setSessionInfo(const VmcpSessionInfo& info)
{
    m_xSessionInfo=info;
}

void Vmcp::setSessionInfo(const string& CallerId,const string& DialedNumber,const string& CalledNumber,const string& ForwardReason,int NumberOfForwards)
{
    m_xSessionInfo.CallerId=CallerId;
    m_xSessionInfo.DialedNumber=DialedNumber;
    m_xSessionInfo.CalledNumber=CalledNumber;
    m_xSessionInfo.ForwardReason=ForwardReason;
    m_xSessionInfo.NumberOfForwards=NumberOfForwards;
}

int Vmcp::reqSessionInfo()
{
    string cmd="REQSESSIONINFO\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::playFile(const char *fileName)
{
    string cmd="PLAYFILE|";
    cmd += fileName;
    cmd += "\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::startPlay()
{
    string cmd="STARTPLAY\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::stopPlay()
{
    string cmd="STOPPLAY\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::playStopped()
{
    string cmd="PLAYSTOPPED\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::recordFile(const char *fileName)
{
    string cmd= "RECORDFILE|";
    cmd += fileName;
    cmd += "\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::startRecord()
{
    string cmd="STARTRECORD\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::stopRecord()
{
    string cmd="STOPRECORD\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::recordStopped()
{
    string cmd="RECORDSTOPPED\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::sendSessionInfo()
{
    int rc;
    strstream cmd;
    cmd     << "SESSIONINFO|"
	    << m_xSessionInfo.CallerId << "|"
	    << m_xSessionInfo.DialedNumber << "|" 
	    << m_xSessionInfo.CalledNumber << "|" 
	    << m_xSessionInfo.ForwardReason << "|" 
	    << m_xSessionInfo.NumberOfForwards << "\r\n";

    cmd.put(0);
    rc=m_xIo.write(cmd.str());
    cmd.freeze(0);
    return rc;
}

VmcpSessionInfo& Vmcp::getSessionInfo()
{
    return m_xSessionInfo;
}

string& Vmcp::getPlayFileName()
{
    return m_xPlayFileName;
}

string& Vmcp::getRecordFileName()
{
    return m_xRecordFileName;
}

int Vmcp::OnClose(Lineparser *parser)
{
    return Close;
}

int Vmcp::OnEmpty()
{
    return Empty;
}

int Vmcp::OnSessionInfo(Lineparser *parser)
{
    *parser  >> m_xSessionInfo.CallerId
	     >> m_xSessionInfo.DialedNumber
	     >> m_xSessionInfo.CalledNumber
	     >> m_xSessionInfo.ForwardReason
	     >> m_xSessionInfo.NumberOfForwards;

    sendAck();
    return SessionInfo;
}

int Vmcp::OnReqSessionInfo(Lineparser *parser)
{
    sendSessionInfo();
    return ReqSessionInfo;
}

int Vmcp::OnDtmf(Lineparser *parser)
{
    *parser >> m_iDtmf;
    sendAck();
    return Dtmf;
}

int Vmcp::OnPlayFile(Lineparser *parser)
{
    *parser >> m_xPlayFileName;
    sendAck();
    return PlayFile;
}

int Vmcp::OnStartPlay(Lineparser *parser)
{
    sendAck();
    return StartPlay;
}

int Vmcp::OnStopPlay(Lineparser *parser)
{
    sendAck();
    return StopPlay;
}

int Vmcp::OnPlayStopped(Lineparser *parser)
{
    sendAck();
    return PlayStopped;
}

int Vmcp::OnRecordFile(Lineparser *parser)
{
    *parser >> m_xRecordFileName;
    sendAck();
    return RecordFile;
}

int Vmcp::OnStartRecord(Lineparser *parser)
{
    sendAck();
    return StartRecord;
}

int Vmcp::OnStopRecord(Lineparser *parser)
{
    sendAck();
    return StopRecord;
}

int Vmcp::OnRecordStopped(Lineparser *parser)
{
    sendAck();
    return RecordStopped;
}

int Vmcp::getFd()
{
    return m_xIo.getInputFd();
}

int Vmcp::OnInvalidCommand()
{
    string cmd="INVALID COMMAND\r\n";
    m_xIo.write(cmd);
    return Invalid;
}

int Vmcp::sendAck()
{
    string cmd="ACK\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::sendNack()
{
    string cmd="NACK\r\n";
    return m_xIo.write(cmd);
}

int Vmcp::sendClose()
{
    string cmd="CLOSE\r\n";
    return m_xIo.write(cmd);
}


/* Local Variables: */
/* c-file-style:"stroustrup" */
/* c-basic-offset:4 */
/* c-file-offsets:((inclass . ++)) */
/* indent-tabs-mode:nil */
/* End: */

