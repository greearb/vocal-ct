
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




#ifndef PARSE3TUPLE_H
#define PARSE3TUPLE_H

static const char* const parse3tupleHeaderVersion =
    "$Id: parse3tuple.h,v 1.1 2004/05/01 04:15:33 greear Exp $";




#define TUPLE3_MAX_LENGTH 1024
#define TAG_MAX_LENGTH 96
#define TYPE_MAX_LENGTH 32
#define VALUE_MAX_LENGTH TUPLE3_MAX_LENGTH - TAG_MAX_LENGTH - TYPE_MAX_LENGTH

/*
 * parse3tuple() parses file fname line by line. Each line is expected
 * to have 3 fields, <tag>-<type>-<value>, separated by blanks or tabs.
 * Lines starting with '#' are comments. Empty lines are ignored.
 * The value field contains a number of characters, even field separators
 * like blanks and tabs, up to the end of the line.
 * User of this utility provides a call back function with 3 arguments
 * (pointers to the tag, type and value strings) to validate and use the
 * data.
 * usage example: if (!parse3tuple ("cs.cfg", processCsConfigData))
 *                    exit (0);
 */
extern bool parse3tuple (const char*, void (*f) (char*, char*, char*));

extern int strConst2i (const char*str, const char* strTable[], const int tableSize);

/* PARSE3TUPLE_H  */
#endif
