#ifdef USE_MPEGLIB
extern void VideoDevice_preview_init(GtkWidget* main_window, GtkWidget* winManager);
extern void VideoDevice_shutdown();
extern void Player_start();
extern void Player_stop();
extern void Video_start();
extern void Video_stop();
#else
#define  VideDevice_preview_init(main_window, winManager) 
#define  VideoDevice_shutdown()
#endif
