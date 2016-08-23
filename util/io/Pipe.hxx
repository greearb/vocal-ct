#if !defined(PIPE_DOT_H)
#define PIPE_DOT_H

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
#include "FileDescriptor.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{

class SystemException;


/** Infrastructure in VOCAL related to Input/Output.<br><br>
 */
namespace IO
{



/** A wrapper class for the operating systems pipe.<br><br>
 *  
 *  @see    	Vocal::IO::FileDescriptor
 *  @see    	Vocal::IO::Writer
 *  @see    	Vocal::SystemException
 */
class Pipe : public Vocal::IO::Writer
{
    public:

	/** The default constructor creates a pipe. If is file descriptor
	 *  array parameter is zero, then a new pipe will be created. 
	 *  Otherwise the given parameter will be used to initialize the pipe.
	 *
	 *  @param  descriptors     An optional array of two file descriptors, 
	 *  	    	    	    created using the pipe(2) system call.
	 * 
	 *  @exception	Vocal::SystemException	
	 *  	Thrown if a pipe could not be created.
	 */
    	Pipe(const file_descriptor_t * descriptors = 0)
	    throw ( Vocal::SystemException );


	/** Creates a pipe given a pair of file descriptors.
	 */
    	Pipe(
	    const FileDescriptor    & 	readFd,
	    const FileDescriptor    &	writeFd
	);
	

	/** The destructor will close() the file descriptors if valid.
	 *
	 *  @exception Vocal::SystemException
	 *  	Thrown if the closure of the pipe failed.
	 */
	virtual ~Pipe()
	    throw ( Vocal::SystemException );


	/** Accessor to FileDescriptors.
	 *
	 *  @param index    Index 0 indicated the file descriptor for reading,
	 *  	    	    and an index of 1 (or non zero) indicates the
	 *  	    	    file descriptor for writing.
	 */
    	FileDescriptor &    operator[](size_t index);
	

    	/** Accessor to file descriptor used for reading.
	 */
	FileDescriptor &    readFD();


    	/** Accessor to file descriptor used for reading.
	 */
	const FileDescriptor &    readFD() const;
	

	/** Accessor to file descriptor used for writing.
	 */
	FileDescriptor &    writeFD();
	

	/** Accessor to file descriptor used for writing.
	 */
	const FileDescriptor &    writeFD() const;

	
	/** Equality relational operator.
	 */
	bool	operator==(const Pipe &) const;


	/** Less than relational operator.
	 */
	bool	operator<(const Pipe &) const;


	/** Write to an ostream.
	 */
	virtual ostream &       writeTo(ostream &) const;
	

    private:

    	FileDescriptor	    readFD_;
	FileDescriptor	    writeFD_;


	/** Copying is suppressed.
	 */
    	Pipe(const Pipe &);


	/** Copying is suppressed.
	 */
	Pipe &	operator=(const Pipe &);
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(PIPE_DOT_H)
