
#ifndef DTMFCOLLECTOR_H
#define DTMFCOLLECTOR_H

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


static const char* const DTMFCollector_h_Version =
"$Id: DTMFCollector.h,v 1.1 2004/05/01 04:15:38 greear Exp $";
                                                                     
#include "global.h"
#include <string>
#include <map>


/** Frequiently used DTMF Codes */

const int DTMF_HASH = 10000;
const int DTMF_STAR = 10001;
const int DTMF_FLASH = 10002;


typedef map <string,unsigned int> TermStrings;
typedef map <unsigned int,unsigned int> TermDigits;

/** DTMF digits Collector class.
  * Whenever the difit gets pressed it's processed by this class 
  */

class DTMFCollector {

public: 
	/** Copy Constructor */
	DTMFCollector();
	
 	/** Destructor */
	virtual ~DTMFCollector();

	/** Digit handler */
  	virtual unsigned int DigitReceived(unsigned int Digit,TermStrings *termStrings = 0, TermDigits *termDigits = 0); 
  
  	/** Handle the termination code */
  	virtual unsigned int TerminationReceived(unsigned int code);
  
  	/** return the termination code */
  	virtual unsigned int GetTermination();
  
  	/** Return the buffer */
  	virtual string * GetString();
  
  	/** Clears the DTMF buffer */
  	virtual void Clear();
  
  	/** Validate the digits pressed */
  	virtual int Validate(TermStrings *);

/** Protected attributes */
protected: 
 	/** Buffer */
  	string buffer;

/** Public attributes */ 
public: 

  	/** Termination Code */
  	unsigned int retCode;
};


typedef DTMFCollector *pDTMFCollector;

#endif
