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


static const char* const vstring_cxx_Version =
    "$Id: vstring.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include <iostream>
#include <cctype>
#include <cstdlib>

#include "global.h"
#include "vstring.hxx"

const size_t    Vocal::MAX_LINE_LENGTH = 1024;
const string    Vocal::SEPARATORS(":= \t");
const string    Vocal::COMMENT_CHARS("#!;");
const string    Vocal::QUOTE_CHARS("\"'`");

const char      Vocal::NL('\n');
const string    Vocal::TAB1(" \t");
const string    Vocal::TAB2(" \t\t");
const string    Vocal::TAB3(" \t\t\t");


string &
Vocal::upper(string & src)
{
    size_t size = src.size();
    
    for ( size_t i = 0; i < size; i++ )
    {
        src[i] = toupper(src[i]);
    }
    
    return ( src );
}


string &
Vocal::lower(string & src)
{
    size_t size = src.size();
    
    for ( size_t i = 0; i < size; i++ )
    {
        src[i] = tolower(src[i]);
    }
    
    return ( src );
}


string &
Vocal::remove_lws(string & src)
{
    size_t  size = src.size();
    size_t  i;
        
    for ( i = 0; i < size; ++i )
    {
        if ( !isspace(src[i]) )
        {
            break;
        }
    }

    src = src.substr(i);
    
    return ( src );
}


string &
Vocal::remove_tws(string & src)
{
    size_t size = src.size();
    size_t i;
    
    for ( i = size; i > 0; --i )
    {
        if ( !isspace(src[i-1]) )
        {
            break;
        }
    }
    
    src.erase(i);
    
    return ( src );
}


string &
Vocal::remove_ws(string & src)
{
    remove_lws(src);
    remove_tws(src);
    
    return ( src );
}


string &
Vocal::remove_comments(
    string          &   src, 
    const string    &   comment_chars
)
{
    size_t  position = src.find_first_of(comment_chars);
        
    if ( position != string::npos )
    {
        src.erase(position);
    }

    return ( remove_ws(src) );
}


string &
Vocal::remove_quotes(
    string          &   src,
    const string    &   quote_chars
)
{
    remove_ws(src);
    
    size_t position = src.find_first_not_of(quote_chars);

    if ( position == string::npos )
    {
        src = "";
    }
    else
    {
        src = src.substr(position);
        position = src.find_last_not_of(quote_chars);
        src = src.substr(0, position+1);
    }
    
    return ( remove_ws(src) );
}


bool     
Vocal::getline(
    istream         &   in, 
    string          &   line, 
    size_t          &   line_num,
    const string    &   comment_chars
)
{
    char    buffer[Vocal::MAX_LINE_LENGTH];
    
    while ( true )
    {
        memset(buffer, 0, Vocal::MAX_LINE_LENGTH);

        if ( in.getline(buffer, Vocal::MAX_LINE_LENGTH).eof() == true )
        {
            return ( false );
        }
        
        ++line_num;

        line = buffer;
        remove_ws(line);

        if  (   line.size() != 0 
            &&  comment_chars.find(line[0]) == string::npos 
            )
        {
            break;
        }
    }

    return ( true );
}


string   
Vocal::getnext(
    string          &   src, 
    const string    &   separators,
    const string    &   comment_chars
)
{
    size_t  position = src.find_first_of(separators);
    string  result   = src.substr(0, position);

    if ( position == string::npos )
    {
        src = "";
    }
    else
    {
        src      = src.substr(position);
        position = src.find_first_not_of(separators);
        src      = src.substr(position);
        remove_comments(src);
    }
    
    return ( result );
}


bool     
Vocal::parse_property(
    string          &   line, 
    const string    &   prop_name, 
    string          &   value,
    const string    &   separators,
    const string    &   comment_chars
)
{
    if ( prop_name.size() == 0 )
    {
        return ( false );
    }
    
    string  property_name(prop_name);
    string  name = getnext(line, separators, comment_chars);
    
    if ( upper(name) != upper(property_name) )
    {
        return ( false );
    }

    value = line;
    
    return ( true );
}


bool     
Vocal::parse_value(
    string          &   line, 
    const   string  &   prop_name,
    int             &   value,
    const string    &   separators,
    const string    &   comment_chars
)
{
    string  p;
    
    if ( parse_property(line, prop_name, p, separators, comment_chars) )
    {
        value = atoi(p.c_str());
        return ( true );
    }

    return ( false );
}


