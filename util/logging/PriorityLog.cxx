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
#include "PriorityLog.hxx"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include <iomanip>


using Vocal::Logging::PriorityLog;
using Vocal::Logging::setFile;
using Vocal::Logging::setLine;


PriorityLog::PriorityLog(
    const char *	    priorityName,
    int 	    	    priority,
    bool	    	    alwaysOn,
    bool	    	    defaultOn
)
    :	buffer_(0),
    	priorityName_(priorityName),
	priority_(priority),
	alwaysOn_(alwaysOn),
	on_(alwaysOn ? true : defaultOn),
        file_(""),
        line_(0)
{
}


PriorityLog::PriorityLog(const PriorityLog & src)
    :	buffer_(0),
    	priorityName_(src.priorityName_),
	priority_(src.priority_),
	alwaysOn_(src.alwaysOn_),
	on_(src.on_),
        file_(src.file_),
        line_(src.line_)
{
}


PriorityLog &
PriorityLog::operator=(const PriorityLog & src)
{
    if ( this != & src )
    {
        delete buffer_;
        buffer_ = 0;
        
    	priorityName_ = src.priorityName_;
	priority_ = src.priority_;
	alwaysOn_ = src.alwaysOn_;
	on_ = src.on_;
        file_ = src.file_;
        line_ = src.line_;
    }

    return ( *this );        
}


PriorityLog::~PriorityLog()
{
    delete buffer_;
}


ostream &	    
PriorityLog::get(bool header)
{
    if ( !on_ )
    {
    	return ( cerr );
    }
    
    if ( !buffer_ )
    {
    	buffer_ = new ostrstream;
    }
    
    if ( header )
    {
    	preWrite();
    }
    
    return ( *buffer_ );
}


bool	    	    
PriorityLog::isOn() const
{
    return ( on_ );
}


void	    	    
PriorityLog::on()
{
    on_ = true;
}


void	    	    
PriorityLog::off()
{
    if ( !alwaysOn_ )
    {
    	on_ = false;
    }
}


void	    	    
PriorityLog::preWrite()
{
}


void	    	    
PriorityLog::postWrite()
{
}


void                
PriorityLog::file(const char * file__)
{
    if ( file__ )
    {
        file_ = file__;
    }
}


void                
PriorityLog::line(int line__)
{
    line_ = line__;
}
                

void	    	    
PriorityLog::end()
{
    if ( !on_ || !buffer_ )
    {
    	return;
    }
    
    *buffer_ << ends;
    const char *    logMessage = buffer_->str();

    preWrite();
                
    writeLog(priorityName_, priority_, logMessage, file_.c_str(), line_);
    
    postWrite();
    
    buffer_->freeze(false);
    delete buffer_;
    buffer_ = 0;
}
		

ostream & 
PriorityLog::writeTo(ostream & out) const
{
    PriorityLog     & priorityLog = *const_cast<PriorityLog *>(this);
    priorityLog.end();

    return ( out );
}


setFile::setFile(PriorityLog & priorityLog, const char * fileName)
{
    priorityLog.file(fileName);
}


ostream & 
setFile::writeTo(ostream & out) const
{
    return ( out );
}        


setLine::setLine(PriorityLog & priorityLog, int fileLine)
{
    priorityLog.line(fileLine);
}

ostream & 
setLine::writeTo(ostream & out) const
{
    return ( out );
}
        
