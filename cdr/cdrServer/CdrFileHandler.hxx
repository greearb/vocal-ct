#ifndef  CdrFileHandler_hxx
#define CdrFileHandler_hxx

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


static const char* const CdrFileHandler_hxx_Version =
    "$Id: CdrFileHandler.hxx,v 1.1 2004/05/01 04:14:55 greear Exp $";


#include <stdio.h>
#include <fcntl.h>
#include <string>

#include "CdrData.hxx"
#include "VCdrException.hxx"

/**
   CdrFileHandler deals with all the file tasks
 **/

class CdrFileHandler
{
    public:

        ///
        CdrFileHandler() {}

        /**
         * Constructor
         * @param string& directory for file
         * @param string& filename to handle
         */
        CdrFileHandler( const string &directory,
                        const string &filename )
            throw(VCdrException&);

        /// Copy constructor
        CdrFileHandler(const CdrFileHandler&);

        /// default action is to close the file
        virtual ~CdrFileHandler();

        /**
         * Write record to file
         * @param CdrRadius& data structure
         * @return int number of bytes written
         */
        int writeCdr( const CdrRadius &ref );

        /**
         * Read 1 line of tokens from file
         * @param list< string >& token list
         * @param char delimiter for token list
         * @return number of tokens read
         */
        int readTokens( list < string > &tokenList,
                        const char delimiter = ',' )
            throw(VCdrException& );

        /**
         * Open file
         * @param int open flags
         * @param mode_t open mode
         * @return void
         */
        void open(const int flags = O_RDONLY,
                  const mode_t mode = 0666)
            throw(VCdrException&);

        /// Close file
        void close();

        /// End of file
        bool eof() const
        {
            return m_eof;
        }

        /**
         * Set working directory
         * @param string& directory
         * @return void
         */
        void setDirectory( const string &dir );

        /**
         * Set working filename
         * @param string& filename
         * @return void
         */
        void setFileName( const string &fn );

        /// Turn file size checking on
        void setCheckFileSizeOn();

        /**
         * Set rollover file suffix
         * @param string& suffex
         * @return void
         */
        void setFileSuffix( const string &suffix )
        {
            m_renameSuffix = suffix;
        }

        /**
         * Set rollover Size
         * @param int size in bytes
         * @return void
         */
        void setRolloverSize( const int n )
        {
            m_rolloverSize = n;
        }

        /**
         * Set rollover Period
         * @param int time in seconds
         * @return void
         */
        void setRolloverPeriod( const unsigned long int n )
        {
            m_rolloverPeriod = n;
        }

        /// return the file name, no directory included
        string getFileName() const
        {
            return m_fileName;
        }

        /// return the fully qualified file name (includes directory path)
        string getFullFileName() const
        {
            return m_dirPathFileName;
        }

        /// update the file size and last modified variables
        void updateDynamicVars() throw (VCdrException&);

        /// return the date of the file, updated by updateDynmicVars()
        long getLastModification() const
        {
            return m_lastModified;
        }

        /// return the size of the file, updated by updateDynmicVars()
        long getFileSize() const
        {
            return m_fileSize;
        }

        /**
         * If file size is larger than m_rolloverSize or older than
         * m_rolloverPeriod, rename it appending the date/time to
         * the end of the file name and open a new file of m_fileName
         */
        void checkFileSize() throw (VCdrException&);

        /**
         * Get the list of filenames from directory filtered based on substrs
         * @param list <string>& list of files found
         * @param string& directory path to search
         * @param string& filename substring to filter for
         * @param string& suffix substring to filter for
         * @return int number of files placed in list
         */
        static int directoryList( list < string > &fileList,
                                  const string &dirPath,
                                  const string &fileNameSubstr,
                                  const string &suffixSubstr );

    private:

        /// Manages the read buffer and returns 1 character at a time
        char readChar();

                                                 ///
        string m_directory;                      ///
        string m_fileName;                       ///
        string m_dirPathFileName;                ///
        string m_renameSuffix;                   ///
        unsigned long int m_fileCheckFreq;       ///
        int m_rolloverSize;                      ///
        unsigned long int m_rolloverPeriod;      ///
        int m_fileDesc;                          ///
        bool m_eof;                              ///
        char m_readBuffer[1024];                 ///
        char *m_bptr;                            ///
        int m_bytesRead;                         ///
        int m_openFlags;                         ///
        int m_openMode;                          ///
        long m_timeOpened;                       ///
        long m_fileSize;                         ///
        long m_lastModified;                     ///
};
#endif
