#include <gtk/gtk.h>

#define GUI_INVITE_STR            "INVITE"
#define GUI_STOP_STR              "STOP"
#define GUI_PREF_STR              "PREFERENCES"
#define GUI_ACCEPT_STR            "ACCEPT"
#define GUI_REJECT_STR            "STOP"
#define GUI_HOLD_STR              "HOLD"
#define GUI_SHUTDOWN_STR          "SHUTDOWN"
#define UA_REDIRECT_STR          "REDIRECT"
#define UA_REGISTERED_STR        "REGISTERED"
#define UA_STOP_STR              "STOP"
#define UA_RINGING_STR           "RINGING"
#define UA_INFO_STR              "INFO"
#define UA_R_HOLD_STR            "R_HOLD"
#define UA_R_RESUME_STR          "R_RESUME"
#define UA_TRYING_STR            "TRYING"
#define UA_INCALL_STR            "INCALL"
#define UA_ERROR_STR             "ERROR"
#define UA_BUSY_STR              "BUSY"
#define UA_UNAUTHORIZED_STR      "UNAUTHORIZED"
#define UA_L_HANGUP_STR          "L_HANGUP"
#define UA_R_HANGUP_STR          "R_HANGUP"

void
on_main_window_destroy                 (GtkObject       *object,
                                        gpointer         user_data);

void
on_basic_configuration1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_advanced1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_messages1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_messages_window_destroy             (GtkObject       *object,
                                        gpointer         user_data);

void
on_basic_config_ok_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_basic_config_cancel_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_advanced_config_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_advanced_config_cancel_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_talk_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

                                        
/****************** not generated *********************/

#define DIAL_HISTORY "dial_history"


/* button labels */
#define HANGUP  "Hang up"
#define TALK    "Talk"

enum states {INACTIVE, INCOMING_REQUEST, CALL_IN_PROGRESS, FATAL_ERROR};

void
sipset_init(void);

void
send_message(char *message);

void
reader (gpointer data, gint source, GdkInputCondition condition);

gboolean
on_messages_window_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_log_dialog_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_log_text_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_log_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_log_clear_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_clear_history1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_missing_number_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_incoming_call_accept_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_incoming_call_reject_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_error_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dial_entry_key_press_event          (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
on_dial_entry_activate                 (GtkEditable     *editable,
                                        gpointer         user_data);


void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
set_registered(int isRegistered);

void
set_name(void);

gboolean
on_Display_Name_key_press_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
on_fatal_error_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
fatal_error_signal(int sig);

void
on_lanforge_label_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_video_enabled_button                (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_lanforge_button_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
