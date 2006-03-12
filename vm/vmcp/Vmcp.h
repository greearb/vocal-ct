#ifndef __VMCP_H
#define __VMCP_H

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
#include <string>
#include <sys/types.h>

#include "Streamio.h"
#include "Lineparser.h"
#include <misc.hxx>

struct VmcpSessionInfo 
{
    string CallerId; 	        // CallerId 
    string DialedNumber;	// Originally dialed number 
    string CalledNumber;	// The last forwarded number
    string ForwardReason;	// The reason of the first one
    int    NumberOfForwards;	// Number of forwards	
};


/** This class is a Voice Mail Control Protocol (VMCP) stack. 
  * This protocol is used to communicate between VM Server and 
  * client (sip front end called as User Agent for Voice Mail (UAVM)
  * It has Event types as Ack, Invalid Close, PlayFile etc as well as
  * event handler methods in it.
  */

class Vmcp 
{
    public:
	Vmcp();

	/** Virtual destructor 
	 */
	virtual ~Vmcp();
	
	/** Vmcp EventTypes
	 */
	enum {
		Empty,
		Close,
		Invalid,
		Ack,
		Nack,
		SessionInfo,
		ReqSessionInfo,
		Dtmf,
		PlayFile,
		StartPlay,
		StopPlay,
		PlayStopped,
		RecordFile,
		StartRecord,
		StopRecord,
		RecordStopped
	} EventType;


/** Server methods
 */
    public:
	virtual int wait(int port);
	virtual int assign(int fd,int fd1);
	virtual int accept();
	virtual int getListenSocket() { return m_xSfd; };

/** Client methods
 */
    public:
	virtual int connect(unsigned int port);
	virtual int connect(const char *server,unsigned int port);
	virtual int connect(uint32 server, unsigned int port);

/** Common methods
 */
	virtual int getFd();
	
	/** close vmcp session */    
	virtual	int sendClose();

	/** Send Ack */
	virtual int sendAck();

	/** Send NAck */
	virtual int sendNack();

	/** Get Message */
	virtual int getMsg();

	/** get a pointer to session info */  
	virtual VmcpSessionInfo& getSessionInfo();
	
	/** set session info */    
	virtual void setSessionInfo(const VmcpSessionInfo& info);
	
	/** set session info */    
	virtual void setSessionInfo(
            const string& CallerId,
            const string& DialedNumber,
            const string& CalledNumber,
            const string& ForwardReason,
            int NumberOfForwards);

	/** request session info */ 
	virtual int reqSessionInfo();

	/** send session info */
	virtual int sendSessionInfo();

	/** send DTMF digits */
	virtual int sendDtmf(int code);

	/** get  DTMF digits */
	virtual int getDtmf();

	virtual int playFile(const char *fileName);

	/** start player */
	virtual int startPlay();
	
	/** stop player */  
	virtual int stopPlay();

	virtual int playStopped();

	/** return play file name */
	virtual string& getPlayFileName();

	/** start recorder */ 
	virtual int recordFile(const char *fileName);

	/** start recorder */   	
	virtual int startRecord();
	
	/** stop recorder */ 
	virtual int stopRecord();

	virtual int recordStopped();

	 /** return record file name */ 
	virtual string& getRecordFileName();
	
/** Event handlers
 */
	virtual int OnEmpty();
	virtual int OnInvalidCommand();
	virtual int OnClose(Lineparser *parser);
	virtual int OnSessionInfo(Lineparser *parser);
	virtual int OnReqSessionInfo(Lineparser *parser);
	virtual int OnDtmf(Lineparser *parser);

	virtual int OnPlayFile(Lineparser *parser);
	virtual int OnStartPlay(Lineparser *parser);
	virtual int OnStopPlay(Lineparser *parser);
	virtual int OnPlayStopped(Lineparser *parser);

	virtual int OnRecordFile(Lineparser *parser);
	virtual int OnStartRecord(Lineparser *parser);
	virtual int OnStopRecord(Lineparser *parser);
	virtual int OnRecordStopped(Lineparser *parser);

    protected:
	virtual int processMsg(Lineparser *parser);

/** Protected variables
 */
    protected:
	/** socket stream */
	Streamio m_xIo; 

	/** Parser buffer */
	Parserstring m_xLineBuffer;

	/** Active session info */
	VmcpSessionInfo m_xSessionInfo; 
	
	/**  Last dtmf code received */
	int m_iDtmf; 
	string m_xPlayFileName;
	string m_xRecordFileName;
	int m_xSfd;
};

#endif // __VMCP_H
