
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

static const char* const TimerEntry_cc_Version =
    "$Id: TimerEntry.cc,v 1.1 2004/05/01 04:15:33 greear Exp $";


template < class Msg >
TimerEntry < Msg > ::TimerEntry(
    Sptr < Msg >        msg, 
    milliseconds_t      time,
    bool                relative
)
    : msg_(msg)
{
    if ( relative )
    {
        absExpires_.now();
    }

    if ( time > 0 )
    {
        absExpires_ += time;
    }
}


template < class Msg >
TimerEntry < Msg > ::TimerEntry(
    Sptr < Msg >        msg, 
    const TimeVal &     time,
    bool                relative
)
    :   absExpires_(time),
        msg_(msg)
        
{
    if ( relative )
    {
        absExpires_ += TimeVal().now();
    }
}


template < class Msg >
TimerEntry < Msg > ::TimerEntry(const TimerEntry & src)
        : absExpires_(src.absExpires_),
        msg_(src.msg_)
{}

template < class Msg >
TimerEntry < Msg > ::TimerEntry()
   : msg_(0)
{}

template < class Msg >
TimerEntry < Msg > ::~TimerEntry()
{}



template < class Msg >
TimerEntry < Msg > &
TimerEntry < Msg > ::operator=(const TimerEntry & src)
{
    if ( this != &src )
    {
        absExpires_ = src.absExpires_;
        msg_ = src.msg_;
    }
    return ( *this );
}


template < class Msg >
milliseconds_t
TimerEntry < Msg > ::getTimeout() const
{
    TimeVal timeDifference = absExpires_ - TimeVal().now();

    milliseconds_t timeout = timeDifference.milliseconds();

    return ( timeout < 0 ? 0 : timeout );
}


template < class Msg >
TimerEntryId
TimerEntry < Msg > ::getId() const
{
    return ( (TimerEntryId)(msg_.operator->()) );
}


template < class Msg >
Sptr < Msg >
TimerEntry < Msg > ::getMessage() const
{
    return ( msg_ );
}


template < class Msg >
bool
TimerEntry < Msg > ::hasExpired() const
{
    return ( absExpires_ < TimeVal().now() );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator==(const TimerEntry & src) const
{
    return ( absExpires_ == src.absExpires_ );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator!=(const TimerEntry & src) const
{
    return ( !operator==(src) );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator<(const TimerEntry & src) const
{
    return ( absExpires_ < src.absExpires_ );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator<=(const TimerEntry & src) const
{
    return ( operator<(src) || operator==(src) );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator>(const TimerEntry & src) const
{
    return ( !operator<=(src) );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator>=(const TimerEntry & src) const
{
    return ( !operator<(src) );
}


template < class Msg >
ostream &
TimerEntry < Msg > ::writeTo(ostream & out) const
{
    return ( out << "id = " << getId()
             << ", absolute time of expiration = "
             << "{ sec: " << absExpires_.tv_sec
             << ", usec: " << absExpires_.tv_usec << " }"
             << ", relative time of expiration = " << getTimeout()
           );
}
