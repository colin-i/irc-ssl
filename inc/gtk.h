
typedef void* gpointer;
typedef unsigned long gulong;
typedef char gchar;
typedef unsigned int guint;
typedef int gint;
typedef gint gboolean;
typedef void GApplication;
typedef void GThread;
typedef void GtkAdjustment;
typedef void GtkApplication;
typedef void GtkBin;
typedef void GtkBox;
typedef void GtkContainer;
typedef void GtkEntry;
typedef void GtkScrolledWindow;
typedef void GtkTextBuffer;
typedef void GtkTextTagTable;
typedef void GtkTextView;
typedef void GtkWidget;
typedef void GtkWindow;
#define G_CALLBACK(f) ((GCallback)(f))
typedef void (*GCallback)(void);
typedef void *GClosureNotify;// (gpointer data);
typedef gpointer (*GThreadFunc)(gpointer data);
typedef enum{G_CONNECT_NONE}GConnectFlags;
typedef enum{G_APPLICATION_FLAGS_NONE} GApplicationFlags;
typedef enum{  GTK_WRAP_NONE,  GTK_WRAP_CHAR,  GTK_WRAP_WORD,  GTK_WRAP_WORD_CHAR
} GtkWrapMode;
typedef enum{  GTK_POLICY_ALWAYS,  GTK_POLICY_AUTOMATIC,  GTK_POLICY_NEVER,  GTK_POLICY_EXTERNAL
} GtkPolicyType;
typedef enum{GTK_ORIENTATION_HORIZONTAL,GTK_ORIENTATION_VERTICAL} GtkOrientation;
typedef enum{  G_OPTION_FLAG_NONE = 0,  G_OPTION_FLAG_HIDDEN = 1 << 0,  G_OPTION_FLAG_IN_MAIN = 1 << 1,  G_OPTION_FLAG_REVERSE = 1 << 2,  G_OPTION_FLAG_NO_ARG = 1 << 3,  G_OPTION_FLAG_FILENAME = 1 << 4,  G_OPTION_FLAG_OPTIONAL_ARG = 1 << 5,  G_OPTION_FLAG_NOALIAS = 1 << 6}
 GOptionFlags;
typedef enum{  G_OPTION_ARG_NONE,  G_OPTION_ARG_STRING,  G_OPTION_ARG_INT,  G_OPTION_ARG_CALLBACK,  G_OPTION_ARG_FILENAME,  G_OPTION_ARG_STRING_ARRAY,  G_OPTION_ARG_FILENAME_ARRAY,  G_OPTION_ARG_DOUBLE,  G_OPTION_ARG_INT64}
 GOptionArg;
typedef struct _GtkTextIter {
  gpointer dummy1;
  gpointer dummy2;
  gint dummy3;
  gint dummy4;
  gint dummy5;
  gint dummy6;
  gint dummy7;
  gint dummy8;
  gpointer dummy9;
  gpointer dummy10;
  gint dummy11;
  gint dummy12;
  gint dummy13;
  gpointer dummy14;
}GtkTextIter;

#ifdef __cplusplus
extern "C" {
#endif

void g_application_add_main_option (GApplication *application, const char *long_name, char short_name, GOptionFlags flags, GOptionArg arg, const char *description, const char *arg_description);
int g_application_run (GApplication *application,int argc,char **argv);
void g_object_unref (gpointer object);
gulong g_signal_connect_data (gpointer instance,const gchar *detailed_signal,GCallback c_handler,gpointer data,GClosureNotify destroy_data,GConnectFlags connect_flags);
GThread * g_thread_new (const gchar *name, GThreadFunc func, gpointer data);
void g_thread_unref (GThread *thread);
GtkApplication * gtk_application_new (const gchar *application_id, GApplicationFlags flags);
GtkWidget * gtk_application_window_new (GtkApplication *application);
GtkWidget *gtk_bin_get_child (GtkBin *bin);
GtkWidget* gtk_box_new (GtkOrientation orientation,gint spacing);
void gtk_box_pack_start (GtkBox *box,GtkWidget *child,gboolean expand,gboolean fill,guint padding);
GtkWidget* gtk_combo_box_text_new_with_entry (void);
void gtk_container_add (GtkContainer *container,GtkWidget *widget);
void gtk_container_set_border_width (GtkContainer *container,guint border_width);
const gchar *gtk_entry_get_text (GtkEntry *entry);
void gtk_entry_set_text (GtkEntry *entry, const gchar *text);
void gtk_text_buffer_get_end_iter (GtkTextBuffer *buffer, GtkTextIter *iter);
void gtk_text_buffer_insert (GtkTextBuffer *buffer,GtkTextIter *iter,const gchar *text,gint len);
GtkTextBuffer *gtk_text_buffer_new (GtkTextTagTable *table);
GtkWidget * gtk_text_view_new_with_buffer (GtkTextBuffer *buffer);
void gtk_text_view_set_wrap_mode (GtkTextView *text_view, GtkWrapMode wrap_mode);
void gtk_text_view_set_editable (GtkTextView *text_view,gboolean setting);
void gtk_widget_show_all (GtkWidget *widget);
void gtk_window_set_default_size (GtkWindow *window, gint width, gint height);
void gtk_window_set_title (GtkWindow *window,const gchar *title);
void gtk_scrolled_window_set_policy (GtkScrolledWindow *scrolled_window,GtkPolicyType hscrollbar_policy,GtkPolicyType vscrollbar_policy);
GtkWidget* gtk_scrolled_window_new (GtkAdjustment *hadjustment,GtkAdjustment *vadjustment);

#ifdef __cplusplus
}
#endif
