
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

#ifndef HTTP_DEF_H
#define  HTTP_DEF_H


static const char* const Http_def_h_Version =
    "$Id: Http_def.h,v 1.2 2004/06/03 23:54:17 greear Exp $";

#define HTTP_VERSION    "VPP/1.0" // Vovida Provisioning Protocol
#define CONT_LENGTH     "Content-Length:"
#define VPP_TIMESTAMP   "Timestamp:"
#define VPPREQ_BUF_LEN  56
#define URL_LENGTH       100
#define CONT_STR_LENGTH  56
#define VPPVER_LENGTH   56
#define GET_REQ          "GET"         // get an item
#define PUT_REQ          "PUT"         // put an item
#define DIRLIST_REQ      "LIST"        // list items in a group
#define DIRNLIST_REQ     "NLIST"       // list items with content-length
#define DELETE_REQ       "DELETE"      // delete an item
#define REMOVE_REQ       "REMOVE"      // remove a user
#define REGISTER_REQ     "REGISTER"
#define UNREGISTER_REQ   "UNREGISTER"
#define UPDATE_REQ       "UPDATE"
#define UPDATEDIR_REQ    "UPDATEDIR"
#define DELDIR_REQ       "DELDIR"

#define SYNC_GRLIST_REQ  "SYNCGRLIST"
#define SYNC_PRLIST_REQ  "SYNCPRLIST"
#define SYNC_GOOD_REQ    "SYNCGOOD"
#define SYNC_GET_REQ     "SYNCGET"
#define SYNC_PUT_REQ     "SYNCPUT"
#define SYNC_REGISTER_REQ     "SYNCREGISTER"
#define SYNC_UNREGISTER_REQ   "SYNCUNREGISTER"
#define SYNC_START_REQ   "SYNCSTART"
#define SYNC_STAT_REQ    "SYNCSTAT"


#endif  //HTTP_DEF_H
