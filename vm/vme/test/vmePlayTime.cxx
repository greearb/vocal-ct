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
#include "PromptServer.h"
#include "cpLog.h"

// Define our class based on State
class MainState : public State {

public:

    MainState(string stateName);
    ~MainState();
    
    virtual int Process(Session *session);

    // We want to catch only player messages
    virtual int ProcessPlayer(pEvent evt);
};

MainState::MainState(string stateName):State(stateName)
{
}

MainState::~MainState()
{
}


// Process state
int MainState::Process(Session *session)
{
    // Save session pointer
    setSession(session);
    
    PromptServer server;
    if( !server.loadTable("promptTable") )
    {
	cerr << "Can't open prompt table" << endl;
	exit(3);
    }


    VmcpSessionInfo info=session->getLine()->getVmcp()->getSessionInfo();
    
    server.m_xIntVar["@Date"]=time(NULL);
    
    vector<string> fileNames;
    
    server.conv("Date",&fileNames,"/usr/home/sdv/vocal/vm/wav/");
    
    
    // Add greeting to the player queue
    for(unsigned int iter=0;iter<fileNames.size();iter++)
    {
	cpLog(LOG_DEBUG,"File:%s",fileNames[iter].c_str());
	getSession()->getLine()->getVmcp()->playFile(fileNames[iter].c_str());
    }
    // Start player
    getSession()->getLine()->getVmcp()->startPlay();
    // Process
    return State::Process(session);
}

int MainState::ProcessPlayer(pEvent evt)
{
    // Event from player
    switch(evt->getType())
    {
	// Play completed. End of session
        case eventVmcpPlayCompleted:
	    {
		cerr << "Play completed" << endl;
		cerr << "Exiting" << endl;
		getSession()->getLine()->getVmcp()->sendClose();
		return StateReturn;
	    }
	    break;
	default:
	    return State::ProcessPlayer(evt);
    }
}

int main()
{
    cpLogSetPriority(LOG_DEBUG);
    // Create LineVMCP (VMCP EventSource) instance
    LineVMCP Line;
    
    // Cretae VMCP instance
    Vmcp vmcp;
    
    // Wait for connection on port 8024
    vmcp.wait(8024);
    
    // Accept connection
    vmcp.accept();
    
    // Create session
    Session session;
    
    // Assign active vmcp to LineVMCP
    Line.setVmcp(&vmcp);
    
    // Set LineVMCP for current session
    session.setLine(&Line);
    
    // Start LineVMCP EventSource
    Line.start();
    
    // Create State
    MainState mainState("MainState");
    
    // Process
    mainState.Process(&session);
    
    // Stop LineVMCP EventSource
    Line.stop();
}
