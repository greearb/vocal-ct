
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
 
#include <iostream.h>        
#include <stdio.h>
#include "mailx.h"
#include "cpLog.h"
#include <time.h>

Mail *Mail::MailInstance = 0;

Mail::Mail()
{

    #include "linkage.cin"

    stream = NIL;
    recentOnly = 0;
    deletedOnly = 0;
    curMessage = 0;
    MailInstance = this;
}

Mail::~Mail()
{
    if( stream !=NIL ) mail_close(stream);
    stream = NIL;
}

int
Mail::open(string mailboxName)
{
    if( stream !=NIL ) mail_close(stream);
    return (int)(stream=mail_open(stream,(char *)mailboxName.c_str(),NIL));
}

int
Mail::close()
{
    cerr << "Closing mail session" << endl;
    if( stream !=NIL ) mail_close(stream);
    stream = NIL;
    return true;
}

int
Mail::refresh()
{
    if( stream == NIL ) return false;
    return mail_status (NIL,stream->mailbox,
           SA_MESSAGES|SA_RECENT|SA_UNSEEN|SA_UIDNEXT|SA_UIDVALIDITY);
}

int
Mail::update()
{
    if( stream == NIL ) return false;
    mail_expunge(stream);
    return true;
}

int
Mail::deleteMessage()
{
    char  tmp[128];
    if( stream == NIL ) return false;
    sprintf(tmp,"%d",currentMessage()+1);
    mail_setflag(stream,tmp,"\\DELETED");
    return true;
}

int
Mail::restoreMessage()
{
    char  tmp[128];
    if( stream == NIL ) return false;
    sprintf(tmp,"%d",currentMessage()+1);
    mail_clearflag(stream,tmp,"\\DELETED");
    return true;
}

int
Mail::nextMessage()
{
    if( stream == NIL ) return 0;
    if( currentMessage() < Messages()-1 ) curMessage++;
    else return 0;
    return currentMessage()+1;
}

int
Mail::prevMessage()
{
    if( stream == NIL ) return 0;
    if( currentMessage() > 0 ) curMessage--;
    else return 0;
    return currentMessage()+1;
}

int
Mail::gotoMessage(int msgno)
{
    if( stream == NIL ) return 0;
    if( msgno >=0 && msgno < Messages() ) curMessage=msgno;
    else return 0;
    return currentMessage()+1;
}

int
Mail::fetchHeader()
{
    char tmp[128];
    if( stream == NIL ) return false;
    sprintf(tmp,"1:%d",Messages());
    mail_search(stream,tmp);
    return true;
}

int
Mail::fetchBody()
{
    return true;
}

int
Mail::saveBody(string fileName)
{
    if( stream == NIL ) return false;
    char *bodyText;
    unsigned long len;
    bodyText=mail_fetchtext_full(stream,currentMessage()+1,&len,NIL);
    if( len < 1 )
    {
	return false;
    }
    FILE *fd;
    fd=fopen(fileName.c_str(),"wt");
    if( !fd ) return false;
    fwrite(bodyText,len,1,fd);
    fclose(fd);
    return true;
}

int
Mail::decodeAndSave(string fileName)
{
    if( stream == NIL ) return false;
    char *bodyText;
    void *decodedData;
    unsigned long len,dlen;
    bodyText=mail_fetchtext_full(stream,currentMessage()+1,&len,NIL);
    if( len < 1 )
    {
	return false;
    }
    decodedData=rfc822_base64((unsigned char *)bodyText,len,&dlen);
    if( dlen < 1 )
    {
	return false;
    }
    FILE *fd;
    fd=fopen(fileName.c_str(),"wt");
    if( !fd ) return false;
    fwrite(decodedData,dlen,1,fd);
    fclose(fd);
    return true;

}

int
Mail::Messages()
{
    if( stream == NIL ) return 0;
    return stream->nmsgs;
}

int
Mail::Recent()
{
    if( stream == NIL ) return 0;
    return stream->recent;
}

int
Mail::Unseen()
{
    if( stream == NIL ) return 0;
    int nUnseen,i;
    for(nUnseen=0,i=0;i<Messages();i++)
    {
	if( !(mail_elt(stream,i+1)->seen) ) nUnseen++;
    }
    return nUnseen;
}

int
Mail::messageRecent()
{
    if( stream == NIL ) return 0;
    return mail_elt(stream,currentMessage()+1)->recent;
}

int
Mail::messageUnseen()
{
    if( stream == NIL ) return 0;
    return !mail_elt(stream,currentMessage()+1)->seen;
}

int
Mail::messageDeleted()
{
    if( stream == NIL ) return 0;
    return mail_elt(stream,currentMessage()+1)->deleted;
}

int
Mail::envelope()
{
    if( stream == NIL ) return false;
	MESSAGECACHE selt;
	env=mail_fetchenvelope(stream,currentMessage()+1);
	mail_parse_date(&selt,env->date);
	message_date=mail_longdate(&selt);
	tzset();
	return true;
}

Mail *Mail::instance()
{
    return MailInstance;
}

/*
void main()
{
    char tmp[1024];

    Mail mail;

//    printf("Mailbox?:");
//    gets(tmp);
    strcpy(tmp,"/var/spool/mail/vm3244");
    printf("RET:%04X:OPEN\n",mail.open(tmp));
    printf("MailBox (%s) status:\n\n",mail.stream->mailbox);
    printf("\tNumber of messages         :%d\n",mail.Messages());
    printf("\tNumber of recent messages  :%d\n\n",mail.Messages());

    for(int i=0;i<mail.Messages();i++)
    {
	printf("5\n");
	mail.gotoMessage(i);
	mail.envelope();
	printf("%d: %s\n",i,mail.env->subject);
	printf("Got it.\n");
	char tname[128];
	sprintf(tname,"message%d.txt",i);
	mail.decodeAndSave(tname);
    }


    mail.deleteMessage();

    if( !mail.messageDeleted() ) {
	printf("Message %d NOT deleted.\n",mail.currentMessage()+1);
    } else {
	printf("Message %d deleted.\n",mail.currentMessage()+1);
    }

    mail.restoreMessage();


    if( !mail.messageDeleted() ) {
	printf("Message %d NOT deleted.\n",mail.currentMessage()+1);
    } else {
	printf("Message %d deleted.\n",mail.currentMessage()+1);
    }


    mail.update();
    mail.close();
}

*/

extern "C"
{

void mm_searched (MAILSTREAM *stream,unsigned long number)
{
}


void mm_exists (MAILSTREAM *stream,unsigned long number)
{
}


void mm_expunged (MAILSTREAM *stream,unsigned long number)
{
}


void mm_flags (MAILSTREAM *stream,unsigned long number)
{
}


void mm_notify (MAILSTREAM *stream,char *string,long errflg)
{
  mm_log (string,errflg);
}

void mm_log (char *string,long errflg)
{
  switch ((short) errflg) {
  case NIL:
/**  printf ("[%s]\n",string); */
    break;
  case PARSE:
  case WARN:
    printf ("%%%s\n",string);
    break;
  case ERROR:
    printf ("?%s\n",string);
    break;
  }
}


void mm_dlog (char *string)
{
  puts (string);
}

void mm_critical (MAILSTREAM *stream)
{
}


void mm_nocritical (MAILSTREAM *stream)
{
}


long mm_diskerror (MAILSTREAM *stream,long errcode,long serious)
{
  return NIL;
}

void mm_fatal (char *string)
{
  printf ("?%s\n",string);
}


void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
{
    Mail *mail=Mail::instance();
    strcpy(user,mail->getUser().c_str());
    strcpy(pwd,mail->getPassword().c_str());
}

void mm_list (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
{
}


void mm_lsub (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
{
}


void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status)
{
}


}
