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
    "$Id: TlsConnection.cxx,v 1.6 2004/06/06 08:32:37 greear Exp $";

#include "TlsConnection.hxx"
#ifdef VOCAL_HAS_OPENSSL
 #include <openssl/err.h>
#endif
#include "cpLog.h"

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
TlsConnection::isTls() const {
    return ssl != 0;
}

int TlsConnection::iclose() {
    if (ssl) {
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


int TlsConnection::iread() {
    if (ssl) {
#ifdef VOCAL_HAS_OPENSSL
        int count = 8196;
        char buf[count];
        int rd = SSL_read(ssl, buf, count);
        if (t < 0) {
            int myerr = SSL_get_error(ssl, t);
            if (myerr == SSL_ERROR_WANT_READ) {
                // Try more later
                rd = 0;
            }
            else {
                ERR_print_errors_fp(stderr);
                // Fatal error, closing
                // We are SOL it appears.
                char errm[256];
                ERR_error_string(SSL_get_error(ssl, errm, 255));
                cpLog(LOG_ERR, "ERROR:  Closing SSL connection due to bad read: %s(%d)\n",
                      errm, myerr);
                close();
            }
        }
        else {
            rcvBuf.append(buf, rd);
        }
        return rd;
#endif
    }
    return Connection::iread();
}


int TlsConnection::iwrite() {
    if (ssl) {
#ifdef VOCAL_HAS_OPENSSL
        int count = outBuf.getCurLen();
        char buf[count];
        outBuf.peekBytes(outbuf, count);
        int t = SSL_write(ssl, buf, count);
	if (t < 0) {
	    ERR_print_errors_fp(stderr);
            int myerr = SSL_get_error(ssl, t);
            if (myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE) {
                return 0;
            }
            else {
                // We are SOL it appears.
                char errm[256];
                ERR_error_string(SSL_get_error(ssl, errm, 255));
                cpLog(LOG_ERR, "ERROR:  Closing SSL connection due to bad write: %s(%d)\n",
                      errm, myerr);
                close();
            }
	}
        else {
            outBuf.dropFromTail(t);
        }
	return t;
#endif
    }
    return Connection::iwrite();
}


int TlsConnection::initTlsClient() {
    assert(_isClient);
#ifdef VOCAL_HAS_OPENSSL
    SSLeay_add_ssl_algorithms();
    meth = TLSv23_client_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth);
    if (ctx == 0) {
	return -1;
    }

    ssl = SSL_new (ctx);
    if (ssl == 0) {
	return -1;
    }
    int err;

    err = SSL_set_fd (ssl, _connId);

    if (!err) {
        cpLog(LOG_DEBUG, "failed to connect socket to TLS!");
        return -1;
    }

    err = SSL_connect (ssl);

    if (err <= 0) {
        // check for ERROR_WANT_READ / ERROR_WANT_WRITE
        int myerr = SSL_get_error(ssl, err);
            
        if (myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE) {
            cpLog(LOG_DEBUG, "try again!\n");
            _inProgress = true;
            err = 0;
        }
        else {
            // issue
            cpLog(LOG_DEBUG, "TLS error: %d ( %s )",
                  err, ERR_error_string(SSL_get_error(ssl, err),0));
            close();
        }
    }

    return err;
#else
    return -1;
#endif
}


void TlsConnection::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                         uint64 now) {
#ifdef VOCAL_HAS_OPENSSL
    int err = 0;
    if (checkIfSet(input_fds)) {
        if (_isClient) {
            if (_inProgress) {
                err = SSL_connect (ssl);
            }
            else {
                err = iread();
            }
        }
        else {
            if (_inProgress) {
                err = SSL_accept (ssl);
            }
            else {
                err = iread();
            }
        }

        if (err <= 0) {
            // check for ERROR_WANT_READ / ERROR_WANT_WRITE
            int myerr = SSL_get_error(ssl, err);
            
            if (myerr == SSL_ERROR_WANT_READ || myerr == SSL_ERROR_WANT_WRITE) {
                cpLog(LOG_DEBUG, "try again!\n");
            }
            else {
                // issue
                cpLog(LOG_ERR, "TLS error: %d ( %s )",
                      err, ERR_error_string(SSL_get_error(ssl, err),0));
                close();
            }
        }
        else {
            if (_inProgress) {
                // Progress completed it appears
                _inProgress = false;
            }
        }
    }
    return err;
#endif
    return Connection::tick(input_fds, output_fds, exc_fds, now);
}

int TlsConnection::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                          int& maxdesc, uint64& timeout, uint64 now) {
    Connection::setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
    return 0;
}


int 
TlsConnection::initTlsServer(const char* certificate,
                             const char* key) {
    assert(!_isClient);
#ifdef VOCAL_HAS_OPENSSL
    // check for TLSness

    char test[100];

    int bytes = ::recv(_connId, test, 32, MSG_PEEK);

    test[bytes-1] = '\0';

    if (strcmp(test, ".") == 0) {
        cpLog(LOG_DEBUG_STACK, "TLS connection!\n");
    }

    char buf2[256];
    char* bptr = buf2;

    for (int i = 0; i < bytes ; ++i) {
        sprintf(bptr, "%2.2x", test[i]);
        bptr += 2;
    }
    cpLog(LOG_DEBUG_STACK, "%s %d", buf2, bytes);

    myCertificate = certificate;
    myKey = key;
    
    cpLog(LOG_DEBUG_STACK, "initalizing TLS server connection");

    SSLeay_add_ssl_algorithms();
    meth = TLSv23_server_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth);
    if (ctx == 0) {
	cpLog(LOG_ERR, "no ctx");
	return -1;
    }

    // at this point, you need your certs.
    if (SSL_CTX_use_certificate_file(ctx, 
				     myCertificate.c_str(),
				     SSL_FILETYPE_PEM) <= 0) {
	ERR_print_errors_fp(stderr);
	cpLog(LOG_ERR, "failed to set certificate file %s",
	      myCertificate.c_str());
	return -1;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, 
				    myKey.c_str(),
				    SSL_FILETYPE_PEM) <= 0) {
	ERR_print_errors_fp(stderr);
	cpLog(LOG_ERR, "failed to set key file %s",
	      myKey.getData(lo1));
	return -1;
    }

    int err;
    assert(ctx != 0);
    ssl = SSL_new (ctx);
    if (ssl == 0) {
	cpLog(LOG_ERR, "failed to create new ssl");
	return -1;
    }
    assert(_connId >= 0);
    SSL_set_fd (ssl, _connId);

    return 0;
#else
    return -1;
#endif
}


SSL* TlsConnection::getSsl() {
    return ssl;
}


bool TlsConnection::hasTls() {
#ifdef VOCAL_HAS_OPENSSL
    return true;
#else
    return false;
#endif
}


Data
TlsConnection::getErrMsg(int e) {
    char tmp[1024];

#ifdef VOCAL_HAS_OPENSSL
    ERR_error_string_n(SSL_get_error(ssl, e), tmp, 1024);
#endif
    return Data(tmp);
}
