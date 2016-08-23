#if !defined(STATISTIC_DOT_HXX)
#define STATISTIC_DOT_HXX

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
#include "StatisticsDb.hxx"
#include "Data.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


/** An abstract statistic class.<br><br> 
 *
 *  The statistic is tightly coupled with the StatisticsDb. The database 
 *  key associated with this statistic's label are allocated by the database. 
 *  The statistic also know how to record itself in the database.
 *
 *  The statistic is a type of composite class, where it may represent 
 *  a single measurement, or it may represent a combination of measurements.
 *  Typically a single measurement is recorded by the user, and then the
 *  database holds a combined statistic.
 */
class Statistic : public Vocal::IO::Writer
{
    protected:


	/** Create the statistic given the associated database, and the 
	 *  label. Statistics with different labels will be assigned 
	 *  different keys.
	 */
	Statistic(StatisticsDb & db, const Data & label);


	/** Create the statistic given the associated database, and the 
	 *  previously allocated key. 
	 */
	Statistic(StatisticsDb & db, StatisticsDbKey);


	/** Copy constructor. All derived statistics should have full
	 *  copy semantics.
	 */
	Statistic(const Statistic &);


	/** Assignment operator. All derived statistics should have full
	 *  copy semantics.
	 */
	const Statistic & operator=(const Statistic &);


    public:
    

	/** Virtual destructor.
	 */
        virtual ~Statistic() throw (Vocal::SystemException) { };

    	
	/** Record this individual statistic to the associated database.
	 */
	void	record();
	
	
	/** Database key accessor.
	 */
    	StatisticsDbKey     key() const;


	/** Label name accessor.
	 */
    	const Data &	name() const;
	

	/** Associate database accessor.
	 */
    	const StatisticsDb &	db() const;
	

	/** This will combine the two statistics. 
    	 */
	virtual void	combine(const Statistic &) = 0;


    	/** Create a copy of this statistic.
    	 */
	virtual Statistic * copy() const = 0;


    protected:
    

    	StatisticsDb *	    myDb;
	

    private:


	StatisticsDbKey     myKey;
};


} // namespace Statistics
} // namespace Vocal 


#endif // !defined(STATISTIC_DOT_HXX)
