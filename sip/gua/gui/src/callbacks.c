#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "vocalconfig.h"
#include "VideoEnable.h"

#include "configuration.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "registered.xpm"
#include "unregistered.xpm"

#include "VideoEnable.h"

#define NamedDir "/tmp/sipset."

extern GtkWidget *main_window;
extern GtkWidget *log_dialog;
extern GtkWidget *basic_config_dialog;
extern GtkWidget *advanced_config_dialog;
extern int videoEnabled;

extern int display_name_set_by_user;

GtkWidget *incoming_call_dialog = NULL;

int state = INACTIVE;

int mediaStartFlg = 0;

GIOChannel *channel_out, *channel_in;
guint handle_to_gtk_input_add_full;

FILE *dial_history;
char dial_history_path[50];
GtkWidget *dial_combo, *tooltips, *register_eventbox, *username_eventbox;
GtkWidget *registered_pixmap, *unregistered_pixmap;
GList *dial_items = NULL;

/* number of the current call in progress */
char *current_call;

void
on_main_window_destroy                 (GtkObject       *object,
                                        gpointer         user_data)
{
  VideoDevice_shutdown();
  send_message(GUI_SHUTDOWN_STR);
  exit(1);
}

void
on_basic_configuration1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *ok_button;
  basic_config_dialog = create_basic_config_dialog();
  ok_button = lookup_widget (basic_config_dialog, "basic_config_ok");

  /* cannot get this working right now */
  gtk_window_set_default(GTK_WINDOW(basic_config_dialog), ok_button), 
  read_basic_config();
  set_basic_config_tooltips(basic_config_dialog);
  gtk_widget_show(basic_config_dialog);
}


void
on_advanced1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  advanced_config_dialog = create_advanced_config_dialog();
  read_advanced_config();
  set_advanced_config_tooltips(advanced_config_dialog);
  gtk_widget_show(advanced_config_dialog);
}


void
on_messages1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gtk_widget_show(log_dialog);
}


void
on_basic_config_ok_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
  if (write_basic_config())
  {
    set_name();
    set_registered(FALSE);
    gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    send_message(GUI_PREF_STR);
  }
}


void
on_basic_config_cancel_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


void
on_advanced_config_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
  write_advanced_config();
  set_name();
  set_registered(FALSE);
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
  send_message(GUI_PREF_STR);
}


void
on_advanced_config_cancel_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

void
hangup_the_phone(void);
void
take_phone_off_hook(void);
void
start_media(void);

void
on_talk_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  static char msgbuf[50];
  gchar *dial_number = NULL;

  switch (state)
  {
    case CALL_IN_PROGRESS:
    {
      GtkWidget *message_entry;
      hangup_the_phone();
      send_message(GUI_STOP_STR);
      message_entry = lookup_widget (main_window, "message_entry");
      gtk_entry_set_text(GTK_ENTRY(message_entry), "STOPPING CALL");
      break;
    }
    case INACTIVE:
    {
      gpointer list_number;
      int i, dial_number_len;
      GtkWidget *dial_entry = lookup_widget(main_window, "dial_entry");

      dial_number = gtk_editable_get_chars(GTK_EDITABLE(dial_entry),
                             0, -1);
      dial_number_len = strlen(dial_number);
      /* strip blanks from beginning of number*/
      i = 0;
      while(isspace(*dial_number))
      {
        if (++i >= dial_number_len)
        {
          GtkWidget *missing_number_popup = create_missing_number_dialog ();
          gtk_widget_show (missing_number_popup);
          gtk_entry_set_text(GTK_ENTRY(dial_entry), "");
          gtk_widget_grab_focus(dial_entry);
          return;
        }
        dial_number++;
      }

      /* strip blanks from end of number */
      for (i=0; i<strlen(dial_number); i++)
      {
        if (isspace(*(dial_number+i)))
        {
          printf ("inserting end-of-string at %d\n", i);
          *(dial_number+i) = '\0';
          break;
        }
      }
      if (strlen(dial_number) <= 0)
      {
        GtkWidget *missing_number_popup = create_missing_number_dialog ();
        gtk_widget_show (missing_number_popup);
        return;
      }
      
      /* disable the Talk button */
      gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

      take_phone_off_hook();

      sprintf(msgbuf, "%s %s", GUI_INVITE_STR, dial_number);
      send_message(msgbuf);

      /* Add the number to dial history, if not already there */
      list_number = g_list_find_custom(dial_items, dial_number, (GCompareFunc)strcmp);

      if (list_number == NULL)
      {
        char *new_number = strdup(dial_number);

        dial_items = g_list_insert_sorted(dial_items, new_number,
           (GCompareFunc)strcmp);
        gtk_combo_set_popdown_strings(GTK_COMBO(dial_combo), dial_items);
        gtk_entry_set_text(GTK_ENTRY(dial_entry), new_number);
        write_phone_history(dial_items, dial_history_path);
      }
      g_free(dial_number);
      break;
    }
    default:
    break;
      /* do nothing */
  }

}


gboolean
on_log_dialog_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_widget_hide(log_dialog);
  return TRUE;
}

void
on_log_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_hide(log_dialog);
}


void
on_log_clear_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *logger = lookup_widget (log_dialog, "log_text");
  guint len = gtk_text_get_length(GTK_TEXT(logger));
  gtk_text_set_point(GTK_TEXT(logger), 0);
  gtk_text_forward_delete(GTK_TEXT(logger), len);
}


void
on_clear_history1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GList *emptyList = NULL;
  emptyList = g_list_append(emptyList, "");
  gtk_combo_set_popdown_strings(GTK_COMBO(dial_combo), emptyList);

  unlink(dial_history_path);
  creat(dial_history_path, 0666);

  g_list_free(dial_items);
  dial_items = NULL;

}


void
on_missing_number_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


void
on_incoming_call_accept_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *message_entry = lookup_widget (main_window, "message_entry");
  char display_string[50];

  send_message(GUI_ACCEPT_STR);
  take_phone_off_hook();

  /* set status message */
  sprintf(display_string, "ACCEPTED CALL FROM %s", current_call);
  gtk_entry_set_text(GTK_ENTRY(message_entry), display_string);

  gtk_widget_destroy(incoming_call_dialog);
  incoming_call_dialog = NULL;
}

void set_state(int new_state);

void
on_incoming_call_reject_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *message_entry = lookup_widget (main_window, "message_entry");
  char display_string[50];

  send_message(GUI_REJECT_STR);

  /* set status message */
  sprintf(display_string, "REJECTED CALL FROM %s", current_call);
  gtk_entry_set_text(GTK_ENTRY(message_entry), display_string);

  gtk_widget_destroy(incoming_call_dialog);
  incoming_call_dialog = NULL;
  set_state(INACTIVE);
}


void
on_error_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

void
on_fatal_error_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  exit(1);
}


void
on_dial_entry_activate                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
  GtkWidget *talk_button = lookup_widget (main_window, "talk_button");
  GtkArg arg;
  arg.name = "sensitive";

  gtk_object_getv(GTK_OBJECT(talk_button), 1, &arg);
  if (GTK_VALUE_BOOL(arg) == TRUE)
  {
    gtk_button_clicked(GTK_BUTTON(talk_button));
  }
}


gboolean
on_Display_Name_key_press_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
  display_name_set_by_user = TRUE;
  return FALSE;
}


void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  VideoDevice_shutdown();
  send_message(GUI_SHUTDOWN_STR);
  gtk_main_quit();

}

/***************** code added *******************/

void print_error(GIOError error)
{
  switch(error)
  {
    case G_IO_ERROR_NONE:
    break;

    case G_IO_ERROR_AGAIN:
    g_message("again error\n");
    break;

    case G_IO_ERROR_INVAL:
    g_message("inval error\n");
    break;

    case G_IO_ERROR_UNKNOWN:
    default:
    g_message("unknown error: %d\n", error);
  }
}

void send_message(char* message)
{
  GIOError error;
  int len = 0;
  gsize bytes_written;

  /* If fatal error occurred, then writing to pipe will cause "Broken pipe" message */
  if (state == FATAL_ERROR)
  {
    return;
  }
  else
  {
    /*
    printf ("in send_message, state is %d\n", state);
    */
  }

  len = strlen(message);

  error = g_io_channel_write(channel_out, message, len+1, &bytes_written);
  /*g_message("wrote %d bytes: %s\n", bytes_written, message);*/

  if (error != G_IO_ERROR_NONE)
  {
    g_message("error occurred during write to UA: ");
    print_error(error);
  }
  else
  {
    /*
    printf("no error on send message\n");
    */
  }
}

void set_state(int new_state)
{
  /* uncomment for debugging
  switch(new_state)
  {
    case INACTIVE:
    printf("setting state to INACTIVE\n");
    break;

    case INCOMING_REQUEST:
    printf("setting state to INCOMING_REQUEST\n");
    break;

    case CALL_IN_PROGRESS:
    printf("setting state to CALL_IN_PROGRESS\n");
    break;

    case FATAL_ERROR:
    printf("setting state to FATAL_ERROR\n");
    break;

  } */

  state = new_state;
}

void take_phone_off_hook()
{
  GtkWidget *talk_button = lookup_widget(main_window, "talk_button");
  GtkWidget *dial_entry = lookup_widget(main_window, "dial_entry");
  gtk_object_set(GTK_OBJECT(talk_button), "GtkButton::label", (gchar*)HANGUP, NULL);
  gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), talk_button, "Stop the call", NULL);
  gtk_tooltips_set_tip (GTK_TOOLTIPS(tooltips), dial_entry, _("Press Enter to stop call"), NULL);
  set_state (CALL_IN_PROGRESS);
  start_media();

}

void start_media()
{
#ifdef USE_MPEGLIB
  if(videoEnabled)
  {
      printf("Starting media\n" );
      if(!mediaStartFlg)
      {
          Video_start();
          Player_start();
          mediaStartFlg = 1;
      }
  }
#endif
}

void hangup_the_phone()
{
  GtkWidget *talk_button = lookup_widget(main_window, "talk_button");
  GtkWidget *dial_entry = lookup_widget(main_window, "dial_entry");

  /* if there is an incoming call dialog, destroy it */
  if (incoming_call_dialog != NULL)
  {
    gtk_widget_destroy(incoming_call_dialog);
    incoming_call_dialog = NULL;
  }

  /* disable the Talk button */
  gtk_widget_set_sensitive(talk_button, FALSE);
  gtk_object_set(GTK_OBJECT(talk_button), "GtkButton::label", (gchar*)TALK, NULL);
  gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), talk_button, "Make a call", NULL);
  gtk_tooltips_set_tip (GTK_TOOLTIPS(tooltips), dial_entry, _("Enter SIP URI or Phone Number"), NULL);
  set_state (INACTIVE);
#ifdef USE_MPEGLIB
  if(mediaStartFlg)
  {
      Video_stop();
      Player_stop();
      mediaStartFlg = 0;
  }
#endif

}

void
gtk_container_remove_callback(gpointer child, gpointer data)
{
  gtk_container_remove (GTK_CONTAINER (register_eventbox), GTK_WIDGET(child));
}

void
set_name()
{
  GtkWidget *name_label = lookup_widget(main_window, "name_label");
  char *name = get_advanced_config_value(DISPLAY_NAME);
  if( (name == NULL) || (strlen(name) == 0) )
  {
    name = get_basic_config_value(USER_NAME);
  }
  gtk_label_set_text(GTK_LABEL(name_label), name);
}

void
set_registered(int isRegistered)
{
  char register_tooltip[100];
  char *user_name = get_basic_config_value(USER_NAME);

  GList *children = gtk_container_children(GTK_CONTAINER(register_eventbox));

  g_list_foreach(children, gtk_container_remove_callback, NULL);

  if (isRegistered)
  {
    gtk_widget_show (registered_pixmap);
    gtk_container_add (GTK_CONTAINER (register_eventbox), registered_pixmap);

    /* set tooltip for register pixmap */
    sprintf(register_tooltip, "user %s is registered", user_name);
  }
  else
  {
    gtk_widget_show (unregistered_pixmap);
    gtk_container_add (GTK_CONTAINER (register_eventbox), unregistered_pixmap);
    gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), register_eventbox, "Unregistered", NULL);

    /* set tooltip for name label */
    sprintf(register_tooltip, "user %s is not registered", user_name);
  }

  /* only put the user name in the tooltip if there is a user name */
  if( (user_name == NULL) || (strlen(user_name) > 0) )
  {
    gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), register_eventbox, register_tooltip, NULL);
  }
  else
  {
    gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), register_eventbox, "No user name to register", NULL);
  }
}


/* called by reader*/
void
process_input(gchar *input_buf)
{
  GtkWidget *logger = lookup_widget (log_dialog, "log_text");
  GtkWidget *message_entry = lookup_widget (main_window, "message_entry");
  char *command;
  char *the_rest;
  guint text_length;

  /* for now, always put full text into message window */
  text_length = gtk_text_get_length(GTK_TEXT(logger));
  gtk_text_set_point(GTK_TEXT(logger), text_length);
  gtk_text_insert (GTK_TEXT(logger), NULL, NULL, NULL, input_buf, strlen(input_buf));
  gtk_text_insert (GTK_TEXT(logger), NULL, NULL, NULL, "\n", 1);

  /* the first token is the command */
  command = strtok(input_buf, " ");
  /*printf ("received command: %s\n", command);*/
  /* the rest is the message for log window */
  the_rest = input_buf+(strlen(command));
  the_rest = strchr(input_buf, '\0');
  the_rest++;

  /*
  {
    text_length = gtk_text_get_length(GTK_TEXT(logger));
    gtk_text_set_point(GTK_TEXT(logger), text_length);
    gtk_text_insert (GTK_TEXT(logger), NULL, NULL, NULL,
                     the_rest, strlen(the_rest));
    gtk_text_insert (GTK_TEXT(logger), NULL, NULL, NULL, "\n\n", 2);
  
  }
  */

  if (strcmp(command, UA_REGISTERED_STR) == 0)
  {
    set_registered(TRUE);
  }
  else if (strcmp(command, UA_INFO_STR) == 0)
  {
    /* do nothing */
  }
  else if (strcmp(command, UA_RINGING_STR) == 0)
  {
    gtk_entry_set_text(GTK_ENTRY(message_entry), UA_RINGING_STR);
    if (state == INACTIVE)
    {
      GtkWidget *label;
      char display_string[50];
      if (current_call != NULL)
      {
        free (current_call);
      }
      current_call = strdup(the_rest);

      set_state(INCOMING_REQUEST);
      incoming_call_dialog = create_incoming_call_dialog();

      /* get incoming_call_label*/
      label = lookup_widget(incoming_call_dialog, "incoming_call_label");

      sprintf(display_string, "You have a call from %s", current_call);
      gtk_label_set_text(GTK_LABEL(label), display_string);
      gtk_widget_show(incoming_call_dialog);
    }
  }
  else if (strcmp(command, UA_TRYING_STR ) == 0)
  {
    /* enable the Talk button*/
    GtkWidget *talk_button = lookup_widget (main_window, "talk_button");
    gtk_widget_set_sensitive(talk_button, TRUE);

    gtk_entry_set_text(GTK_ENTRY(message_entry), UA_TRYING_STR);
  }
  else if (strcmp(command, UA_REDIRECT_STR ) == 0)
  {
    /* enable the Talk button*/
    GtkWidget *talk_button = lookup_widget (main_window, "talk_button");
    gtk_widget_set_sensitive(talk_button, TRUE);

    gtk_entry_set_text(GTK_ENTRY(message_entry), UA_REDIRECT_STR);
  }
  else if(strcmp(command, UA_INCALL_STR ) == 0)
  {
    gtk_entry_set_text(GTK_ENTRY(message_entry), UA_INCALL_STR);

    start_media();
  }
   else if(strcmp(command, UA_R_HOLD_STR ) == 0)
  {
    gtk_entry_set_text(GTK_ENTRY(message_entry), UA_R_HOLD_STR);

  }
  else if(strcmp(command, UA_R_RESUME_STR ) == 0)
  {
    gtk_entry_set_text(GTK_ENTRY(message_entry), UA_R_RESUME_STR);

  }
  else if(strcmp(command, UA_STOP_STR ) == 0)
  {
    gtk_entry_set_text(GTK_ENTRY(message_entry), "STOP CALL");
    hangup_the_phone();
  }

  else if(strcmp(command, UA_ERROR_STR ) == 0)
  {
    GtkWidget *error_dialog = create_error_dialog();
    GtkWidget *error_label = lookup_widget (error_dialog, "error_label");
    GtkWidget *talk_button = lookup_widget (main_window, "talk_button");

    hangup_the_phone();
    gtk_label_set_text(GTK_LABEL(error_label), the_rest);
    gtk_widget_show(error_dialog);
    gtk_entry_set_text(GTK_ENTRY(message_entry), "ERROR");
    gtk_widget_set_sensitive(talk_button, TRUE);
  }

  else if(strcmp(command, UA_BUSY_STR ) == 0)
  {
    gtk_entry_set_text(GTK_ENTRY(message_entry), "BUSY");
  }
  else if(strcmp(command, UA_L_HANGUP_STR ) == 0)
  {
    /* local caller hung up and call has been torn down */
    GtkWidget *talk_button = lookup_widget (main_window, "talk_button");

    gtk_entry_set_text(GTK_ENTRY(message_entry), "CALL FINISHED (LOCAL HANGUP)");
    /* enable the Talk button */
    gtk_widget_set_sensitive(GTK_WIDGET(talk_button), TRUE);
  }
  else if(strcmp(command, UA_R_HANGUP_STR ) == 0)
  {
    /* remote caller hung up and call has been torn down */
    GtkWidget *talk_button = lookup_widget (main_window, "talk_button");
    hangup_the_phone();

    gtk_entry_set_text(GTK_ENTRY(message_entry), "CALL FINISHED (REMOTE HANGUP)");
    /* enable the Talk button */
    gtk_widget_set_sensitive(GTK_WIDGET(talk_button), TRUE);
  }
  else
  {
    g_message("Error: receive unrecognized command: %s\n", command);
  }
  
}

/* callback function for UI_TO_GUI pipe */
void
reader (gpointer data, gint source, GdkInputCondition condition)
{
  GtkWidget *message_entry = lookup_widget (main_window, "message_entry");
  GIOError error;
  gchar *buf = g_new0 (gchar, 3000);
  gchar *token;
  guint bytes_read;
  int i;

  static int reads_missed = 0;

  error = g_io_channel_read(channel_in, buf, 3000, &bytes_read);

  /* stop reading if the UA Controller goes away */
  /* give it a few chances to come back first */
  if (bytes_read <= 0)
  {
    reads_missed++;
    if (reads_missed > 10)
    {
      GtkWidget *fatal_error_dialog = create_fatal_error_dialog();
      GtkWidget *fatal_error_label = lookup_widget (fatal_error_dialog, "fatal_error_label");

      hangup_the_phone();
      set_state(FATAL_ERROR);
      gtk_input_remove(handle_to_gtk_input_add_full);

      gtk_entry_set_text(GTK_ENTRY(message_entry), "PLEASE RESTART");

      gtk_label_set_text(GTK_LABEL(fatal_error_label), "Fatal Error. Please restart");
      gtk_widget_show(fatal_error_dialog);

    }
    return;
  }
  reads_missed = 0;
  if (error != G_IO_ERROR_NONE)
  {
    g_message("error occurred during read from UA: ");
    print_error(error);
    return;
  }

  token = buf;

  for (i=0; i<bytes_read; i++)
  {
    if (buf[i] == '|')
    {
      buf[i] = '\0';
      process_input(token);
      token = &buf[i+1];
    }
  }

  g_free(buf);

}

GtkWidget*
create_pixmap_d(GtkWidget *window, gchar **data)
{
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;

  colormap = gtk_widget_get_colormap (window);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask,
                                                   NULL, data);
  pixmap = gtk_pixmap_new (gdkpixmap, mask);
  gdk_pixmap_unref (gdkpixmap);
  gdk_bitmap_unref (mask);
  return pixmap;
}

/* handle termination signals */
void fatal_error_signal(int sig)
{
  signal(sig, SIG_DFL);
  send_message(GUI_SHUTDOWN_STR);
  kill(getpid(), SIGINT);
}

void
sipset_init()
{
  char dial_buf[50];
  int dial_len = 50;
  int items_in_history;
  char ua_to_gui_str[100];
  char gui_to_ua_str[100];
  pid_t pid;
  int fd_out, fd_in;
  char *config_name = "gua.cfg";
  char config_file_path[150], argument[150], location_of_exe[150];
  char local_dir[256];
  struct stat statbuf;

  struct stat stbuf;

  int err;

  char *home = getenv("HOME");

  /* set up configuration */
  snprintf(config_file_path, 149, "%s/.sipset", home);
  config_file_path[149] = 0;
  
  /* VOCAL_INSTALL_PATH comes from vocalconfig.h */
  init_config(config_file_path, config_name);


  pid = fork();
  if (pid == 0)
  {
    static char *argv[10];
    int i = 0;
    
    /* this is the child process  */
    /* start up the UA Controller */
    snprintf(argument, 149, "%s/%s", config_file_path, config_name);
    argument[149] = 0;
    
    /* for some reason, execv eats argv[0] */
    argv[i++] = "";
    argv[i++] = "-g";
    argv[i++] = "-f";
    argv[i++] = argument;

    argv[i++] = NULL;
    
    /* if gua is in the Vocal install dir, then run it from there */
    snprintf (location_of_exe, 149, "%s/bin/gua", VOCAL_INSTALL_PATH);
    location_of_exe[149] = 0;
    
    if( (stat(location_of_exe, &stbuf) == 0)
       && S_ISREG(stbuf.st_mode))
    {
      printf ("Starting gua from %s\n", location_of_exe);
      execv(location_of_exe, argv);
    }
    else if( (stat("./gua", &stbuf) == 0 )
            && S_ISREG(stbuf.st_mode))
    {
      printf ("Starting gua from current directory\n");
      execv("./gua", argv);
    }
    else
    {
      printf("Error: cannot find gua executable\n");
      exit(1);
    }
    _exit;
  }

  /* figure out where the fifo pipes go */


  snprintf(local_dir, 255, "/tmp/sipset.%d", getuid());
  local_dir[254] = '\0';

  mkdir(local_dir, 0700);
  chmod(local_dir, 0700);


  err = lstat(local_dir, &statbuf);
  if(err)
  {
      printf("Cannot continue, failed to stat directory: %s",
	    strerror(errno));
      exit(-1);
  }

  if(statbuf.st_uid != getuid() ||
     statbuf.st_mode != 0040700)
  {
      printf(
	    "Cannot continue, directory %s does not have right permissions", 
	    local_dir);
      exit(-1);
  }


  sprintf(ua_to_gui_str, "/tmp/sipset.%d/UaToGui.%d", getuid(), pid);
  sprintf(gui_to_ua_str, "/tmp/sipset.%d/GuiToUa.%d", getuid(), pid);
  mkfifo(ua_to_gui_str, 0600);
  mkfifo(ua_to_gui_str, 0600);

  /* this is the parent process */
  fd_in = open(ua_to_gui_str, O_RDONLY, 0);
  if (fd_in == -1)
  {
    printf ("cannot open %s for reading!", ua_to_gui_str);
    exit(1);
  }

  fd_out = open(gui_to_ua_str, O_WRONLY, 0);
  if (fd_out == -1)
  {
    printf ("cannot open %s for writing!", ua_to_gui_str);
    exit(1);
  }

  channel_out = g_io_channel_unix_new(fd_out);
  channel_in = g_io_channel_unix_new(fd_in);

  handle_to_gtk_input_add_full
    = gtk_input_add_full(fd_in, GDK_INPUT_READ, reader, NULL, NULL, NULL);

  /**** set up dial history ***/
  dial_combo = lookup_widget (main_window, "dial_combo");

  /**** set up tooltips ***/
  tooltips = lookup_widget (main_window, "tooltips");

  /*** set up the registration icon ***/
  register_eventbox = lookup_widget (main_window, "register_eventbox");
  username_eventbox = lookup_widget (main_window, "username_eventbox");

  sprintf(dial_history_path, "%s/%s", config_file_path, DIAL_HISTORY);
  dial_history = fopen(dial_history_path, "r");

  if (dial_history == NULL)
  {
    if (creat(dial_history_path, 0666) != -1)
    {
      dial_history = fopen(dial_history_path, "r");
    }
    else
    {
      g_message ("Cannot open dial history for reading\n");
    }
  }

  /* ensure that there is a blank space at the top of combo */
  dial_items = g_list_append(dial_items, "");

  items_in_history = 0;
  while (fgets (dial_buf, dial_len, dial_history) != NULL)
  {
    if (strlen(dial_buf) > 0)
    {
      char *newline = strchr(dial_buf, '\n');
      if (newline != NULL)
      {
        *newline = '\0';
      }
      /* ignore blank line */
      if (dial_buf[0] != '\0')
      {
        char *new_number = strdup(dial_buf);
        dial_items = g_list_append(dial_items, new_number);
        items_in_history++;
      }
    }
  }
  fclose(dial_history);
  
  if (items_in_history > 0)
  {
    dial_items = g_list_sort(dial_items, (GCompareFunc)strcmp);
    gtk_combo_set_popdown_strings(GTK_COMBO(dial_combo), dial_items);
  }

  /* stop pop-up from showing when Enter pressed */
  gtk_combo_disable_activate(GTK_COMBO(dial_combo));

  /* set up pixmaps */
  registered_pixmap = create_pixmap_d (main_window, registered_xpm);
  gtk_widget_ref (registered_pixmap);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "registered_pixmap", registered_pixmap,
                            (GtkDestroyNotify) gtk_widget_unref);
  unregistered_pixmap = create_pixmap_d (main_window, unregistered_xpm);
  gtk_widget_ref (unregistered_pixmap);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "unregistered_pixmap", unregistered_pixmap,
                            (GtkDestroyNotify) gtk_widget_unref);

  set_name();
  set_registered(FALSE);
 
}

void
on_video_enabled_button                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_lanforge_button_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

