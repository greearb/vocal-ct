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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspTransportHdr_cxx_Version =
    "$Id: RtspTransportHdr.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <stdio.h>
#include "RtspTransportHdr.hxx"
#include "cpLog.h"
#include "support.hxx"

RtspTransportSpec::RtspTransportSpec()
    : myIsTcp(false),
      myIsMulticast(false),
      myIsRecord(false),
      myIsAppend(false),
      myClientPortA(0),
      myClientPortB(0),
      myServerPortA(0),
      myServerPortB(0)
{ }

RtspTransportSpec::RtspTransportSpec(const RtspTransportSpec& x)
{
    myIsTcp = x.myIsTcp;
    myIsMulticast = x.myIsMulticast;
    myIsRecord = x.myIsRecord;
    myIsAppend = x.myIsAppend;
    myClientPortA = x.myClientPortA;
    myClientPortB = x.myClientPortB;
    myServerPortA = x.myServerPortA;
    myServerPortB = x.myServerPortB;
    myDestination = x.myDestination;
    mySource = x.mySource;
}

RtspTransportSpec&
RtspTransportSpec::operator=(const RtspTransportSpec& x)
{
    if (&x != this)
    {
        myIsTcp = x.myIsTcp;
        myIsMulticast = x.myIsMulticast;
        myIsRecord = x.myIsRecord;
        myIsAppend = x.myIsAppend;
        myClientPortA = x.myClientPortA;
        myClientPortB = x.myClientPortB;
        myServerPortA = x.myServerPortA;
        myServerPortB = x.myServerPortB;
        myDestination = x.myDestination;
        mySource = x.mySource;
    }
    return *this;
}

bool
RtspTransportSpec::operator==(const RtspTransportSpec& x) const
{
    return ( (myIsTcp == x.myIsTcp) &&
             (myIsMulticast == x.myIsMulticast) &&
             (myIsRecord == x.myIsRecord) &&
             (myIsAppend == x.myIsAppend) &&
             (myClientPortA == x.myClientPortA) &&
             (myClientPortB == x.myClientPortB) &&
             (myServerPortA == x.myServerPortA) &&
             (myServerPortB == x.myServerPortB) &&
             (myDestination == x.myDestination) &&
             (mySource == x.mySource)
           );
}

void
RtspTransportSpec::reset()
{
    myIsTcp = false;
    myIsMulticast = false;
    myIsRecord = false;
    myIsAppend = false;
    myClientPortA = 0;
    myClientPortB = 0;
    myServerPortA = 0;
    myServerPortB = 0;
    myDestination.erase();
    mySource.erase();
}

void
RtspTransportSpec::parse(CharDataParser& parser)
{
    CharData oneParamData;

    while (parser.parseThru(&oneParamData, ';'))
    {
        CharDataParser paramParser(&oneParamData);
        switch (paramParser.getCurChar())
        {
            case 'R' :
                parseProtocol(paramParser);
                break;
            case 'm' :  // may be mode
                parseMulticastOrMode(paramParser);
                break;
            case 'u' : 
                parseUnicast(paramParser);
                break;
            case 'd' :
                parseDestination(paramParser);
                break;
            case 's' :
                parseSource(paramParser);
                break;
            case 'a' :
                parseAppend(paramParser);
                break;
            case 'c' :
                parseClientPort(paramParser);
                break;
            default :
                break;
        }

        /** To get rid of spaces after ';', and to make sure 
         *  oneParamData start with non spaces
         */
        parser.getThruSpaces(NULL); 

    } 
    
    // parse the last param
    CharDataParser paramParser(&oneParamData);
    switch (paramParser.getCurChar())
    {
        case 'R' :
            parseProtocol(paramParser);
            break;
        case 'm' :  // may be mode
            parseMulticastOrMode(paramParser);
            break;
        case 'u' : 
            parseUnicast(paramParser);
            break;
        case 'd' :
            parseDestination(paramParser);
            break;
        case 's' :
            parseSource(paramParser);
            break;
        case 'a' :
            parseAppend(paramParser);
            break;
        case 'c' :
            parseClientPort(paramParser);
            break;
        default :
            break;
    }
}

void
RtspTransportSpec::parseProtocol(CharDataParser& paramParser)
{
    CharData param;
    paramParser.getNextWord(&param);
    if (param.isEqualNoCase("RTP", 3))
    {
        paramParser.getThruLength(NULL,1); // get thru '/'
        paramParser.getNextWord(&param);
        if (param.isEqualNoCase("AVP", 3))
        {
            paramParser.getThruLength(NULL,1); // get thru '/'
            if (paramParser.getNextWord(&param) != 0)
            {
                if (param.isEqualNoCase("TCP", 3))
                {
                    myIsTcp = true;
                }
            }
        }
        else
        {
            cpLog(LOG_ERR, "Transport hdr: The profile is not AVP.");
        }
    }
    else
    {
        cpLog(LOG_ERR, "Transport hdr: The protocol is not RTP.");
    }
}
 
void
RtspTransportSpec::parseMulticastOrMode(CharDataParser& paramParser)
{
    CharData param;
    paramParser.getNextWord(&param);
    if (param.isEqualNoCase("multicast", 9))
    {
       myIsMulticast = true;
    }
    else
    {
        if (param.isEqualNoCase("mode", 4))
        { 
            // get thru "=""
            paramParser.parseThru(NULL, paramParser.myMaskNonWord);
            paramParser.getNextWord(&param);
            if (param.isEqualNoCase("RECORD", 6))
            {
                myIsRecord = true;
            }
        }
        else
        {
            cpLog(LOG_ERR, "Transport hdr: unknown m parameter.");
        }
    }
}

            
void
RtspTransportSpec::parseUnicast(CharDataParser& paramParser)
{
    CharData param;
    paramParser.getNextWord(&param);
    if (!(param.isEqualNoCase("unicast", 7)))
        cpLog(LOG_ERR, "Transport hdr: unknown u parameter.");
}

void
RtspTransportSpec::parseDestination(CharDataParser& paramParser)
{
    CharData param;
    paramParser.getNextWord(&param);
    if (param.isEqualNoCase("destination", 11))
    {
        paramParser.getThruLength(NULL, 1); // get thru '='
        paramParser.parseUntil(&param, paramParser.myMaskEolSpace);
        if (param[param.getLen() - 1] == ';')
        {
            // get rid of the last ';' in the host
            param.setLen(param.getLen() - 1);
        }
        Data host(param.getPtr(), param.getLen());
        myDestination = host;
    }
    else
    {
        cpLog(LOG_ERR, "Transport hdr: unknown d parameter.");
    }
}

void
RtspTransportSpec::parseSource(CharDataParser& paramParser)
{
    CharData param;
    paramParser.getNextWord(&param);
    if (param.isEqualNoCase("source",6))
    {
        paramParser.getThruLength(NULL, 1); // get thru '='
        paramParser.parseUntil(&param, paramParser.myMaskEolSpace);
        if (param[param.getLen() - 1] == ';')
        {
            // get rid of the last ';' in the host
            param.setLen(param.getLen() - 1);
        }
        Data host(param.getPtr(), param.getLen());
        mySource = host;
    }
    else
    {
        cpLog(LOG_ERR, "Transport hdr: unknown s  parameter.");
    }
}
void
RtspTransportSpec::parseAppend(CharDataParser& paramParser)
{
    CharData param;
    paramParser.getNextWord(&param);
    if (!(param.isEqualNoCase("append", 6)))
        cpLog(LOG_ERR, "Transport hdr: unknown a parameter.");
    else
        myIsAppend = true;
    
}
void
RtspTransportSpec::parseClientPort(CharDataParser& paramParser)
{
    CharData param;
    paramParser.getNextWord(&param);
    if (param.isEqualNoCase("client_port", 11))
    {
        paramParser.getThruLength(NULL, 1); // get thru '='
        u_int32_t clientPort;
        if( paramParser.getNextInteger(clientPort) )
        {
            myClientPortA = clientPort;
            paramParser.getThruLength(NULL, 1); // get thru '-'
            if( paramParser.getNextInteger(clientPort) )
            {
                myClientPortB = clientPort;
            }
        }
        else
        {
            cpLog(LOG_ERR, "Transport hdr: error read client port.");
        }
    }
    else
    {
        cpLog(LOG_ERR, "Transport hdr: unknown c parameter.");
    }

}

Data
RtspTransportSpec::encode()
{
    Data specData;
    if ((!myIsTcp) && (!myIsMulticast))
        specData = "RTP/AVP;unicast;";
    else
        if (myIsTcp)
            specData = "RTP/AVP/TCP;unicast;";
        else
            specData = "RTP/AVP;multicast;";
    
    if (myDestination.length() > 0)
    {
        specData += "destination=";
        specData += myDestination;
        specData += ";";
    }
    if (mySource.length() > 0)
    {
        specData += "source=";
        specData += mySource;
        specData += ";";
    }
    if (myIsAppend)
    {
        specData += "append;";
    }
    if (myClientPortA)
    {
        specData += "client_port=";
        specData += itos(myClientPortA);
        if (myClientPortB)
        {
            specData += "-";
            specData += itos(myClientPortB);
        }
        specData += ";";
    }
    if (myServerPortA)
    {
        specData += "server_port=";
        specData += itos(myServerPortA);
        if (myServerPortB)
        {
            specData += "-";
            specData += itos(myServerPortB);
        }
        specData += ";";
    }
    if (myIsRecord)
    {
        specData += "mode=\"RECORD\"";
    }
    else
    {
        specData += "mode=\"PLAY\"";
    }

    return specData;
}


bool
RtspTransportHdr::isSuitableSpec(Sptr< RtspTransportSpec > spec)
{
    //TODO need to add API, because server should be able to tell 
    // if it is prefer TCP or multicast
    if( spec->myIsTcp | spec->myIsMulticast )
    {
        if (spec->myIsTcp)
           cpLog(LOG_DEBUG_STACK, "TCP for transport media is not suported.");
        else
           cpLog(LOG_DEBUG_STACK, "Multicast for transport is not supported.");
        return false;
    }
    if( spec->myClientPortA == 0 )
    {
        cpLog(LOG_DEBUG, "Client port is not provided");
        return false;
    }

    return true;
}

Sptr< RtspTransportSpec >
RtspTransportHdr::parseTransportHeader(CharDataParser& parser)
{
    Sptr< RtspTransportSpec > spec = new RtspTransportSpec();

    bool found = false;
    CharData oneSpecData;

    while ( (!found) && (parser.parseThru(&oneSpecData, ',')) )
    {
        CharDataParser specParser(&oneSpecData);
        spec->parse(specParser);
        if (isSuitableSpec(spec))
        {
            found = true;
        }
        else
        {
            /** To get rid of spaces after ',', and to make sure 
             *  oneSpecData start with non spaces
             */
            parser.getThruSpaces(NULL);

            spec->reset();
        }
    } 

    if( !found )
    {
        spec->reset();
        CharDataParser specParser(&oneSpecData);
        spec->parse(specParser);
        if (isSuitableSpec(spec))
        {
            found = true;
        }
    }

    if (!found)
    {
//        delete spec;
          spec = 0; //should invoke destructor
    }

    return spec;
    
}

void
RtspTransportHdr::appendTransportSpec(Sptr< RtspTransportSpec > transSpec)
{
    assert (transSpec != 0);

    if (myTransport.length() == 0)
    {
        myTransport = "Transport: ";
    }
    else
    {
        myTransport += ",";
    }
    myTransport += transSpec->encode();
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
