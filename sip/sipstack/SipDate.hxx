#ifndef SipDate_HXX
#define SipDate_HXX

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

static const char* const SipDateVersion =
    "$Id: SipDate.hxx,v 1.2 2004/06/16 06:51:25 greear Exp $";



#include "Data.hxx"
#include "SipHeader.hxx"
#include "VException.hxx"


namespace Vocal
{


enum SipDateErrorType
{
    DECODE_DATE_FAILED,
    DECODE_WEEKDAY_FAILED,
    DECODE_HOUR_FAILED,
    DECODE_MONTH_FAILED,
    DECODE_SECONDS_FAILED,
    DECODE_TIMEZONE_FAILED,
    DECODE_DAY_FAILED,
    DECODE_YEAR_FAILED,
    DECODE_MINS_FAILED
    //may need to change this to be more specific
};

/// Exception handling class SipDateParserException
class SipDateParserException : public VException
{
    public:
        SipDateParserException( const string& msg,
                                const string& file,
                                const int line,
                                const int error = 0 );
        SipDateParserException( const string& msg,
                                const string& file,
                                const int line,
                                SipDateErrorType i)
        : VException( msg, file, line, static_cast < int > (i))
        {
            value = i;
        }
        SipDateErrorType getError() const
        {
            return value;
        }
        string getName() const ;
    private:
        SipDateErrorType value;
};



/// data container for Date header
class SipDate : public SipHeader
{
    public:

        /*** Create by decoding the data string passed in. This is the decode
        or parse.  */
        SipDate( const Data& srcData, const string& local_ip );
        SipDate( const SipDate& src );

        ///
        bool operator ==(const SipDate& src) const;

        bool operator <(const SipDate& src) const;

        ///
        const SipDate& operator=( const SipDate& src );
        ///
        Data get();
        ///
        void scanSipDate(const Data& data);
        ///
        void parseDate(const Data& data);
        ///
        void parseTime(const Data& data);
        ///
        void setMonth(const Data& newData);
        ///
        void setYear(const Data& newData);
        ///
        void setDay(const Data& newData);
        ///
        void setSeconds(const Data& newseconds);
        ///
        void setHour(const Data& newhour);
        ///
        void setMinutes(const Data& newminutes);
        ///
        void setTimezone(const Data& newtimezone);
        ///
        void setWeekday(const Data& newweekday);
        ///
        Data getDay() const
        {
            return day;
        }
        ///
        Data getMonth() const
        {
            return month;
        }
        ///
        Data getYear() const
        {
            return year;
        }
        ///
        Data getSeconds() const
        {
            return seconds;
        }
        ///
        Data getHour() const
        {
            return hour;
        }
        ///
        Data getMinutes() const
        {
            return minutes;
        }
        ///
        Data getTimezone() const
        {
            return timezone;
        }
        ///
        Data getWeekday() const
        {
            return timezonediff;
        }

        /*** return the encoded string version of this. This call
	     should only be used inside the stack and is not part of
	     the API */
        Data encode() const;

        ///
        //Data getDay() const { return day;}

        ///
        Data getData() const;

        ///
        bool getFlagDate() const;
        ///
        //Data getDay() const { return day;}
        /// takes the entire date string
        void set(const Data& data);   

	/** method for copying sip headers of any type without knowing
            which type */
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        bool flagDate;
        Data day;
        Data year;
        Data month;
        Data weekday;
        Data hour;
        Data seconds;
        Data minutes;
        Data timezone;
        Data timezonediff;
        void decode(const Data& data);
    
        friend class SipMsg;

        SipDate(); //not implemented
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
