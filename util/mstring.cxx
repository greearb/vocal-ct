
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
#include <cstdio>
#include "mstring.hxx"
#include "vsock.hxx"


namespace std
{
//--------------------------------------------------------------------------
mstring & mstring::assigni(int val, const char *format)
{
    char buf[64];
    sprintf(buf, format, val);
    return *this = buf;
}


//--------------------------------------------------------------------------
mstring & mstring::assignd(double val, const char *format)
{
    char buf[64];
    sprintf(buf, format, val);
    return *this = buf;
}


//--------------------------------------------------------------------------
mstring & mstring::ltrims(const string &s)
{
    erase(0, find_first_not_of(s));
    return *this;
}

//--------------------------------------------------------------------------
mstring & mstring::rtrims(const string &s)
{
    size_type pos = find_last_not_of(s);
    if (pos == string::npos) pos = 0;
    else pos++;
    erase(pos);
    return *this;
}


//--------------------------------------------------------------------------
mstring & mstring::toleft(unsigned newlen, char fill, bool cut)
{
    unsigned lstr = length();
    if (lstr < newlen)
    {
        insert(lstr, newlen - lstr, fill);
    }
    if (cut && newlen < lstr)
    {
        erase(newlen, string::npos);
    }
    return *this;
}



//--------------------------------------------------------------------------
mstring & mstring::toright(unsigned newlen, char fill, bool cut)
{
    unsigned lstr = length();
    if (lstr < newlen)
    {
        insert(size_type(0), newlen - lstr, fill);
    }
    if (cut && newlen < lstr)
    {
        erase(size_type(0), lstr - newlen);
    }
    return *this;
}



//--------------------------------------------------------------------------
unsigned mstring::replace_all(const string &find_str, const string &repl_str)
{
    size_type pos = 0;
    size_type find_len = find_str.length();
    size_type repl_len = repl_str.length();
    unsigned nrepl = 0;

    while ((pos = find(find_str, pos)) != string::npos)
    {
        replace(pos, find_len, repl_str);
        pos += repl_len;
        nrepl++;
    }
    return nrepl;
}



//--------------------------------------------------------------------------
int mstring::compare(const string &str, size_type len) const
{
    if (len == string::npos)
    {
        return strcmp(c_str(), str.c_str());
    }
    return strncmp(c_str(), str.c_str(), len);
}




//--------------------------------------------------------------------------
int mstring::comparei(const string &str, size_type len) const
{
    if (len == string::npos)
    {
        return strcasecmp(c_str(), str.c_str());
    }
    return strncasecmp(c_str(), str.c_str(), len);
}




//--------------------------------------------------------------------------
mstring & mstring::tab2sp(unsigned tab_size)
{
    size_type pos = 0;

    while ((pos = find('\t')) != string::npos)
    {
        size_type n_sp = tab_size - pos % tab_size;
        replace(pos, 1, n_sp, ' ');
        pos += n_sp;
    }
    return *this;
}




//---------------------------------------------------------------------------------
mstring & mstring::ins_pair(const string &chrset, char pair_symb)
{
    string::size_type i;
    char prev = 0;

    for (i = 0; i < length(); ++i)
    {
        if ((chrset.find((*this)[i]) != string::npos) == (prev != pair_symb))
        {
            insert(i++, 1, pair_symb);
        }
        prev = (*this)[i];
    }
    if (i > 0 && (*this)[i - 1] == pair_symb) append(1, pair_symb);
    return *this;


}




//---------------------------------------------------------------------------------
mstring & mstring::del_pair(char pair_symb)
{
    string::size_type i;

    for (i = 0; i < length(); i++)
    {
        if ((*this)[i] == pair_symb)
        {
            erase(i, 1);
            if ((*this)[i] == 0) break;
        }
    }
    return *this;
}



//---------------------------------------------------------------------------------
mstring & mstring::ins_escaped(const string &chrset, char escaped_symb)
{
    ///!!! for further implementation
    return *this;
}

//---------------------------------------------------------------------------------
mstring & mstring::del_escaped(char escaped_symb)
{
    ///!!! for further implementation
    return *this;
}


//---------------------------------------------------------------------------------
mstring & mstring::quote(const string &q_start, const string &q_end)
{
    insert(0, q_start);
    append(q_end);
    return *this;
}



//---------------------------------------------------------------------------------
/**
Get the next token from string

@return:  the next position in string to be continue or 
          string::npos if no more tokens

@param start              Start position of the string - 0 or 
                          the previous returned value

@param sep                String with possible separators. Every symbol
                          of this string is separator

@param quote              String with quotation symbols, for example, "'"
                          Inside quote string is not divided. Quotation can
                          start with any of these symbols, but finish with
                          the same symbol only.

@param pair_chr           Usually '\' symbol to include quotation symbols
                          inside the quote parts

@param sep_flag           Flag sep_single or sep_multiple to determine, 
                          whether the separators, going successively are 
                          nerpreted as detached empty tokens (sep_single) 
                          or one token (sep_multiple), as strtok does.
                          sep_single:   string "123,,456" will be "123", "", "456"
                          sep_multiple: string "123,,456" will be "123", "456"
\\================================================================================*/
string::size_type mstring::next_token(size_type start, const string &sep,
                                      const string &quote, char pair_chr,
                                      sep_flag flag) const
{
    size_type count = 0;
    char quote_chr = 0;

    if (start == string::npos) return string::npos;

    const char *pstr = c_str() + start;

    if (*pstr == 0) return string::npos;


    if (flag == sep_multiple)
    {
        //Pass all the separators symbols at the begin of the string
        while (*pstr && sep.find(*pstr) != string::npos)
        {
            ++pstr;
            ++start;
        }
    }

    for (count = 0; ; count++)
    {
        char c = *pstr++;

        if (c == 0)
        {
            start = string::npos;
            break;
        }

        //Outside quote find one of separator symbols
        if (quote_chr == 0 && sep.find(c) != string::npos)
        {
            start += count;
            break;
        }

        //Switch quote. If it is not a quote yet, try to check any of
        //quote symbols. Otherwise quote must be finished with quote_symb
        if (quote_chr == 0)
        {
            if (quote.find(c) != string::npos)
            {
                quote_chr = c;
                continue;
            }
        }
        else
        {
            //Inside quote pass all the pair symbols
            if (pair_chr && c == pair_chr)
            {
                if (*pstr)
                {
                    ++count;
                    ++pstr;
                }
                continue;
            }
            if (c == quote_chr)
            {
                quote_chr = 0;
                continue;
            }
        }
    }
    return start;
}



//---------------------------------------------------------------------------------
/**
Get the next token from string

@return:  the next position in string to be continue or 
          string::npos if no more tokens


@param dst                Pointer to the destination string. 
                          If this parameter is null
                          the function may be used to counting 
                          tokens

@param start              Start position of the string - 0 or 
                          the previous returned value

@param sep                String with possible separators. Every symbol
                          of this string is separator

@param quote              String with quotation symbols, for example, "'"
                          Inside quote string is not divided. Quotation can
                          start with any of these symbols, but finish with
                          the same symbol only.

@param pair_chr           Usually '\' symbol to include quotation symbols
                          inside the quote parts

@param sep_flag           Flag sep_single or sep_multiple to determine, 
                          whether the separators, going successively are 
                          nerpreted as detached empty tokens (sep_single) 
                          or one token (sep_multiple), as strtok does.
                          sep_single:   string "123,,456" will be "123", "", "456"
                          sep_multiple: string "123,,456" will be "123", "456"
\\================================================================================*/
string::size_type mstring::token(string *dst, size_type start, const string &sep,
                                 const string &quote, char pair_chr,
                                 sep_flag flag) const
{
    size_type count = 0;
    char quote_chr = 0;

    if (dst) dst->erase();

    if (start == string::npos) return string::npos;

    const char *pstr = c_str() + start;
    if (*pstr == 0) return string::npos;

    size_type sep_len = 1;
    if (flag == sep_whole_str) sep_len = sep.length();
    const char *psep = sep.c_str();

    if (flag == sep_multiple)
    {
        //Pass all the separators symbols at the begin of the string
        while (*pstr && sep.find(*pstr) != string::npos)
        {
            ++pstr;
            ++start;
        }
    }

    for (count = 0; ; count++)
    {
        char c = *pstr;
        bool found = false;

        //Outside quote find one of separator symbols
        if (quote_chr == 0)
        {
            if (sep_len == 1)
            {
                found = sep.find(c) != string::npos;
            }
            else
            {
                found = strncmp(psep, pstr, sep_len) == 0;  //sep.compare(0, sep_len, pstr, sep_len) == 0;
            }

        }

        ++pstr;

        if (c == 0 || found)
        {
            if (dst) dst->assign(*this, start, count);

            //The next time it will be the next separator character
            //But we must check, whether it is the end of the string.
            start += count;
            if (c) start += sep_len;
            break;
        }

        //Switch quote. If it is not a quote yet, try to check any of
        //quote symbols. Otherwise quote must be finished with quote_symb
        if (quote_chr == 0)
        {
            if (quote.find(c) != string::npos)
            {
                quote_chr = c;
                continue;
            }
        }
        else
        {
            //Inside quote pass all the pair symbols
            if (pair_chr && c == pair_chr)
            {
                if (*pstr)
                {
                    ++count;
                    ++pstr;
                }
                continue;
            }
            if (c == quote_chr)
            {
                quote_chr = 0;
                continue;
            }
        }
    }
    return start;
}


//-------------------------------------------------------------------------------------
/**
Split up the string into the many pieces. This function uses token function.

@return  Number of found tokens

@param str_vec             Pointer to the destination vector of strings. 
                           If this parameter is null
                           the function may be used to counting 
                           tokens

@param sep                 String with possible separators. Every symbol
                           of this string is separator

@param quote               String with quotation symbols, for example, "'"
                           Inside quote string is not divided. Quotation can
                           start with any of this symbol, but finish with
                           the same symbol only.

@param pair_chr            Usually '\' symbol to include quotation symbols
                           inside the quote parts

@param flag                Flag sep_single or sep_multiple to determine, 
                           whether the separators, going successively are 
                           nerpreted as detached empty tokens (sep_single) 
                           or one token (sep_multiple), as strtok does.
                           sep_single:   string "123,,456" will be "123", "", "456"
                           sep_multiple: string "123,,456" will be "123", "456"
                           
\\================================================================================*/
unsigned mstring::split(mstr_vector *str_vec, const string &sep,
                        const string &quote, char pair_chr, sep_flag flag) const
{
    if (str_vec) str_vec->clear();

    unsigned count = 0;
    size_type start = 0;
    mstring tmp;

    while ((start = token(&tmp, start, sep, quote, pair_chr, flag)) != string::npos)
    {
        if (str_vec) str_vec->push_back(tmp);
        count++;
    }
    return count;
}




//-------------------------------------------------------------------------------------
/**
Check quote parts balance.

@return true = balance OK, false - no balance
   

@param quote               String with quotation symbols, for example, "'"
                           Quotation can start with any of this symbol, 
                           but finish with
                           the same symbol only.

@param pair_chr            Usually '\' symbol to include quotation symbols
                           inside the quote parts
\\================================================================================*/
bool mstring::chkquote(const string &quote, char pair_chr) const
{
    char quote_chr = 0;

    const char *pstr = c_str();
    char c;

    while ((c = *pstr++) != 0)
    {
        //Switch quote. If it is not a quote yet, try to check any of
        //quote symbols. Otherwise quote must be finished with quote_symb
        if (quote_chr == 0)
        {
            if (quote.find(c) != string::npos)
            {
                quote_chr = c;
                continue;
            }
        }
        else
        {
            //Inside quote pass all the pair symbols
            if (pair_chr && c == pair_chr)
            {
                if (*pstr)
                {
                    ++pstr;
                }
                continue;
            }
            if (c == quote_chr)
            {
                quote_chr = 0;
                continue;
            }
        }
    }
    return quote_chr == 0;
}



//-------------------------------------------------------------------------------------
/**
    Get the next BRACKET token from string. 
    Parsing nested brackets can be done recursively. For example:	
    \begin{verbatim}
    void print_brackets(int ident, std::mstring str, std::string &quote, char pair)
    {
        std::mstring tmp;
        std::string::size_type start = 0;
        int bal;
  
       while((start = str.brtok(&tmp, start, &bal, 
                      '+', '-', quote, pair)) != 
              std::string::npos)
       {
           for(unsigned i = 0; i < ident; i++) cout << '*';
           cout << tmp.c_str() << " = " << bal << "\n";
           print_brackets(ident+1, tmp, quote, pair);
       }
    }
    \end{verbatim}
  @return  the next position in string to be continue or 
           string::npos if no more tokens
 
 
  @param dst                 Pointer to the destination string. 
                             If this parameter is null
                             the function may be used to counting 
                             tokens
 
  @param start               Start position in the string - 0 or 
                             the previous returned value
 
  @param balance             Returns bracket balance
                             positive - a lake of closing brackets
                             negatine - an overage of closing brackets
 
  @param br_open             Open bracket character
 
  @param br_close            Close bracket character
 
  @param quote               String with quotation symbols, for example, "'"
                             Inside quote string is not divided. Quotation can
                             start with any of this symbol, but finish with
                             the same symbol only.
 
  @param pair_chr            Usually '\' symbol to include quotation symbols
                             inside the quote parts
\\================================================================================*/
string::size_type mstring::brtok(string *dst, size_type *start, int *balance,
                                 char br_open, char br_close,
                                 const string &quote, char pair_chr) const
{
    int brbal = 0;
    char quote_chr = 0;
    size_type count = 0;

    size_type start_pos = string::npos;
    size_type end_pos = string::npos;

    if (dst) dst->erase();

    if (*start == string::npos) return string::npos;

    const char *pstr = c_str() + *start;

    if (*pstr == 0) return string::npos;

    char c;
    for (count = 0; (c = *pstr++) != 0; count++)
    {
        //Outside quote find a bracket
        if (quote_chr == 0)
        {
            if (c == br_open)
            {
                if (start_pos == string::npos && brbal == 0) start_pos = count;
                brbal++;
            }

            if (c == br_close)
            {
                brbal--;
                if (end_pos == string::npos && brbal == 0) end_pos = count;
            }
        }

        //Switch quote. If it is not a quote yet, try to check any of
        //quote symbols. Otherwise quote must be finished with quote_symb
        if (quote_chr == 0)
        {
            if (quote.find(c) != string::npos)
            {
                quote_chr = c;
                continue;
            }
        }
        else
        {
            //Inside quote pass all the pair symbols
            if (pair_chr && c == pair_chr)
            {
                if (*pstr)
                {
                    ++count;
                    ++pstr;
                }
                continue;
            }
            if (c == quote_chr)
            {
                quote_chr = 0;
                continue;
            }
        }
    }

    if (start_pos != string::npos && end_pos != string::npos)
    {
        if (dst) dst->assign(*this, *start + start_pos + 1,
                                 end_pos - start_pos - 1);
        ++end_pos;
    }

    if (balance) *balance = brbal;

    if (end_pos == string::npos) return npos;

    end_pos += *start;
    *start += start_pos;
    return end_pos;
}


}
