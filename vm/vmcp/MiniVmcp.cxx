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

static const char* const MiniVmcp_cxx_Version =
    "$Id: MiniVmcp.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include "Vmcp.h"

int main(int argc, char** argv)
{
    Vmcp vm;

    if(argc != 2)
    {
	cout << "usage: " << argv[0] << " hostname" << endl;
	return -1;
    }

    vm.connect(argv[1],8024);

    int ss = vm.getFd();

    vm.setSessionInfo("1000","1001","5000","",0);   

//    vm.sendDtmf(digit);

    while(1) 
    {
	int msg = vm.getMsg();
	
	switch (msg)
	{
	case Vmcp::Close:
	    cout << "closing" << endl;
	    vm.sendClose();
	    close(ss);
	    return 0;
	    break;
	case Vmcp::PlayFile:
	    cout << "play: " << vm.getPlayFileName() << endl;
	    break;
	    
	case Vmcp::StartPlay:
	    cout << "start playing" << endl;
	    {
		bool x = false;
		while(!x)
		{
		    string z;
		    cin >> z;
		    if(z == "q") 
			x = true;
		    else
			vm.sendDtmf('0' + atoi(z.c_str()));
		}
	    }
	    vm.playStopped();
	    break;
	    
	case Vmcp::RecordFile:
	    cout << "record: " << vm.getRecordFileName() << endl;
	    break;
	    
	case Vmcp::StartRecord:
	    cout << "start recording" << endl;
	    {
		bool x = false;
		while(!x)
		{
		    string z;
		    cin >> z;
		    if(z == "q") 
			x = true;
		    else
			vm.sendDtmf('0' + atoi(z.c_str()));
		}
	    }
	    
	    break;
	    
	case Vmcp::StopRecord:
	    cout << "stop recording" << endl;
	    break;
	    
	case Vmcp::StopPlay:
	    cout << "stop playing" << endl;
	    break;     
	    
	default:
	    break;               
	}
    }
//    vm.getPlayFileName();

    vm.sendClose();
    return 0;
}
