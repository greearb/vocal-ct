
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
#include <assert.h>
#include "DTMFCollector.h"

DTMFCollector::DTMFCollector(){
}

DTMFCollector::~DTMFCollector(){
}
/** Digit handler */
unsigned int DTMFCollector::DigitReceived(unsigned int Digit,TermStrings *termStrings,TermDigits *termDigits)
{
	if( Digit != 0 ) buffer+=(char)Digit;
	else
	{
		if( buffer.empty() ) return 0;
		Digit=buffer[buffer.size()-1];
	}

	if( termStrings!=NULL && termStrings->find(buffer) != termStrings->end() )
	{
		return TerminationReceived((*termStrings)[buffer]);
	}
	if( termDigits!=NULL && termDigits->find(Digit) != termDigits->end() )
	{
		buffer.erase(buffer.rfind((char)Digit));
		return TerminationReceived((*termDigits)[Digit]);
	}
	return 0;
}

/** Return the termination code */
 unsigned int DTMFCollector::GetTermination(){
	return retCode;
}

/**  */
unsigned int DTMFCollector::TerminationReceived(unsigned int code){
	retCode=code;
	return code;
}

/** Clears the DTMF buffer */
void DTMFCollector::Clear(){
	buffer.erase();
}

/** Returns the string */
string * DTMFCollector::GetString(){
	return &buffer;
}

/** Validate the string */
int DTMFCollector::Validate(TermStrings *termStrings){
	assert(termStrings);
	map<string,unsigned int>::iterator i=termStrings->begin();
	while( i != termStrings->end() )
	{
   	    //To make it compile on Sun
	    #if defined (__SUNPRO_CC) || (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__INTEL_COMPILER)
		if(!((*i).first.compare((string::size_type)0,buffer.size(),buffer)) ) 
		    return true;
	    #else
		if(!((*i).first.compare(buffer,(string::size_type)0,buffer.size())) ) 
		    return true;  
	    #endif                           
	
            i++;
	}
	return false;
}
