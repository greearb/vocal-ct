#if !defined(VOCAL_STRING_HXX)
#define VOCAL_STRING_HXX

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2001 Vovida Networks, Inc.  All rights reserved.
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


static const char* const vstrign_hxx_Version =
    "$Id: vstring.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include <string>
#include "vtypes.h"


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Vocal 
{

using std::string;
using std::istream;

extern const size_t     MAX_LINE_LENGTH;
extern const string     SEPARATORS;
extern const string     COMMENT_CHARS;
extern const string     QUOTE_CHARS;
extern const char       NL;
extern const string     TAB1;
extern const string     TAB2;
extern const string     TAB3;


extern string &     upper(string & src);
extern string &     lower(string & src);


extern string &     remove_lws(string & src);
extern string &     remove_tws(string & src);
extern string &     remove_ws(string & src);


extern string &     remove_comments(
    string          &   src, 
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);

extern string &     remove_quotes(
    string          &   src,
    const string    &   quote_chars = Vocal::QUOTE_CHARS
);


extern bool     getline(
    istream         &   in, 
    string          &   line, 
    size_t          &   line_num,
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);
extern string   getnext(
    string          &   line, 
    const string    &   separators = Vocal::SEPARATORS,
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);


extern bool     parse_property(
    string          &   line, 
    const string    &   name, 
    string          &   new_prop,
    const string    &   separators = Vocal::SEPARATORS,
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);

extern bool     parse_value(
    string          &   line, 
    const string    &   name,
    int             &   new_value,
    const string    &   separators = Vocal::SEPARATORS,
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);

        
} // namespace Vocal


#endif // !defined(VOCAL_STRING_HXX)
