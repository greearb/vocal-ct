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

#ifndef __MAILX_H
#define __MAILX_H

#include "global.h"
#include <string>
#include <stdio.h>

#include "c-client.h"

#undef T

class Mail {

public:

    Mail();
    virtual ~Mail();

public:

    MAILSTREAM		*stream;
    MESSAGECACHE	*msgCache;
    ENVELOPE    	*env;
    long		message_date;
    struct 		tm *Tm;

protected:

    string		user;
    string		password;
    int			curMessage;
    int 		recentOnly;
    int 		deletedOnly;


private:


public:

    static Mail * instance();

/**  Open and Close Mail session */

    virtual int open(string);
    virtual int close();

/**  Messages list manipulation methods */

    virtual int refresh();
    virtual int update();

/**  Message selection methods */

    virtual int nextMessage();
    virtual int prevMessage();
    virtual int gotoMessage(int);
    virtual int deleteMessage();
    virtual int restoreMessage();

/**  Message status methods */

    virtual int messageRecent();
    virtual int messageDeleted();
    virtual int messageUnseen();
    virtual int currentMessage() { return curMessage; };

    virtual int envelope();

/**  Message data manipulation methods */

    virtual int fetchHeader();
    virtual int fetchBody();
    virtual int saveBody(string);
    virtual int decodeAndSave(string);

/**  Get/Set methods */

    virtual int Messages();
    virtual int Recent();
    virtual int Unseen();
    
    
    virtual void setUser(const char *_user) { user=_user; };
    virtual void setPassword(const char *pwd) { password=pwd; };
    
    virtual string& getUser() { return user; }
    virtual string& getPassword() { return password; }
        
private:

    static Mail *MailInstance;

};


#endif

