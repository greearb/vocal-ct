#if !defined(VOCAL_GETOPT_HXX)
#define VOCAL_GETOPT_HXX

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
 * IN EXCESS OF ,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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


#include "NonCopyable.hxx"
#include "NameValue.hxx"

#include <string>
#include <vector>
#include <list>
#include <cstdio>

// Yet another hack here for Sparc
#ifdef sparc
#include "../../contrib/libcext_bsd/getopt_long.h"
#else

#ifdef VOCAL_USE_GETOPT_LONG
#include "getopt_long.h"
#else
#if defined(__svr4__)
#include <stdlib.h>
#else
  #if defined(__APPLE__)
    #include <unistd.h>
  #else
    #include <getopt.h>
  #endif
#endif
#endif
#endif


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Vocal 
{


/** 
 */
namespace Configuration
{


using std::string;
using std::vector;
using std::list;


/**
 */
class GetOpt : public Vocal::NonCopyable
{
    public:
   

        /**
         */ 
        GetOpt();
        
        
        /**
         */
        virtual ~GetOpt() throw (Vocal::SystemException);


        /**
         */
    	ReturnCode  add
        (
            const char      *   longOption,
	    char                optionChar  = '\0',
	    ValueType           valType     = NO_VALUE
        );


        /**
         */
    	ReturnCode  add
        (
            const string &      longOption,
	    char                optionChar  = '\0',
	    ValueType           valType     = NO_VALUE
        );

        /**
         */
        void        printError(bool);


        /**
         */
    	ReturnCode  parse(int argc, char ** argv);


        /**
         */
        void        addValue
        (
            const string    &   name, 
            const char      *   value = 0, 
            ValueType           type = NO_VALUE
        );


        /**
         */
        void        addValue
        (
            const string    &   name, 
            const Value     &   value
        );
        
        
        /**
         */
        const NameValueMap &    options() const;
        

        /**
         */
        int         remainingArgc() const;
        
    
        /**
         */
        char **     remainingArgv() const;

        
        /**
         */
        const list<string> &    remaining() const;


        /**
         */
        bool        get(const string & name);


        /**
         */
        bool        get(const string & name, string & value);
        

        /**
         */
        bool        get(const string & name, int & value);


    private:

        /**
         */
        const option &      find(const char *, char) const;
        
      	string	    	    	    	myOptString;
	vector<option>	    	    	myAllOpts;
	vector<option>	    	    	myLongOpts;
        int                             myPrintError;
    
        int                             myRemainingArgc;
        char **                         myRemainingArgv;
        list<string>                    myRemaining;
        
        NameValueMap                    myOptionMap;
};


} // namespace Configuration
} // namespace Vocal


#endif // !defined(VOCAL_GETOPT_HXX)
