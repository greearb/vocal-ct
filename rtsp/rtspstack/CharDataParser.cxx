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

static const char* const CharDataParser_cxx_Version =
    "$Id: CharDataParser.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <stdio.h>
#include <assert.h>
#include "CharDataParser.hxx"

u_int8_t CharDataParser::myMaskNonWord[] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //0-9 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //10-19 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //20-29
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //30-39 
	1, 1, 1, 1, 1, 0, 1, 1, 1, 1, //40-49 '-' consider  a word
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //50-59
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, //60-69 65-90 are word
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //70-79
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //80-89
	0, 1, 1, 1, 1, 0, 1, 0, 0, 0, //90-99 '_' consider a word, 97-122 are word
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //100-109
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //110-119
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, //120-129
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //130-139
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //140-149
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //150-159
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //160-169
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //170-179
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //180-189
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //190-199
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //200-209
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //210-219
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //220-229
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //230-239
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //240-249
	1, 1, 1, 1, 1, 1 			  //250-255
};


u_int8_t CharDataParser::myMaskDigit[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0-9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //10-19 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //20-29
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //30-39
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, //40-49 48-57 are digit
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, //50-59
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //60-69 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //70-79
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //80-89
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //90-99
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //100-109
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //110-119
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //120-129
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //130-139
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //140-149
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //150-159
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160-169
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //170-179
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //180-189
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //190-199
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //200-209
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //210-219
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //220-229
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //230-239
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //240-249
	0, 0, 0, 0, 0, 0			  //250-255
};

u_int8_t CharDataParser::myMaskNonSpace[] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, //0-9      '\t' is space
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //10-19  
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //20-29
	1, 1, 0, 1, 1, 1, 1, 1, 1, 1, //30-39    ' ' is space 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //40-49
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //50-59
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //60-69
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //70-79
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //80-89
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //90-99
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //100-109
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //110-119
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //120-129
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //130-139
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //140-149
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //150-159
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //160-169
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //170-179
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //180-189
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //190-199
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //200-209
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //210-219
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //220-229
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //230-239
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //240-249
	1, 1, 1, 1, 1, 1 			  //250-255
};

u_int8_t CharDataParser::myMaskEol[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0-9   
	1, 0, 0, 1, 0, 0, 0, 0, 0, 0, //10-19  '\r' & '\n' are EOL
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //20-29
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //30-39 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //40-49
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //50-59
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //60-69  
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //70-79
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //80-89
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //90-99
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //100-109
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //110-119
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //120-129
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //130-139
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //140-149
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //150-159
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160-169
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //170-179
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //180-189
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //190-199
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //200-209
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //210-219
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //220-229
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //230-239
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //240-249
	0, 0, 0, 0, 0, 0 			  //250-255
};

u_int8_t CharDataParser::myMaskEolSpace[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, //0-9   	'\t' is space
	1, 0, 0, 1, 0, 0, 0, 0, 0, 0, //10-19   '\r' & '\n' are EOL
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //20-29
	0, 0, 1, 0, 0, 0, 0, 0, 0, 0, //30-39 	' '  is space
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //40-49
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //50-59
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //60-69  
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //70-79
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //80-89
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //90-99
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //100-109
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //110-119
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //120-129
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //130-139
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //140-149
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //150-159
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160-169
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //170-179
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //180-189
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //190-199
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //200-209
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //210-219
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //220-229
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //230-239
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //240-249
	0, 0, 0, 0, 0, 0 			  //250-255
};


int
CharDataParser::parseUntil(CharData* outbuf, char stopChar)
{
	assert(myCurPtr != NULL);
	assert(myEndPtr != NULL);
	assert(myCurPtr <= myEndPtr);

	const char* startSave = myCurPtr;

	while ((myCurPtr < myEndPtr) && (*myCurPtr != stopChar))
		myCurPtr++;
		
	if (outbuf != NULL)
	{
		outbuf->set(startSave, myCurPtr - startSave);
	}

    if (myCurPtr == myEndPtr)
        return 0;

    return 1;
}

int
CharDataParser::parseUntil(CharData* outbuf, u_int8_t* mask)
{
	assert(myCurPtr != NULL);
	assert(myEndPtr != NULL);
	assert(myCurPtr <= myEndPtr);

	const char* startSave = myCurPtr;

	while ((myCurPtr < myEndPtr) && (!mask[*myCurPtr]))
		myCurPtr++;

	if (outbuf != NULL)
	{
		outbuf->set(startSave, myCurPtr - startSave);
	}

    if (myCurPtr == myEndPtr)
        return 0;

    return 1;
}

int
CharDataParser::parseThru(CharData* outbuf, char stopChar)
{
    if ( parseUntil(outbuf, stopChar) )
    {
        myCurPtr++;
        if (outbuf)
            outbuf->setLen(outbuf->getLen() + 1);
        return 1;
    }
    return 0;
}

int
CharDataParser::parseThru(CharData* outbuf, u_int8_t* mask)
{
	assert(myCurPtr != NULL);
	assert(myEndPtr != NULL);
	assert(myCurPtr <= myEndPtr);

	const char* startSave = myCurPtr;

	while ((myCurPtr < myEndPtr) && (mask[*myCurPtr]))
		myCurPtr++;

	if (outbuf != NULL)
	{
		outbuf->set(startSave, myCurPtr - startSave);
	}

    if (myCurPtr == startSave)
        return 0;

    return 1;
}

int 
CharDataParser::getNextLine(CharData* outbuf)
{
	int retVal = 0;
    if ( parseUntil(outbuf, myMaskEol) )
    {
	    if ((*myCurPtr == '\r') || (*myCurPtr == '\n'))
	    {
		    retVal = 1;
		    myCurPtr++;
            if (outbuf)
                outbuf->setLen(outbuf->getLen() + 1);
		    //check for a \r\n
		    if ((myCurPtr < myEndPtr) && ((*(myCurPtr - 1) == '\r') 
                && (*myCurPtr == '\n')))
            {
			    myCurPtr++;
                if (outbuf)
                    outbuf->setLen(outbuf->getLen() + 1);
            }
	    }
    }
	return retVal;
}

int
CharDataParser::getNextInteger(u_int32_t& num)
{
    CharData numberChar;
    if ( parseThru(&numberChar, myMaskDigit) )
    {
	    u_int32_t value = 0;
	
        for (u_int32_t i = 0; i < numberChar.getLen(); i++)
        {
            value = (value * 10) + (numberChar[i] - '0');
        }
	    num = value;
        return 1;
    }
    return 0;
}

int
CharDataParser::getNextDouble(double& doubleNum)
{
    doubleNum = 0.0;
    u_int32_t decimal;
    if (getNextInteger(decimal))
    {
        doubleNum = decimal;
        if (*myCurPtr == '.')
            myCurPtr++;
    }
    else
    {
        if (*myCurPtr == '.')
            myCurPtr++;
        else
            return 0;
    }
    double multip = .1;
    CharData fractChar;
    if ( parseThru(&fractChar, myMaskDigit) )
    {
        for (u_int32_t i = 0; i < fractChar.getLen(); i++)
        {
            doubleNum += (multip * (fractChar[i] - '0'));
            multip *= .1;
        }
    }
    return 1;
}

int
CharDataParser::getThruLength(CharData* outbuf, int length)
{
    assert(myCurPtr != NULL);
    assert(myEndPtr != NULL);
    assert(myCurPtr <= myEndPtr);
    int ret = 1;

    if ((myEndPtr - myCurPtr) < length)
    {
        length = myEndPtr - myCurPtr;
        ret = 0;
    }

    if (outbuf != NULL)
    {
        outbuf->set(myCurPtr, length);
    }

    myCurPtr += length;

    return ret;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
