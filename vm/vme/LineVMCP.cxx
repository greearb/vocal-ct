
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

static const char* const LineVMCP_cpp_Version = 
"$Id: LineVMCP.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "LineVMCP.h"

LineVMCP::LineVMCP ()
{
  m_xType = "LineVMCP";
}

LineVMCP::~LineVMCP ()
{
}

/** add Vcmp socket handle to fdset for select() */
int
LineVMCP::FdSet (fd_set * rfds, fd_set * wfds, fd_set * efds)
{
  m_xRfds = rfds;		// remember rd fdset
  FD_SET (m_xVmcp->getFd (), rfds);	// add socket fd to fdset

  // return the socket handle+1
  return m_xVmcp->getFd () + 1;
}

/** Process source and report events if any */
bool LineVMCP::Process (pEventQueue evt)
{
  // check if there is an activity on vmcp socket
  if (!FD_ISSET (m_xVmcp->getFd (), m_xRfds))
    return false;
  Event
    e;
  e.setSource (this);
  // set event source to sourceVmcp
  e.setSourceType (sourceVmcp);	// set source to Vmcp

  // get next VMCP message
  int
  rc = m_xVmcp->getMsg ();
  switch (rc)
  {
      case Vmcp::Empty:
        return false;

      case Vmcp::Close:		// VmcpClose event. Peer asks us to close a connection
	e.setType (eventVmcpClose);
	evt->push (e);
	m_bActive = false;
	return true;

      case Vmcp::Dtmf:		// DTMF digit arrived
	// set event type to VmcpDtmf
	e.setType (eventVmcpDtmf);
	// push dtmf code to the integer parameters
	e.IParm ()->push_back (m_xVmcp->getDtmf ());
	// push an event to the event queue
	evt->push (e);
	return true;

      case Vmcp::PlayFile:	// PlayFile event.
	// Peer asks us to add a filename to the
	// player queue (Server->Client)
	e.setSourceType (sourcePlayer);
	e.setType (eventVmcpPlayFile);
	// Add a filename to the string parameters list
	e.SParm ()->push_back (m_xVmcp->getPlayFileName ());
	// push an event to the event queue
	evt->push (e);
	return true;

      case Vmcp::RecordFile:	// Record file event.
	// Peer asks us to create and open a
	// file for recording. (Server->Client)
	e.setSourceType (sourceRecorder);
	e.setType (eventVmcpRecordFile);
	// Add a filename to the string parameters list
	e.SParm ()->push_back (m_xVmcp->getRecordFileName ());
	// push an event to the event queue
	evt->push (e);
	return true;

      case Vmcp::SessionInfo:	// Session Info arrived from peer.
	// Ussually as an answer to
	// VmcpReqSessionInfo signal.
	e.setType (eventVmcpEventSessionInfo);
	// push an event to the event queue
	evt->push (e);
	return true;

      case Vmcp::StartPlay:	// Start Player.
	// Peer ask us to start player. Should follow
	// VmcpPlayFile.
	e.setSourceType (sourcePlayer);
	e.setType (eventVmcpPlayStart);
	evt->push (e);
	return true;

      case Vmcp::StartRecord:	// Start Recorder.
	// Peer ask us to start recording.
	// Should follow VmcpRecordFile.
	e.setSourceType (sourceRecorder);
	e.setType (eventVmcpRecordStart);
	evt->push (e);
	return true;

      case Vmcp::StopPlay:	// Stop Player
	e.setSourceType (sourcePlayer);
	e.setType (eventVmcpPlayStop);
	evt->push (e);
	return true;

      case Vmcp::StopRecord:	// Stop Recorder
	e.setSourceType (sourceRecorder);
	e.setType (eventVmcpRecordStop);
	evt->push (e);
	return true;

      case Vmcp::PlayStopped:	// Play completed.
	// Client signals that the player queue is empty.
	e.setSourceType (sourcePlayer);
	e.setType (eventVmcpPlayCompleted);
	evt->push (e);
	return true;

      case Vmcp::RecordStopped:	// Record Completed.
	// Client signals that recording was stopped
	// for some reason.
	e.setSourceType (sourceRecorder);
	e.setType (eventVmcpRecordCompleted);
	evt->push (e);
	return true;
  }

  // Unknown event, just ignore it.

  return false;
}

/** bind a connected socket */
int
LineVMCP::assign(Vmcp *vmcp)
{
  m_xVmcp=vmcp;
  return 1;
}

/** get a pointer to Vmcp instance */
Vmcp *
LineVMCP::getVmcp ()
{
  return m_xVmcp;
}

/** activate */
void
LineVMCP::start ()
{
  m_bActive = true;
}

/** deactivate */
void
LineVMCP::stop ()
{
  m_bActive = false;
}
