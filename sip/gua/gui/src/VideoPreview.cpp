
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
static const char* const VideoDevice_cxx_Version = 
    "$Id: VideoPreview.cpp,v 1.1 2004/05/01 04:15:25 greear Exp $";



#include "global.h"

#define DECLARE_CONFIG_VARIABLES 1
#define HAVE_STDINT_H 1

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "gdk/gdkx.h"

//MPEG4IP
#include "stdint.h"
#include "mp4live.h"
#include "media_flow.h"
#include "video_v4l_source.h"
#include "audio_oss_source.h"
#include "mp4live_config.h"
#include "codec_plugin_private.h"

CLiveConfig* myCConfig;                             
extern void DoStart();
extern void DoStop();
extern "C" void gtk_main_quit();
CAVMediaFlow* myAVFlow;
GtkWidget* myMainWindow;
GtkWidget* myVideoHBox;

extern "C" {
    void VideoDevice_preview_init(GtkWidget* w, GtkWidget* winManager);
    void VideoDevice_shutdown();
    void on_exit_activate(GtkWidget *widget, gpointer *data);
    void Video_start()
    {
        char userFileName[256];
        char* home = getenv("HOME");
        if (home) {
            sprintf(userFileName, "%s/%s", home, "/.mp4live_rc.r");
        }
        myCConfig->ReadFromFile(userFileName);
        myAVFlow->Start();
        free(home);
    };
    void Video_stop() 
    {
        myAVFlow->Stop();
    };
}

void VideoDevice_shutdown();
extern void PlayerThread_init();

void
VideoDevice_init()
{
    //Initialize the configuration
    initialize_plugins(); 

    char userFileName[512];
    char sdpFileName[512];
    char* home = getenv("HOME");
    if (home) {
        sprintf(userFileName, "%s/%s", home, "/.mp4live_rc");
        sprintf(sdpFileName, "%s/%s", home, "/.localSdp.sdp.t");
    }

    myCConfig = 0;
            myCConfig = new CLiveConfig(MyConfigVariables,
                    sizeof(MyConfigVariables) / sizeof(SConfigVariable),
                    home ? userFileName : NULL);

            // read user config file if present
            myCConfig->ReadDefaultFile();

            myCConfig->m_appAutomatic = false;

            extern bool PrintDebugMessages;
            PrintDebugMessages =
                    myCConfig->GetIntegerValue(CONFIG_APP_DEBUG);
            myCConfig->SetStringValue(CONFIG_SDP_FILE_NAME, sdpFileName);
            myCConfig->SetStringValue(CONFIG_VIDEO_SOURCE_TYPE, "V4L");

    // probe for capture cards
    if (!strcasecmp(myCConfig->GetStringValue(CONFIG_VIDEO_SOURCE_TYPE),
      VIDEO_SOURCE_V4L)) {
            CV4LVideoSource::InitialVideoProbe(myCConfig);
    }

    // probe for sound card capabilities
    if (!strcasecmp(myCConfig->GetStringValue(CONFIG_AUDIO_SOURCE_TYPE),
      AUDIO_SOURCE_OSS)) {
            myCConfig->m_audioCapabilities = new CAudioCapabilities(
                    myCConfig->GetStringValue(CONFIG_AUDIO_SOURCE_NAME));
    }

//    myCConfig->SetStringValue(CONFIG_VIDEO_ENCODER, VIDEO_ENCODER_FFMPEG);
 
    myCConfig->Update();
 
    // attempt to exploit any real time features of the OS
    // will probably only succeed if user has root privileges
    if (myCConfig->GetBoolValue(CONFIG_APP_REAL_TIME_SCHEDULER)) {
#ifdef _POSIX_PRIORITY_SCHEDULING
        // put us into the lowest real-time scheduling queue
        struct sched_param sp;
        sp.sched_priority = 1;
#endif /* _POSIX_PRIORITY_SCHEDULING */
#ifdef _POSIX_MEMLOCK
        // recommendation is to reserve some stack pages
        u_int8_t huge[1024 * 1024];
        memset(huge, 1, sizeof(huge));
#endif /* _POSIX_MEMLOCK */
    }
    free(home);
}  // end VideoDevice_init()

void
VideoDevice_preview_init(GtkWidget* main_window, GtkWidget* winManager)
{
  
  VideoDevice_init();
  myAVFlow = new CAVMediaFlow(myCConfig);
  myMainWindow = main_window;
  myVideoHBox = gtk_hbox_new (FALSE, 0);
  gtk_container_add(GTK_CONTAINER(winManager), myVideoHBox); 
  gtk_widget_show (myVideoHBox);
#if 1
  GtkWidget* main_vbox1;
  GtkWidget* video_preview;
  main_vbox1 = gtk_vbox_new(FALSE, 1);
  gtk_container_set_border_width(GTK_CONTAINER(main_vbox1), 4);
  gtk_widget_show(main_vbox1);
  gtk_box_pack_start(GTK_BOX(myVideoHBox), main_vbox1, FALSE, FALSE, 0);

  video_preview = gtk_preview_new(GTK_PREVIEW_COLOR);
 
  gtk_preview_size(GTK_PREVIEW(video_preview),
                myCConfig->m_videoWidth, myCConfig->m_videoHeight);
 
  gtk_widget_show(video_preview);
 
  gtk_box_pack_start(GTK_BOX(main_vbox1), video_preview, FALSE, FALSE, 5); 
  // Let video source know which window to draw into
  gtk_widget_realize(video_preview);      // so XCreateWindow() is called
 
  if (video_preview->window) {
        myCConfig->m_videoPreviewWindowId =
                        GDK_WINDOW_XWINDOW(video_preview->window);
  }
  myAVFlow->StartVideoPreview(); 
#else
   gtk_widget_set_usize (myVideoHBox, 
           myCConfig->m_videoWidth, myCConfig->m_videoHeight+50);
#endif

  PlayerThread_init();
  gtk_widget_show(main_window);
}

void
VideoDevice_shutdown()
{
    if(myCConfig ==0) return;
    if(myAVFlow) myAVFlow->StopVideoPreview();
    if (myCConfig->GetBoolValue(CONFIG_APP_REAL_TIME_SCHEDULER)) {
#ifdef _POSIX_MEMLOCK
            munlockall();
#endif /* _POSIX_MEMLOCK */
    }
    close_plugins();
    delete myAVFlow;
    delete(myCConfig);
    myAVFlow = 0;
    myCConfig = 0;
}
