#ifndef SpeechHandler_HXX
#define SpeechHandler_HXX

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

static const char* const SpeechHandler_hxx_Version =
    "$Id: SpeechHandler.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";

#ifdef WIN32
#include <posixwin32.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif


#include <stdio.h>
#include <cassert>

extern "C" {
#include "s2types.h"
#include "CM_macros.h"
#include "basic_types.h"
#include "CM_macros.h"
#include "err.h"
#include "ad.h"
#include "cont_ad.h"
#include "kb.h"
#include "fbs.h"

};


#include "ThreadIf.hxx"
#include "vtypes.h"
#include "Data.hxx"
#include "Fifo.h"


namespace Vocal
{

class SpeechHandler : public ThreadIf
{
    public:
        ///
        SpeechHandler(Fifo<Data>& utteranceFifo) : 
            myUtteranceFifo(utteranceFifo), myAd(0), myCont(0), 
            myInitFlag(false), myBuf(0),
            myStartUtterance(false)
        { };

        ///Initialize the decode engine
        void initialize();

        ///
        virtual ~SpeechHandler();
        ///
        const char* const name() const { return "SpeechHandler"; };

        ///
        void recognize(unsigned char* buf, int size);
    private:
        ///
        void calibrateDevice();

        ///
        Data recognize_impl(unsigned char* buf, int size);

        ///
        void thread();

        ///
        bool playWhatYouGot(char* hypt, search_hyp_t** alt, int32 n_alt);

	Fifo<Data>& myUtteranceFifo;

        ///Dirctory that contains the language model and all the files
        ///necessary to initialize the decoder
        Data myModelDataDir;
        ///
        Data myHMMDir;
        ///
        ad_rec_t *myAd;            
        ///
        cont_ad_t *myCont;         
        ///
        bool  myInitFlag;

        ///
        int32  myStartwid;
        ///
        unsigned char* myBuf;
        ///
        int32          myPktSize;
        ///
        bool           myStartUtterance;
};
 
}

#endif
