#ifndef AccountingData_hxx
#define AccountingData_hxx

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


static const char* const AccountingDara_hxx_Version =
    "$Id: AccountingData.hxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include <BugCatcher.hxx>


/** Data container for holding Accounting Data for a given session
    <pre>
          <br>Usage of this Class</br>

           long MyAuthId;
           Sptr<AccoutingData> acctdata = new AccountingData(MyAuthId);
           int unusedsec = acctdata->getUnusedSeconds();

        For detailed Informantion about extensive use of this object please
        refer to /vocal/sip/b2b/AuthAgent.cxx

    </pre>
*/
class AccountingData : public BugCatcher {
   public:
      /// Create one with default values
      AccountingData(unsigned long sessionId)
        : mySessionId(sessionId),
          myUnusedSeconds(0),
          myStartTime(0),
          myEndTime(0)
      { };

      ///
      AccountingData( const AccountingData& src )
      {
          copyObj(src);
      }

      /// 
      const AccountingData& operator =( const AccountingData& src )
      {
          if(this != &src)
          {
              copyObj(src);
          }
          return *this;
      }

      ///
      unsigned long getSessionId() const { return mySessionId; };

      ///
      void copyObj(const AccountingData& src)
      {
          mySessionId = src.mySessionId;
          myUnusedSeconds = src.myUnusedSeconds; 
          myStartTime = src.myStartTime; 
          myEndTime = src.myEndTime; 
      }

      ///
      void setUnusedSeconds(int unusedCredit) { myUnusedSeconds = unusedCredit; };
      ///
      int getUnusedSeconds() const { return myUnusedSeconds; };

      ///
      

      ///
      void setStartTime(time_t t) { myStartTime = t; };
      ///
      time_t getStartTime() const { return myStartTime; };
      ///
      void setEndTime(time_t t) { myEndTime = t; };
      ///
      time_t getEndTime() const { return myEndTime; };

   private:
      /// 
      unsigned long mySessionId;
      ///
      int myUnusedSeconds; 
      ///
      time_t myStartTime; 
      ///
      time_t myEndTime; 
};


#endif
