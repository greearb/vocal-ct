
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
#include <cassert>
#include "cpLog.h"

#include "MediaCapability.hxx"
#include "SdpHandler.hxx"

using namespace Vocal::UA;


void 
MediaCapability::addCodec(Sptr<CodecAdaptor> cAdp)
{
    if(cAdp == 0) 
    {
        cpLog(LOG_DEBUG, "MediaCapability::addCodec: Empty adaptor, ignoring...");
        return;
    }
    if(myCodecMap.count(cAdp->getType()))
    {
        cpLog(LOG_DEBUG, "MediaCapability::addCodec: Adaptor for codec (%d) already added.", cAdp->getType());
        return;
    }

    myCodecMap[cAdp->getType()] = cAdp;
}
 
Sptr<CodecAdaptor> 
MediaCapability::getCodec(VCodecType cType)
{
    Sptr<CodecAdaptor> cAdp = 0;
    if(myCodecMap.count(cType))
    {
        cAdp = myCodecMap[cType];
    }
    else
    {
        cpLog(LOG_ERR, "Codec (%d) not supported", cType);
    }
    return cAdp;
}

Sptr<CodecAdaptor>
MediaCapability::getCodec(const string& codecName)
{
    for (CodecMap::iterator itr = myCodecMap.begin();
         itr != myCodecMap.end(); itr++) {
       if ((*itr).second->getEncodingName() == codecName) {
          return (*itr).second;
       }
    }

    // If no match, search by number
    int cType = atoi(codecName.c_str());
    for (CodecMap::iterator itr = myCodecMap.begin();
         itr != myCodecMap.end(); itr++) {
       if ((*itr).second->getType() == cType) {
          return (*itr).second;
       }
    }
    
    Sptr<CodecAdaptor> cAdp = 0;
    return cAdp;
}

//Get the order by priority, higher priority first
struct ltOpr
{
  bool operator()(const int i1, const int i2) const
  {
    return  i2 < i1;;
  }
};
 
list<Sptr<CodecAdaptor> > 
MediaCapability::getSupportedAudioCodecs()
{
    //Give a list of supported codec ordered by the priority,
    //highest priority first
    list<Sptr<CodecAdaptor> > retList;
    map<int, list<Sptr<CodecAdaptor> >, ltOpr > tmpMap;
    for(CodecMap::iterator itr = myCodecMap.begin();
                  itr != myCodecMap.end(); itr++)
    {
        if((*itr).second->getMediaType() == AUDIO)
        {
            tmpMap[itr->second->getPriority()].push_back(itr->second);
        }
    }

    for(map<int, list<Sptr<CodecAdaptor> > >::iterator itr2 = tmpMap.begin();
                 itr2 != tmpMap.end(); itr2++)
    {
        retList.insert(retList.end(), itr2->second.begin(), itr2->second.end());
    }

    return retList;
}

list<Sptr<CodecAdaptor> > 
MediaCapability::getSupportedVideoCodecs()
{
    list<Sptr<CodecAdaptor> > retList;
    for(CodecMap::iterator itr = myCodecMap.begin();
                  itr != myCodecMap.end(); itr++)
    {
        if((*itr).second->getMediaType() == VIDEO)
        {
            retList.push_back((*itr).second);
        }
    }
    return retList;
}

bool
MediaCapability::isSupported(int payloadType, const SdpSession& remoteSdp, int& supportedType)
{
    //Initialise the return type same as give type, which is true for static codec type
    //and may be different for dynamic codec type
    supportedType = payloadType;
    cpLog(LOG_DEBUG, "Checkig for type:%d", payloadType);
    if(payloadType >= 96)
    {
        cpLog(LOG_DEBUG, "Checkig for Dynamic type:%d", payloadType);
        //Handle the dynamic codecs here
        bool payLoadSupported = false;
        for (CodecMap::iterator itr = myCodecMap.begin();
             itr != myCodecMap.end(); itr++)
        {
           int realPayloadType = payloadType;
           if(itr->first < 96) continue; 
           if(checkType(remoteSdp,  realPayloadType, 
                        itr->second->getEncodingName()))
           {
              //support payload
              supportedType = itr->first;
              payLoadSupported =  true;
              cpLog(LOG_DEBUG, "Playload supported :%s", itr->second->getEncodingName().c_str());
              break;
           }
        }
        return payLoadSupported;
    }
    VCodecType cType = static_cast<VCodecType>(payloadType);

    if(payloadType == 18 && myCodecMap.count(cType) > 0)
      cpLog(LOG_DEBUG, "G729 Found");
    else
      cpLog(LOG_DEBUG, "G729 Not Found");

    return (myCodecMap.count(cType) ? true : false );
}
