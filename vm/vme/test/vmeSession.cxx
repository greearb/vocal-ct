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
 
#include "State.h"

class VmSession : public Session {
};


class StateVM : public State {
public:
    StateVM(string stateName);
    virtual void setSession(VmSession *session){ State::setSession(session); };
    virtual VmSession *getSession() { return (VmSession *)State::getSession(); };
};


StateVM::StateVM(string stateName):State(stateName)
{
}

class MainState : public StateVM {

public:

    MainState(string stateName);
    ~MainState();
    
    virtual int Process(VmSession *session);
    virtual int ProcessPlayer(pEvent evt);
    virtual int ProcessLine(pEvent evt);
    virtual int ProcessTimer(pEvent evt);
    
private:

    Timer	m_xTimer;
};

MainState::MainState(string stateName):StateVM(stateName)
{
}

MainState::~MainState()
{
}


int MainState::Process(VmSession *session)
{
    setSession(session);
    m_xEventSources.push_back(&m_xTimer);
    getSession()->getLine()->getVmcp()->playFile("test.wav");
    getSession()->getLine()->getVmcp()->startPlay();
    return State::Process(session);
}

int MainState::ProcessPlayer(pEvent evt)
{
    switch(evt->getType())
    {
        case eventVmcpPlayCompleted:
	    {
		getSession()->getLine()->getVmcp()->recordFile("Message.wav");
		getSession()->getLine()->getVmcp()->startRecord();
		m_xTimer.set(30000,0xEAEA);
		m_xTimer.start();
		return StateStay;
	    }
	    break;
	default:
	    return State::ProcessPlayer(evt);
    }
}

int MainState::ProcessLine(pEvent evt)
{
    switch(evt->getType())
    {
	case eventVmcpClose:
	    {
		getSession()->getLine()->getVmcp()->stopRecord();
		getSession()->getLine()->getVmcp()->sendClose();
		return StateReturn;
	    }
	default:
	    return State::ProcessLine(evt);
    }
}

int MainState::ProcessTimer(pEvent evt)
{
    getSession()->getLine()->getVmcp()->stopRecord();
    getSession()->getLine()->getVmcp()->sendClose();
    return StateReturn;
}

int main()
{
    LineVMCP Line;
    Vmcp vmcp;
    vmcp.wait(8024);
    vmcp.accept();
    VmSession session;
    Line.setVmcp(&vmcp);
    session.setLine(&Line);
    Line.start();
    
    MainState mainState("MainState");
    
    mainState.Process(&session);
    Line.stop();
}
