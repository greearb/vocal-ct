#ifndef CPLOG_H
#define CPLOG_H

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2003 Vovida Networks, Inc.  All rights reserved.
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

/**
@name cpLog.h
Functions to facilitate logging errors and diagnostic messages, modeled after
the priority system of Unix syslog and used by all VOCAL servers
@author see CVS
*/

#include <stdarg.h>


/**@name Log priority levels
These are the priority levels cpLog recognizes.  LOG_EMERG through
LOG_DEBUG are lifted directly from the priority system in Unix syslog.
The additional log levels are specific to VOCAL.
*/

//@{
/* the #ifndef protects these values from being clobbered by values
of the same name in Unix syslog.h */
#ifndef LOG_EMERG
/** system is unusable */
#define LOG_EMERG       0
/** action must be taken immediately */
#define LOG_ALERT       1
/** critical conditions */
#define LOG_CRIT        2
/** error conditions */
#define LOG_ERR         3
/** warning conditions */
#define LOG_WARNING     4
/** normal but significant condition */
#define LOG_NOTICE      5
/** informational */
#define LOG_INFO        6
/** debug-level messages */
#define LOG_DEBUG       7
/** stack debug-level messages */
#endif

#define LOG_DEBUG_STACK 8
/** stack operator debug-level messages */
#define LOG_DEBUG_OPER  9
/** heartbeat debug-level messages */
#define LOG_DEBUG_HB   10
/** an alias for the last cpLog priority level, for use in bounds-checking
code */
#define LAST_PRIORITY 10
//@}

/*
 * tbd:
 *      cpLogClose(),
 *      remote log server
 *      cpLogSetMask ()
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
Generate a log message using a printf-style format string and option arguments.
This function should not be called directly; it exists only as a helper function
for the cpLog macro, which in turn is defined only if we are using the GNU
C compiler (i.e. if the preprocessor macro __GNUC__ is defined).
@param pri the priority to assign to this message
@param file the file that contains the code that is calling cpLog
@param line the specific line in the file where cpLog is being called
@param fmt a printf-style format string
@param ... any number of additional values to substitute into the string, according to printf rules
*/
extern void cpLog_impl_ (int pri, const char* file, int line, const char* fmt, ...);

/**
Generate a log message using a vprintf-style format string and option arguments.
This function should not be called directly; it exists only as a helper functionfor cpLog, which in turn is defined only if we are using the GNU
C compiler (i.e. if the preprocessor macro __GNUC__ is defined).
@param pri the priority to assign to this message
@param file the file that contains the code that is calling cpLog
@param line the specific line in the file where cpLog is being called
@param fmt a printf-style format string
@param ap  a va_list (varaible-argument list), used to pass variable arguments around
*/
void vCpLog (int pri, const char* file, int line, const char* fmt, va_list ap);

#ifdef __FAKE_CPPDOC__

/**
   Log debugging or error information, possibly to a file.
   <p>
   
   This function logs debugging or error information at the given
   priority, possibly to a file.<P>

   @param pri the priority to assign to this message
   @param fmt a printf-style format string
   @param ... any number of additional values to substitute into the
   string, according to printf rules
   <p>
   
   <b>EXAMPLES</b>
   <P>
   Here are some basic examples of using cpLog.
   <PRE>
   int x;
   Data y;
   
   cpLog(LOG_ERR, "error in input");
   cpLog(LOG_DEBUG, "the value of x is %d", x);
   
   LocalScopeAllocator lo;
   cpLog(LOG_DEBUG, "the value of y is %s", y.getData(lo));
   
   </PRE>
   <p>
   Here's how you change the log level, set a file to log to, etc:
   <p>
   <pre>
   // set to LOG_DEBUG using the enum / macro
   cpLogSetPriority(LOG_DEBUG); 
   
   // set to LOG_ERR using cstr
   cpLogSetPriority(cpLogStrToPriority("LOG_ERR"));
   
   // log to the file output.log
   if(!cpLogOpen("output.log"))
   {
       cerr << "could not log to output.log" << endl;
   }
   </pre>
*/

    void cpLog(int pri, const char* file, ...);
#endif

#ifdef __GNUC__
/**
Implement cpLog as a macro only if we are using the GNU C compiler, i.e. if
__GNUC__ is defined.  The GNU C compiler defines __FILE__ and __LINE__
preprocessor macros that we can use to tell cpLog_impl_ exactly where the
calling code is located, allowing for easier debugging.<p>

This idiom ensures that certain macro expansions work by wrapping the
if statement into a single block.  Without the do-while "armor", the
macro would cause syntax errors inside a one-statement block without
brackets (e.g. the body of a while loop or of an if statement).<p>

The naive and more readable solution of placing brackets around this macro's
expansion also would not work, because 'cpLog;' would expand to include a
semicolon after the brackets, which constitutes illegal syntax.  Hence the
do-while idiom is the right solution.<p>
*/
#define cpLog(priority__, fmt__, args__...) \
do {if (priority__ <= cpLogGetPriority()) \
cpLog_impl_ (priority__, __FILE__, __LINE__, fmt__ , ##args__);} \
while (0)
#else
/**
If GNU C's __FILE__ and __LINE__ macros are unavailable, use the regular
cpLog function, which omits that information.
@param pri the priority to assign to this message
@param fmt a printf-style format string
@param ... any number of additional values to substitute into the string, according to printf rules

<p>
 default logging is enable, To disable logging, comment the following
line and uncomment the #define. Note - This is ia a hack as Solaris Forte
compiler can not handle variable argument list in define macro.This would 
cause lot of warnings to be generated which can be ignored.

*/
//extern void cpLog (int pri, const char *fmt, ...);
//#define  cpLog(pri, fmt, args, a, b, c, d, e, f)  while(0)

/////////////////
// ThienChange
/////////////////

#ifdef NDEBUG
#ifndef __USING_OLD_LOG
#define __USING_OLD_LOG
#endif
#endif

extern void cpLog_old_version (int pri, const char *fmt, ...);

#ifdef __USING_OLD_LOG    
    #define cpLog cpLog_old_version
#else
    struct WrapLog
    {
        const char *m_fname;
        int m_lineno;
        WrapLog(const char *fname, int lineno);
        void operator() (int pri, const char *fmt, ...);
    };
    #define cpLog WrapLog(__FILE__, __LINE__)
#endif


#endif

/**
Set the priority level at which messages should be printed (for the current
thread).  Messages of a priority level conceptually greater than or equal
to this number (numerically less than or equal) are printed.  Messages with
conceptually lower (numerically higher) priorities than this level are
ignored.  Don't blame us for the backwards semantics; syslog started them!

@param pri the new priority level
*/
extern void cpLogSetPriority (int pri);

/**
Get the current priority level.
@return the current priority level
@see cpLogSetPriority
*/
extern int cpLogGetPriority (int id = 0);

/**
Set the priority level at which messages should be printed for a particular
thread, in a thread-safe manner.
@see cpLogSetPriority
@param pri the new priority level
*/
extern void cpLogSetPriorityThread (int id, int pri);

/**
Set the priority level for a particular thread to an undefined value.
* @param thread_id a designator for the thread
* @see cpLogSetPriority
*/
extern void cpLogClearPriorityThread (int id);

/**
Give a thread a desciptive label, which will be included in all the log
messages that come from that thread.
@param thread_id a designator for the thread
@param label the label
*/
extern void cpLogSetLabelThread (int id, const char* label);

/**
Remove a thread's descriptive label, so that no special identifier will
be included in log messages it sends.
@param thread_id a designator for the thread
*/
extern void cpLogClearLabelThread (int id);

/**
Give the current thread a descriptive label, which will be included in all
the log messages it sends.  Every program should call cpLogSetLabel
before it begins logging.
@param label the label
*/
extern void cpLogSetLabel (const char* label);

extern void cpLogDeleteInstance();

extern void cpLogSetFileSize (const int size);

extern void cpLogSetNumOfBackupFiles (const int num);

/**
Print to standard error the current label, priority, and log file path.
*/
extern void cpLogShow (void);

/**
Start logging to the Unix syslog facility, rather than to a file.
*/
void cpLogOpenSyslog();

/**
Open a log file.  A program should call cpLogOpen when it wants to begin
logging to a file, as opposed to some other sink like standard error.
@param filename the path to the file
@return 1 if the log file was successfully opened for writing, 0 otherwise
*/
extern int cpLogOpen (const char* filename);

/**
Given a string with a descriptive name for a priority level, return the
number that is the priority level itself.
@param priority a descriptive name for a priority level
@return the priority level, or -1 if the string is not a recognized name
for any level
*/
extern int cpLogStrToPriority(const char* priority);

/**
Given a numerical priority level, return a descriptive name for that level.
* @param priority the numerical priority level
* @return a descriptive name, or a null pointer if the number passed in
* does not correspond to a recognized priority level
*/
extern const char* cpLogPriorityToStr(int priority);

/* CPLOG_H */

#ifdef __cplusplus
}
#endif

#endif
