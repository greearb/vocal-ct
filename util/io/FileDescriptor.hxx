#if !defined(FILEDESCRIPTOR_DOT_H)
#define FILEDESCRIPTOR_DOT_H

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
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "vtypes.h"
#include <string>
#include <vector>

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{




/** Infrastructure in VOCAL related to Input/Output.<br><br>
 */
namespace IO
{


/** Type for the native file descriptor, for portability 
 *  and readability.<br><br>
 */
typedef     int     file_descriptor_t;


/** A wrapper class for the operating systems file descriptor.<br><br>
 *  
 *  @see    	Vocal::IO::file_descriptor_t
 *  @see    	Vocal::IO::Writer
 *  @see    	Vocal::SystemException
 */
class FileDescriptor : public Vocal::IO::Writer
{
    public:

	/** The value of a file descriptor when it is unusable. This
	 *  value may vary across operating systems.
	 */
    	static const file_descriptor_t	    INVALID;


	/** The default constructor creates an INVALID file descriptor.
	 *  This can also be used to attach to pre-existing file descriptors.
	 */
    	FileDescriptor(file_descriptor_t = INVALID);
	

	/** The destructor will close() the file descriptor if it is valid.
	 *
	 *  @exception Vocal::SystemException
	 */
        virtual ~FileDescriptor() throw (Vocal::SystemException);


	/** Mutator given a new native file descriptor.
	 */
    	void	setFD(file_descriptor_t);


	/** Accessor to native file descriptor.
	 */
    	file_descriptor_t   	getFD() const;
	

	/** When set, this file descriptor will block when used with
	 *  a system call that could potentially block, e.g select().
	 *
	 *  @exception Vocal::SystemException
	 */
	void	setBlocking()
	    	   throw ( Vocal::SystemException );


	/** When set, this file descriptor will not block when used with
	 *  a system call that could potentially block, e.g select(),
	 *  potentially returning a status code like EAGAIN.
	 */
	void	setNonblocking()
	    	    throw ( Vocal::SystemException );
	

    	/** Writes the string to this file descriptor.
	 *  <br><br>
    	 *  
	 *  Returns the number of bytes written. If a SystemStatus was 
	 *  thrown, the descriptor was set non-blocking and it would 
	 *  have blocked (status EAGAIN) or the read was interrupted by 
	 *  a signal (status EINTR). On a SystemException the descriptor 
	 *  encountered some system error.<br><br>
	 *
	 *  See write(2).
	 */
    	int 	write(const string &)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** See write(const string &).
	 */
    	int 	write(const char *)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of write. See write(const string &).
	 */
    	int 	write(const vector<u_int8_t> &)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of write. See write(const string &).
	 */
    	int 	write(const u_int8_t *, size_t)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );
				

    	/** This will read a string from the descriptor.
	 *  Attempts to read bytes for the descriptor up to the size
	 *  of the given string.<br><br>
    	 *
	 *  Returns the number of bytes read, which may be less than the
	 *  size of the string. If a SystemStatus was thrown, the 
	 *  descriptor was set non-blocking and it would have blocked 
	 *  (status EAGAIN) or the read was interrupted by a signal (status
	 *  EINTR). On a SystemException the descriptor encountered some 
	 *  system error.<br><br>
	 *
	 *  See read(2).
	 */
    	int 	read(string &)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Text version of read. See read(string &).
	 */
    	int 	read(char *, size_t)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of read See read(string &).
	 */
    	int 	read(vector<u_int8_t> &)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Binary version of read. See read(string &).
	 */
    	int 	read(u_int8_t *, size_t)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


	/** Return the resource back to the operating system. The file 
	 *  descriptor is set INVALID.
	 */
	void	close()
	    	    throw ( Vocal::SystemException );


	/** Equality relational operator.
	 */
	bool	operator==(const FileDescriptor &) const;


	/** Inequality relational operator.
	 */
	bool	operator!=(const FileDescriptor &) const;


	/** Less than relational operator.
	 */
	bool	operator<(const FileDescriptor &) const;


	/** Less than or equal relational operator.
	 */
	bool	operator<=(const FileDescriptor &) const;


	/** Greater than relational operator.
	 */
	bool	operator>(const FileDescriptor &) const;


	/** Greater than or equal relational operator.
	 */
	bool	operator>=(const FileDescriptor &) const;


	/** Write to an ostream.
	 */
	virtual ostream &       writeTo(ostream &) const;
	

    protected:

	/** The native file descriptor is available to classes extending
	 *  FileDescriptor.
	 */
    	file_descriptor_t   	fd_;


    private:


    	int 	writeToFD(void *, int length)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );
				

    	int 	readFromFD(void *, int length)
    	    	    throw ( Vocal::SystemException, Vocal::SystemStatus );

	/** Copying is suppressed.
	 */
    	FileDescriptor(const FileDescriptor &);


	/** Copying is suppressed.
	 */
	FileDescriptor &    	operator=(const FileDescriptor &);
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(FILEDESCRIPTOR_DOT_H)
