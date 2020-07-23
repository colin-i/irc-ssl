
typedef void* gpointer;
typedef unsigned long gulong;
typedef char gchar;
typedef unsigned int guint;
typedef int gint;
typedef gint gboolean;
typedef unsigned int gsize;
typedef double gdouble;
typedef gsize GType;
typedef void GApplication;
typedef void GVariant;
typedef void GVariantDict;
typedef void GVariantType;
typedef void GdkEvent;
typedef void GtkAdjustment;
typedef void GtkApplication;
typedef void GtkBin;
typedef void GtkBox;
typedef void GtkButton;
typedef void GtkCellRenderer;
typedef void GtkComboBox;
typedef void GtkComboBoxText;
typedef void GtkContainer;
typedef void GtkDialog;
typedef void GtkEntry;
typedef void GtkEntryBuffer;
typedef void GtkListStore;
typedef void GtkMenu;
typedef void GtkMenuShell;
typedef void GtkPaned;
typedef void GtkScrolledWindow;
typedef void GtkTextBuffer;
typedef void GtkTextMark;
typedef void GtkTextTagTable;
typedef void GtkTextView;
typedef void GtkTreeModel;
typedef void GtkTreePath;
typedef void GtkTreeSelection;
typedef void GtkTreeView;
typedef void GtkTreeViewColumn;
typedef void GtkWidget;
typedef void GtkWindow;
typedef void (*GCallback)(void);
typedef void *GClosureNotify;
typedef gboolean(*GSourceFunc)(gpointer user_data);
#define G_CALLBACK(f) ((GCallback)(f))
#define G_TYPE_FUNDAMENTAL_SHIFT (2)
#define G_TYPE_MAKE_FUNDAMENTAL(x) ((GType) ((x) << G_TYPE_FUNDAMENTAL_SHIFT))
#define G_TYPE_STRING G_TYPE_MAKE_FUNDAMENTAL (16)
#define G_VARIANT_TYPE_STRING ((const GVariantType *) "s")
typedef enum{G_APPLICATION_FLAGS_NONE}
 GApplicationFlags;
typedef enum{G_CONNECT_SWAPPED = 1 << 1}
GConnectFlags;//_BEFORE G_CONNECT_AFTER = 1 << 0,
typedef enum{  G_OPTION_ARG_NONE,  G_OPTION_ARG_STRING}//,  G_OPTION_ARG_INT,  G_OPTION_ARG_CALLBACK,  G_OPTION_ARG_FILENAME,  G_OPTION_ARG_STRING_ARRAY,  G_OPTION_ARG_FILENAME_ARRAY,  G_OPTION_ARG_DOUBLE,  G_OPTION_ARG_INT64
 GOptionArg;
typedef enum{G_OPTION_FLAG_IN_MAIN = 1 << 1}//  G_OPTION_FLAG_NONE = 0,  G_OPTION_FLAG_HIDDEN = 1 << 0,
 GOptionFlags;//,  G_OPTION_FLAG_REVERSE = 1 << 2,  G_OPTION_FLAG_NO_ARG = 1 << 3,  G_OPTION_FLAG_FILENAME = 1 << 4,  G_OPTION_FLAG_OPTIONAL_ARG = 1 << 5,  G_OPTION_FLAG_NOALIAS = 1 << 6
typedef enum{GTK_ORIENTATION_HORIZONTAL,GTK_ORIENTATION_VERTICAL} GtkOrientation;
typedef enum{  GTK_DIALOG_MODAL = 1 << 0, GTK_DIALOG_DESTROY_WITH_PARENT = 1 << 1}//, GTK_DIALOG_USE_HEADER_BAR = 1 << 2
 GtkDialogFlags;
typedef enum{  GTK_POLICY_ALWAYS,  GTK_POLICY_AUTOMATIC}//,  GTK_POLICY_NEVER,  GTK_POLICY_EXTERNAL
 GtkPolicyType;
typedef enum{  GTK_RESPONSE_NONE = -1
} GtkResponseType;
typedef enum{  GTK_WRAP_NONE,  GTK_WRAP_CHAR,  GTK_WRAP_WORD}//,  GTK_WRAP_WORD_CHAR
 GtkWrapMode;

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
typedef struct _GtkTreeIter
{
  gint stamp;
  gpointer user_data;
  gpointer user_data2;
  gpointer user_data3;
}GtkTreeIter;

#ifdef __cplusplus
extern "C" {
#endif

void g_application_add_main_option (GApplication *application, const char *long_name, char short_name, GOptionFlags flags, GOptionArg arg, const char *description, const char *arg_description);
int g_application_run (GApplication *application,int argc,char **argv);
void g_free (gpointer mem);
guint g_idle_add (GSourceFunc function,gpointer data);
void g_object_unref (gpointer object);
gulong g_signal_connect_data (gpointer instance,const gchar *detailed_signal,GCallback c_handler,gpointer data,GClosureNotify destroy_data,GConnectFlags connect_flags);
void g_signal_handler_block (gpointer instance, gulong handler_id);
void g_signal_handler_unblock (gpointer instance, gulong handler_id);
guint g_timeout_add (guint interval, GSourceFunc function, gpointer data);
gboolean g_variant_dict_lookup (GVariantDict *dict, const gchar *key, const gchar *format_string, ...);
GVariant * g_variant_dict_lookup_value (GVariantDict *dict, const gchar *key, const GVariantType *expected_type);
gchar * g_variant_dup_string (GVariant *value, gsize *length);
GtkApplication * gtk_application_new (const gchar *application_id, GApplicationFlags flags);
GtkWidget * gtk_application_window_new (GtkApplication *application);
GtkWidget *gtk_bin_get_child (GtkBin *bin);
GtkWidget* gtk_box_new (GtkOrientation orientation,gint spacing);
void gtk_box_pack_start (GtkBox *box,GtkWidget *child,gboolean expand,gboolean fill,guint padding);
GtkWidget* gtk_button_new_with_label (const gchar *label);
GtkCellRenderer *gtk_cell_renderer_text_new (void);
GtkTreeModel*gtk_combo_box_get_model (GtkComboBox *combo_box);
void gtk_combo_box_set_active (GtkComboBox *combo_box, gint index_);
void gtk_combo_box_text_append_text (GtkComboBoxText *combo_box, const gchar *text);
GType gtk_combo_box_text_get_type (void) __attribute__((__const__));
GtkWidget* gtk_combo_box_text_new_with_entry (void);
void gtk_combo_box_text_remove (GtkComboBoxText *combo_box, gint position);
void gtk_container_add (GtkContainer *container,GtkWidget *widget);
void gtk_container_set_border_width (GtkContainer *container,guint border_width);
GtkWidget * gtk_dialog_get_content_area (GtkDialog *dialog);
GtkWidget* gtk_dialog_new_with_buttons (const gchar *title,  GtkWindow *parent, GtkDialogFlags flags, const gchar *first_button_text, ...) __attribute__((__sentinel__));
guint gtk_entry_buffer_delete_text (GtkEntryBuffer *buffer, guint position, gint n_chars);
const gchar* gtk_entry_buffer_get_text (GtkEntryBuffer *buffer);
GtkEntryBuffer *gtk_entry_get_buffer (GtkEntry *entry);
const gchar *gtk_entry_get_text (GtkEntry *entry);
GtkWidget* gtk_entry_new (void);
void gtk_entry_set_max_length (GtkEntry *entry, gint max);
void gtk_entry_set_text (GtkEntry *entry, const gchar *text);
void gtk_list_store_append (GtkListStore *list_store, GtkTreeIter *iter);
void gtk_list_store_insert_before (GtkListStore *list_store, GtkTreeIter *iter, GtkTreeIter *sibling);
void gtk_list_store_move_before (GtkListStore *store,GtkTreeIter *iter,GtkTreeIter *position);
GtkListStore *gtk_list_store_new (gint n_columns, ...);
gboolean gtk_list_store_remove (GtkListStore *list_store, GtkTreeIter *iter);
void gtk_list_store_set (GtkListStore *list_store, GtkTreeIter *iter, ...);
void gtk_list_store_swap (GtkListStore *store, GtkTreeIter *a, GtkTreeIter *b);
GtkWidget* gtk_menu_item_new_with_label (const gchar *label);
GtkWidget* gtk_menu_new (void);
void gtk_menu_popup_at_pointer (GtkMenu *menu, const GdkEvent *trigger_event);
void gtk_menu_shell_append (GtkMenuShell *menu_shell, GtkWidget *child);
GtkWidget * gtk_paned_new (GtkOrientation orientation);
void gtk_paned_pack1 (GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink);
void gtk_paned_pack2 (GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink);
void gtk_scrolled_window_set_policy (GtkScrolledWindow *scrolled_window,GtkPolicyType hscrollbar_policy,GtkPolicyType vscrollbar_policy);
GtkWidget* gtk_scrolled_window_new (GtkAdjustment *hadjustment,GtkAdjustment *vadjustment);
GtkTextMark *gtk_text_buffer_create_mark (GtkTextBuffer *buffer,const gchar *mark_name,const GtkTextIter *where,gboolean left_gravity);
void gtk_text_buffer_get_end_iter (GtkTextBuffer *buffer, GtkTextIter *iter);
void gtk_text_buffer_insert (GtkTextBuffer *buffer,GtkTextIter *iter,const gchar *text,gint len);
void gtk_text_buffer_set_text (GtkTextBuffer *buffer, const gchar *text, gint len);
GtkTextBuffer* gtk_text_view_get_buffer(GtkTextView *);
GtkWidget * gtk_text_view_new (void);
void gtk_text_view_scroll_to_mark (GtkTextView *text_view,GtkTextMark *mark,gdouble within_margin,gboolean use_align,gdouble xalign,gdouble yalign);
void gtk_text_view_set_wrap_mode (GtkTextView *text_view, GtkWrapMode wrap_mode);
void gtk_text_view_set_editable (GtkTextView *text_view,gboolean setting);
void gtk_tree_model_get (GtkTreeModel *tree_model, GtkTreeIter *iter, ...);
gboolean gtk_tree_model_get_iter_first(GtkTreeModel *tree_model, GtkTreeIter *iter);
GtkTreePath *gtk_tree_model_get_path (GtkTreeModel *tree_model,GtkTreeIter *iter);
gint gtk_tree_model_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean gtk_tree_model_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean gtk_tree_model_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
gboolean gtk_tree_model_iter_previous (GtkTreeModel *tree_model, GtkTreeIter *iter);
gint *gtk_tree_path_get_indices (GtkTreePath *path);
void gtk_tree_path_free (GtkTreePath *path);
gboolean gtk_tree_selection_get_selected (GtkTreeSelection *selection,GtkTreeModel **model,GtkTreeIter *iter);
gint gtk_tree_view_append_column (GtkTreeView *tree_view, GtkTreeViewColumn *column);
GtkTreeViewColumn *gtk_tree_view_column_new_with_attributes (const gchar *title, GtkCellRenderer *cell, ...) __attribute__((__sentinel__));
GtkTreeSelection *gtk_tree_view_get_selection (GtkTreeView *tree_view);
GtkWidget *gtk_tree_view_new (void);
void gtk_tree_view_set_headers_visible (GtkTreeView *tree_view, gboolean headers_visible);
void gtk_tree_view_set_model (GtkTreeView *tree_view, GtkTreeModel *model);
void gtk_widget_destroy (GtkWidget *widget);
GtkWidget* gtk_widget_get_ancestor (GtkWidget *widget, GType widget_type);
GtkWidget* gtk_widget_get_toplevel (GtkWidget *widget);
void gtk_widget_set_size_request (GtkWidget *widget,gint width,gint height);
void gtk_widget_show (GtkWidget *widget);
void gtk_widget_show_all (GtkWidget *widget);
void gtk_window_get_size (GtkWindow *window,gint *width,gint *height);
void gtk_window_set_default_size (GtkWindow *window, gint width, gint height);
void gtk_window_set_title (GtkWindow *window,const gchar *title);

#ifdef __cplusplus
}
#endif
