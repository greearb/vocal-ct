#ifndef _BASE_COMMAND_LINE_HXX_
#define _BASE_COMMAND_LINE_HXX_

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

static const char* const BaseCommandLine_hxx_Version =
    "$Id: BaseCommandLine.hxx,v 1.2 2004/06/06 08:32:36 greear Exp $";


#include "global.h"
#include <string>

/** BaseCommandLine
<pre>
<br> Usage of this Class </br>

      BaseCommandData is a structure which can be passed into
      a baseCommandLine call by the main() program of a proxy.
      baseCommandLine creates an instance of the CommandLine base class,
      copies the command line arguments into the BaseCommandData structure
      and reads/configures the retransmission parameters for the
      sipstack.

</pre>
*/
struct BaseCommandData {
   string localIp; /* Local IP to bind to */
   int sipPort;
   string pServer;
   int pServerPort;
   string backupPServer;
   int backupPServerPort;
   string debugLevel;
   string readSecretFile;
   string writeSecretFile;
   bool useTls; //when talking to the provisioning server
};


/**
 * baseCommandLine creates an instance of the CommandLine base class,
 * copies the command line arguments into the BaseCommandData structure
 * and reads/configures the retransmission parameters for the
 * sipstack.
 */   
int baseCommandLine(const int argc, const char** argv, BaseCommandData* data);


#endif // _BASE_COMMAND_LINE_HXX_
