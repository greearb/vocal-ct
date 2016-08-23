#if !defined(TOTAL_STATISTIC_DOT_HXX)
#define TOTAL_STATISTIC_DOT_HXX

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


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** Keeps a running total.<br><br>
 */
class TotalStatistic : public Vocal::Statistics::Statistic
{
    public:


	/** Create the statistic given the associated database, and the 
	 *  label. Statistics with different labels will be assigned 
	 *  different keys.
	 */
	TotalStatistic(StatisticsDb & db, const Data & label);


	/** Copy constructor. 
	 */
	TotalStatistic(const TotalStatistic &);


	/** Assignment operator. 
	 */
	const TotalStatistic & operator=(const TotalStatistic &);

	/** This will combine the two statistics, such that the total
	 *  will be the sum.
    	 */
	virtual void	combine(const Statistic &);


    	/** Create a copy of this statistic.
    	 */
	virtual Statistic * 	copy() const;


    	/** Set the total to the given value. This is useful for 
	 *  individual statistics.
	 */
	void	set(uint32 value);


	/** Set's individual statistic's total to one, and then records
	 *  the statistic, such that the combined statistic in the database
	 *  is increased by one. Prefix operator.
	 */
        const TotalStatistic &    operator++();
	 

	/** Set's individual statistic's total to one, and then records
	 *  the statistic, such that the combined statistic in the database
	 *  is increased by one. Postfix operator.
	 */
        const TotalStatistic &    operator++(int);


	/** Set's individual statistic's total to the given value, and then 
	 *  records the statistic, such that the combined statistic in the 
	 *  database is increased by given value.
	 */
        const TotalStatistic &    operator+=(uint32);


    	/** Returns the total. More meaningful for combined statistics.
	 */
	uint32   get() const;


	/** Write the statistic to an ostream.
	 */
        virtual ostream & writeTo(ostream &) const;


    private:
    

    	uint32   	    m_total;
};


} // namespace Statistics
} // namespace Vocal 


#endif // !defined(TOTAL_STATISTIC_DOT_HXX)
