#if !defined(VOCAL_PRIORITYLOG_HXX)
#define VOCAL_PRIORITYLOG_HXX

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



#include "Writer.hxx"
#include <string>
#include <strstream>
#include "RCObject.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace Logging
{


/** The ostream based log associated with a given priority.<br><br>
 *
 *  @see    Vocal::Logging::Log
 *  @see    Vocal::Logging::VLog
 *  @see    Vocal::Logging::CpPriorityLog   
 */
   class PriorityLog : public Vocal::IO::Writer
{
    protected:


    	/** Create a prototype PriorityLog. See the Prototype pattern
	 *  in GOF's Design Pattern.
	 *
	 *  @param  priorityName    The name of the priority.
	 *  @param  priority	    The value of the priority.
	 *  @param  alwaysOn	    If set, the log cannot be disabled.
	 *  @param  defaultOn	    If set, the log will be created enabled.
	 */
	PriorityLog(
	    const char  	*   priorityName,
	    int 	    	    priority,
	    bool	    	    alwaysOn	= false,
	    bool	    	    defaultOn 	= true);
	

    	/** Copy constructor.
	 */
	PriorityLog(const PriorityLog &);


        /** Assignment
         */
        PriorityLog &	    operator=(const PriorityLog &);


    public:
    					

    	/** Virtual destructor.
	 */
        virtual ~PriorityLog();


    	/** Create a clone of this object. See the Prototype pattern from
	 *  Design Patterns.
	 */
    	virtual PriorityLog *	    clone() const = 0;
	

    	/** Get the native ostream.
	 */
		ostream &	    get(bool header = false);
	

    	/** Test to see if this object is enabled.
	 */
	    	bool	    	    isOn() const;


    	/** Enable this log.
	 */
		void	    	    on();


    	/** Disable this log.
	 */
		void	    	    off();


    	/** Callback that is called from end(), before writeLog() is called.
	 */
    	virtual void	    	    preWrite();


    	/** Callback that is called from end(), after writeLog() is called.
	 */
	virtual void	    	    postWrite();


        /** Set the file name for this log.
         */
                void                file(const char *);
            

        /** Set the file line for this log.
         */
                void                line(int);
                
                
    	/** Commit the contents of the native ostream to the log.
	 */
    	    	void	    	    end();
	

    	/** Abstract callback that is called from end() after preWrite and
	 *  before postWrite. The intent is that the user will override this
	 *  method and write the given log message to the log.
	 *
	 *  @param  priorityName    The name of the priority.
	 *  @param  priority	    The value of the priority.
	 *  @param  logMessage	    The actual message to log.
	 *  @param  file    	    The name of the file where the log is taking
	 *  	    	    	    place.
	 *  @param  line    	    The line number of the file where the log
	 *  	    	    	    is taking place.
	 */
	virtual void	    	    writeLog(
	    	    	    	    	const string	&   priorityName,
					int 	    	    priority,
					const char  	*   logMessage,
					const char  	*   file = 0, 
					int 	    	    line = 0) = 0;


        /** Another way to call end.
         */
        ostream & writeTo(ostream &) const;


    private:

    	    	ostrstream  	*   buffer_;
    	    	string	    	    priorityName_;
		int 	    	    priority_;
		bool	    	    alwaysOn_;
		bool	    	    on_;
                string              file_;
                int                 line_;
};


class setFile : public Vocal::IO::Writer
{
    public:
        setFile(PriorityLog &, const char *);
        ostream & writeTo(ostream &) const;
};


class setLine : public Vocal::IO::Writer
{
    public:
        setLine(PriorityLog &, int);
        ostream & writeTo(ostream &) const;
};


} // namespace Logging
} // namespace Vocal


#endif // !defined(VOCAL_PRIORITYLOG_HXX)
