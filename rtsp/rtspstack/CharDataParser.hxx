#ifndef CharDataParser_Hxx
#define CharDataParser_Hxx
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


static const char* const CharDataParser_hxx_Version =
    "$Id: CharDataParser.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "CharData.hxx"

/** */
class CharDataParser
{
    public:
        /** */
        CharDataParser(CharData *dataBuf)
            : myCurPtr(dataBuf == NULL ? NULL : dataBuf->getPtr()),
              myEndPtr(dataBuf == NULL ? NULL : 
                      dataBuf->getPtr() + dataBuf->getLen())
            {}
        /** */
        ~CharDataParser() {}


        /* All following methods will return 1 if it finds the stopchar or
           the char meets the mask, and put the chars before stop condition
           in outbuf; return 0 if it cannot find the stop chars 
         */
        /**	Get all chars, and stop before the stopChar */
        int parseUntil(CharData* outBuf, char stopChar);
        /** Get all chars and stop before the char that mask[char]=1 */
        int parseUntil(CharData* outBuf, u_int8_t* stopMask);
        /** Get all chars and stop after the stopChar */
        int parseThru(CharData* outBuf, char stopChar);
        /** Get all chars if mask[char]=1, and stop otherwise */
        int parseThru(CharData* outBuf, u_int8_t* stopMask);

        /** Get all the chars before \r or \n or \r\n, inclusive */
        int getNextLine(CharData* outBuf);
        /** Get all the chars before a non-letter */
        int getNextWord(CharData* outBuf) 
            { return parseUntil(outBuf, myMaskNonWord); }
        /** Get the interger value of the digit chars, only support unsigned */
        int getNextInteger(u_int32_t& num);
        /** Get the double value of the digit and '.' chars */
        int getNextDouble(double& doubleNum);
        /** Get all the chars before a non space char */
        int getThruSpaces(CharData* outBuf) 
            { return parseUntil(outBuf, myMaskNonSpace); }
        /** Get thru length of chars */
        int getThruLength(CharData* outBuf, int length);
        /** Get the current char but not advance curPtr */
        char getCurChar() { return *myCurPtr; }


		static u_int8_t myMaskNonWord[];
		static u_int8_t myMaskDigit[]; 		
		static u_int8_t myMaskNonSpace[];
		static u_int8_t myMaskEol[];	
		static u_int8_t myMaskEolSpace[]; 

	private:
		const char*		myCurPtr;
		const char*		myEndPtr;
		
};


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
