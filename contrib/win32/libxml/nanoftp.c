/**
 * ftp.c: basic handling of an FTP command connection to check for
 *        directory availability. No transfer is needed.
 *
 *  Reference: RFC 959
 */

#ifdef WIN32
#define INCLUDE_WINSOCK
#include "win32config.h"
#else
#include "config.h"
#endif
#include "xmlversion.h"

#ifdef LIBXML_FTP_ENABLED
#include <stdio.h>
#include <string.h>

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h> 
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_RESOLV_H
#include <resolv.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/nanoftp.h>

/* #define DEBUG_FTP 1  */
#ifdef STANDALONE
#ifndef DEBUG_FTP
#define DEBUG_FTP 1
#endif
#endif

static char hostname[100];

#define FTP_COMMAND_OK		200
#define FTP_SYNTAX_ERROR	500
#define FTP_GET_PASSWD		331

typedef struct xmlNanoFTPCtxt {
    char *protocol;	/* the protocol name */
    char *hostname;	/* the host name */
    int port;		/* the port */
    char *path;		/* the path within the URL */
    char *user;		/* user string */
    char *passwd;	/* passwd string */
    struct sockaddr_in ftpAddr; /* the socket address struct */
    int passive;	/* currently we support only passive !!! */
    int controlFd;	/* the file descriptor for the control socket */
    int dataFd;		/* the file descriptor for the data socket */
    int state;		/* WRITE / READ / CLOSED */
    int returnValue;	/* the protocol return value */
} xmlNanoFTPCtxt, *xmlNanoFTPCtxtPtr;

static int initialized = 0;
static char *proxy = NULL;	/* the proxy name if any */
static int proxyPort = 0;	/* the proxy port if any */
static char *proxyUser = NULL;	/* user for proxy authentication */
static char *proxyPasswd = NULL;/* passwd for proxy authentication */
static int proxyType = 0;	/* uses TYPE or a@b ? */

/**
 * xmlNanoFTPInit:
 *
 * Initialize the FTP protocol layer.
 * Currently it just checks for proxy informations,
 * and get the hostname
 */

void
xmlNanoFTPInit(void) {
    const char *env;

    if (initialized)
	return;

    gethostname(hostname, sizeof(hostname));

    proxyPort = 21;
    env = getenv("no_proxy");
    if (env != NULL)
	return;
    env = getenv("ftp_proxy");
    if (env != NULL) {
	xmlNanoFTPScanProxy(env);
    } else {
	env = getenv("FTP_PROXY");
	if (env != NULL) {
	    xmlNanoFTPScanProxy(env);
	}
    }
    env = getenv("ftp_proxy_user");
    if (env != NULL) {
	proxyUser = xmlMemStrdup(env);
    }
    env = getenv("ftp_proxy_password");
    if (env != NULL) {
	proxyPasswd = xmlMemStrdup(env);
    }
    initialized = 1;
}

/**
 * xmlNanoFTPClenup:
 *
 * Cleanup the FTP protocol layer. This cleanup proxy informations.
 */

void
xmlNanoFTPCleanup(void) {
    if (proxy != NULL) {
	xmlFree(proxy);
	proxy = NULL;
    }
    if (proxyUser != NULL) {
	xmlFree(proxyUser);
	proxyUser = NULL;
    }
    if (proxyPasswd != NULL) {
	xmlFree(proxyPasswd);
	proxyPasswd = NULL;
    }
    hostname[0] = 0;
    initialized = 0;
    return;
}

/**
 * xmlNanoFTPProxy:
 * @host:  the proxy host name
 * @port:  the proxy port
 * @user:  the proxy user name
 * @passwd:  the proxy password
 * @type:  the type of proxy 1 for using SITE, 2 for USER a@b
 *
 * Setup the FTP proxy informations.
 * This can also be done by using ftp_proxy ftp_proxy_user and
 * ftp_proxy_password environment variables.
 */

void
xmlNanoFTPProxy(const char *host, int port, const char *user,
	        const char *passwd, int type) {
    if (proxy != NULL)
	xmlFree(proxy);
    if (proxyUser != NULL)
	xmlFree(proxyUser);
    if (proxyPasswd != NULL)
	xmlFree(proxyPasswd);
    if (host)
	proxy = xmlMemStrdup(host);
    if (user)
	proxyUser = xmlMemStrdup(user);
    if (passwd)
	proxyPasswd = xmlMemStrdup(passwd);
    proxyPort = port;
    proxyType = type;
}

/**
 * xmlNanoFTPScanURL:
 * @ctx:  an FTP context
 * @URL:  The URL used to initialize the context
 *
 * (Re)Initialize an FTP context by parsing the URL and finding
 * the protocol host port and path it indicates.
 */

static void
xmlNanoFTPScanURL(void *ctx, const char *URL) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    const char *cur = URL;
    char buf[4096];
    int index = 0;
    int port = 0;

    if (ctxt->protocol != NULL) { 
        xmlFree(ctxt->protocol);
	ctxt->protocol = NULL;
    }
    if (ctxt->hostname != NULL) { 
        xmlFree(ctxt->hostname);
	ctxt->hostname = NULL;
    }
    if (ctxt->path != NULL) { 
        xmlFree(ctxt->path);
	ctxt->path = NULL;
    }
    if (URL == NULL) return;
    buf[index] = 0;
    while (*cur != 0) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[index] = 0;
	    ctxt->protocol = xmlMemStrdup(buf);
	    index = 0;
            cur += 3;
	    break;
	}
	buf[index++] = *cur++;
    }
    if (*cur == 0) return;

    buf[index] = 0;
    while (1) {
        if (cur[0] == ':') {
	    buf[index] = 0;
	    ctxt->hostname = xmlMemStrdup(buf);
	    index = 0;
	    cur += 1;
	    while ((*cur >= '0') && (*cur <= '9')) {
	        port *= 10;
		port += *cur - '0';
		cur++;
	    }
	    if (port != 0) ctxt->port = port;
	    while ((cur[0] != '/') && (*cur != 0)) 
	        cur++;
	    break;
	}
        if ((*cur == '/') || (*cur == 0)) {
	    buf[index] = 0;
	    ctxt->hostname = xmlMemStrdup(buf);
	    index = 0;
	    break;
	}
	buf[index++] = *cur++;
    }
    if (*cur == 0) 
        ctxt->path = xmlMemStrdup("/");
    else {
        index = 0;
        buf[index] = 0;
	while (*cur != 0)
	    buf[index++] = *cur++;
	buf[index] = 0;
	ctxt->path = xmlMemStrdup(buf);
    }	
}

/**
 * xmlNanoFTPUpdateURL:
 * @ctx:  an FTP context
 * @URL:  The URL used to update the context
 *
 * Update an FTP context by parsing the URL and finding
 * new path it indicates. If there is an error in the 
 * protocol, hostname, port or other information, the
 * error is raised. It indicates a new connection has to
 * be established.
 *
 * Returns 0 if Ok, -1 in case of error (other host).
 */

int
xmlNanoFTPUpdateURL(void *ctx, const char *URL) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    const char *cur = URL;
    char buf[4096];
    int index = 0;
    int port = 0;

    if (URL == NULL)
	return(-1);
    if (ctxt == NULL)
	return(-1);
    if (ctxt->protocol == NULL)
	return(-1);
    if (ctxt->hostname == NULL)
	return(-1);
    buf[index] = 0;
    while (*cur != 0) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[index] = 0;
	    if (strcmp(ctxt->protocol, buf))
		return(-1);
	    index = 0;
            cur += 3;
	    break;
	}
	buf[index++] = *cur++;
    }
    if (*cur == 0)
	return(-1);

    buf[index] = 0;
    while (1) {
        if (cur[0] == ':') {
	    buf[index] = 0;
	    if (strcmp(ctxt->hostname, buf))
		return(-1);
	    index = 0;
	    cur += 1;
	    while ((*cur >= '0') && (*cur <= '9')) {
	        port *= 10;
		port += *cur - '0';
		cur++;
	    }
	    if (port != ctxt->port)
		return(-1);
	    while ((cur[0] != '/') && (*cur != 0)) 
	        cur++;
	    break;
	}
        if ((*cur == '/') || (*cur == 0)) {
	    buf[index] = 0;
	    if (strcmp(ctxt->hostname, buf))
		return(-1);
	    index = 0;
	    break;
	}
	buf[index++] = *cur++;
    }
    if (ctxt->path != NULL) {
	xmlFree(ctxt->path);
	ctxt->path = NULL;
    }

    if (*cur == 0) 
        ctxt->path = xmlMemStrdup("/");
    else {
        index = 0;
        buf[index] = 0;
	while (*cur != 0)
	    buf[index++] = *cur++;
	buf[index] = 0;
	ctxt->path = xmlMemStrdup(buf);
    }	
    return(0);
}

/**
 * xmlNanoFTPScanProxy:
 * @URL:  The proxy URL used to initialize the proxy context
 *
 * (Re)Initialize the FTP Proxy context by parsing the URL and finding
 * the protocol host port it indicates.
 * Should be like ftp://myproxy/ or ftp://myproxy:3128/
 * A NULL URL cleans up proxy informations.
 */

void
xmlNanoFTPScanProxy(const char *URL) {
    const char *cur = URL;
    char buf[4096];
    int index = 0;
    int port = 0;

    if (proxy != NULL) { 
        xmlFree(proxy);
	proxy = NULL;
    }
    if (proxyPort != 0) { 
	proxyPort = 0;
    }
#ifdef DEBUG_FTP
    if (URL == NULL)
	printf("Removing FTP proxy info\n");
    else
	printf("Using FTP proxy %s\n", URL);
#endif
    if (URL == NULL) return;
    buf[index] = 0;
    while (*cur != 0) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[index] = 0;
	    index = 0;
            cur += 3;
	    break;
	}
	buf[index++] = *cur++;
    }
    if (*cur == 0) return;

    buf[index] = 0;
    while (1) {
        if (cur[0] == ':') {
	    buf[index] = 0;
	    proxy = xmlMemStrdup(buf);
	    index = 0;
	    cur += 1;
	    while ((*cur >= '0') && (*cur <= '9')) {
	        port *= 10;
		port += *cur - '0';
		cur++;
	    }
	    if (port != 0) proxyPort = port;
	    while ((cur[0] != '/') && (*cur != 0)) 
	        cur++;
	    break;
	}
        if ((*cur == '/') || (*cur == 0)) {
	    buf[index] = 0;
	    proxy = xmlMemStrdup(buf);
	    index = 0;
	    break;
	}
	buf[index++] = *cur++;
    }
}

/**
 * xmlNanoFTPNewCtxt:
 * @URL:  The URL used to initialize the context
 *
 * Allocate and initialize a new FTP context.
 *
 * Returns an FTP context or NULL in case of error.
 */

void *
xmlNanoFTPNewCtxt(const char *URL) {
    xmlNanoFTPCtxtPtr ret;

    ret = (xmlNanoFTPCtxtPtr) xmlMalloc(sizeof(xmlNanoFTPCtxt));
    if (ret == NULL) return(NULL);

    memset(ret, 0, sizeof(xmlNanoFTPCtxt));
    ret->port = 21;
    ret->passive = 1;
    ret->returnValue = 0;

    if (URL != NULL)
	xmlNanoFTPScanURL(ret, URL);

    return(ret);
}

/**
 * xmlNanoFTPFreeCtxt:
 * @ctx:  an FTP context
 *
 * Frees the context after closing the connection.
 */

void
xmlNanoFTPFreeCtxt(void * ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    if (ctxt == NULL) return;
    if (ctxt->hostname != NULL) xmlFree(ctxt->hostname);
    if (ctxt->protocol != NULL) xmlFree(ctxt->protocol);
    if (ctxt->path != NULL) xmlFree(ctxt->path);
    ctxt->passive = 1;
    if (ctxt->controlFd >= 0) close(ctxt->controlFd);
    ctxt->controlFd = -1;
    xmlFree(ctxt);
}

/**
 * Parsing of the server answer, we just extract the code.
 * return 0 for errors
 *     +XXX for last line of response
 *     -XXX for response to be continued
 */
static int
xmlNanoFTPParseResponse(void *ctx, char *buf, int len) {
    int val = 0;

    if (len < 3) return(-1);
    if ((*buf >= '0') && (*buf <= '9')) 
        val = val * 10 + (*buf - '0');
    else
        return(0);
    buf++;
    if ((*buf >= '0') && (*buf <= '9')) 
        val = val * 10 + (*buf - '0');
    else
        return(0);
    buf++;
    if ((*buf >= '0') && (*buf <= '9')) 
        val = val * 10 + (*buf - '0');
    else
        return(0);
    buf++;
    if (*buf == '-') 
        return(-val);
    return(val);
}

/**
 * xmlNanoFTPReadResponse:
 * @ctx:  an FTP context
 * @buf:  buffer to read in
 * @size:  buffer length
 *
 * Read the response from the FTP server after a command.
 * Returns the code number
 */
static int
xmlNanoFTPReadResponse(void *ctx, char *buf, int size) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char *ptr, *end;
    int len;
    int res = -1;

    if (size <= 0) return(-1);

get_more:
    if ((len = recv(ctxt->controlFd, buf, size - 1, 0)) < 0) {
	close(ctxt->controlFd); ctxt->controlFd = -1;
        ctxt->controlFd = -1;
        return(-1);
    }
    if (len == 0) {
        return(-1);
    }

    end = &buf[len];
    *end = 0;
#ifdef DEBUG_FTP
    printf(buf);
#endif
    ptr = buf;
    while (ptr < end) {
        res = xmlNanoFTPParseResponse(ctxt, ptr, end - ptr);
	if (res > 0) break;
	if (res == 0) {
#ifdef DEBUG_FTP
	    fprintf(stderr, "xmlNanoFTPReadResponse failed: %s\n", ptr);
#endif
	    return(-1);
	}
	while ((ptr < end) && (*ptr != '\n')) ptr++;
	if (ptr >= end) {
#ifdef DEBUG_FTP
	    fprintf(stderr, "xmlNanoFTPReadResponse: unexpected end %s\n", buf);
#endif
	    return((-res) / 100);
	}
	if (*ptr != '\r') ptr++;
    }

    if (res < 0) goto get_more;

#ifdef DEBUG_FTP
    printf("Got %d\n", res);
#endif
    return(res / 100);
}

/**
 * xmlNanoFTPGetResponse:
 * @ctx:  an FTP context
 *
 * Get the response from the FTP server after a command.
 * Returns the code number
 */

int
xmlNanoFTPGetResponse(void *ctx) {
    char buf[16 * 1024 + 1];

/**************
    fd_set rfd;
    struct timeval tv;
    int res;

    tv.tv_sec = 10;
    tv.tv_usec = 0;
    FD_ZERO(&rfd);
    FD_SET(ctxt->controlFd, &rfd);
    res = select(ctxt->controlFd + 1, &rfd, NULL, NULL, &tv);
    if (res <= 0) return(res);
 **************/

    return(xmlNanoFTPReadResponse(ctx, buf, 16 * 1024));
}

/**
 * xmlNanoFTPCheckResponse:
 * @ctx:  an FTP context
 *
 * Check if there is a response from the FTP server after a command.
 * Returns the code number, or 0
 */

int
xmlNanoFTPCheckResponse(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[1024 + 1];
    fd_set rfd;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&rfd);
    FD_SET(ctxt->controlFd, &rfd);
    switch(select(ctxt->controlFd + 1, &rfd, NULL, NULL, &tv)) {
	case 0:
	    return(0);
	case -1:
#ifdef DEBUG_FTP
	    perror("select");
#endif
	    return(-1);
			
    }

    return(xmlNanoFTPReadResponse(ctx, buf, 1024));
}

/**
 * Send the user authentification
 */

static int
xmlNanoFTPSendUser(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[200];
    int len;
    int res;

    if (ctxt->user == NULL)
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "USER anonymous\r\n");
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "USER anonymous\r\n");
#endif /* HAVE_SNPRINTF */
    else
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "USER %s\r\n", ctxt->user);
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "USER %s\r\n", ctxt->user);
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
    printf(buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) return(res);
    return(0);
}

/**
 * Send the password authentification
 */

static int
xmlNanoFTPSendPasswd(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[200];
    int len;
    int res;

    if (ctxt->passwd == NULL)
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "PASS libxml@%s\r\n", hostname);
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "PASS libxml@%s\r\n", hostname);
#endif /* HAVE_SNPRINTF */
    else
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "PASS %s\r\n", ctxt->passwd);
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "PASS %s\r\n", ctxt->passwd);
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
    printf(buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) return(res);
    return(0);
}

/**
 * xmlNanoFTPQuit:
 * @ctx:  an FTP context
 *
 * Send a QUIT command to the server
 *
 * Returns -1 in case of error, 0 otherwise
 */


int
xmlNanoFTPQuit(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[200];
    int len;
    int res;

#ifndef HAVE_SNPRINTF
    len = sprintf(buf, "QUIT\r\n");
#else /* HAVE_SNPRINTF */
    len = snprintf(buf, sizeof(buf), "QUIT\r\n");
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
    printf(buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    return(0);
}

/**
 * xmlNanoFTPConnect:
 * @ctx:  an FTP context
 *
 * Tries to open a control connection
 *
 * Returns -1 in case of error, 0 otherwise
 */

int
xmlNanoFTPConnect(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    struct hostent *hp;
    int port;
    int res;

    if (ctxt == NULL)
	return(-1);
    if (ctxt->hostname == NULL)
	return(-1);

    /*
     * do the blocking DNS query.
     */
    if (proxy)
	hp = gethostbyname(proxy);
    else
	hp = gethostbyname(ctxt->hostname);
    if (hp == NULL)
        return(-1);

    /*
     * Prepare the socket
     */
    memset(&ctxt->ftpAddr, 0, sizeof(ctxt->ftpAddr));
    ctxt->ftpAddr.sin_family = AF_INET;
    memcpy(&ctxt->ftpAddr.sin_addr, hp->h_addr_list[0], hp->h_length);
    if (proxy) {
        port = proxyPort;
    } else {
	port = ctxt->port;
    }
    if (port == 0)
	port = 21;
    ctxt->ftpAddr.sin_port = htons(port);
    ctxt->controlFd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctxt->controlFd < 0)
        return(-1);

    /*
     * Do the connect.
     */
    if (connect(ctxt->controlFd, (struct sockaddr *) &ctxt->ftpAddr,
                sizeof(struct sockaddr_in)) < 0) {
        close(ctxt->controlFd); ctxt->controlFd = -1;
        ctxt->controlFd = -1;
	return(-1);
    }

    /*
     * Wait for the HELLO from the server.
     */
    res = xmlNanoFTPGetResponse(ctxt);
    if (res != 2) {
        close(ctxt->controlFd); ctxt->controlFd = -1;
        ctxt->controlFd = -1;
	return(-1);
    }

    /*
     * State diagram for the login operation on the FTP server
     *
     * Reference: RFC 959
     *
     *                       1
     * +---+   USER    +---+------------->+---+
     * | B |---------->| W | 2       ---->| E |
     * +---+           +---+------  |  -->+---+
     *                  | |       | | |
     *                3 | | 4,5   | | |
     *    --------------   -----  | | |
     *   |                      | | | |
     *   |                      | | | |
     *   |                 ---------  |
     *   |               1|     | |   |
     *   V                |     | |   |
     * +---+   PASS    +---+ 2  |  ------>+---+
     * |   |---------->| W |------------->| S |
     * +---+           +---+   ---------->+---+
     *                  | |   | |     |
     *                3 | |4,5| |     |
     *    --------------   --------   |
     *   |                    | |  |  |
     *   |                    | |  |  |
     *   |                 -----------
     *   |             1,3|   | |  |
     *   V                |  2| |  |
     * +---+   ACCT    +---+--  |   ----->+---+
     * |   |---------->| W | 4,5 -------->| F |
     * +---+           +---+------------->+---+
     *
     * Of course in case of using a proxy this get really nasty and is not
     * standardized at all :-(
     */
    if (proxy) {
        int len;
	char buf[400];

        if (proxyUser != NULL) {
	    /*
	     * We need proxy auth
	     */
#ifndef HAVE_SNPRINTF
	    len = sprintf(buf, "USER %s\r\n", proxyUser);
#else /* HAVE_SNPRINTF */
	    len = snprintf(buf, sizeof(buf), "USER %s\r\n", proxyUser);
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
	    printf(buf);
#endif
	    res = send(ctxt->controlFd, buf, len, 0);
	    if (res < 0) {
		close(ctxt->controlFd);
		ctxt->controlFd = -1;
	        return(res);
	    }
	    res = xmlNanoFTPGetResponse(ctxt);
	    switch (res) {
		case 2:
		    if (proxyPasswd == NULL)
			break;
		case 3:
		    if (proxyPasswd != NULL)
#ifndef HAVE_SNPRINTF
			len = sprintf(buf, "PASS %s\r\n", proxyPasswd);
#else /* HAVE_SNPRINTF */
			len = snprintf(buf, sizeof(buf), "PASS %s\r\n", proxyPasswd);
#endif /* HAVE_SNPRINTF */
		    else
#ifndef HAVE_SNPRINTF
			len = sprintf(buf, "PASS libxml@%s\r\n",
			               hostname);
#else /* HAVE_SNPRINTF */
			len = snprintf(buf, sizeof(buf), "PASS libxml@%s\r\n",
			               hostname);
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
		    printf(buf);
#endif
		    res = send(ctxt->controlFd, buf, len, 0);
		    if (res < 0) {
			close(ctxt->controlFd);
			ctxt->controlFd = -1;
			return(res);
		    }
		    res = xmlNanoFTPGetResponse(ctxt);
		    if (res > 3) {
			close(ctxt->controlFd);
			ctxt->controlFd = -1;
			return(-1);
		    }
		    break;
		case 1:
		    break;
		case 4:
		case 5:
		case -1:
		default:
		    close(ctxt->controlFd);
		    ctxt->controlFd = -1;
		    return(-1);
	    }
	}

	/*
	 * We assume we don't need more authentication to the proxy
	 * and that it succeeded :-\
	 */
	switch (proxyType) {
	    case 0:
		/* we will try in seqence */
	    case 1:
		/* Using SITE command */
#ifndef HAVE_SNPRINTF
		len = sprintf(buf, "SITE %s\r\n", ctxt->hostname);
#else /* HAVE_SNPRINTF */
		len = snprintf(buf, sizeof(buf), "SITE %s\r\n", ctxt->hostname);
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
		printf(buf);
#endif
		res = send(ctxt->controlFd, buf, len, 0);
		if (res < 0) {
		    close(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(res);
		}
		res = xmlNanoFTPGetResponse(ctxt);
		if (res == 2) {
		    /* we assume it worked :-\ 1 is error for SITE command */
		    proxyType = 1;
		    break;
		}    
		if (proxyType == 1) {
		    close(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(-1);
		}
	    case 2:
		/* USER user@host command */
		if (ctxt->user == NULL)
#ifndef HAVE_SNPRINTF
		    len = sprintf(buf, "USER anonymous@%s\r\n",
#else /* HAVE_SNPRINTF */
		    len = snprintf(buf, sizeof(buf), "USER anonymous@%s\r\n",
#endif /* HAVE_SNPRINTF */
			           ctxt->hostname);
		else
#ifndef HAVE_SNPRINTF
		    len = sprintf(buf, "USER %s@%s\r\n",
#else /* HAVE_SNPRINTF */
		    len = snprintf(buf, sizeof(buf), "USER %s@%s\r\n",
#endif /* HAVE_SNPRINTF */
			           ctxt->user, ctxt->hostname);
#ifdef DEBUG_FTP
		printf(buf);
#endif
		res = send(ctxt->controlFd, buf, len, 0);
		if (res < 0) {
		    close(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(res);
		}
		res = xmlNanoFTPGetResponse(ctxt);
		if ((res == 1) || (res == 2)) {
		    /* we assume it worked :-\ */
		    proxyType = 2;
		    return(0);
		}    
		if (ctxt->passwd == NULL)
#ifndef HAVE_SNPRINTF
		    len = sprintf(buf, "PASS libxml@%s\r\n", hostname);
#else /* HAVE_SNPRINTF */
		    len = snprintf(buf, sizeof(buf), "PASS libxml@%s\r\n", hostname);
#endif /* HAVE_SNPRINTF */
		else
#ifndef HAVE_SNPRINTF
		    len = sprintf(buf, "PASS %s\r\n", ctxt->passwd);
#else /* HAVE_SNPRINTF */
		    len = snprintf(buf, sizeof(buf), "PASS %s\r\n", ctxt->passwd);
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
		printf(buf);
#endif
		res = send(ctxt->controlFd, buf, len, 0);
		if (res < 0) {
		    close(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(res);
		}
		res = xmlNanoFTPGetResponse(ctxt);
		if ((res == 1) || (res == 2)) {
		    /* we assume it worked :-\ */
		    proxyType = 2;
		    return(0);
		}
		if (proxyType == 2) {
		    close(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(-1);
		}
	    case 3:
		/*
		 * If you need support for other Proxy authentication scheme
		 * send the code or at least the sequence in use.
		 */
	    default:
		close(ctxt->controlFd); ctxt->controlFd = -1;
		ctxt->controlFd = -1;
		return(-1);
	}
    }
    /*
     * Non-proxy handling.
     */
    res = xmlNanoFTPSendUser(ctxt);
    if (res < 0) {
        close(ctxt->controlFd); ctxt->controlFd = -1;
        ctxt->controlFd = -1;
	return(-1);
    }
    res = xmlNanoFTPGetResponse(ctxt);
    switch (res) {
	case 2:
	    return(0);
	case 3:
	    break;
	case 1:
	case 4:
	case 5:
        case -1:
	default:
	    close(ctxt->controlFd); ctxt->controlFd = -1;
	    ctxt->controlFd = -1;
	    return(-1);
    }
    res = xmlNanoFTPSendPasswd(ctxt);
    if (res < 0) {
        close(ctxt->controlFd); ctxt->controlFd = -1;
        ctxt->controlFd = -1;
	return(-1);
    }
    res = xmlNanoFTPGetResponse(ctxt);
    switch (res) {
	case 2:
	    break;
	case 3:
	    fprintf(stderr, "FTP server asking for ACCNT on anonymous\n");
	case 1:
	case 4:
	case 5:
        case -1:
	default:
	    close(ctxt->controlFd); ctxt->controlFd = -1;
	    ctxt->controlFd = -1;
	    return(-1);
    }

    return(0);
}

/**
 * xmlNanoFTPConnectTo:
 * @server:  an FTP server name
 * @directory:  the port (use 21 if 0)
 *
 * Tries to open a control connection to the given server/port
 *
 * Returns and fTP context or NULL if it failed
 */


void *
xmlNanoFTPConnectTo(const char *server, int port) {
    xmlNanoFTPCtxtPtr ctxt;
    int res;

    xmlNanoFTPInit();
    if (server == NULL) 
	return(NULL);
    ctxt = xmlNanoFTPNewCtxt(NULL);
    ctxt->hostname = xmlMemStrdup(server);
    if (port != 0)
	ctxt->port = port;
    res = xmlNanoFTPConnect(ctxt);
    if (res < 0) {
	xmlNanoFTPFreeCtxt(ctxt);
	return(NULL);
    }
    return(ctxt);
}

/**
 * xmlNanoFTPGetConnection:
 * @ctx:  an FTP context
 * @directory:  a directory on the server
 *
 * Tries to change the remote directory
 *
 * Returns -1 incase of error, 1 if CWD worked, 0 if it failed
 */

int
xmlNanoFTPCwd(void *ctx, char *directory) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[400];
    int len;
    int res;

    /*
     * Expected response code for CWD:
     *
     * CWD
     *     250
     *     500, 501, 502, 421, 530, 550
     */
#ifndef HAVE_SNPRINTF
    len = sprintf(buf, "CWD %s\r\n", directory);
#else /* HAVE_SNPRINTF */
    len = snprintf(buf, sizeof(buf), "CWD %s\r\n", directory);
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
    printf(buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) return(res);
    res = xmlNanoFTPGetResponse(ctxt);
    if (res == 4) {
	return(-1);
    }
    if (res == 2) return(1);
    if (res == 5) {
	return(0);
    }
    return(0);
}

/**
 * xmlNanoFTPGetConnection:
 * @ctx:  an FTP context
 *
 * Try to open a data connection to the server. Currently only
 * passive mode is supported.
 *
 * Returns -1 incase of error, 0 otherwise
 */

int
xmlNanoFTPGetConnection(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[200], *cur;
    int len, i;
    int res;
    unsigned char ad[6], *adp, *portp;
    unsigned int temp[6];
    struct sockaddr_in dataAddr;
    size_t dataAddrLen;

    ctxt->dataFd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ctxt->dataFd < 0) {
        fprintf(stderr, "xmlNanoFTPGetConnection: failed to create socket\n");
    }
    dataAddrLen = sizeof(dataAddr);
    memset(&dataAddr, 0, dataAddrLen);
    dataAddr.sin_family = AF_INET;

    if (ctxt->passive) {
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "PASV\r\n");
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "PASV\r\n");
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
	printf(buf);
#endif
	res = send(ctxt->controlFd, buf, len, 0);
	if (res < 0) {
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return(res);
	}
        res = xmlNanoFTPReadResponse(ctx, buf, sizeof(buf) -1);
	if (res != 2) {
	    if (res == 5) {
	        close(ctxt->dataFd); ctxt->dataFd = -1;
		return(-1);
	    } else {
		/*
		 * retry with an active connection
		 */
	        close(ctxt->dataFd); ctxt->dataFd = -1;
	        ctxt->passive = 0;
	    }
	}
	cur = &buf[4];
	while (((*cur < '0') || (*cur > '9')) && *cur != '\0') cur++;
	if (sscanf(cur, "%d,%d,%d,%d,%d,%d", &temp[0], &temp[1], &temp[2],
	            &temp[3], &temp[4], &temp[5]) != 6) {
	    fprintf(stderr, "Invalid answer to PASV\n");
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return(-1);
	}
	for (i=0; i<6; i++) ad[i] = (unsigned char) (temp[i] & 0xff);
	memcpy(&dataAddr.sin_addr, &ad[0], 4);
	memcpy(&dataAddr.sin_port, &ad[4], 2);
	if (connect(ctxt->dataFd, (struct sockaddr *) &dataAddr, dataAddrLen) < 0) {
	    fprintf(stderr, "Failed to create a data connection\n");
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return (-1);
	}
    } else {
        getsockname(ctxt->dataFd, (struct sockaddr *) &dataAddr, &dataAddrLen);
	dataAddr.sin_port = 0;
	if (bind(ctxt->dataFd, (struct sockaddr *) &dataAddr, dataAddrLen) < 0) {
	    fprintf(stderr, "Failed to bind a port\n");
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return (-1);
	}
        getsockname(ctxt->dataFd, (struct sockaddr *) &dataAddr, &dataAddrLen);

	if (listen(ctxt->dataFd, 1) < 0) {
	    fprintf(stderr, "Could not listen on port %d\n",
	            ntohs(dataAddr.sin_port));
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return (-1);
	}
	adp = (unsigned char *) &dataAddr.sin_addr;
	portp = (unsigned char *) &dataAddr.sin_port;
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "PORT %d,%d,%d,%d,%d,%d\r\n",
	       adp[0] & 0xff, adp[1] & 0xff, adp[2] & 0xff, adp[3] & 0xff,
	       portp[0] & 0xff, portp[1] & 0xff);
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "PORT %d,%d,%d,%d,%d,%d\r\n",
	       adp[0] & 0xff, adp[1] & 0xff, adp[2] & 0xff, adp[3] & 0xff,
	       portp[0] & 0xff, portp[1] & 0xff);
#endif /* HAVE_SNPRINTF */
        buf[sizeof(buf) - 1] = 0;
#ifdef DEBUG_FTP
	printf(buf);
#endif

	res = send(ctxt->controlFd, buf, len, 0);
	if (res < 0) {
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return(res);
	}
        res = xmlNanoFTPGetResponse(ctxt);
	if (res != 2) {
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return(-1);
        }
    }
    return(ctxt->dataFd);
    
}

/**
 * xmlNanoFTPCloseConnection:
 * @ctx:  an FTP context
 *
 * Close the data connection from the server
 *
 * Returns -1 incase of error, 0 otherwise
 */

int
xmlNanoFTPCloseConnection(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    int res;
    fd_set rfd, efd;
    struct timeval tv;

    close(ctxt->dataFd); ctxt->dataFd = -1;
    tv.tv_sec = 15;
    tv.tv_usec = 0;
    FD_ZERO(&rfd);
    FD_SET(ctxt->controlFd, &rfd);
    FD_ZERO(&efd);
    FD_SET(ctxt->controlFd, &efd);
    res = select(ctxt->controlFd + 1, &rfd, NULL, &efd, &tv);
    if (res < 0) {
#ifdef DEBUG_FTP
	perror("select");
#endif
	close(ctxt->controlFd); ctxt->controlFd = -1;
	return(-1);
    }
    if (res == 0) {
	fprintf(stderr, "xmlNanoFTPCloseConnection: timeout\n");
	close(ctxt->controlFd); ctxt->controlFd = -1;
    } else {
	res = xmlNanoFTPGetResponse(ctxt);
	if (res != 2) {
	    close(ctxt->controlFd); ctxt->controlFd = -1;
	    return(-1);
	}
    }
    return(0);
}

/**
 * xmlNanoFTPParseList:
 * @list:  some data listing received from the server
 * @callback:  the user callback
 * @userData:  the user callback data
 *
 * Parse at most one entry from the listing. 
 *
 * Returns -1 incase of error, the lenght of data parsed otherwise
 */

static int
xmlNanoFTPParseList(const char *list, ftpListCallback callback, void *userData) {
    const char *cur = list;
    char filename[151];
    char attrib[11];
    char owner[11];
    char group[11];
    char month[4];
    int year = 0;
    int minute = 0;
    int hour = 0;
    int day = 0;
    unsigned long size = 0;
    int links = 0;
    int i;

    if (!strncmp(cur, "total", 5)) {
        cur += 5;
	while (*cur == ' ') cur++;
	while ((*cur >= '0') && (*cur <= '9'))
	    links = (links * 10) + (*cur++ - '0');
	while ((*cur == ' ') || (*cur == '\n')  || (*cur == '\r'))
	    cur++;
	return(cur - list);
    } else if (*list == '+') {
	return(0);
    } else {
	while ((*cur == ' ') || (*cur == '\n')  || (*cur == '\r'))
	    cur++;
	if (*cur == 0) return(0);
	i = 0;
	while (*cur != ' ') {
	    if (i < 10) 
		attrib[i++] = *cur;
	    cur++;
	    if (*cur == 0) return(0);
	}
	attrib[10] = 0;
	while (*cur == ' ') cur++;
	if (*cur == 0) return(0);
	while ((*cur >= '0') && (*cur <= '9'))
	    links = (links * 10) + (*cur++ - '0');
	while (*cur == ' ') cur++;
	if (*cur == 0) return(0);
	i = 0;
	while (*cur != ' ') {
	    if (i < 10) 
		owner[i++] = *cur;
	    cur++;
	    if (*cur == 0) return(0);
	}
	owner[i] = 0;
	while (*cur == ' ') cur++;
	if (*cur == 0) return(0);
	i = 0;
	while (*cur != ' ') {
	    if (i < 10) 
		group[i++] = *cur;
	    cur++;
	    if (*cur == 0) return(0);
	}
	group[i] = 0;
	while (*cur == ' ') cur++;
	if (*cur == 0) return(0);
	while ((*cur >= '0') && (*cur <= '9'))
	    size = (size * 10) + (*cur++ - '0');
	while (*cur == ' ') cur++;
	if (*cur == 0) return(0);
	i = 0;
	while (*cur != ' ') {
	    if (i < 3)
		month[i++] = *cur;
	    cur++;
	    if (*cur == 0) return(0);
	}
	month[i] = 0;
	while (*cur == ' ') cur++;
	if (*cur == 0) return(0);
        while ((*cur >= '0') && (*cur <= '9'))
	    day = (day * 10) + (*cur++ - '0');
	while (*cur == ' ') cur++;
	if (*cur == 0) return(0);
	if ((cur[1] == 0) || (cur[2] == 0)) return(0);
	if ((cur[1] == ':') || (cur[2] == ':')) {
	    while ((*cur >= '0') && (*cur <= '9'))
		hour = (hour * 10) + (*cur++ - '0');
	    if (*cur == ':') cur++;
	    while ((*cur >= '0') && (*cur <= '9'))
		minute = (minute * 10) + (*cur++ - '0');
	} else {
	    while ((*cur >= '0') && (*cur <= '9'))
		year = (year * 10) + (*cur++ - '0');
	}
	while (*cur == ' ') cur++;
	if (*cur == 0) return(0);
	i = 0;
	while ((*cur != '\n')  && (*cur != '\r')) {
	    if (i < 150)
		filename[i++] = *cur;
	    cur++;
	    if (*cur == 0) return(0);
	}
	filename[i] = 0;
	if ((*cur != '\n') && (*cur != '\r'))
	    return(0);
	while ((*cur == '\n')  || (*cur == '\r'))
	    cur++;
    }
    if (callback != NULL) {
        callback(userData, filename, attrib, owner, group, size, links,
		 year, month, day, hour, minute);
    }
    return(cur - list);
}

/**
 * xmlNanoFTPList:
 * @ctx:  an FTP context
 * @callback:  the user callback
 * @userData:  the user callback data
 * @filename:  optional files to list
 *
 * Do a listing on the server. All files info are passed back
 * in the callbacks.
 *
 * Returns -1 incase of error, 0 otherwise
 */

int
xmlNanoFTPList(void *ctx, ftpListCallback callback, void *userData,
	       char *filename) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[4096 + 1];
    int len, res;
    int index = 0, base;
    fd_set rfd, efd;
    struct timeval tv;

    if (filename == NULL) {
        if (xmlNanoFTPCwd(ctxt, ctxt->path) < 1)
	    return(-1);
	ctxt->dataFd = xmlNanoFTPGetConnection(ctxt);
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "LIST -L\r\n");
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "LIST -L\r\n");
#endif /* HAVE_SNPRINTF */
    } else {
	if (filename[0] != '/') {
	    if (xmlNanoFTPCwd(ctxt, ctxt->path) < 1)
		return(-1);
	}
	ctxt->dataFd = xmlNanoFTPGetConnection(ctxt);
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "LIST -L %s\r\n", filename);
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "LIST -L %s\r\n", filename);
#endif /* HAVE_SNPRINTF */
    }
#ifdef DEBUG_FTP
    printf(buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) {
	close(ctxt->dataFd); ctxt->dataFd = -1;
	return(res);
    }
    res = xmlNanoFTPReadResponse(ctxt, buf, sizeof(buf) -1);
    if (res != 1) {
	close(ctxt->dataFd); ctxt->dataFd = -1;
	return(-res);
    }

    do {
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&rfd);
	FD_SET(ctxt->dataFd, &rfd);
	FD_ZERO(&efd);
	FD_SET(ctxt->dataFd, &efd);
	res = select(ctxt->dataFd + 1, &rfd, NULL, &efd, &tv);
	if (res < 0) {
#ifdef DEBUG_FTP
	    perror("select");
#endif
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return(-1);
	}
	if (res == 0) {
	    res = xmlNanoFTPCheckResponse(ctxt);
	    if (res < 0) {
		close(ctxt->dataFd); ctxt->dataFd = -1;
		ctxt->dataFd = -1;
		return(-1);
	    }
	    if (res == 2) {
		close(ctxt->dataFd); ctxt->dataFd = -1;
		return(0);
	    }

	    continue;
	}

	if ((len = read(ctxt->dataFd, &buf[index], sizeof(buf) - (index + 1))) < 0) {
#ifdef DEBUG_FTP
	    perror("read");
#endif
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    ctxt->dataFd = -1;
	    return(-1);
	}
#ifdef DEBUG_FTP
        write(1, &buf[index], len);
#endif
	index += len;
	buf[index] = 0;
	base = 0;
	do {
	    res = xmlNanoFTPParseList(&buf[base], callback, userData);
	    base += res;
	} while (res > 0);

	memmove(&buf[0], &buf[base], index - base);
	index -= base;
    } while (len != 0);
    xmlNanoFTPCloseConnection(ctxt);
    return(0);
}

/**
 * xmlNanoFTPGetSocket:
 * @ctx:  an FTP context
 * @filename:  the file to retrieve (or NULL if path is in context).
 *
 * Initiate fetch of the given file from the server.
 *
 * Returns the socket for the data connection, or <0 in case of error
 */


int
xmlNanoFTPGetSocket(void *ctx, const char *filename) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[300];
    int res, len;
    if ((filename == NULL) && (ctxt->path == NULL))
	return(-1);
    ctxt->dataFd = xmlNanoFTPGetConnection(ctxt);

#ifndef HAVE_SNPRINTF
    len = sprintf(buf, "TYPE I\r\n");
#else /* HAVE_SNPRINTF */
    len = snprintf(buf, sizeof(buf), "TYPE I\r\n");
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
    printf(buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) {
	close(ctxt->dataFd); ctxt->dataFd = -1;
	return(res);
    }
    res = xmlNanoFTPReadResponse(ctxt, buf, sizeof(buf) -1);
    if (res != 2) {
	close(ctxt->dataFd); ctxt->dataFd = -1;
	return(-res);
    }
    if (filename == NULL)
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "RETR %s\r\n", ctxt->path);
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "RETR %s\r\n", ctxt->path);
#endif /* HAVE_SNPRINTF */
    else
#ifndef HAVE_SNPRINTF
	len = sprintf(buf, "RETR %s\r\n", filename);
#else /* HAVE_SNPRINTF */
	len = snprintf(buf, sizeof(buf), "RETR %s\r\n", filename);
#endif /* HAVE_SNPRINTF */
#ifdef DEBUG_FTP
    printf(buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) {
	close(ctxt->dataFd); ctxt->dataFd = -1;
	return(res);
    }
    res = xmlNanoFTPReadResponse(ctxt, buf, sizeof(buf) -1);
    if (res != 1) {
	close(ctxt->dataFd); ctxt->dataFd = -1;
	return(-res);
    }
    return(ctxt->dataFd);
}

/**
 * xmlNanoFTPGet:
 * @ctx:  an FTP context
 * @callback:  the user callback
 * @userData:  the user callback data
 * @filename:  the file to retrieve
 *
 * Fetch the given file from the server. All data are passed back
 * in the callbacks. The last callback has a size of 0 block.
 *
 * Returns -1 incase of error, 0 otherwise
 */

int
xmlNanoFTPGet(void *ctx, ftpDataCallback callback, void *userData,
	      const char *filename) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[4096];
    int len = 0, res;
    fd_set rfd;
    struct timeval tv;

    if ((filename == NULL) && (ctxt->path == NULL))
	return(-1);
    if (callback == NULL)
	return(-1);
    if (xmlNanoFTPGetSocket(ctxt, filename) < 0)
	return(-1);

    do {
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&rfd);
	FD_SET(ctxt->dataFd, &rfd);
	res = select(ctxt->dataFd + 1, &rfd, NULL, NULL, &tv);
	if (res < 0) {
#ifdef DEBUG_FTP
	    perror("select");
#endif
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return(-1);
	}
	if (res == 0) {
	    res = xmlNanoFTPCheckResponse(ctxt);
	    if (res < 0) {
		close(ctxt->dataFd); ctxt->dataFd = -1;
		ctxt->dataFd = -1;
		return(-1);
	    }
	    if (res == 2) {
		close(ctxt->dataFd); ctxt->dataFd = -1;
		return(0);
	    }

	    continue;
	}
	if ((len = read(ctxt->dataFd, &buf, sizeof(buf))) < 0) {
	    callback(userData, buf, len);
	    close(ctxt->dataFd); ctxt->dataFd = -1;
	    return(-1);
	}
	callback(userData, buf, len);
    } while (len != 0);

    return(xmlNanoFTPCloseConnection(ctxt));
}

/**
 * xmlNanoFTPRead:
 * @ctx:  the FTP context
 * @dest:  a buffer
 * @len:  the buffer length
 *
 * This function tries to read @len bytes from the existing FTP connection
 * and saves them in @dest. This is a blocking call.
 *
 * Returns the number of byte read. 0 is an indication of an end of connection.
 *         -1 indicates a parameter error.
 */
int
xmlNanoFTPRead(void *ctx, void *dest, int len) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;

    if (ctx == NULL) return(-1);
    if (ctxt->dataFd < 0) return(0);
    if (dest == NULL) return(-1);
    if (len <= 0) return(0);

    len = read(ctxt->dataFd, dest, len);
#ifdef DEBUG_FTP
    printf("Read %d bytes\n", len);
#endif
    if (len <= 0) {
	xmlNanoFTPCloseConnection(ctxt);
    }
    return(len);
}

/**
 * xmlNanoFTPOpen:
 * @URL: the URL to the resource
 *
 * Start to fetch the given ftp:// resource
 *
 * Returns an FTP context, or NULL 
 */

void *
xmlNanoFTPOpen(const char *URL) {
    xmlNanoFTPCtxtPtr ctxt;
    int sock;

    xmlNanoFTPInit();
    if (URL == NULL) return(NULL);
    if (strncmp("ftp://", URL, 6)) return(NULL);

    ctxt = xmlNanoFTPNewCtxt(URL);
    if (ctxt == NULL) return(NULL);
    if (xmlNanoFTPConnect(ctxt) < 0) {
	xmlNanoFTPFreeCtxt(ctxt);
	return(NULL);
    }
    sock = xmlNanoFTPGetSocket(ctxt, ctxt->path);
    if (sock < 0) {
	xmlNanoFTPFreeCtxt(ctxt);
	return(NULL);
    }
    return(ctxt);
}

/**
 * xmlNanoFTPClose:
 * @ctx: an FTP context
 *
 * Close the connection and both control and transport
 *
 * Returns -1 incase of error, 0 otherwise
 */

int
xmlNanoFTPClose(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;

    if (ctxt == NULL)
	return(-1);

    if (ctxt->dataFd >= 0) {
	close(ctxt->dataFd);
	ctxt->dataFd = -1;
    }
    if (ctxt->controlFd >= 0) {
	xmlNanoFTPQuit(ctxt);
	close(ctxt->controlFd);
	ctxt->controlFd = -1;
    }
    xmlNanoFTPFreeCtxt(ctxt);
    return(0);
}

#ifdef STANDALONE
/************************************************************************
 * 									*
 * 			Basic test in Standalone mode			*
 * 									*
 ************************************************************************/
void ftpList(void *userData, const char *filename, const char* attrib,
	     const char *owner, const char *group, unsigned long size, int links,
	     int year, const char *month, int day, int hour, int minute) {
    printf("%s %s %s %ld %s\n", attrib, owner, group, size, filename);
}
void ftpData(void *userData, const char *data, int len) {
    if (userData == NULL) return;
    if (len <= 0) {
	fclose(userData);
	return;
    }	
    fwrite(data, len, 1, userData);
}

int main(int argc, char **argv) {
    void *ctxt;
    FILE *output;
    char *tstfile = NULL;

    xmlNanoFTPInit();
    if (argc > 1) {
	ctxt = xmlNanoFTPNewCtxt(argv[1]);
	if (xmlNanoFTPConnect(ctxt) < 0) {
	    fprintf(stderr, "Couldn't connect to %s\n", argv[1]);
	    exit(1);
	}
	if (argc > 2)
	    tstfile = argv[2];
    } else
	ctxt = xmlNanoFTPConnectTo("localhost", 0);
    if (ctxt == NULL) {
        fprintf(stderr, "Couldn't connect to localhost\n");
        exit(1);
    }
    xmlNanoFTPList(ctxt, ftpList, NULL, tstfile);
    output = fopen("/tmp/tstdata", "w");
    if (output != NULL) {
	if (xmlNanoFTPGet(ctxt, ftpData, (void *) output, tstfile) < 0)
	    fprintf(stderr, "Failed to get file\n");
	
    }
    xmlNanoFTPClose(ctxt);
    xmlMemoryDump();
    exit(0);
}
#endif /* STANDALONE */
#else /* !LIBXML_FTP_ENABLED */
#ifdef STANDALONE
#include <stdio.h>
int main(int argc, char **argv) {
    printf("%s : FTP support not compiled in\n", argv[0]);
    return(0);
}
#endif /* STANDALONE */
#endif /* LIBXML_FTP_ENABLED */
