#ifndef SUPPORT_HXX_
#define SUPPORT_HXX_

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

static const char* const support_hxx_version =
    "$Id: support.hxx,v 1.2 2006/03/12 07:41:28 greear Exp $";

#include "global.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <deque>
#include <string>
#include "substring.h"
#include "Data.hxx"

/***********************************************************************
 
  Utility and support functions for the stack.
 
 **********************************************************************/

#ifndef __sparc

#if !(defined(__FreeBSD__) || defined(__APPLE__))
typedef deque < substring > sub_split_t;
typedef deque < string > split_t;
#else
// Apple OSX
typedef vector < substring > sub_split_t;
typedef vector < string > split_t;
#endif

#else
typedef vector < substring > sub_split_t;
typedef vector < string > split_t;
#endif


/** paren_match will return the data that is in the first matched
    set of parentheses.
 
    e.g. if the text is like this:
 
    foo(la, la, la)
 
    it will return a substring pointing to "la, la, la" (no quotes,
    obviously)
 
    or, if it is
 
    (a, b(d, e(g, h), f), c)
 
    it will return "a, b(d, e(g, h), f), c" .
 
    paren_match will work both on strings and substrings.
 
*/
substring
paren_match(const string& inputText);


/// same as the one for strings, but for substrings
substring
paren_match(const substring& inputText);

/// the same as above, but it returns a string, not a substring
string
str_paren_match(const string& inputText);


/** splits a string into a vector of substrings while first matching
   parentheses.
 
   This function will NOT match the characters when they are inside
   brackets.  At the moment, this is specialized to only match "()" as
   brackets, although this may change in the future.
 
   sub_split_paren_patch does not copy the strings itself.  Instead,
   it substrings, which are pointers into the original string and
   length counts.
 
   The substring.h file contains the useful information about what a
   substring is, and what you can do with one.
 
   Clearly, this was influenced by perl.  
*/
sub_split_t
sub_split_paren_match(const string& inputText, const string& characters);


/** splits a string into a vector of strings while first matching
   parentheses.
 
   This function will NOT match the characters when they are inside
   brackets.  At the moment, this is specialized to only match "()" as
   brackets, although this may change in the future.
 
   split_paren_patch copies the strings itself.  
 
   Clearly, this was influenced by perl.  */
split_t
split_paren_match( const string& inputText, const string& characters);


/**
   splits a string into a vector of substrings. 
 
   This version is more efficient than split() because it does not
   copy the strings itself.  Instead, it uses substrings, which are
   pointers into the original string and length counts.
 
   The substring.h file contains the useful information about what a
   substring is, and what you can do with one.
 
   Clearly, this was influenced by perl.  */
sub_split_t
sub_split(const string& inputText, const string& characters);


/**
   splits a string into a vector of substrings. 
 
   This is inefficient because copying is generally involved here.  A
   more efficient implementation which uses pointers into the original
   string (and length counts) may replace this in a future version.
 
   Clearly, this was influenced by perl.
*/
split_t
split(const string& inputText, const string& characters);


/// slower, new split code
vector < string >
new_split(const string& inputText, const string& characters);


/**
   removes the trailing character from a string.
 
   Clearly, this was influenced by perl.
*/
void
chop(string* input);


/**
    removes the trailing character from a string if it is \r or \n.
 
    Clearly, this was influenced by perl.
*/
void
chomp(string* input);


/**
   convert an int to a string
*/
inline string itos(unsigned int i)
{
    char buf[32];
    sprintf(buf, "%d", i);
    return buf;
}


/// convert an int to a base 16 (hex) string
inline string itohexs(unsigned int i)
{
    char buf[32];
    sprintf(buf, "%x", i);
    return buf;
}


/**
   returns a string converted to lowercase.
*/
string
str2lower(string str);


/**
   returns a string converted to lowercase.
*/
string
c2lower_s(const char* cstr);


/**
   check to see if a string is all legal digit characters
*/
#ifndef __MINGW32__
int
matchString(string a, string regex);
#endif


/**
    get the integer from the string s, e.g. "   2598   ", it may contain
    arbituary spaces in the front or end, but not in between digits
    otherwise, the first whole integer is read and returned,
    e.g. "  235   67" is returned as 235.
    if true is returned, the num variable contains a valid integer for use
*/
bool stringToInt ( const string& s, int *num);


/**
    Assume the string s contains an integer or a number
    range between two integers, e.g. 20 or 23-45.
    Returns: If string s contains only a single integer, it is
    returned in start variable and isRange is set to false, and the function
    in whole returns true.
    If string s contains a range, start variable will contain
    the starting number, and end variable will contain the ending
    number, and isRange is set to true if the range is valid, i.e. start < end.
    the function in whole returns true.
    Any other case, the function returns false.
*/
bool getRange( const string& s, int* start, int* end, bool* isRange);


/// convert data to an unsigned char array
int convertToUnsigned(const Data& data, unsigned char* dest);


/// convert Hex-encoded ASCII in src to a Data containing the appropriate data
Data convertToHex(const unsigned char* src, int len);

#endif
