#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "configuration.h"

#include "../../../build/vocalconfig.h"


extern GtkWidget *main_window;
extern GtkWidget *messages_window;
extern GtkWidget *basic_config_dialog;
extern GtkWidget *advanced_config_dialog;
extern int videoEnabled;
extern int lanforgeEnabled;

/* set to TRUE when the Display Name is edited */
int display_name_set_by_user = FALSE;

#define GUA_HOME ".sipset";
#define STRING_LEN 100

char config_file_name[100];

char  *basic_values[NUM_BASIC_VALUES];
char  *advanced_values[NUM_ADVANCED_VALUES];

char* basic_names[] = {"User_Name", "Proxy_Server", "Pass_Word"};

char *basic_defaults[] = {"","",""};

char *basic_hints[] =
  {
    "Your phone number",
    "IP address of SIP Proxy",
    "Your password",
  };

/* If you change this, change the enum in the .h file as well. */
char* advanced_names[] = {"Display_Name",
                          "SIP_Transport",
                          "Register_From",
                          "Register_To",
                          "Register_Expires",
                          "Device_Name",
                          "Min_RTP_Port",
                          "Max_RTP_Port",
                          "NATAddress_IP",
                          "LogFilename",
                          "LogLevel",
                          "Video",
                          "LANforge",
                          "Wave_File"
                         };

char *advanced_defaults[] = {"",
                             "UDP",
                             "",
                             "",
                             "300",
                             "/dev/dsp",
                             "10000",
                             "10999",
                             "",
                             "",
                             "LOG_ERR",
                             "0",
                             "0",
                             "default.wav"
                         };

char *advanced_hints[] =
  {
    "Name to display instead of User Name",
    "Transport can be either TCP or UDP",
    "IP of machine this user agent is registering from",
    "IP of registrar, if different from SIP Proxy",
    "Time (in seconds) until registration expires",
    "Name of sound card (ie /dev/dsp)",
    "Lowest port number in range of RTP ports",
    "Highest port number in range of RTP ports",
    "If behind NAT/firewall, enter WAN address",
    "Name of file for log messages (ie /home/logfiles)",
    "Use LOG_ERR for normal operation, LOG_DEBUG_STACK for debugging",
    "Use 1 to turn on the Video capability.",
    "Use 1 to turn on the LANforge media device.",
    "Wave file name for LANforge media device."
  };


void
read_basic_config(void)
{
  GtkWidget *widget, *password_entry, *repeat_password_entry;
  int i;

  for (i=0; i<NUM_BASIC_VALUES; i++)
  {
    if ( (basic_values[i] != NULL) && (strlen(basic_values[i]) > 0) )
    {
      widget = lookup_widget(basic_config_dialog, basic_names[i]);
      gtk_entry_set_text(GTK_ENTRY(widget), basic_values[i]);
    }
  }
  /* Make sure that repeat password has password in it */
  password_entry = lookup_widget(basic_config_dialog, "Pass_Word");
  repeat_password_entry = lookup_widget(basic_config_dialog, "repeat_password");
  gtk_entry_set_text(GTK_ENTRY(repeat_password_entry),
       gtk_editable_get_chars(GTK_EDITABLE(password_entry), 0, -1));

}

void
read_advanced_config(void)
{
  GtkWidget *widget;
  int i;

/*  read_all_config();*/

  /* make the Display Name default to User Name if it is blank */
  if( (advanced_values[DISPLAY_NAME] == NULL)
    ||(strlen(advanced_values[DISPLAY_NAME]) == 0) )
  {
    if( (basic_values[USER_NAME] != NULL)
      &&(strlen(basic_values[USER_NAME]) > 0) )
    {
      advanced_values[DISPLAY_NAME] = strdup(basic_values[USER_NAME]);
    }
    display_name_set_by_user = FALSE;
  }

  for (i=0; i<NUM_ADVANCED_VALUES; i++)
  {
    if( (advanced_values[i] != NULL)
      &&(strlen(advanced_values[i]) > 0) )
    {
      widget = lookup_widget(advanced_config_dialog, advanced_names[i]);
      if(strcmp(advanced_names[i], "Video") == 0)
      {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),
                       atoi(advanced_values[i]));
      }
      else if(strcmp(advanced_names[i], "LANforge") == 0)
      {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),
                       atoi(advanced_values[i]));
      }
      else
      {
          gtk_entry_set_text(GTK_ENTRY(widget), advanced_values[i]);
      }
    }
  }

}

void
write_all_config(void)
{
  int i;
  FILE *config_file;
      
  config_file = fopen(config_file_name, "w");
  if (config_file == NULL)
  {
    printf("could not open %s for writing", config_file_name);
    exit(1);
  }

  /* ensure that Display Name does not get written unless it was edited by user */
  if (!display_name_set_by_user)
  {
    advanced_values[DISPLAY_NAME][0] = '\0';
  }

  /* write out config file */
  fprintf(config_file,"# %s\n",
    "Config file for VOCAL Graphical User Agent (SIPset)");

  for (i=0; i<NUM_BASIC_VALUES; i++)
  {
    fprintf(config_file,"\n# %s\n",basic_hints[i]);
    if (basic_values[i][0] == '\0')
    {
      fprintf(config_file,"# %s string <put value here>\n",basic_names[i]);
    }
    else
    {
      fprintf(config_file,"%s string  %s\n",basic_names[i],basic_values[i]);
    }
  }

  for (i=0; i<NUM_ADVANCED_VALUES; i++)
  {
    fprintf(config_file,"\n# %s\n",advanced_hints[i]);
    if (advanced_values[i][0] == '\0')
    {
      fprintf(config_file,"# %s string <put value here>\n",advanced_names[i]);
    }
    else
    {
      fprintf(config_file,"%s string %s\n",advanced_names[i],advanced_values[i]);
    }
  }
  fclose(config_file);
}

int
write_basic_config(void)
{
  GtkWidget *widget, *password_entry, *repeat_password_entry;
  gchar *data, *password_data, *repeat_password_data;
  int i;

  /* Check that the password matches the repeat password */
  password_entry = lookup_widget(basic_config_dialog, "Pass_Word");
  repeat_password_entry = lookup_widget(basic_config_dialog, "repeat_password");
  password_data = gtk_editable_get_chars(GTK_EDITABLE(password_entry), 0, -1);
  repeat_password_data = gtk_editable_get_chars(GTK_EDITABLE(repeat_password_entry), 0, -1);
  if (strcmp(password_data, repeat_password_data) != 0)
  {
    /* pop up error message and clear password fields */
    GtkWidget *error_dialog = create_error_dialog();
    GtkWidget *error_label = lookup_widget (error_dialog, "error_label");
    
    gtk_label_set_text(GTK_LABEL(error_label), "Passwords do not match");
    gtk_window_set_modal(GTK_WINDOW(error_dialog), TRUE);
    gtk_entry_set_text(GTK_ENTRY(password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(repeat_password_entry), "");
    gtk_widget_grab_focus (password_entry);
    gtk_widget_show(error_dialog);
    return FALSE;
  }

  for (i = 0; i<NUM_BASIC_VALUES; i++)
  {
    widget = lookup_widget(basic_config_dialog, basic_names[i]);
    data = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
    basic_values[i] = strdup(data);
  }

  write_all_config();
  return TRUE;
}

void
write_advanced_config(void)
{
  GtkWidget *widget;
  gchar *data;
  int i;

  for (i = 0; i<NUM_ADVANCED_VALUES; i++)
  {
    widget = lookup_widget(advanced_config_dialog, advanced_names[i]);
    if(strcmp(advanced_names[i], "Video") == 0)
    {
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
        {
            data = "1";
            videoEnabled = 1;
            gtk_widget_set_usize (main_window, 380, 350);
        }
        else
        {
            data = "0";
            videoEnabled = 0;
            gtk_widget_set_usize (main_window, 380, 170);
        }
    }
    else if(strcmp(advanced_names[i], "LANforge") == 0)
    {
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
        {
            data = "1";
            lanforgeEnabled = 1;
            //gtk_widget_set_usize (main_window, 380, 350);
        }
        else
        {
            data = "0";
            lanforgeEnabled = 0;
            //gtk_widget_set_usize (main_window, 380, 170);
        }
    }
    else
    {
        data = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
    }
    advanced_values[i] = strdup(data);
  }

  write_all_config();
}

/* this function is used in the foreach loop of write_phone_history */
void write_to_file(gpointer data, gpointer file)
{
  fprintf(file, "%s\n", (char *)data);
}

void write_phone_history(GList *history_list, char *dial_history_path)
{
  FILE *dial_history;

  dial_history = fopen(dial_history_path, "w");

  if (dial_history != NULL)
  {
    g_list_foreach(history_list, write_to_file, (gpointer)dial_history);
    fclose(dial_history);
  }
  else
  {
    printf("could not open %s for writing\n", dial_history_path);
  }
}

/*
 * Get a basic value from config file
 */
char *
get_basic_config_value(int key)
{
  if (basic_values[key] == NULL)
  {
    return "";
  }
  return(basic_values[key]);
}

/*
 * Get an advanced value from config file
 */
char *
get_advanced_config_value(int key)
{
  if (advanced_values[key] == NULL)
  {
    return "";
  }
  return(advanced_values[key]);
}

/*
 * set the bubble help for each basic config field.
 */
void
set_basic_config_tooltips(GtkWidget *basic_config_dialog)
{
  int i;
  for (i=0; i<NUM_BASIC_VALUES; i++)
  {
    GtkWidget *entry = lookup_widget(basic_config_dialog, basic_names[i]);
    GtkWidget *tooltips = lookup_widget(basic_config_dialog, "tooltips");
    gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), entry,
      basic_hints[i], NULL);
  }
}

/*
 * set the bubble help for each advanced config field.
 */
void
set_advanced_config_tooltips(GtkWidget *advanced_config_dialog)
{
  int i;
  for (i=0; i<NUM_ADVANCED_VALUES; i++)
  {
    GtkWidget *entry = lookup_widget(advanced_config_dialog, advanced_names[i]);
    GtkWidget *tooltips = lookup_widget(advanced_config_dialog, "tooltips");
    gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), entry,
      advanced_hints[i], NULL);
  }
}


/*
 * param config_file_path should look like $HOME/.sipset
 *
 * We have a set of defaults predefined in code.
 * These get over-written by the config file in INSTALL_DIR,
 * if that config file exists.
 * We look for a config file in HOME.
 * If that does not exist, we create one using defaults.
 * If config file does exist, we need to add any missing values
 * (in case software has been upgraded).
 */
void
init_config(char *config_file_path, char *config_name)
{
  char install_file_path[100];

  FILE *config_file, *install_config_file;
  char line[100];
  char log_path[50];
  char *key;
  char *value;

  struct stat stbuf;

  int config_file_needs_writing = FALSE;

  /* see if we can get default values from install dir */
  sprintf(install_file_path, "%s/etc/gua.cfg", VOCAL_INSTALL_PATH);
  if( (stat(install_file_path, &stbuf) == 0) && S_ISREG(stbuf.st_mode))
  {
    printf("default configuration from %s\n", install_file_path);
    install_config_file = fopen(install_file_path, "r");
    if (install_config_file != NULL)
    {
      while (fgets(line, 100, install_config_file) != NULL)
      {
        int found = 0;
        int i;
  
        /* skip comments and blank lines*/
        if (line[0] == '#' || isspace(line[0]))
        {
          continue;
        }

        key = strtok(line, " \t");
        /* throw away the datatype specifier */
        strtok(NULL, " \t");
        value = strtok(NULL, " \t\n");
 
        for (i=0; i<NUM_BASIC_VALUES; i++)
        {
          if (strcmp(basic_names[i], key) == 0)
          {
            basic_defaults[i] = strdup(value);
            found = 1;
            break;
          }
        }
        if (!found)
        {
          for (i=0; i<NUM_ADVANCED_VALUES; i++)
          {
            if (strcmp(advanced_names[i], key) == 0)
            {
              advanced_defaults[i] = strdup(value);
              break;
            }
          }
        }
      }
      fclose(install_config_file);
    }
    else
    {
      printf ("Error: cannot open default config %s for reading\n", install_file_path);
    }
  }
  else
  {
    printf("no default configuration at %s\n", install_file_path);
  }

  /* make the default log file be $HOME/.sipset/gua.log */
  sprintf(log_path, "%s/gua.log", config_file_path);
  advanced_defaults[LOG_FILENAME] = strdup(log_path);

  /* see if there is already a user config directory */
  if( (stat(config_file_path, &stbuf) == 0) && S_ISDIR(stbuf.st_mode))
  {
    /* printf("config dir %s exists\n", config_file_path); */
  }
  else
  {
    printf("config dir %s does not exist\n", config_file_path);
    if (mkdir(config_file_path, S_IRWXU) != 0)
    {
      printf ("error: cannot create directory %s\n", config_file_path);
      exit(1);
    }
  }

  /* now set global config_file_name to location of config file */
  sprintf(config_file_name, "%s/%s", config_file_path, config_name);

  if( (stat(config_file_name, &stbuf) == 0) && S_ISREG(stbuf.st_mode))
  {
    int basic_values_check[NUM_BASIC_VALUES];
    int advanced_values_check[NUM_ADVANCED_VALUES];
    int i;

    /* add any missing values */
    config_file = fopen(config_file_name, "r");

    /* initialize the arrays to FALSE */
    for (i=0; i<NUM_BASIC_VALUES; i++)
    {
      basic_values_check[i] = FALSE;
    }
    for (i=0; i<NUM_ADVANCED_VALUES; i++)
    {
      advanced_values_check[i] = FALSE;
    }

    /* read config file and check off every value found */
    while (fgets(line, 100, config_file) != NULL)
    {
      int found = 0;
      int i;

      /* skip comments and blank lines*/
      if (line[0] == '#' || isspace(line[0]))
      {
        continue;
      }
  
      key = strtok(line, " \t");
      /* throw away the datatype specifier */
      strtok(NULL, " \t");
      value = strtok(NULL, " \t\n");
 
      for (i=0; i<NUM_BASIC_VALUES; i++)
      {
        if (strcmp(basic_names[i], key) == 0)
        {
          basic_values[i] = strdup(value);
          basic_values_check[i] = TRUE;
          found = 1;
          break;
        }
      }
      if (!found)
      {
        for (i=0; i<NUM_ADVANCED_VALUES; i++)
        {
          if (strcmp(advanced_names[i], key) == 0)
          {
            if(strcmp(advanced_names[i], "Video") == 0)
            {
               if(value && atoi(value))
                   videoEnabled = 1;
            }
            else if(strcmp(advanced_names[i], "LANforge") == 0)
            {
               if(value && atoi(value))
                   lanforgeEnabled = 1;
            }
            if (value)
               advanced_values[i] = strdup(value);
            else
               advanced_values[i] = strdup("");
            advanced_values_check[i] = TRUE;
            break;
          }
        }
      }
    }
    /* Now look for any missing values */
    for (i=0; i< NUM_BASIC_VALUES; i++)
    {
      if (!basic_values_check[i])
      {
        basic_values[i] = strdup(basic_defaults[i]);
        config_file_needs_writing = TRUE;
      }
    }
    for (i=0; i< NUM_ADVANCED_VALUES; i++)
    {
      if (!advanced_values_check[i])
      {
        advanced_values[i] = strdup(advanced_defaults[i]);
        config_file_needs_writing = TRUE;
      }
    }
    fclose(config_file);

    /* If Display Name was in file, then leave that field as is */
    if (strlen(get_advanced_config_value(DISPLAY_NAME)) > 0)
    {
      display_name_set_by_user = TRUE;
    }

    /* If any values were missing, write the config file */
    if (config_file_needs_writing)
    {
      write_all_config();
    }
  }
  else
  {
    /* if config file does not exist, create it with defaults */
    int i;
    printf ("creating config file in home directory %s\n", config_file_name);
    if (creat(config_file_name, 0666) != -1)
    {
      config_file = fopen(config_file_name, "w");
      if (config_file == NULL)
      {
        printf("could not open %s for writing", config_file_name);
        exit(1);
      }

      /* write out config file */
      fprintf(config_file,"# %s\n",
        "Config file for VOCAL Graphical User Agent (SIPset)");

      /* copy defaults to values and print non-blanks to file */
      for (i=0; i<NUM_BASIC_VALUES; i++)
      {
        basic_values[i] = strdup(basic_defaults[i]);
        fprintf(config_file,"\n# %s\n",basic_hints[i]);
        if (basic_defaults[i][0] == '\0')
        {
          fprintf(config_file,"# %s string <put value here>\n",basic_names[i]);
        }
        else
        {
          fprintf(config_file,"%s string  %s\n",basic_names[i],basic_defaults[i]);
        }
      }

      /* copy defaults to values and print non-blanks to file */
      for (i=0; i<NUM_ADVANCED_VALUES; i++)
      {
        advanced_values[i] = strdup(advanced_defaults[i]);
        fprintf(config_file,"\n# %s\n",advanced_hints[i]);
        if (advanced_defaults[i][0] == '\0')
        {
          fprintf(config_file,"# %s string <put value here>\n",advanced_names[i]);
        }
        else
        {
          fprintf(config_file,"%s string %s\n",advanced_names[i],advanced_defaults[i]);
        }
      }
      fclose(config_file);
    }
    else
    {
      printf("could not create config file %s", config_file_name);
      exit(1);
    }
  }
  
}
