
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


static const char* const CdrBilling_cxx_Version =
    "$Id: CdrBilling.cxx,v 1.2 2004/06/09 07:19:34 greear Exp $";


#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <list>
#include <map>
#include <string>

#include "CdrBilling.hxx"
#include "VCdrException.hxx"
#include "VRadiusException.hxx"
#include "CdrFileHandler.hxx"
#include "MindClient.hxx"
#include "cpLog.h"


const int BILLING_FILE_LOCK_LIMIT = 3600*6;       // 6 hours
const long int BILLING_STORAGE_LIMIT = 3600*72;   // 72 hours
const int BILLING_MIN_DELETE_SIZE = 1000000;      // total to delete each cycle
const long int BILLING_MAX_TOTAL_SIZE = 20000000; // maximum space for all billing files


CdrBilling::CdrBilling() {
   char* envptr = getenv("BILLING_FILE_LOCK_LIMIT");
   if (envptr) {
      cpLog(LOG_INFO, "Set BILLING_FILE_LOCK_LIMIT from env:%s", envptr);
      billingLockTimeLimit = atoi(envptr);
   }
   else {
      billingLockTimeLimit = BILLING_FILE_LOCK_LIMIT;
   }

   errorFileExt = ".error";

   envptr = getenv("BILLING_ERROR_FILE_EXT");
   if (envptr) {
      cpLog(LOG_INFO, "Set BILLING_ERROR_FILE_EXT from env:%s", envptr);
      errorFileExt = envptr;
   }
}


int CdrBilling::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                       int& maxdesc, uint64& timeout, uint64 now) {
   uint64 nxtx = lastConnectTime + cdata.m_billingFrequency;
   if (nxtx <= now) {
      timeout = 0;
   }
   else {
      if (timeout > (nxtx - now)) {
         timeout = nxtx - now;
      }
   }
}
   
void CdrBilling::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      uint64 now) {

   if (sendBillingRecords( cdata, userAliases )) {
      lastConnectTime = now;
   }
   else {
      if (now - lastConnectTime > BILLING_STORAGE_LIMIT) {
         deleteOldestFiles(cdata);
      }
   }
}

bool
CdrBilling::sendBillingRecords( const CdrConfig &cdata,
                                CdrUserCache &userAliases ) {
    try {
        MindClient::initialize(cdata.m_localIp,
                               cdata.m_radiusServerHost.c_str(),
                               cdata.m_radiusSecretKey.c_str(),
                               cdata.m_radiusRetries);
    }
    catch (VRadiusException&e) {
        cpLog(LOG_ALERT,
              "Cannot connect with Mind Billing Server, skipping billing cycle");
        return false;
    }

    if (! MindClient::instance()) {
       return false;
    }

    string buf = cdata.m_billingDirectory + "/" + cdata.m_billingLockFile;
    FileStackLock fsl(buf, billingLockTimeLimit);
    if (fsl.isLocked()) {

       // get a listing of the billing files which need to be sent to
       // the billing server
       
       list < string > cdrFileList;
       CdrFileHandler::directoryList(cdrFileList,
                                     cdata.m_billingDirectory,
                                     cdata.m_billingFileName,
                                     cdata.m_unsentFileExt);

       for (list < string > ::iterator itr = cdrFileList.begin();
            itr != cdrFileList.end(); itr++) {
          int errCount = 0;
          int recCount = 0;

          CdrFileHandler bfile(cdata.m_billingDirectory, *itr);
          try {
             bfile.open(O_RDONLY);
             cpLog(LOG_INFO, "Opened billing file %s" , (*itr).c_str());
          }
          catch (VCdrException &e) {
             cpLog(LOG_ALERT, "Failed to open billing file %s", (*itr).c_str());
             continue;
          }

          while (!bfile.eof()) {
             CdrRadius ref;

             list < string > tokenList;
             bfile.readTokens(tokenList);
             ref.setValues(tokenList);
             
             if (ref.m_callEvent == CALL_BILL) {
                recCount++;

                //
                // convert user aliases to the master user
                //
      
                // Provisioning supplies the customer code/ANI based on userId
                string ANI(userAliases.getCustomerId(ref.m_userId));

                string recvNum;
                if (ref.m_DTMFCalledNum[0] != 0) {
                   recvNum = userAliases.getCustomerId(ref.m_DTMFCalledNum);
                }
                else {
                   recvNum = userAliases.getCustomerId(ref.m_E164CalledNum);
                }

                // set userID and ANI
                strncpy(ref.m_userId, ANI.c_str(), sizeof(ref.m_userId));
                strncpy(ref.m_ANI, ANI.c_str(), sizeof(ref.m_ANI));

                // clear DTMF and use E164 (standardized format)
                memset(ref.m_DTMFCalledNum, 0, sizeof(ref.m_DTMFCalledNum));
                strncpy(ref.m_E164CalledNum, recvNum.c_str(), sizeof(ref.m_E164CalledNum));

                //
                // send the record to MIND
                //

                // need to emulate a call by sending start and stop records
                if (!MindClient::instance().accountingStartCall(ref) ||
                    !MindClient::instance().accountingStopCall(ref)) {
                   errCount++;
                   if ((errorFile == 0) && (!errFileBusted)) {
                      // Opening error file for call records which are rejected by
                      // Mind billing server

                      string errFile(cdata.m_billingFileName + errorFileExt);
                      errorFile = new CdrFileHandler(cdata.m_billingDirectory,
                                                     errFile.c_str());

                      try {
                         errorFile.open(O_WRONLY | O_CREAT | O_APPEND);
                      }
                      catch (VCdrException &e) {
                         cpLog(LOG_ALERT, "Failed to open billing error file %s",
                               errFile.c_str());
                         errFileBusted = true;
                      }
                   }
                   if (errorFile != 0) {
                      errorFile->writeCdr(ref);
                   }
                }
             }
          }//while

          cpLog(LOG_INFO, "Billing File %s:, Total recs:%d, Rejected recs:%d, copied to file %s",
                (*itr).c_str(), recCount, errCount, errFile.c_str());

          bfile.close();

          int pos = bfile.getFullFileName().rfind(cdata.m_unsentFileExt);
          string newFileName = bfile.getFullFileName().erase(pos);
          rename(bfile.getFullFileName().c_str(), newFileName.c_str());
       }
    }

    // I see no reason to get rid of this...  --Ben
    //MindClient::destroy();
    
    return true;
}

void
CdrBilling::deleteOldestFiles( const CdrConfig &cdata )
{
    int maxHours = BILLING_STORAGE_LIMIT / 3600;
    cpLog(LOG_ALERT, "The billing server has been unreachable for more than %d hours",
          maxHours);

    int totalByteCount = 0;
    int deletedByteCount = 0;

    map < long, CdrFileHandler > fileDateMap;
    list < string > cdrFileList;

    // get a list of files eligible for deletion (to free up space)
    //
    CdrFileHandler::directoryList(cdrFileList,
                                  cdata.m_billingDirectory,
                                  cdata.m_billingFileName,
                                  cdata.m_unsentFileExt);

    // iterate through list to get file attributes - file size,
    // and last modified time. Key off of the last modified time
    // so we can reverse iterate and delete the files at the
    // end of the list
    //
    for (list < string > ::iterator itr = cdrFileList.begin();
            itr != cdrFileList.end();
            itr++)
    {
        try
        {
            CdrFileHandler bfile(cdata.m_billingDirectory, *itr);
            bfile.open(O_RDONLY);
            totalByteCount += bfile.getFileSize();
            bfile.close();
            fileDateMap[bfile.getLastModification()] = bfile;
        }
        catch (VCdrException &e)
        {
            cpLog(LOG_ALERT, "Failed to open billing file %s", (*itr).c_str());
            continue;
        }
    }

    if ( totalByteCount < BILLING_MAX_TOTAL_SIZE )
    {
        int kbCount = totalByteCount / 1000;
        int maxKbCount = BILLING_MAX_TOTAL_SIZE / 1000;

        cpLog(LOG_ALERT,
              "Since the billing files are only consuming %dKB of the %dKB limit, no billing files will be deleted",
              kbCount, maxKbCount);
        return ;
    }

    // iterate through the list and delete the oldest files
    //
    map < long, CdrFileHandler > ::iterator itr = fileDateMap.begin();
    int fileCount = 0;

    for (; itr != fileDateMap.end(); itr++)
    {
        if ( deletedByteCount >= BILLING_MIN_DELETE_SIZE )
            break;

        long fileSize = ((*itr).second).getFileSize();
        string fileName = ((*itr).second).getFullFileName();

        deletedByteCount += fileSize;
        fileCount++;

        unlink(fileName.c_str());
        cpLog(LOG_ALERT, "Deleted file %s", fileName.c_str());
    }

    int deletedKb = deletedByteCount / 1000;
    cpLog(LOG_ALERT, "Deleted %d file(s) totaling %dKb", fileCount, deletedKb);
}
