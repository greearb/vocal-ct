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


static const char* const UaFacade_cxx_Version = 
    "$Id: UaCli.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include <unistd.h>
#include <stdio.h>
#include "UaCli.hxx"
#include "UaConfiguration.hxx"
#include "cpLog.h"
#include <Data.hxx>
#include <Lock.hxx>
#include "GuiEvent.hxx"

using namespace Vocal;
using namespace Vocal::UA;


void showDebug() {
   cout << " Debug info:  Data count: " << Data::getInstanceCount()
        << " Data Buffers: " << Data::getTotalBuff() << endl;
}

UaCli::UaCli(int readFd, int writeFd)
   : myReadFd(readFd),
     myWriteFd(writeFd),
     shutdown(false),
     inCall(false)
{
    cpLog(LOG_DEBUG, "Reading on (%d), writing on (%d)",
                      readFd, writeFd);
    myReadThread.spawn(readerThrWrapper, this);

    // Don't start this guy if we're embedded in LANforge...
    if (UaConfiguration::instance().getValue(ReadStdinTag) != "0") {
       myKeyinThread.spawn(keyinThrWrapper, this);
    }
}

void*
UaCli::readerThrWrapper(void* args)
{
    UaCli* self = static_cast<UaCli*>(args);
    self->readThr();
    return 0;
}

void*
UaCli::keyinThrWrapper(void* args)
{
    UaCli* self = static_cast<UaCli*>(args);
    self->keyinThr();
    return 0;
}

UaCli::~UaCli()
{
    shutdown = true;
    write(myReadFd, "X", 1);
    myReadThread.join();
    myKeyinThread.join();
}

void 
UaCli::readThr()
{
   char buf[1024];
   memset(buf, 0, 1024);

   int rv;
   while (!shutdown) {
      if ((rv = read(myReadFd, buf, 1023)) < 0) {
         cpLog(LOG_ERR, "Failed to read input");
         continue;
      }
      if (shutdown)
         break;
      buf[rv] = 0; //Null terminate the fellow.
      cpLog(LOG_DEBUG, "UaCli::readThr: Read %d bytes, string: -:%s:-\n", rv, buf);

      // Due to the lame-assed messaging protocol, we can read several messages
      // at once.  They seem to be separated by a "|" character.  I will pray
      // that that character is not used in normal messages...
      char* msg = buf;
      char tmp;
      int sofar = 0;
      bool done_one = false;
      int i = 0;
      while (sofar < rv) {
         if (msg[i] == '|') {
            if (sofar+1 < rv) {
               tmp = msg[i+1];
               msg[i+1] = 0;
               done_one = true;
               parseInput(msg);
               msg[i+1] = tmp;
               msg = msg + i + 1;
               i = 0;
            }
            else {
               done_one = true;
               parseInput(msg);
               break;
            }
         }
         else {
            i++;
         }
         sofar++;
      }//for

      if (!done_one) {
         parseInput(msg);
      }
   }

   cpLog(LOG_ERR, "NOTE:  Done with UaCli reader thread!!\n");
}

void 
UaCli::keyinThr()
{
    cout << "\nVocal UA\n!!! Type ? for help  !!!" << endl; 
    cout << "Not Registered" << endl;
    while(1)
    {
        if(shutdown) break;
        cout << "gua>" ;
        char buf[256];
        memset(buf, 0, 256);
        cin.getline(buf, 256);
        cpLog(LOG_INFO, "Read from keyboard:%s\n", buf);
        if(buf[0] == 'q') shutdown = true;
        parseInput(buf);
    }
}

void 
UaCli::parseInput(const string& input)
{
    Lock lck(lock); //Stack lock, only let one thing in here at a time.

    // Don't handle anything if we're embedded in LANforge...
    if (UaConfiguration::instance().getValue(ReadStdinTag) == "0") {
       return;
    }

    cpLog(LOG_DEBUG, "UaCli::parseInput, input -:%s:-\n", input.c_str());
    if(input.size() == 0) return;
    if(input.size() == 1)
    {
        switch(input[0])
        {
            case '\n': return;
            break;
            case '?': help();
            break;
            case 'c': printConfig();
            break;
            case 'q': 
            {
                cout << "Exiting.. wait..." << endl;
                writeToController(UA_SHUTDOWN_STR); 
            }
            break;
            case 'a':
            {
                //Accept a Call
                writeToController(UA_ACCEPT_STR); 
            }
            break;
            case 'd':
                //Debug
                showDebug();
                break;
            case 'z':
            {
                //Hangup a Call
                writeToController(UA_STOP_STR);
            }
            break;
	    case 'h':
	    {
		writeToController(UA_HOLD_STR);
	    }
	    break;
	    case 'r':
	    {
		writeToController(UA_RESUME_STR);
	    }
	    break;
           case 's':
           {
              writeToController(UA_DOSUBSCRIBE_STR);
           }
	    case 'v':
	    {
		writeToController(UA_REGISTRATIONEXPIRED_STR);
	    }
	    break;
            default:
                cout << "Unrecognized input:" << input[0] << endl;
            break;
        }
        return;
    }

    string::size_type pos = input.find("call ");
    if((pos = input.find("call ")) != string::npos)
    {
        string rhs = input.substr(pos+5); 
        string ctlString(UA_INVITE_STR);
        ctlString += " ";
        ctlString += rhs;
        cerr << "Calling :" << ctlString << endl;
        UaCli::writeToController(ctlString);
        //Sendit to the Controller
    }
    else if((pos = input.find("REGISTRATIONEXPIRED")) != string::npos)
    {
        cout << "\ngua>" << "Expired Registration...\ngua>"; 
    }
    else if((pos = input.find("REGISTER")) != string::npos)
    {
        if(pos == 0)
           cout << "\ngua> Registered OK \ngua>";
    }
    else if((pos = input.find("RINGING")) != string::npos)
    {
        cout << "\ngua> Ringing!!\ngua>";
    }
    else if((pos = input.find("STOP")) != string::npos)
    {
        cout << "\ngua>" << "Call ended\ngua>";
    }
    else if((pos = input.find("BUSY")) != string::npos)
    {
        cout << "\ngua>" << "Callee busy\ngua>"; 
    }
    else if((pos = input.find("HOLD")) != string::npos)
    {
        cout << "\ngua>" << "Call is on Hold\ngua>"; 
    }
    else if((pos = input.find("RESUME")) != string::npos)
    {
        cout << "\ngua>" << "Resuming...\ngua>"; 
    }
    else if((pos = input.find("UNAUTHORIZED")) != string::npos)
    {
        cout << "\ngua>" << "Caller not authorized to make call\ngua>"; 
    }
    else if((pos = input.find("ERROR")) != string::npos)
    {
        cout << "\ngua>" << input.substr(pos+5).c_str() << "\ngua>";
    }
    else if((pos = input.find("INFO")) != string::npos)
    {
        cout << "\ngua>" ;
    }
    else if((pos = input.find("quit")) != string::npos)
    {
        cout << "Exiting.. wait..." << endl;
        writeToController(UA_SHUTDOWN_STR); 
    }
    else
    {
        //Do nothing
        cout << "\ngua>" ;
    }
}

void
UaCli::writeToController(string txt)
{
    if(write(myWriteFd, txt.c_str(), txt.length()) < 0)
    {
        cpLog(LOG_ERR, "Failed to write (%s) to controller", txt.c_str());
    }
}

void
UaCli::help()
{
   cout << "? - help" << endl; 
   cout << "c - Configuration" << endl; 
   cout << "a - To accept a call." << endl;
   cout << "z - To stop a call." << endl;
   cout << "q - To Quit gua." << endl;
   cout << "h - To Hold a call." << endl;
   cout << "r - To Resume the Hold call." << endl;
   cout << "v - To Unregister with proxy, send register with contact: * , expire: 0" <<endl;
   cout << "To make a call:" << endl;
   cout << "call user" << endl; 
   cout << "call user@host" << endl; 
   cout << "call user@host:port" << endl; 
   cout << "call user@host:port" << endl; 
   cout << "call user@host:port;user=phone" << endl; 
   cout << "call sip:user@host:port;user=phone" << endl; 
}

void
UaCli::printConfig()
{
    cout << "SIP User Agent Configuration"  << endl;
    cout <<  "--- User ---" << endl;
    cout <<  "              Name : " <<  UaConfiguration::instance().getValue(UserNameTag).c_str() << endl;
    cout <<  "      Display Name : " <<  UaConfiguration::instance().getValue(DisplayNameTag).c_str() << endl;
    cout <<  "         Pass Word : " <<  UaConfiguration::instance().getValue(PasswordTag).c_str() << endl;

    cout <<  "\n--- SIP ---" << endl;
    cout <<  "              Port : " <<  UaConfiguration::instance().getValue(LocalSipPortTag).c_str() << endl;
    cout <<  "         Transport : " <<  UaConfiguration::instance().getValue(SipTransportTag).c_str() << endl;
    cout <<  "      Proxy Server : " <<  UaConfiguration::instance().getValue(ProxyServerTag).c_str() << endl;

    cout <<  "\n--- Registration ---" << endl;
    cout <<  "          Register : " <<  UaConfiguration::instance().getValue(RegisterOnTag).c_str()<< endl;
    cout <<  "              From : " <<  UaConfiguration::instance().getValue(RegisterFromTag).c_str() << endl;
    cout <<  "                To : " <<  UaConfiguration::instance().getValue(RegisterToTag).c_str() << endl;
    cout <<  "           Expires : " <<  UaConfiguration::instance().getValue(RegisterExpiresTag).c_str()<< endl;

    cout <<  "\n--- RTP ---" << endl;
    cout <<  "      Min RTP Port : " <<  UaConfiguration::instance().getValue(MinRtpPortTag).c_str() << endl;
    cout <<  "      Max RTP Port : " <<  UaConfiguration::instance().getValue(MaxRtpPortTag).c_str()<< endl;


    cout <<  "\n\n" << endl;
}
