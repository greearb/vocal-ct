enum basic { USER_NAME=0, PASS_WORD=1, PROXY_SERVER=2, NUM_BASIC_VALUES=3 };


/* If you change this, change the strings in the .c file too. */
enum advanced { DISPLAY_NAME,
                SIP_TRANSPORT,
                REGISTER_FROM,
                REGISTER_TO,
                REGISTER_EXPIRES,
                DEVICE_NAME,
                MIN_RTP_PORT,
                MAX_RTP_PORT,
                NAT_IP,
                LOG_FILENAME,
                LOG_LEVEL,
                VIDEO,
                LANFORGE,
                WAVE_FILE,
                NUM_ADVANCED_VALUES
              };

/*
void
read_all_config(void);
*/

void
read_basic_config(void);

void
read_advanced_config(void);

void
write_all_config(void);

int
write_basic_config(void);

void
write_advanced_config(void);

void
write_phone_history(GList *history_list, char *dial_history_path);

char *
get_basic_config_value(int key);

char *
get_advanced_config_value(int key);

void
set_basic_config_tooltips(GtkWidget *dialog);

void
set_advanced_config_tooltips(GtkWidget *dialog);

void
init_config(char *config_file_path, char *config_name);

