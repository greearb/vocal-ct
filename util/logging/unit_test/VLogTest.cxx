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
static const char* const VLogTest_cxx_Version = 
"$Id: VLogTest.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";
#include "Application.hxx"
#include "CpPriorityLog.hxx"
#include "VLog.hxx"
#include <map>


using Vocal::Application;
using Vocal::Logging::VLog;
using Vocal::Logging::CpPriorityLog;
using Vocal::ReturnCode;
using Vocal::SUCCESS;

class VLogTest : public Application
{
    public:
    	VLogTest() {}
	~VLogTest() {}

    	ReturnCode  	    init(int, char **, char **);
	void	    	    uninit();
	
    	ReturnCode  	    run();	
};


Application *	Application::create()
{
    return ( new VLogTest );
}

ReturnCode
VLogTest::init(int argc, char ** argv, char ** arge)
{
    if ( argc > 1 )
    {
        VLog::init(LOG_VERBOSE, argv[1]);
    }
    else
    {
    	VLog::init(LOG_VERBOSE);
    }
    return ( SUCCESS );
}

void
VLogTest::uninit()
{
    VLog::uninit();
}

ReturnCode
VLogTest::run()
{
    cpLogSetPriority(LOG_DEBUG_STACK);
    
    CpPriorityLog debugLog("Debug", LOG_DEBUG);
    
    ostream & out = debugLog.get();
    debugLog.preWrite();
    out << "Testing.";
    debugLog.end();
    
    if ( debugLog.isOn() )
    {
    	ostream & out = debugLog.get();
	debugLog.preWrite();
        out << "This is a test of the VLog system. This is "
            << "only a test. For more information, please consult the "
            << "VLog system documentation. Again, this is only a test.";
	debugLog.postWrite();
	debugLog.end();
    }

    debugLog.off();
    
    if ( debugLog.isOn() )
    {
    	ostream & out = debugLog.get();
        debugLog.preWrite();
        out << "This is a test of the VLog system. This is "
            << "only a test. For more information, please consult the "
            << "VLog system documentation. Again, this is only a test.";
        debugLog.postWrite();
        debugLog.end();
    }

    VLog    log;
    
    VEMERG(log)     << "Emergency." << VEMERG_END(log);
    VALERT(log)     << "Alert."     << VALERT_END(log);
    VCRIT(log)	    << "Critical."  << VCRIT_END(log);
    VERR(log)	    << "Error."     << VERR_END(log);
    VWARN(log)	    << "Warning."   << VWARN_END(log);
    VNOTICE(log)    << "Notice."    << VNOTICE_END(log);
    VINFO(log)	    << "Info."	    << VINFO_END(log);
    VDEBUG(log)     << "Debug."     << VDEBUG_END(log);
    VDEBUG_STACK(log)   << "Debug Stack."       << VDEBUG_STACK_END(log);
    VDEBUG_OPER(log)    << "Debug Oper."        << VDEBUG_OPER_END(log);
    VDEBUG_HB(log)      << "Debug Heartbeat."   << VDEBUG_HB_END(log);
    VTRACE(log)     << "Trace."     << VTRACE_END(log);
    VVERBOSE(log)   << "Verbose."   << VVERBOSE_END(log);

    log.logOff(LOG_EMERG);
    log.logOff(LOG_ALERT);
    log.logOff(LOG_CRIT);
    log.logOff(LOG_NOTICE);
    log.logOff(LOG_WARNING);
    log.logOff(LOG_NOTICE);
    log.logOff(LOG_INFO);
    log.logOff(LOG_DEBUG);
    log.logOn(LOG_DEBUG_STACK);
    log.logOn(LOG_DEBUG_OPER);
    log.logOn(LOG_DEBUG_HB);
    log.logOn(LOG_TRACE);
    log.logOn(LOG_VERBOSE);

    VEMERG(log)     << "Emergency 2." << VEMERG_END(log);
    VALERT(log)     << "Alert 2."     << VALERT_END(log);
    VCRIT(log)	    << "Critical 2."  << VCRIT_END(log);
    VERR(log)	    << "Error 2."     << VERR_END(log);
    VWARN(log)	    << "Warning 2."   << VWARN_END(log);
    VNOTICE(log)    << "Notice 2."    << VNOTICE_END(log);
    VINFO(log)	    << "Info 2."      << VINFO_END(log);
    VDEBUG(log)     << "Debug 2."     << VDEBUG_END(log);
    VDEBUG_STACK(log)   << "Debug Stack 2."     << VDEBUG_STACK_END(log);
    VDEBUG_OPER(log)    << "Debug Oper 2."      << VDEBUG_OPER_END(log);
    VDEBUG_HB(log)      << "Debug Heartbeat 2." << VDEBUG_HB_END(log);
    VTRACE(log)     << "Trace 2."     << VTRACE_END(log);
    VVERBOSE(log)   << "Verbose 2."     << VVERBOSE_END(log);

    VLog::off(LOG_EMERG);
    VLog::off(LOG_ALERT);
    VLog::off(LOG_CRIT);
    VLog::off(LOG_NOTICE);
    VLog::off(LOG_WARNING);
    VLog::off(LOG_NOTICE);
    VLog::off(LOG_INFO);
    VLog::off(LOG_DEBUG);
    VLog::off(LOG_DEBUG_STACK);
    VLog::off(LOG_DEBUG_OPER);
    VLog::off(LOG_DEBUG_HB);
    VLog::off(LOG_TRACE);
    VLog::off(LOG_VERBOSE);

    VLog    log1;

    VEMERG(log1)    << "Emergency 3." << VEMERG_END(log1);
    VALERT(log1)    << "Alert 3."     << VALERT_END(log1);
    VCRIT(log1)	    << "Critical 3."  << VCRIT_END(log1);
    VERR(log1)	    << "Error 3."     << VERR_END(log1);
    VWARN(log1)	    << "Warning 3."   << VWARN_END(log1);
    VNOTICE(log1)   << "Notice 3."    << VNOTICE_END(log1);
    VINFO(log1)	    << "Info 3."      << VINFO_END(log1);
    VDEBUG(log1)    << "Debug 3."     << VDEBUG_END(log1);
    VDEBUG_STACK(log1)  << "Debug Stack 3."     << VDEBUG_STACK_END(log);
    VDEBUG_OPER(log1)   << "Debug Oper 3."      << VDEBUG_OPER_END(log);
    VDEBUG_HB(log1)     << "Debug Heartbeat 3." << VDEBUG_HB_END(log);
    VTRACE(log1)    << "Trace 3."     << VTRACE_END(log);
    VVERBOSE(log1)  << "Verbose 3."   << VVERBOSE_END(log);
    
    return ( 0 );
}


int main(int argc, char ** argv, char ** arge)
{
    return ( Application::main(argc, argv, arge) );
}
