#if !defined(STATISTICS_DB_DOT_HXX)
#define STATISTICS_DB_DOT_HXX

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


static const char* const StatisticsDb_hxx_Version = 
    "$Id: StatisticsDb.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include <map>
#include <vector>

#include "Writer.hxx"
#include "Data.hxx"
#include "Mutex.hxx"

/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal 
{


/** Infrastructure in VOCAL to measuring, recording and reporting
 *  statistics.<br><br>
 */
namespace Statistics
{


using Vocal::Threads::Mutex;


class Statistic;


/** Using an unsigned int for simplicity.<br><br>
 */
typedef     u_int32_t	    StatisticsDbKey;


/** Never used as a database key.<br><br>
 *
 * All database keys will be greater than this value.
 */
extern const StatisticsDbKey  INVALID_STAT_DB_KEY;


/** A statistics database.<br><br>
 */
class StatisticsDb : public Vocal::IO::Writer
{
    public:


	/** Create the statistics database, given the optional name 
	 *  of the database (defaults to "StatisticsDb").
	 */
	StatisticsDb(const char * name = 0);


	/** Destroy the statistics database.
	 */
	virtual ~StatisticsDb();

    	
    	/** Return the key given the label. If this is a new label,
	 *  a key will be allocated. Keys are not consistent across
	 *  different instances of the StatisticsDb.
	 */
    	StatisticsDbKey     key(const Data & label);


    	/** Return the label name of the given key. If the key doesn't exist,
	 *  the label for INVALID_STAT_DB_KEY is returned.
	 */
    	const Data &	keyName(StatisticsDbKey) const;


    	/** Return the next key. All keys will be less than this value.
	 */
    	StatisticsDbKey     nextKey() const;


	/** Adds the statistic into the database. The statistic must be created 
	 *  using this database.
	 */
	virtual void	record(const Statistic &);


	/** Finds the statistic in the database. Returns 0 if not found.
	 */
	virtual Statistic * find(const Statistic &) const;


	/** Erases the individual statistic from the database. 
	 */
	virtual void	erase(const Statistic &);


    	/** Clear the entire database.
	 */
	virtual void	clear();

    	
    	/** Dump the database to an ostream.
	 */
        virtual ostream &   writeTo(ostream &) const;


	/** Returns the name of the database.
	 */
    	const Data &	name() const;	


    	/** Returns true if the databases are the same.
	 *  Since databases cannot be copied, this is an identity check.
	 */
	bool operator==(const StatisticsDb &) const;


    	/** The databases are ordered by their addresses.
	 */
	bool operator<(const StatisticsDb &) const;


    private:

    	/** Mutlithreaded protection.
	 */
	mutable Mutex     myLock;
	 
	    
    	/** Name of the database.
	 */
	Data	myName;
    

    	/** The next key to be allocated.
	 */
	StatisticsDbKey	myNextKey;


    	/** 
	 */
    	typedef std::map<Data, StatisticsDbKey>  StatisticRegistrations;
	

    	/**
	 */
	StatisticRegistrations    myStatRegistrations;
	

    	/**
	 */
    	typedef std::vector<Data>  StatisticNames;
	

    	/**
	 */
	StatisticNames    myStatNames;


    	/**
    	 */
    	typedef std::map<StatisticsDbKey, Statistic *>  Database;
	
	
	/**
	 */
	Database    m_db;
    

    	/** Suppress copying
	 */
	StatisticsDb(const StatisticsDb &);


    	/** Suppress copying
	 */
	const StatisticsDb & operator=(const StatisticsDb &);
};


} // namespace Statistics
} // namespace Vocal 


#endif // !defined(STATISTICS_DB_DOT_HXX)
