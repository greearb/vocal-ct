
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

static const char* const PlayerThread_cxx_Version =
    "$Id: PlayerThread.cpp,v 1.1 2004/05/01 04:15:25 greear Exp $";

#include "global.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cassert>
#include <pthread.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "gdk/gdkx.h"

#define DECLARE_CONFIG_VARIABLES 1
#define HAVE_STDINT_H 1
#include "stdint.h"
#include "our_config_file.h"
#include "player_session.h"
#include "player_media.h"
#include "player_util.h"
#include "media_utils.h"
#include "playlist.h"
#include "mp4live_config.h"

static CPlayerSession *myPSession;
int shutdown_flg;
extern GtkWidget* myVideoHBox;
static GtkWidget* player;
extern GtkWidget* main_window;
extern GtkWidget* main_window_manager;
extern CLiveConfig* myCConfig;
int myPlayerId;

void* PlayerThread_startPlaying(void*);

extern "C" {
   void Player_start()
   {
       pthread_t th;
       shutdown_flg = false;
       pthread_create(&th, 0, PlayerThread_startPlaying, 0);
       pthread_detach(th);
   }

   void Player_stop()
   {
       shutdown_flg = true;
   }
};

void
PlayerThread_init()
{
  GtkWidget* main_vbox1;
  main_vbox1 = gtk_vbox_new(FALSE, 1);
  gtk_container_set_border_width(GTK_CONTAINER(main_vbox1), 4);
  gtk_box_pack_start(GTK_BOX(myVideoHBox), main_vbox1, TRUE, TRUE, 1);
  gtk_widget_show(main_vbox1);

  player = gtk_preview_new(GTK_PREVIEW_COLOR);
  gtk_preview_size(GTK_PREVIEW(player),
                myCConfig->m_videoWidth, myCConfig->m_videoHeight);
  gtk_box_pack_start(GTK_BOX(main_vbox1), player, FALSE, FALSE, 0); 
  gtk_widget_realize(player);      // so XCreateWindow() is called
  gtk_widget_show (player);
  if (player->window) {
        myPlayerId = GDK_WINDOW_XWINDOW(player->window);
  }
}

void*
PlayerThread_startPlaying(void* args)
{
  char buffer[80];
cerr << "Starting **** PlayerThread_startPlaying" << endl;
  gtk_widget_show (player);

  snprintf(buffer, sizeof(buffer), "%u", myPlayerId);
  setenv("SDL_WINDOWID", buffer, strlen(buffer));
  //unsetenv("SDL_WINDOWID");

  config.read_config_file();
  rtsp_set_error_func(player_library_message);
  sdp_set_error_func(player_library_message);
 
  CMsgQueue master_queue;
  SDL_sem *master_sem;
  char* home = getenv("HOME");
  assert(home);
  char argName[256];
  sprintf(argName, "%s/.localSdp.sdp", home);
  char* name = argName;
 
  master_sem = SDL_CreateSemaphore(0);
  if(myPSession == 0)
  {
      snprintf(buffer, sizeof(buffer), "%s %s - %s", "mpeg4Ip", "0.9.6", name);
      myPSession = new CPlayerSession(&master_queue, master_sem,
                             // this should probably be name...
                             buffer);
      if (myPSession == 0) return 0;
  }
 
  char errmsg[512];
  int ret = parse_name_for_session(myPSession, name, errmsg, sizeof(errmsg), NULL);
  if (ret < 0) {
    player_debug_message("%s %s", errmsg, name);
    delete myPSession;
    myPSession = 0;
    return 0;
  }

  myPSession->set_up_sync_thread();
//  gtk_widget_set_usize (main_window, 400, 350);
//  gtk_widget_set_usize (main_window_manager, 400, 350);
  myPSession->set_screen_location(player->allocation.x+10, 
                                  player->allocation.y+10);
  myPSession->set_screen_size(2);

    int keep_going = 0;
    do {
      CMsg *msg;
      SDL_SemWaitTimeout(master_sem, 10000);
      while ((msg = master_queue.get_message()) != NULL) {
        switch (msg->get_value()) {
        case MSG_SESSION_FINISHED:
            keep_going = 1;
          break;
        case MSG_RECEIVED_QUIT:
          keep_going = 1;
          break;
        }
        delete msg;
      }

      if(shutdown_flg) break;

    } while (keep_going == 0);
    myPSession->pause_all_media();
    delete myPSession;
    myPSession = 0;
    SDL_DestroySemaphore(master_sem);
    free(home);
    return 0;
}

