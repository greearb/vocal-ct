#ifndef __MSTRING_H__
#define __MSTRING_H__

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





static const char* const mstringHeaderVersion =
    "$Id: mstring.hxx,v 1.2 2004/11/19 01:54:38 greear Exp $";


#include "global.h"
#include <string>
#include <vector>
#include <stdlib.h>

namespace std
{
class mstring;

///mstr_vector definition typedef vector<mstring> mstr_vector
typedef std::vector < mstring > mstr_vector;

/**mstring class provides basic parser methods as geting tokens,
 *splitting, brackets processing an so on. */
class mstring : public string
{
    public:

        /** Separator flag sep_single or sep_multiple.
         *
         *  sep_single - Single mode - each symbol is a single separator 
         *                            (1,2,,,3,4 will be "1"  "2"  ""  ""  "3"  "4")
         *  sep_multiple - Multiple mode - all symbols are one separator 
         *                            (1,,2,,3,,4 will be "1"  "2"  "3"  "4") 
         *  sep_whole_str - Whole string is a single separator
         *                            (1,,2,3,,4 will be "1"  "2,3"  "4") 
         */
        enum sep_flag
        {
            sep_single,
            sep_multiple,
            sep_whole_str
    };

        ///Default Constructor
        mstring() : string()
        {}

        ///Constructor from char*

        mstring(const char *str) : string(str)
        {}

        ///Copy Constructor

        mstring(const string &str) : string(str)
        {}

        ///Assignment from int

        mstring & assigni(int val, const char *format);

        ///Assignment from double
        mstring & assignd(double val, const char *format);


        ///Assignment from int
        mstring & operator = (int val)
        {
            return assigni(val, "%d");
        }

        //        ///Assignment from double
        //        mstring & operator = (double val) { return assignd(val, "%.1f"); }

        ///Conversion to int
        operator int() const
        {
            return atoi(c_str());
        }

        ///Conversion to double
        operator double() const
        {
            return atof(c_str());
        }

        ///Trim left all the characters in s
        mstring & ltrims(const string &s);

        ///Trim right all the characters in s
        mstring & rtrims(const string &s);

        ///Trim left and right all the characters in s
        mstring & atrims(const string &s)
        {
            ltrims(s);
            return rtrims(s);
        }

        ///Trim left the character c
        mstring & ltrimc(char c)
        {
            char s[2] = {c, 0};
            return ltrims(s);
        }

        ///Trim right the character c
        mstring & rtrimc(char c)
        {
            char s[2] = {c, 0};
            return rtrims(s);
        }

        ///Trim left and right the character c
        mstring & atrimc(char c)
        {
            char s[2] = {c, 0};
            return atrims(s);
        }

        ///Trim left whitespaces
        mstring & ltrim()
        {
            return ltrims("\t ");
        }

        ///Trim right whitespaces
        mstring & rtrim()
        {
            return rtrims("\t ");
        }

        ///Trim left and right whitespaces
        mstring & atrim()
        {
            return atrims("\t ");
        }

        ///Left align to 'newlen' with filler 'fill'
        mstring & toleft(unsigned newlen, char fill = ' ', bool cut = false);

        ///Right align to 'newlen' with filler 'fill'
        mstring & toright(unsigned newlen, char fill = ' ', bool cut = false);

        ///Find and replace all the 'find_str' to 'repl_str'
        unsigned replace_all(const string &find_str, const string &repl_str);

        /// Convert all tabs to spaces
        mstring & tab2sp(unsigned tab_size = 8);

        ///Compare two strings. The function is provided just for convinience and uniformity
        int compare(const string &str, size_type len = string::npos) const;

        ///Compare two strings ignoring case. The function is provided just for convinience and uniformity
        int comparei(const string &str, size_type len = npos) const;

        ///Compare two strings. The function is provided just for convinience and uniformity
        friend int compare(const mstring &str1, const mstring &str2, size_type len = string::npos)
        {
            return str1.compare(str2, len);
        }

        ///Compare two strings ignoring case. The function is provided just for convinience and uniformity
        friend int comparei(const mstring &str1, const mstring &str2, size_type len = string::npos)
        {
            return str1.comparei(str2, len);
        }

        ///Insert pair-symb into the string
        mstring & ins_pair(const string &chrset, char pair_symb);

        ///Delete pair-symb from the string
        mstring & del_pair(char pair_symb);

        ///Insert escaped symbols into the string
        mstring & ins_escaped(const string &chrset, char escaped_symb);

        ///Delete escaped symbols from the string
        mstring & del_escaped(char escaped_symb);

        ///Add quote symbols into the string
        mstring & quote(const string &q_start, const string &q_end);

        //see mstring.cxx
        size_type next_token(size_type start, const string &sep,
                             const string &quote, char pair_chr = 0,
                             sep_flag flag = sep_multiple) const;

        //see mstring.cxx
        size_type token(string *dst, size_type start, const string &sep,
                        const string &quote, char pair_chr = 0,
                        sep_flag flag = sep_multiple) const;

        //see mstring.cxx
        unsigned split(mstr_vector *vec, const string &sep, const string &quote,
                       char pair_chr = 0, sep_flag flag = sep_multiple) const;

        //see mstring.cxx
        size_type brtok(string *dst, size_type *start, int *balance,
                        char br_open, char br_close,
                        const string &quote, char pair_chr = 0) const;

        //see mstring.cxx
        bool chkquote(const string &quote, char pair_chr = 0) const;
};
}







#endif
