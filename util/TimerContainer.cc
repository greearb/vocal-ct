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


static const char* const TimerContainer_cc_Version =
    "$Id: TimerContainer.cc,v 1.1 2004/05/01 04:15:33 greear Exp $";

using namespace std;

template < class Msg >
TimerContainer < Msg > ::TimerContainer()
{
}



template < class Msg >
TimerContainer < Msg > ::~TimerContainer()
{
}



template < class Msg >
TimerEntryId
TimerContainer < Msg > ::add(
    Sptr < Msg > msg,
    milliseconds_t when,
    bool relative
)
{
    TimerEntry < Msg > newTimerEntry(msg, when, relative);

    postAdd(newTimerEntry);
    
    return ( newTimerEntry.getId() );
}


template < class Msg >
TimerEntryId
TimerContainer < Msg > ::add(
    Sptr < Msg > msg,
    const TimeVal & when,
    bool relative
)
{
    TimerEntry < Msg > newTimerEntry(msg, when, relative);
    postAdd(newTimerEntry);
    return ( newTimerEntry.getId() );
}


template < class Msg >
bool
TimerContainer < Msg > ::cancel(TimerEntryId msg_id)
{
   //Lock lock(myMutex);
   typename map<TimerEntryId, TimerEntry<Msg> >::iterator j = _idToTimer.find(msg_id);
   if (j == _idToTimer.end())
   {
      return false;
   }
   
#if defined(_MSC_VER) && _MSC_VER<=1200
   pair<TimerSet::iterator, TimerSet::iterator>
#else
   pair<typename TimerSet::iterator, typename TimerSet::iterator> 
#endif   
      range = equal_range(_timerSet.begin(),
                          _timerSet.end(),
                          j->second);
   _idToTimer.erase(j);
   for (typename TimerSet::iterator i = range.first; i != range.second; i++)
   {
      if (i->getId() == msg_id)
      {
         _timerSet.erase(i);
         return true;
      }
   }
   assert(0);

   return false;
}      

template < class Msg >
int
TimerContainer < Msg > ::getTimeout()
{
   //Lock lock(myMutex);
   return (_timerSet.size() > 0
           ? _timerSet.begin()->getTimeout()
           : -1);
}


template < class Msg >
TimerEntryId
TimerContainer < Msg > ::getFirstTimerEntryId()
{
   //Lock lock(myMutex);
   return (_timerSet.size() > 0 
           ? _timerSet.begin()->getId() 
           : 0);
}


template < class Msg >
bool
TimerContainer < Msg > ::messageAvailable()
{
   bool msgAvailable = (_timerSet.size() > 0 && _timerSet.begin()->hasExpired());
   return msgAvailable;
}


template < class Msg >
Sptr < Msg >
TimerContainer < Msg > ::getMessage()
throw ( VMissingDataException )
{
    if ( !messageAvailable() )
    {
        throw VMissingDataException("TimerContainer::getMessage: no message available.",
                                    __FILE__, __LINE__);
    }
    typename TimerSet::iterator it = _timerSet.begin();
    _idToTimer.erase(it->getId());

    Sptr<Msg> msg = it->getMessage();
    _timerSet.erase(it);

    return msg;
}

template < class Msg >
ostream &
TimerContainer < Msg > ::writeTo(ostream & out) const
{
   out << "timer container (size: " << size() << ") = { ";
   bool first = true;

   for ( typename TimerSet::const_iterator it = _timerSet.begin(); it != _timerSet.end(); it++ )
   {
      if ( first )
      {
         out << " (";
         first = false;
      }
      else
      {
         out << ", (";
      }
      
      it->writeTo(out);
      out << " )";
   }
   return ( out << " }" );
}


template < class Msg >
unsigned int
TimerContainer < Msg > ::size() const
{
   //Lock lock(myMutex);
   return _timerSet.size();
}

template < class Msg >
void    
TimerContainer < Msg > ::postAdd(const TimerEntry<Msg> & newTimerEntry)
{
    //Lock lock(myMutex);
   _timerSet.insert(newTimerEntry);
   _idToTimer[newTimerEntry.getId()] = newTimerEntry;
}
