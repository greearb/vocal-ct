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
#include "VLog.hxx"
#include "CpPriorityLog.hxx"
#include <cassert>
#include <cctype>
#include <string>


using Vocal::Logging::VLog;
using Vocal::Logging::Log;
using Vocal::Logging::PriorityLog;
using Vocal::Logging::CpPriorityLog;
using std::string;


Sptr<PriorityLog>   VLog::null_ = 0;
bool	    	    VLog::traceOn_ = false;
bool	    	    VLog::verboseOn_ = false;


VLog::VLog()
    :	emergency_(0),
    	alert_(0),
    	critical_(0),
	error_(0),
    	warning_(0),
    	notice_(0),
    	info_(0),
    	debug_(0),
    	debugStack_(0),
    	debugOper_(0),
    	debugHB_(0),
	trace_(traceOn_),
	verbose_(verboseOn_),
	funcName_("")
{
    if ( null_ == 0 )
    {
    	init(cpLogGetPriority());
    }
}


VLog::VLog(const string & funcName)
    :	emergency_(0),
    	alert_(0),
    	critical_(0),
	error_(0),
    	warning_(0),
    	notice_(0),
    	info_(0),
    	debug_(0),
    	debugStack_(0),
    	debugOper_(0),
    	debugHB_(0),
	trace_(traceOn_),
	verbose_(verboseOn_),
	funcName_(funcName)
{
    if ( null_ == 0 )
    {
    	init(cpLogGetPriority());
    }
    
    if ( funcName.size() )
    {
        VTRACE(*this) << "Function Entry: " << funcName << VTRACE_END(*this);
    }
}


VLog::~VLog()
{
    if ( funcName_.size() )
    {
        VTRACE(*this) << "Function Exit: " << funcName_ << VTRACE_END(*this);
    }
}
	

void
VLog::init(
    int 	    	priority,
    const char *	file
)
{
    int priorityLevel = priority;
    
    if ( priorityLevel > LAST_PRIORITY )
    {
        priorityLevel = LAST_PRIORITY;
    }
    
    // cpLogSetPriority(priorityLevel);
    if ( file )
    {
    	// cpLogOpen's signature is broken, so let's cast away const.
        //
    	char * f = const_cast<char *>(file);
        cpLogOpen(f);
    }

    null_ = new CpPriorityLog("--NULL---", -1, false, false);
    registerLog(-1, null_);
    
    registerLog(LOG_EMERG,  new CpPriorityLog("EMERGENCY", LOG_EMERG, true));
    registerLog(LOG_ALERT,  new CpPriorityLog("ALRT     ", LOG_ALERT, true));
    registerLog(LOG_CRIT,   new CpPriorityLog("CRITICAL ", LOG_CRIT,  true));
    registerLog(LOG_ERR,    new CpPriorityLog("ERROR    ", LOG_ERR,     false, false));
    registerLog(LOG_WARNING,new CpPriorityLog("WARNING  ", LOG_WARNING, false, false));
    registerLog(LOG_NOTICE, new CpPriorityLog("NOTICE   ", LOG_NOTICE,  false, false));
    registerLog(LOG_INFO,   new CpPriorityLog("INFO     ", LOG_INFO,    false, false));
    registerLog(LOG_DEBUG,  new CpPriorityLog("DEBUG    ", LOG_DEBUG,   false, false));
    registerLog(LOG_DEBUG_STACK,new CpPriorityLog("DEBUG_STK", LOG_DEBUG_STACK, false, false));
    registerLog(LOG_DEBUG_OPER, new CpPriorityLog("DEBUG_OP ", LOG_DEBUG_OPER,  false, false));
    registerLog(LOG_DEBUG_HB,   new CpPriorityLog("DEBUG_HB ", LOG_DEBUG_HB,    false, false));

    switch ( priority )
    {
        case LOG_VERBOSE:       VLog::on(LOG_VERBOSE);
        case LOG_TRACE:         VLog::on(LOG_TRACE);
        case LOG_DEBUG_HB:
        case LOG_DEBUG_OPER:    VLog::on(LOG_DEBUG_OPER);
        case LOG_DEBUG_STACK:   VLog::on(LOG_DEBUG_STACK);
        case LOG_DEBUG:         VLog::on(LOG_DEBUG);
        case LOG_INFO:          VLog::on(LOG_INFO);
        case LOG_NOTICE:        VLog::on(LOG_NOTICE);
        case LOG_WARNING:       VLog::on(LOG_WARNING);
        case LOG_ERR:           VLog::on(LOG_ERR);
    }

    // You only get heartbeat logging if you explicitly ask for it.
    //
    if ( priority == LOG_DEBUG_HB )
    {
        VLog::on(LOG_DEBUG_HB);
    }
}
					

void
VLog::uninit()
{
    clearLogs();
    null_ = 0;
}


void	    	    	    
VLog::on(int priority)
{
    if ( priority == LOG_TRACE )
    {
    	traceOn_ = true;
	priority = LOG_DEBUG_STACK;
    }

    if ( priority == LOG_VERBOSE )
    {
    	verboseOn_ = true;
	priority = LOG_DEBUG_STACK;
    }
    
    LogMap::iterator	it = log_.find(priority);
    Sptr<PriorityLog> nullLog = 0;
    Sptr<PriorityLog> log = ( it == log_.end() ? nullLog : (*it).second );
    if ( priority != -1 && log != 0 )
    {
    	log->on();
    }
}


void	    	    	    
VLog::off(int priority)
{
    if ( priority == LOG_TRACE )
    {
    	traceOn_ = false;
	return;
    }

    if ( priority == LOG_VERBOSE )
    {
    	verboseOn_ = false;
	return;
    }
    
    LogMap::iterator	it = log_.find(priority);
    Sptr<PriorityLog> nullLog = 0;
    Sptr<PriorityLog> log = ( it == log_.end() ? nullLog : (*it).second );
    if ( log != 0 )
    {
    	log->off();
    }
}
	
void	    	    	    
VLog::logOn(int priority)
{
    if ( priority == LOG_TRACE )
    {
    	priority = LOG_DEBUG_STACK;
	trace_ = true;
    }

    if ( priority == LOG_VERBOSE )
    {
    	priority = LOG_DEBUG_STACK;
	verbose_ = true;
    }

    LogMap::iterator	it = log_.find(priority);
    Sptr<PriorityLog> nullLog = 0;
    Sptr<PriorityLog> log = ( it == log_.end() ? nullLog : (*it).second );
    if ( priority != -1 && log != 0 )
    {
	switch ( priority )
	{
    	    case LOG_EMERG:
	    {
	    	if ( emergency_ == 0 || emergency_ == VLog::null_ )
		{
        	    emergency_ = log->clone();
    		    emergency_->on();
		}
	    	break;
	    }
    	    case LOG_ALERT:
	    {
	    	if ( alert_ == 0 || alert_ == VLog::null_ )
		{
        	    alert_ = log->clone();
    		    alert_->on();
		}
	    	break;
	    }
    	    case LOG_CRIT:
	    {
	    	if ( critical_ == 0 || critical_ == VLog::null_ )
		{
        	    critical_ = log->clone();
    		    critical_->on();
		}
	    	break;
    	    }
    	    case LOG_ERR:
	    {
	    	if ( error_ == 0 || error_ == VLog::null_ )
		{
        	    error_ = log->clone();
    		    error_->on();
		}
	    	break;
	    }
    	    case LOG_WARNING:
	    {
	    	if ( warning_ == 0 || warning_ == VLog::null_ )
		{
        	    warning_ = log->clone();
    		    warning_->on();
		}
	    	break;
	    }
    	    case LOG_NOTICE:
	    {
	    	if ( notice_ == 0 || notice_ == VLog::null_ )
		{
        	    notice_ = log->clone();
    		    notice_->on();
		}
	    	break;
	    }
    	    case LOG_INFO:
	    {
	    	if ( info_ == 0 || info_ == VLog::null_ )
		{
        	    info_ = log->clone();
    		    info_->on();
		}
	    	break;
	    }
    	    case LOG_DEBUG:
	    {
	    	if ( debug_ == 0 || debug_ == VLog::null_ )
		{
        	    debug_ = log->clone();
    		    debug_->on();
		}
	    	break;
	    }
    	    case LOG_DEBUG_STACK:
	    {
	    	if ( debugStack_ == 0 || debugStack_ == VLog::null_ )
		{
        	    debugStack_ = log->clone();
    		    debugStack_->on();
		}
	    	break;
	    }
    	    case LOG_DEBUG_OPER:
	    {
	    	if ( debugOper_ == 0 || debugOper_ == VLog::null_ )
		{
        	    debugOper_ = log->clone();
    		    debugOper_->on();
		}
	    	break;
	    }
    	    case LOG_DEBUG_HB:
	    {
	    	if ( debugHB_ == 0 || debugHB_ == VLog::null_ )
		{
        	    debugHB_ = log->clone();
    		    debugHB_->on();
		}
	    	break;
	    }
	    default:
	    {
	    	break;
	    }
	}
    }
}


void	    	    	    
VLog::logOff(int priority)
{
    if ( priority == LOG_TRACE )
    {
	trace_ = false;
	return;
    }

    if ( priority == LOG_VERBOSE )
    {
	verbose_ = false;
	return;
    }

    LogMap::iterator	it = log_.find(priority);
    Sptr<PriorityLog> nullLog = 0;
    Sptr<PriorityLog> log = ( it == log_.end() ? nullLog : (*it).second );
    if ( log != 0 )
    {
	switch ( priority )
	{
    	    case LOG_EMERG:
	    {
	    	if ( emergency_ != 0 )
		{
    		    emergency_->off();
		}
	    	break;
	    }
    	    case LOG_ALERT:
	    {
	    	if ( alert_ != 0 )
		{
    		    alert_->off();
		}
	    	break;
	    }
    	    case LOG_CRIT:
	    {
	    	if ( critical_ != 0 )
		{
    		    critical_->off();
		}
	    	break;
    	    }
    	    case LOG_ERR:
	    {
	    	if ( error_ != 0 )
		{
    		    error_->off();
		}
	    	break;
	    }
    	    case LOG_WARNING:
	    {
	    	if ( warning_ != 0 )
		{
    		    warning_->off();
		}
	    	break;
	    }
    	    case LOG_NOTICE:
	    {
	    	if ( notice_ != 0 )
		{
    		    notice_->off();
		}
	    	break;
	    }
    	    case LOG_INFO:
	    {
	    	if ( info_ != 0 )
		{
    		    info_->off();
		}
	    	break;
	    }
    	    case LOG_DEBUG:
	    {
	    	if ( debug_ != 0 )
		{
    		    debug_->off();
		}
	    	break;
	    }
    	    case LOG_DEBUG_STACK:
	    {
	    	if ( debugStack_ != 0 )
		{
    		    debugStack_->off();
		}
	    	break;
	    }
    	    case LOG_DEBUG_OPER:
	    {
	    	if ( debugOper_ != 0 )
		{
    		    debugOper_->off();
		}
	    	break;
	    }
    	    case LOG_DEBUG_HB:
	    {
	    	if ( debugHB_ != 0 )
		{
    		    debugHB_->off();
		}
	    	break;
	    }
	    default:
	    {
	    	break;
	    }
	}
    }
}

PriorityLog &	    	    
VLog::logEmergency()
{
    assert( VLog::null_ != 0 );
    
    if ( emergency_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_EMERG);
        emergency_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *emergency_ );
}


PriorityLog &	    	    
VLog::logAlert()
{
    assert( VLog::null_ != 0 );

    if ( alert_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_ALERT);
        alert_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *alert_ );
}


PriorityLog &	    	    
VLog::logCritical()
{
    assert( VLog::null_ != 0 );

    if ( critical_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_CRIT);
        critical_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *critical_ );
}


PriorityLog &	    	    
VLog::logError()
{
    assert( VLog::null_ != 0 );

    if ( error_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_ERR);
        error_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *error_ );
}


PriorityLog &	    	    
VLog::logWarning()
{
    assert( VLog::null_ != 0 );

    if ( warning_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_WARNING);
        warning_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *warning_ );
}


PriorityLog &	    	    
VLog::logNotice()
{
    assert( VLog::null_ != 0 );

    if ( notice_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_NOTICE);
        notice_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *notice_ );
}


PriorityLog &	    	    
VLog::logInfo()
{
    assert( VLog::null_ != 0 );

    if ( info_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_INFO);
        info_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *info_ );
}


PriorityLog &	    	    
VLog::logDebug()
{
    assert( VLog::null_ != 0 );

    if ( debug_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_DEBUG);
        debug_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *debug_ );
}


PriorityLog &	    	    
VLog::logDebugStack()
{
    assert( VLog::null_ != 0 );

    if ( debugStack_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_DEBUG_STACK);
        debugStack_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *debugStack_ );
}


PriorityLog &	    	    
VLog::logDebugOper()
{
    assert( VLog::null_ != 0 );

    if ( debugOper_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_DEBUG_OPER);
        debugOper_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *debugOper_ );
}


PriorityLog &	    	    
VLog::logDebugHB()
{
    assert( VLog::null_ != 0 );

    if ( debugHB_ == 0 )
    {
    	Sptr<PriorityLog> log = getLog(LOG_DEBUG_HB);
        debugHB_ = ( log != 0 ? Sptr<PriorityLog>(log->clone()) : VLog::null_ );
    }
    
    return ( *debugHB_ );
}


PriorityLog &	    
VLog::logTrace()
{
    assert( VLog::null_ != 0 );

    return ( trace_ ? logDebugStack() : *VLog::null_ );
}


PriorityLog &	    
VLog::logVerbose()
{
    assert( VLog::null_ != 0 );

    return ( verbose_ ? logDebugStack() : *VLog::null_ );
}


const char *        
VLog::logName(int logLevel)
{
    switch ( logLevel )
    {
        case LOG_EMERG:         return "LOG_EMERG";
        case LOG_ALERT:         return "LOG_ALERT";
        case LOG_CRIT:          return "LOG_CRIT";
        case LOG_ERR:           return "LOG_ERR";
        case LOG_WARNING:       return "LOG_WARNING";
        case LOG_NOTICE:        return "LOG_NOTICE";
        case LOG_INFO:          return "LOG_INFO";
        case LOG_DEBUG:         return "LOG_DEBUG";
        case LOG_DEBUG_STACK:   return "LOG_DEBUG_STACK";
        case LOG_DEBUG_OPER:    return "LOG_DEBUG_OPER";
        case LOG_DEBUG_HB:      return "LOG_DEBUG_HB";
        case LOG_TRACE:         return "LOG_TRACE";
        case LOG_VERBOSE:       return "LOG_VERBOSE";
    }    
    return ( "LOG_UNKNOWN" );
}


int                 
VLog::logValue(const char * name)
{
    string  log_emerg("log_emerg"),             emerg("emerg"),
            log_alert("log_alert"),             alert("alert"),
            log_crit("log_crit"),               crit("crit"),          
            log_err("log_err"),                 err("error"),           
            log_warning("log_warning"),         warning("warn"),       
            log_notice("log_notice"),           notice("notice"),        
            log_info("log_info"),               info("info"),          
            log_debug("log_debug"),             debug("debug"),         
            log_debug_stack("log_debug_stack"), debug_stack("debug_stack"),   
            log_debug_oper("log_debug_oper"),   debug_oper("debug_oper"),    
            log_debug_hb("log_debug_hb"),       debug_hb("debug_hb"),      
            log_trace("log_trace"),             trace("trace"),         
            log_verbose("log_verbose"),         verbose("verbose");    

    string  logname(name);
    size_t size = logname.size();
    for ( size_t i = 0; i < size; i++ )
    {
        logname[i] = tolower(logname[i]);
    }

    if ( log_verbose == logname || verbose == logname )
    {
        return LOG_VERBOSE;
    }
    if ( log_trace == logname || trace == logname )
    {
        return LOG_TRACE;
    }
    if ( log_debug_hb == logname || debug_hb == logname )
    {
        return LOG_DEBUG_HB;
    }
    if ( log_debug_oper == logname || debug_oper == logname )
    {
        return LOG_DEBUG_OPER;
    }
    if ( log_debug_stack == logname || debug_stack == logname )
    {
        return LOG_DEBUG_STACK;
    }
    if ( log_debug == logname || debug == logname )
    {
        return LOG_DEBUG;
    }
    if ( log_info == logname || info == logname )
    {
        return LOG_INFO;
    }
    if ( log_notice == logname || notice == logname )
    {
        return LOG_NOTICE;
    }
    if ( log_warning == logname || warning == logname )
    {
        return LOG_WARNING;
    }
    if ( log_err == logname || err == logname )
    {
        return LOG_ERR;
    }
    if ( log_crit == logname || crit == logname )
    {
        return LOG_CRIT;
    }
    if ( log_alert == logname || alert == logname )
    {
        return LOG_ALERT;
    }
    if ( log_emerg == logname || emerg == logname )
    {
        return LOG_EMERG;
    }
    return ( LOG_WARNING );
}


int                 
VLog::logValue(const string & name)
{
    return ( logValue(name.c_str()) );
}
