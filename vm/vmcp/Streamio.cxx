
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
 */  



#include "Streamio.h"

#include <sys/types.h>
#include <sys/time.h>
#include "cpLog.h"
#include <stdio.h>
#include <errno.h>

/** Default Constructor */
Streamio::Streamio() 
{
	m_iOutputFd=1;
	m_iInputFd=0;
#if defined(unix) || defined(__MACH__)
	m_ioType=1;
#else
	m_ioType=0;
#endif

}

Streamio::Streamio(int in,int out)
{
	m_iOutputFd=out;
	m_iInputFd=in;
}

/** Destructor */
Streamio::~Streamio()
{
	m_iOutputFd=-1;
	m_iInputFd=-1;
}

/** Assign the socket file descriptor */   
void Streamio::assign(int in,int out)
{
	m_iOutputFd=out;
	m_iInputFd=in;
}

/** Check if the socket is ready */
bool Streamio::isReady()
{
	fd_set ifd;
	struct timeval tv;

	FD_ZERO(&ifd);
	FD_SET(m_iInputFd,&ifd);
	tv.tv_sec=0;
	tv.tv_usec=0;
	::select(m_iInputFd+1,&ifd,NULL,NULL,&tv);
	if( FD_ISSET(m_iInputFd,&ifd) ) return true;
	return false;
}


/** Read from the socket */  
int Streamio::read()
{
	char val;
	switch(m_ioType)
	{
		case filehandle:
		{
			if( ::read(m_iInputFd,&val,1) != -1 ) return val;
			else
			{
			    cpLog(LOG_INFO, "bad read from filehandle %d", errno);
			}
		}
		break;
		case sockethandle:
		{
			if( ::recv(m_iInputFd,&val,1,0) > 0 ) return val;
			else
			{
			    cpLog(LOG_ERR, "bad read from sockethandle %s", strerror(errno));
			    perror("");
			}
		}
		break;
	}		
	return -1;
}

/** Write to the socket */ 
int Streamio::write(char c)
{
	int rc;
	switch(m_ioType)
	{
		case filehandle:
		{
			if( (rc=::write(m_iOutputFd,&c,1)) != -1 ) return rc;
		}
		break;
		case sockethandle:
		{
			if( (rc=::send(m_iOutputFd,&c,1,0)) > 0 ) return rc;
		}
		break;
	}		
	return 0;
}


/** Write to the socket */ 
int Streamio::write(const string& str)
{
	switch(m_ioType)
	{
		case filehandle:
		{
			if( ::write(m_iOutputFd,str.c_str(),str.size()) != -1 ) return 1;
		}
		break;
		case sockethandle:
		{
			if( ::send(m_iOutputFd,str.c_str(),str.size(),0) > 0 ) return 1;
		}
		break;
	}		
	return 0;
}


/** Set the type */
void Streamio::setIoType(int type)
{
	m_ioType=type;
}

/** Return the Input File Descriptor */
int Streamio::getInputFd()
{
	return m_iInputFd;
}

/** Return the Output File Descriptor */    
int Streamio::getOutputFd()
{
	return m_iOutputFd;
}
