#if !defined(VLOG_DOT_H)
#define VLOG_DOT_H

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

static const char* const VLog_hxx_Version = 
    "$Id: VLog.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "Log.hxx"
#include "CpPriorityLog.hxx"
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace Logging
{


/** Intended for a complete stack trace.<br><br>
 */
#define     LOG_TRACE	    	    100


/** Intended for annoyingly verbose and potentially system slowing
 *  logging messages.<br><br>
 */
#define     LOG_VERBOSE	    	    101


/** Vocal specific stream based logging facility.<br><br>
 *
 *  @see    Vocal::Logging::Log
 *  @see    Vocal::Logging::PriorityLog
 *  @see    Vocal::Logging::CpPriorityLog
 */
class VLog : public Vocal::Logging::Log
{
    public:

    	/** A VLog should be constructed locally on the stack. This allows
	 *  a local PriorityLog for writing, see the Prototype pattern in
	 *  Design Patterns. The local PriorityLog isn't created until it 
	 *  is used.
	 */
    	VLog();


    	/** Construct a VLog and create a Trace entry indicating function entry. 
	 *  On destruction a Trace entry indicating function exit will be made.
	 */
    	VLog(const string & func_name);


    	/** A Trace entry indicating function exit will be made if the Trace
	 *  constructor was used.
	 */
	virtual ~VLog();
	

    	/** Initialize the VLog facility, this should be done in 
	 *  Application::init(). 
	 *
	 *  @param  priorityLevel   The lowest level priority to be logged.
	 *  @param  file    	    If given, the name of the log file to be 
	 *  	    	    	    created.
	 *
	 *  @see    Vocal::Process::Application
	 */
	static	void  	    	    init(
	    	    	    	    	int 	    	priorityLevel,
					const char *	file = 0);
					

    	/** Uninitialize the VLog facility, this should be done in 
	 *  Application::uninit(). 
	 *
	 *  @see    Vocal::Process::Application
	 */
	static	void 	    	    uninit();
	

    	/** Dynamically enable logging for the given priority across the entire
	 *  application. This only applies to VLogs created after this class
	 *  method is called.
	 */
    	static	void	    	    on(int priority);


    	/** Dynamically disable logging for the given priority. This only 
	 *  applies to VLogs created after this class method is called.
	 */
	static	void	    	    off(int);


    	/** Dynamically enable logging for the given priority for this instance
	 *  of the VLog. The global priority is unchanged. 
	 */
		void		    logOn(int);


    	/** Dynamically disable logging for the given priority for this instance
	 *  of the VLog. The global priority is unchanged. 
	 */
    	    	void	    	    logOff(int);
		

    	/** Dynamically disable logging for the given priority for this instance
	 *  of the VLog. The global priority is unchanged. 
	 */
		PriorityLog &	    logEmergency();


    	/** Return the PriorityLog associated with the LOG_ALERT priority.
	 */
		PriorityLog &	    logAlert();


    	/** Return the PriorityLog associated with the LOG_CRITICAL priority.
	 */
		PriorityLog &	    logCritical();


    	/** Return the PriorityLog associated with the LOG_ERROR priority.
	 */
		PriorityLog &	    logError();


    	/** Return the PriorityLog associated with the LOG_WARNING priority.
	 */
		PriorityLog &	    logWarning();


    	/** Return the PriorityLog associated with the LOG_NOTICE priority.
	 */
		PriorityLog &	    logNotice();


    	/** Return the PriorityLog associated with the LOG_INFO priority.
	 */
		PriorityLog &	    logInfo();


    	/** Return the PriorityLog associated with the LOG_DEBUG priority.
	 */
		PriorityLog &	    logDebug();


    	/** Return the PriorityLog associated with the LOG_DEBUG_STACK priority.
	 */
		PriorityLog &	    logDebugStack();


    	/** Return the PriorityLog associated with the LOG_DEBUG_OPER priority.
	 */
		PriorityLog &	    logDebugOper();


    	/** Return the PriorityLog associated with the LOG_DEBUG_HB priority.
	 */
		PriorityLog &	    logDebugHB();
		

    	/** Return the PriorityLog associated with the LOG_TRACE priority.
	 */
		PriorityLog &	    logTrace();


    	/** Return the PriorityLog associated with the LOG_VERBOSE priority.
	 */
		PriorityLog &	    logVerbose();


        /** Return the name of the corresponding log level.
         */
        static  const char *        logName(int);


        /** Return the log level corresponding to the name.
         */
        static  int                 logValue(const char *);
        static  int                 logValue(const string &);


    private:

    	static	Sptr<PriorityLog>   null_;
    	static	bool	    	    traceOn_;
    	static	bool	    	    verboseOn_;

    	    	Sptr<PriorityLog>   emergency_;
    	    	Sptr<PriorityLog>   alert_;
    	    	Sptr<PriorityLog>   critical_;
		Sptr<PriorityLog>   error_;
    	    	Sptr<PriorityLog>   warning_;
    	    	Sptr<PriorityLog>   notice_;
    	    	Sptr<PriorityLog>   info_;
    	    	Sptr<PriorityLog>   debug_;
    	    	Sptr<PriorityLog>   debugStack_;
    	    	Sptr<PriorityLog>   debugOper_;
    	    	Sptr<PriorityLog>   debugHB_;
		bool	    	    trace_;
		bool	    	    verbose_;
		
		string	    	    funcName_;
};  	


} // namespace Logging
} // namespace Vocal


/**@name VLog ostream MACROS <br><br>
 *
 *  MACROS for logging messages rather than just using VLog.<br><br>
 *
 *  This checks if the log level is enabled before inserting into the ostream.
 *  This should be used similarly to an ostream, e.g., 
 *  <br><br>
 *  <code>
 *  VDEBUG(log) << "message" << VDEBUG_END(log);
 *  </code>
 *
 *  @see    Vocal::Logging::VLog
 */


//@{


/** Corresponds to LOG_EMERG
 */
#define     VEMERG(x)   if ( !((x).logEmergency().isOn()) );\
    	    	    	else (x).logEmergency().get(true)


/** Corresponds to LOG_ALERT
 */
#define     VALERT(x)	if ( !((x).logAlert().isOn()) );\
    	    	    	else (x).logAlert().get(true)


/** Corresponds to LOG_CRIT
 */
#define     VCRIT(x)	if ( !((x).logCritical().isOn()) );\
    	    	    	else (x).logCritical().get(true)


/** Corresponds to LOG_ERR
 */
#define     VERR(x)	if ( !((x).logError().isOn()) );\
    	    	    	else (x).logError().get(true)


/** Corresponds to LOG_WARNING
 */
#define     VWARN(x)	if ( !((x).logWarning().isOn()) );\
    	    	    	else (x).logWarning().get(true)


/** Corresponds to LOG_NOTICE
 */
#define     VNOTICE(x)	if ( !((x).logNotice().isOn()) );\
    	    	    	else (x).logNotice().get(true)


/** Corresponds to LOG_EMERG
 */
#define     VINFO(x)	if ( !((x).logInfo().isOn()) );\
    	    	    	else (x).logInfo().get(true)


/** Corresponds to LOG_DEBUG
 */
#define     VDEBUG(x)	if ( !((x).logDebug().isOn()) );\
    	    	    	else (x).logDebug().get(true)


/** Corresponds to LOG_DEBUG_STACK
 */
#define     VDEBUG_STACK(x) if ( !((x).logDebugStack().isOn()) );\
    	    	    	    else (x).logDebugStack().get(true)


/** Corresponds to LOG_DEBUG_OPER
 */
#define     VDEBUG_OPER(x)  if ( !((x).logDebugOper().isOn()) );\
    	    	    	    else (x).logDebugOper().get(true)


/** Corresponds to LOG_DEBUG_HB
 */
#define     VDEBUG_HB(x)    if ( !((x).logDebugHB().isOn()) );\
    	    	    	    else (x).logDebugHB().get(true)


/** Corresponds to LOG_TRACE
 */
#define     VTRACE(x)	    if ( !((x).logTrace().isOn()) );\
    	    	    	    else (x).logTrace().get(true)


/** Corresponds to LOG_VERBOSE
 */
#define     VVERBOSE(x)	    if ( !((x).logVerbose().isOn()) );\
    	    	    	    else (x).logVerbose().get(true)


//@}


/**@name VLog ostream continuation MACROS<br><br>
 *
 * MACRO for the continuation of a VLog ostream.<br><br>
 *
 *  This checks if the log level is enabled before inserting into the ostream.
 *  This should be used similarly to an ostream, e.g., 
 *  <br><br>
 *  <code>
 *  VDEBUG(log) << "message";<br>
 *  VDEBUG_CONT(log) << "message_cont" << VDEBUG_END(log);
 *  </code>
 *
 *  @see    Vocal::Logging::VLog
 */


//@{


/** Corresponds to LOG_EMERG
 */
#define     VEMERG_CONT(x)  if ( !((x).logEmergency().isOn()) );\
    	    	    	    else (x).logEmergency().get()


/** Corresponds to LOG_ALERT
 */
#define     VALERT_CONT(x)  if ( !((x).logAlert().isOn()) );\
    	    	    	    else (x).logAlert().get()


/** Corresponds to LOG_CRIT
 */
#define     VCRIT_CONT(x)   if ( !((x).logCritical().isOn()) );\
    	    	    	    else (x).logCritical().get()


/** Corresponds to LOG_ERR
 */
#define     VERR_CONT(x)    if ( !((x).logError().isOn()) );\
    	    	    	    else (x).logError().get()


/** Corresponds to LOG_WARNING
 */
#define     VWARN_CONT(x)   if ( !((x).logWarning().isOn()) );\
    	    	    	    else (x).logWarning().get()


/** Corresponds to LOG_NOTICE
 */
#define     VNOTICE_CONT(x) if ( !((x).logNotice().isOn()) );\
    	    	    	    else (x).logNotice().get()


/** Corresponds to LOG_EMERG
 */
#define     VINFO_CONT(x)   if ( !((x).logInfo().isOn()) );\
    	    	    	    else (x).logInfo().get()


/** Corresponds to LOG_DEBUG
 */
#define     VDEBUG_CONT(x)  if ( !((x).logDebug().isOn()) );\
    	    	    	    else (x).logDebug().get()


/** Corresponds to LOG_DEBUG_STACK
 */
#define     VDEBUG_STACK_CONT(x)    if ( !((x).logDebugStack().isOn()) );\
    	    	    	            else (x).logDebugStack().get()


/** Corresponds to LOG_DEBUG
 */
#define     VDEBUG_OPER_CONT(x)     if ( !((x).logDebugOper().isOn()) );\
    	    	    	            else (x).logDebugOper().get()


/** Corresponds to LOG_DEBUG
 */
#define     VDEBUG_HB_CONT(x)   if ( !((x).logDebugHB().isOn()) );\
    	    	    	        else (x).logDebugHB().get()


/** Corresponds to LOG_TRACE
 */
#define     VTRACE_CONT(x)  if ( !((x).logTrace().isOn()) );\
    	    	    	    else (x).logTrace().get()


/** Corresponds to LOG_VERBOSE
 */
#define     VVERBOSE_CONT(x)    if ( !((x).logVerbose().isOn()) );\
    	    	    	        else (x).logVerbose().get()


//@}


/**@name VLog ostream termination MACROS<br><br>
 *
 * MACRO for ending VLog ostream messages.<br><br>
 *
 *  This should be used to end an ostream, e.g., 
 *  <br><br>
 *  <code>
 *  VCRIT(log) << "message" << VCRIT_END(log);
 *  </code>
 *
 *  @see    Vocal::Logging::VLog
 */


//@{


using Vocal::Logging::setFile;
using Vocal::Logging::setLine;

/** Corresponds to LOG_EMERG
 */
#define     VEMERG_END(x) \
    setFile((x).logEmergency(),__FILE__) \
    << setLine((x).logEmergency(),__LINE__)\
    << (x).logEmergency()


/** Corresponds to LOG_ALERT
 */
#define     VALERT_END(x) \
    setFile((x).logAlert(),__FILE__)\
    << setLine((x).logAlert(),__LINE__)\
    << (x).logAlert()


/** Corresponds to LOG_CRIT
 */
#define     VCRIT_END(x) \
    setFile((x).logCritical(),__FILE__)\
    << setLine((x).logCritical(),__LINE__)\
    << (x).logCritical()


/** Corresponds to LOG_ERR
 */
#define     VERR_END(x) \
    setFile((x).logError(),__FILE__)\
    << setLine((x).logError(),__LINE__)\
    << (x).logError()


/** Corresponds to LOG_WARNING
 */
#define     VWARN_END(x) \
    setFile((x).logWarning(),__FILE__)\
    << setLine((x).logWarning(),__LINE__)\
    << (x).logWarning()


/** Corresponds to LOG_NOTICE
 */
#define     VNOTICE_END(x) \
    setFile((x).logNotice(),__FILE__)\
    << setLine((x).logNotice(),__LINE__)\
    << (x).logNotice()


/** Corresponds to LOG_INFO
 */
#define     VINFO_END(x) \
    setFile((x).logInfo(),__FILE__)\
    << setLine((x).logInfo(),__LINE__)\
    << (x).logInfo()


/** Corresponds to LOG_DEBUG
 */
#define     VDEBUG_END(x) \
    setFile((x).logDebug(),__FILE__)\
    << setLine((x).logDebug(),__LINE__)\
    << (x).logDebug()


/** Corresponds to LOG_DEBUG_STACK
 */
#define     VDEBUG_STACK_END(x) \
    setFile((x).logDebugStack(),__FILE__)\
    << setLine((x).logDebugStack(),__LINE__)\
    << (x).logDebugStack()


/** Corresponds to LOG_DEBUG_OPER
 */
#define     VDEBUG_OPER_END(x) \
    setFile((x).logDebugOper(),__FILE__)\
    << setLine((x).logDebugOper(),__LINE__)\
    << (x).logDebugOper()


/** Corresponds to LOG_DEBUG_HB
 */
#define     VDEBUG_HB_END(x) \
    setFile((x).logDebugHB(),__FILE__)\
    << setLine((x).logDebugHB(),__LINE__)\
    << (x).logDebugHB()


/** Corresponds to LOG_TRACE
 */
#define     VTRACE_END(x) \
    setFile((x).logTrace(),__FILE__)\
    << setLine((x).logTrace(),__LINE__)\
    << (x).logTrace()


/** Corresponds to LOG_VERBOSE
 */
#define     VVERBOSE_END(x) \
    setFile((x).logVerbose(),__FILE__)\
    << setLine((x).logVerbose(),__LINE__)\
    << (x).logVerbose()


//@}


#endif // !defined(VLOG_DOT_H)
