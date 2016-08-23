#if !defined(DURATION_STATISTIC_DOT_HXX)
#define DURATION_STATISTIC_DOT_HXX

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


#include "Statistic.hxx"
#include "Duration.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** One or more duration statistics.<br><br>
 */
class DurationStatistic : public Vocal::Statistics::Statistic
{
    public:


	/** Create the statistic given the associated database,
	 *  the label and the optional window width. The window width
	 *  default to 0 indicating no window, thus it remembers all
	 *  combined durations. 
	 */
	DurationStatistic(StatisticsDb  &   db, 
	    	    	  const Data 	&   name, 
			  u_int32_t 	    window = 0);


	/** Copy constructor.
	 */
	DurationStatistic(const DurationStatistic &);


	/** Assignment operator.
	 */
	const DurationStatistic & operator=(const DurationStatistic &);


	/** Virtual destructor.
	 */
        virtual ~DurationStatistic();

    	
	/** This will combine the two statistics, chaining them together.
	 *  If a window set and this addition exceeds the window size, the 
	 *  oldest list element will be removed from the list.
    	 */
	virtual void	combine(const Statistic &);


    	/** Create a copy of this statistic.
    	 */
	virtual Statistic * 	copy() const;


	/** Start measuring the duration. If this is a combined statistic,
	 *  it will be cleared first.
	 */
	void	start();


	/** Stop measuring the duration.
	 */
	void	stop();
	
	
	/** Return the next statistic or 0 if this is the last statistic.
	 */
	DurationStatistic * 	next() const;


    	/** Return the number of statistics.
	 */
    	u_int32_t   size() const;
	
	
	/** Return the window size or 0 if there is no window.
	 */
	u_int32_t   window() const;
	
	
	/** Returns the combined length of all durations.
	 */
	int64_t length() const;
	

	/** The length divided by the size.
	 */
	double	average() const;


	/** Write the statistic to an ostream.
	 */
        virtual ostream &   writeTo(ostream &) const;
	 

    private:
    	

    	/*  Copy constructor used by append. Copies only the first element
	 *  from a combination statistic, creating an individual statistic.
	 */
	DurationStatistic(const Statistic &, const Duration  &, u_int32_t);


	/* Append a list to the end.
	 */
	void	append(const DurationStatistic &);
	

	/* Pop from the front as needed.
	 */
	void	pop();
	

	/* Clears list.
	 */
	void	clear();
    

    	Duration   	    	    m_duration;
    	DurationStatistic   	*   m_next;
    	DurationStatistic   	*   m_last;
    	u_int32_t   	    	    m_window;
	u_int32_t   	    	    m_size;
	int64_t     	    	    m_length;
};


inline void	
DurationStatistic::start()
{
    clear();
    m_duration.start();
}


inline void	
DurationStatistic::stop()
{
    m_duration.stop();
    m_length = m_duration.length();
}



} // namespace Statistics
} // namespace Vocal 


#endif // !defined(DURATION_STATISTIC_DOT_HXX)
