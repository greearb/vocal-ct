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

static const char* const TlsConnection_cxx_version =
    "$Id: TlsConnection.cxx,v 1.3 2004/05/07 17:30:46 greear Exp $";

#include "TlsConnection.hxx"
#ifdef VOCAL_HAS_OPENSSL
 #include <openssl/err.h>
#endif
#include "cpLog.h"

#if 0
// No more locking needed I think. --Ben
extern "C"
{
    void my_locking_function(int mode, int n, const char *file, int line);
};




void my_locking_function(int mode, int n, const char *file, int line)
{
#ifdef VOCAL_HAS_OPENSSL
    static Vocal::Threads::Mutex myMutex[2000];

    if(mode & CRYPTO_LOCK)
    {
        myMutex[n].lock();
    }
    else
    {
        myMutex[n].unlock();
    }
#endif
}

//id_function()




static void setCallbacks()
{
#ifdef VOCAL_HAS_OPENSSL
    CRYPTO_set_locking_callback(my_locking_function);
#endif
}
#endif

TlsConnection::TlsConnection(bool blocking)
    : Connection(blocking),
      ctx(0),
      ssl(0),
      meth(0),
      myCertificate(),
      myKey()
{
}

TlsConnection::TlsConnection(Connection& other, bool really)
    : Connection(other, really),
      ctx(0),
      ssl(0),
      meth(0),
      myCertificate(),
      myKey()
{
    assert(!shouldCloseOnDestruct());
}

TlsConnection::~TlsConnection() {
#ifdef VOCAL_HAS_OPENSSL
    if (ssl) {
        SSL_free (ssl);
        ssl = NULL;
    }

    if (ctx) {
        SSL_CTX_free (ctx);
        ctx = NULL;
    }
#endif

}



bool 
TlsConnection::isTls() const
{
    return ssl != 0;
}

int 
TlsConnection::iclose()
{
    if(ssl) {
#ifdef VOCAL_HAS_OPENSSL
	cpLog(LOG_DEBUG_STACK, "closing SSL");
        SSL_shutdown(ssl);
	
	SSL_free (ssl);
	SSL_CTX_free (ctx);
        ssl = NULL;
        ctx = NULL;
#endif
    }
    cpLog(LOG_DEBUG_STACK, "closing regular connection");

    return Connection::iclose();
}


ssize_t 
TlsConnection::iread(char* buf, size_t count)
{
    if(ssl)
    {
#ifdef VOCAL_HAS_OPENSSL
        while(1)
        {
            ssize_t t = SSL_read(ssl, buf, count);
            if(t == -1) 
            {
                ERR_print_errors_fp(stderr);
                int myerr = SSL_get_error(ssl, t);
                if(myerr == SSL_ERROR_WANT_READ)
                {
                    // select here to wait for the right bits but keep
                    // from looping relentlessly.  this only will work
                    // on UNIX.
                    int fd = SSL_get_fd(ssl);

                    if(fd >= 0)
                    {
                        fd_set rfds;
                        struct timeval tv;
                        int retval;
                        
                        FD_ZERO(&rfds);
                        FD_SET(fd, &rfds);
                        tv.tv_sec = 5;
                        tv.tv_usec = 0;
                        
                        retval = select(fd + 1, &rfds, NULL, NULL, &tv);
                        
                        if(retval <= 0)
                        {
                            errno = EIO;
                            return -1;
                        }
                    }
                    else
                    {
                        return -1;
                    }
                }
                else 
                {
                    return -1;
                }
            }
            else
            {
                return t;
            }
        }
#endif
    }
    return Connection::iread(buf, count);
}


ssize_t TlsConnection::iwrite(char* buf, size_t count)
{
    if(ssl)
    {
#ifdef VOCAL_HAS_OPENSSL
        ssize_t t = SSL_write(ssl, buf, count);
	if(t == -1) 
	{
	    ERR_print_errors_fp(stderr);
            int myerr = SSL_get_error(ssl, t);
            if(myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE)
            {
                return 0;
            }
	}
	return t;
#endif
    }
    return Connection::iwrite(buf, count);
}


int 
TlsConnection::initTlsClient()
{
#ifdef VOCAL_HAS_OPENSSL
    SSLeay_add_ssl_algorithms();
    meth = TLSv1_client_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth);
    if(ctx == 0)
    {
	return -1;
    }

    ssl = SSL_new (ctx);
    if(ssl == 0)
    {
	return -1;
    }
    int err;

    err = SSL_set_fd (ssl, _connId);

    if(!err)
    {
        cpLog(LOG_DEBUG, "failed to connect socket to TLS!");
        return -1;
    }


    while(1)
    {
        err = SSL_connect (ssl);

        if(err <= 0)
        {
            // check for ERROR_WANT_READ / ERROR_WANT_WRITE
            int myerr = SSL_get_error(ssl, err);
            
            if(myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE)
            {
                cpLog(LOG_DEBUG, "try again!\n");
                vusleep(1000);
            }
            else
            {
                // issue
                cpLog(LOG_DEBUG, "TLS error: %d ( %s )", err, ERR_error_string(SSL_get_error(ssl, err),0));
                break;
            }
        }
        else
        {
            break;
        }
    }

    return err;
#else
    return -1;
#endif
}


int 
TlsConnection::initTlsServer(const char* certificate,
                             const char* key)
{
#ifdef VOCAL_HAS_OPENSSL
    // check for TLSness

    char test[100];

    int bytes = ::recv(_connId, test, 32, MSG_PEEK);

    test[bytes-1] = '\0';

    if(strcmp(test, ".") == 0)
    {
        cpLog(LOG_DEBUG_STACK, "TLS connection!\n");
    }

    char buf2[256];
    char* bptr = buf2;

    for(int i = 0; i < bytes ; ++i)
    {
        sprintf(bptr, "%2.2x", test[i]);
        bptr += 2;
    }
    cpLog(LOG_DEBUG_STACK, "%s %d", buf2, bytes);

    myCertificate = certificate;
    myKey = key;
    
    cpLog(LOG_DEBUG_STACK, "initalizing TLS server connection");

    SSLeay_add_ssl_algorithms();
    meth = TLSv1_server_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth);
    if(ctx == 0) {
	cpLog(LOG_ERR, "no ctx");
	return -1;
    }

    // at this point, you need your certs.
    LocalScopeAllocator lo1;
    LocalScopeAllocator lo2;

    if (SSL_CTX_use_certificate_file(ctx, 
				     myCertificate.getData(lo1), 
				     SSL_FILETYPE_PEM) <= 0) 
    {
	ERR_print_errors_fp(stderr);
	cpLog(LOG_ERR, "failed to set certificate file %s",
	      myCertificate.getData(lo1));
	return -1;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, 
				    myKey.getData(lo2), 
				    SSL_FILETYPE_PEM) <= 0) 
    {
	ERR_print_errors_fp(stderr);
	cpLog(LOG_ERR, "failed to set key file %s",
	      myKey.getData(lo1));
	return -1;
    }

    int err;
    assert(ctx != 0);
    ssl = SSL_new (ctx);
    if(ssl == 0) {
	cpLog(LOG_ERR, "failed to create new ssl");
	return -1;
    }
    assert(_connId >= 0);
    SSL_set_fd (ssl, _connId);

    while(1)
    {
        err = SSL_accept (ssl);
        if(err <= 0)
        {
            // check for ERROR_WANT_READ / ERROR_WANT_WRITE
            int myerr = SSL_get_error(ssl, err);
            
            if(myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE)
            {
                cpLog(LOG_DEBUG, "try again!\n");
                vusleep(1000);
            }
            else
            {
                // issue
                cpLog(LOG_DEBUG, "TLS error: %d ( %s )", err, ERR_error_string(SSL_get_error(ssl, err),0));
                break;
            }
        }
        else
        {
            break;
        }
    }
    return err;
#else
    return -1;
#endif
}


SSL* 
TlsConnection::getSsl()
{
    return ssl;
}


bool 
TlsConnection::hasTls()
{
#ifdef VOCAL_HAS_OPENSSL
    return true;
#else
    return false;
#endif
}


Data
TlsConnection::getErrMsg(int e)
{
    char tmp[1024];

#ifdef VOCAL_HAS_OPENSSL
    ERR_error_string_n(SSL_get_error(ssl, e), tmp, 1024);
#endif
    return Data(tmp);
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
