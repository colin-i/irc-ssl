
typedef char gchar;
typedef signed char gint8;
typedef unsigned char guchar;
typedef unsigned char guint8;
typedef int gint;
typedef gint gboolean;
typedef unsigned short guint16;
typedef unsigned int guint;
typedef unsigned int guint32;
typedef unsigned int gsize;
typedef guint32 GQuark;
typedef gsize GType;
typedef unsigned long gulong;
//__extension__  `-pedantic' and other options cause warnings for many GNU C extensions.
//but still got warning
typedef signed long long gint64;
typedef double gdouble;
typedef void GApplication;
typedef void GDateTime;
typedef void GError;
typedef void GVariant;
typedef void GVariantDict;
typedef void GVariantType;
typedef void GdkEvent;
typedef void GdkPixbuf;
typedef void GdkWindow;
typedef void GtkAdjustment;
typedef void GtkApplication;
typedef void GtkBin;
typedef void GtkBox;
typedef void GtkButton;
typedef void GtkCellRenderer;
typedef void GtkCheckMenuItem;
typedef void GtkClipboard;
typedef void GtkComboBox;
typedef void GtkComboBoxText;
typedef void GtkContainer;
typedef void GtkDialog;
typedef void GtkEntry;
typedef void GtkEntryBuffer;
typedef void GtkLabel;
typedef void GtkListStore;
typedef void GtkMenu;
typedef void GtkMenuItem;
typedef void GtkMenuShell;
typedef void GtkNotebook;
typedef void GtkPaned;
typedef void GtkRadioMenuItem;
typedef void GtkScrolledWindow;
typedef void GtkTextBuffer;
typedef void GtkTextMark;
typedef void GtkTextTagTable;
typedef void GtkTextView;
typedef void GtkTooltip;
typedef void GtkTreeModel;
typedef void GtkTreePath;
typedef void GtkTreeSelection;
typedef void GtkTreeView;
typedef void GtkTreeViewColumn;
typedef void GtkWidget;
typedef void GtkWindow;
typedef void* gpointer;
typedef const void *gconstpointer;
typedef void (*GCallback)(void);
typedef void *GClosureNotify;
typedef void (*GDestroyNotify)(gpointer data);
typedef void *GdkPixbufDestroyNotify;
typedef gboolean(*GSourceFunc)(gpointer user_data);
typedef enum{G_APPLICATION_FLAGS_NONE}
 GApplicationFlags;
typedef enum{G_CONNECT_SWAPPED = 1 << 1}
GConnectFlags;//_BEFORE G_CONNECT_AFTER = 1 << 0,
typedef enum{  G_OPTION_ARG_NONE,  G_OPTION_ARG_STRING,  G_OPTION_ARG_INT,  G_OPTION_ARG_CALLBACK,  G_OPTION_ARG_FILENAME}//,  G_OPTION_ARG_STRING_ARRAY,  G_OPTION_ARG_FILENAME_ARRAY,  G_OPTION_ARG_DOUBLE,  G_OPTION_ARG_INT64
 GOptionArg;
typedef enum{G_OPTION_FLAG_IN_MAIN = 1 << 1,G_OPTION_FLAG_OPTIONAL_ARG = 1 << 5}
 GOptionFlags;
typedef enum{
  G_SIGNAL_MATCH_ID = 1 << 0}
 GSignalMatchType;
typedef enum {GDK_COLORSPACE_RGB}
 GdkColorspace;
typedef enum{GDK_KEY_PRESS = 8}
 GdkEventType;
typedef enum{  GDK_GRAVITY_NORTH_WEST = 1}
 GdkGravity;
typedef enum{GTK_ORIENTATION_HORIZONTAL,GTK_ORIENTATION_VERTICAL}
 GtkOrientation;
typedef enum{  GTK_DIALOG_MODAL = 1 << 0, GTK_DIALOG_DESTROY_WITH_PARENT = 1 << 1}//, GTK_DIALOG_USE_HEADER_BAR = 1 << 2
 GtkDialogFlags;
typedef enum{  GTK_ICON_SIZE_INVALID,  GTK_ICON_SIZE_MENU}
 GtkIconSize;
typedef enum{GDK_SHIFT_MASK = 1 << 0, GDK_LOCK_MASK = 1 << 1, GDK_CONTROL_MASK = 1 << 2}
 GdkModifierType;
typedef enum{  GTK_PACK_START,  GTK_PACK_END}
 GtkPackType;
typedef enum{  GTK_POLICY_ALWAYS,  GTK_POLICY_AUTOMATIC,  GTK_POLICY_NEVER,  GTK_POLICY_EXTERNAL}
 GtkPolicyType;
typedef enum{  GTK_RELIEF_NORMAL,  GTK_RELIEF_HALF,  GTK_RELIEF_NONE}
 GtkReliefStyle;
typedef enum{  GTK_RESPONSE_NONE = -1,GTK_RESPONSE_OK = -5}//, GTK_RESPONSE_REJECT = -2,GTK_RESPONSE_ACCEPT = -3,GTK_RESPONSE_DELETE_EVENT = -4
 GtkResponseType;
typedef enum{  GTK_WRAP_NONE,  GTK_WRAP_CHAR,  GTK_WRAP_WORD,  GTK_WRAP_WORD_CHAR}
 GtkWrapMode;
typedef struct _GClosure GClosure;
typedef struct _GList GList;
struct _GList
{
  gpointer data;
  GList *next;
  GList *prev;
};
typedef struct{
  const gchar *long_name;
  gchar        short_name;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
  gint         flags;
#pragma GCC diagnostic pop
  GOptionArg   arg;
  gpointer arg_data;//GOptionArgFunc
  const gchar *description;
  const gchar *arg_description;
} GOptionEntry;
typedef struct _GQueue GQueue;
struct _GQueue
{
  GList *head;
  GList *tail;
  guint length;
};
typedef struct _GSList GSList;
struct _GSList
{
  gpointer data;
  GSList *next;
};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
typedef struct GdkEventKey {
  GdkEventType type;
  GdkWindow *window;
  gint8 send_event;
  guint32 time;
  guint state;
  guint keyval;
  gint length;
  gchar *string;
  guint16 hardware_keycode;
  guint8 group;
  guint is_modifier : 1;
} GdkEventKey;
#pragma GCC diagnostic pop
typedef struct {
    int x, y;
    int width, height;
} GdkRectangle;
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
typedef struct{
  GType g_type;
} GTypeClass;
typedef struct{
  GTypeClass *g_class;
} GTypeInstance;
typedef struct _GdkAtom *GdkAtom;

#define G_CALLBACK(f) ((GCallback)(f))
#define G_TYPE_FROM_CLASS(g_class) (((GTypeClass*) (g_class))->g_type)
#define G_TYPE_FROM_INSTANCE(instance) (G_TYPE_FROM_CLASS (((GTypeInstance*) (instance))->g_class))
#define G_TYPE_FUNDAMENTAL_SHIFT (2)
#define G_TYPE_MAKE_FUNDAMENTAL(x) ((GType) ((x) << G_TYPE_FUNDAMENTAL_SHIFT))
#define G_TYPE_STRING G_TYPE_MAKE_FUNDAMENTAL (16)
#define G_VARIANT_TYPE_STRING ((const GVariantType *) "s")
#define GUINT_TO_POINTER(u) ((gpointer) (guint) (u))
#define GDK_SELECTION_CLIPBOARD ((GdkAtom)GUINT_TO_POINTER(69))
#define GDK_KEY_C 0x043
#define GDK_KEY_Q 0x051
#define GDK_KEY_T 0x054
#define GDK_KEY_X 0x058
#define GDK_KEY_Up 0xff52
#define GDK_KEY_Down 0xff54

#ifdef __cplusplus
extern "C" {
#endif

void g_application_add_main_option (GApplication *application, const char *long_name, char short_name, GOptionFlags flags, GOptionArg arg, const char *description, const char *arg_description);
void g_application_add_main_option_entries (GApplication *application,const GOptionEntry *entries);
void g_application_quit (GApplication *application);
int g_application_run (GApplication *application,int argc,char **argv);
gint g_date_time_get_hour (GDateTime *datetime);
gint g_date_time_get_minute (GDateTime *datetime);
gint g_date_time_get_second (GDateTime *datetime);
GDateTime * g_date_time_new_now_local (void);
gint64 g_date_time_to_unix (GDateTime *datetime);
void g_date_time_unref (GDateTime *datetime);
void g_free (gpointer mem);
guint g_idle_add (GSourceFunc function,gpointer data);
void g_list_free (GList *list);
GList* g_list_last (GList *list);
#define g_list_next(list) ((list) ? (((GList *)(list))->next) : nullptr)
#ifdef FN_G_MEMDUP2
	gpointer g_memdup2 (gconstpointer mem, gsize byte_size);
#else
	gpointer g_memdup (gconstpointer mem, guint byte_size);
#endif
gpointer g_object_ref (gpointer object);
void g_object_set (gpointer object, const gchar *first_property_name, ...) __attribute__((__sentinel__));
void g_object_unref (gpointer object);
void g_queue_free_full (GQueue *queue,GDestroyNotify free_func);
GQueue *g_queue_new (void);
gpointer g_queue_pop_head (GQueue *queue);
void g_queue_push_tail (GQueue *queue,gpointer data);
gulong g_signal_connect_data (gpointer instance,const gchar *detailed_signal,GCallback c_handler,gpointer data,GClosureNotify destroy_data,GConnectFlags connect_flags);
void g_signal_handler_block (gpointer instance, gulong handler_id);
gulong g_signal_handler_find (gpointer instance,GSignalMatchType mask,guint signal_id,GQuark detail,GClosure *closure,gpointer func,gpointer data);
void g_signal_handler_unblock (gpointer instance, gulong handler_id);
guint g_signal_lookup (const gchar *name, GType itype);
gboolean g_source_remove (guint tag);
gboolean g_spawn_command_line_async (const gchar *command_line, GError **error);
gchar * g_strdup ( const gchar *str );
guint g_timeout_add (guint interval, GSourceFunc function, gpointer data);
char *g_uri_unescape_string (const char *escaped_string, const char *illegal_characters);
gboolean g_variant_dict_contains (GVariantDict *dict, const gchar *key);
gboolean g_variant_dict_lookup (GVariantDict *dict, const gchar *key, const gchar *format_string, ...);
GVariant * g_variant_dict_lookup_value (GVariantDict *dict, const gchar *key, const GVariantType *expected_type);
const gchar * g_variant_get_string (GVariant *value, gsize *length);
guint gdk_keyval_to_upper (guint keyval) __attribute__((__const__));
GdkPixbuf *gdk_pixbuf_new_from_data (const guchar *data,GdkColorspace colorspace,gboolean has_alpha,int bits_per_sample,int width, int height,int rowstride,GdkPixbufDestroyNotify destroy_fn,gpointer destroy_fn_data);
void gtk_adjustment_set_value (GtkAdjustment *adjustment, gdouble value);
GtkApplication * gtk_application_new (const gchar *application_id, GApplicationFlags flags);
GtkWidget * gtk_application_window_new (GtkApplication *application);
GtkWidget *gtk_bin_get_child (GtkBin *bin);
GType gtk_box_get_type (void) __attribute__((__const__));
GtkWidget* gtk_box_new (GtkOrientation orientation,gint spacing);
void gtk_box_pack_end (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
void gtk_box_pack_start (GtkBox *box,GtkWidget *child,gboolean expand,gboolean fill,guint padding);
void gtk_button_clicked (GtkButton *button);
GType gtk_button_get_type (void) __attribute__((__const__));
GtkWidget* gtk_button_new (void);
GtkWidget* gtk_button_new_with_label (const gchar *label);
void gtk_button_set_image (GtkButton *button, GtkWidget *image);
void gtk_button_set_relief (GtkButton *button, GtkReliefStyle relief);
GtkCellRenderer *gtk_cell_renderer_text_new (void);
gboolean gtk_check_menu_item_get_active (GtkCheckMenuItem *check_menu_item);
GtkWidget* gtk_check_menu_item_new_with_label (const gchar *label);
void gtk_check_menu_item_set_active (GtkCheckMenuItem *check_menu_item, gboolean is_active);
GtkClipboard *gtk_clipboard_get (GdkAtom selection);
void gtk_clipboard_set_text (GtkClipboard *clipboard,const gchar *text,gint len);
gint gtk_combo_box_get_active (GtkComboBox *combo_box);
GtkTreeModel*gtk_combo_box_get_model (GtkComboBox *combo_box);
void gtk_combo_box_set_active (GtkComboBox *combo_box, gint index_);
void gtk_combo_box_text_append_text (GtkComboBoxText *combo_box, const gchar *text);
GtkWidget* gtk_combo_box_text_new_with_entry (void);
void gtk_combo_box_text_remove (GtkComboBoxText *combo_box, gint position);
void gtk_container_add (GtkContainer *container, GtkWidget *widget);
GList* gtk_container_get_children (GtkContainer *container);
void gtk_container_remove (GtkContainer *container, GtkWidget *widget);
void gtk_container_set_border_width (GtkContainer *container,guint border_width);
GtkWidget * gtk_dialog_get_content_area (GtkDialog *dialog);
GtkWidget* gtk_dialog_new_with_buttons (const gchar *title,  GtkWindow *parent, GtkDialogFlags flags, const gchar *first_button_text, ...) __attribute__((__sentinel__));
guint gtk_entry_buffer_delete_text (GtkEntryBuffer *buffer, guint position, gint n_chars);
const gchar* gtk_entry_buffer_get_text (GtkEntryBuffer *buffer);
guint gtk_entry_buffer_insert_text (GtkEntryBuffer *buffer,guint position,const gchar *chars,gint n_chars);
GtkEntryBuffer *gtk_entry_get_buffer (GtkEntry *entry);
const gchar *gtk_entry_get_text (GtkEntry *entry);
GtkWidget* gtk_entry_new (void);
void gtk_entry_set_placeholder_text (GtkEntry *entry, const gchar *text);
void gtk_entry_set_text (GtkEntry *entry, const gchar *text);
GType gtk_image_get_type (void) __attribute__((__const__));
GtkWidget* gtk_image_new_from_icon_name (const gchar *icon_name, GtkIconSize size);
GtkWidget *gtk_label_get_mnemonic_widget (GtkLabel *label);
void gtk_label_set_mnemonic_widget (GtkLabel *label, GtkWidget *widget);
const gchar* gtk_label_get_text (GtkLabel *label);
GtkWidget* gtk_label_new (const gchar *str);
void gtk_list_store_append (GtkListStore *list_store, GtkTreeIter *iter);
void gtk_list_store_clear (GtkListStore *list_store);
void gtk_list_store_insert_after (GtkListStore *list_store, GtkTreeIter *iter, GtkTreeIter *sibling);
void gtk_list_store_insert_before (GtkListStore *list_store, GtkTreeIter *iter, GtkTreeIter *sibling);
void gtk_list_store_move_after (GtkListStore *store, GtkTreeIter *iter, GtkTreeIter *position);
void gtk_list_store_move_before (GtkListStore *store, GtkTreeIter *iter, GtkTreeIter *position);
GtkListStore *gtk_list_store_new (gint n_columns, ...);
void gtk_list_store_prepend (GtkListStore *list_store, GtkTreeIter *iter);
gboolean gtk_list_store_remove (GtkListStore *list_store, GtkTreeIter *iter);
void gtk_list_store_set (GtkListStore *list_store, GtkTreeIter *iter, ...);
void gtk_list_store_swap (GtkListStore *store, GtkTreeIter *a, GtkTreeIter *b);
const gchar *gtk_menu_item_get_label (GtkMenuItem *menu_item);
GtkWidget* gtk_menu_item_new_with_label (const gchar *label);
void gtk_menu_item_set_submenu (GtkMenuItem *menu_item, GtkWidget *submenu);
GtkWidget* gtk_menu_new (void);
void gtk_menu_popup_at_pointer (GtkMenu *menu, const GdkEvent *trigger_event);
void gtk_menu_popup_at_widget (GtkMenu *menu, GtkWidget *widget, GdkGravity widget_anchor, GdkGravity menu_anchor, const GdkEvent *trigger_event);
void gtk_menu_shell_append (GtkMenuShell *menu_shell, GtkWidget *child);
gint gtk_notebook_append_page_menu (GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label, GtkWidget *menu_label);
GtkWidget* gtk_notebook_get_action_widget (GtkNotebook *notebook, GtkPackType pack_type);
gint gtk_notebook_get_current_page (GtkNotebook *notebook);
const gchar *gtk_notebook_get_menu_label_text (GtkNotebook *notebook, GtkWidget *child);
GtkWidget* gtk_notebook_get_nth_page (GtkNotebook *notebook, gint page_num);
GtkWidget * gtk_notebook_get_tab_label (GtkNotebook *notebook, GtkWidget *child);
GType gtk_notebook_get_type (void) __attribute__((__const__));
GtkWidget * gtk_notebook_new (void);
gint gtk_notebook_page_num (GtkNotebook *notebook, GtkWidget *child);
void gtk_notebook_popup_enable (GtkNotebook *notebook);
void gtk_notebook_remove_page (GtkNotebook *notebook, gint page_num);
void gtk_notebook_set_action_widget (GtkNotebook *notebook, GtkWidget *widget, GtkPackType pack_type);
void gtk_notebook_set_current_page (GtkNotebook *notebook, gint page_num);
void gtk_notebook_set_scrollable (GtkNotebook *notebook, gboolean scrollable);
void gtk_notebook_set_tab_reorderable (GtkNotebook *notebook, GtkWidget *child, gboolean reorderable);
GtkWidget *gtk_paned_get_child1 (GtkPaned *paned);
GtkWidget *gtk_paned_get_child2 (GtkPaned *paned);
GtkWidget * gtk_paned_new (GtkOrientation orientation);
void gtk_paned_pack1 (GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink);
void gtk_paned_pack2 (GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink);
GSList* gtk_radio_menu_item_get_group (GtkRadioMenuItem *radio_menu_item);
GtkWidget* gtk_radio_menu_item_new_with_label (GSList *group, const gchar *label);
GtkAdjustment* gtk_scrolled_window_get_vadjustment (GtkScrolledWindow *scrolled_window);
void gtk_scrolled_window_set_policy (GtkScrolledWindow *scrolled_window,GtkPolicyType hscrollbar_policy,GtkPolicyType vscrollbar_policy);
GtkWidget* gtk_scrolled_window_new (GtkAdjustment *hadjustment,GtkAdjustment *vadjustment);
void gtk_text_buffer_get_bounds (GtkTextBuffer *buffer,GtkTextIter *start,GtkTextIter *end);
void gtk_text_buffer_get_end_iter (GtkTextBuffer *buffer, GtkTextIter *iter);
gchar *gtk_text_buffer_get_text (GtkTextBuffer *buffer,const GtkTextIter *start,const GtkTextIter *end,gboolean include_hidden_chars);
void gtk_text_buffer_insert (GtkTextBuffer *buffer,GtkTextIter *iter,const gchar *text,gint len);
void gtk_text_buffer_set_text (GtkTextBuffer *buffer, const gchar *text, gint len);
GtkTextBuffer* gtk_text_view_get_buffer(GtkTextView *);
void gtk_text_view_get_iter_location (GtkTextView *text_view, const GtkTextIter *iter, GdkRectangle *location);
void gtk_text_view_get_visible_rect (GtkTreeView *tree_view, GdkRectangle *visible_rect);
GtkWidget * gtk_text_view_new (void);
void gtk_text_view_set_wrap_mode (GtkTextView *text_view, GtkWrapMode wrap_mode);
void gtk_text_view_set_editable (GtkTextView *text_view,gboolean setting);
void gtk_tooltip_set_text (GtkTooltip *tooltip, const gchar *text);
void gtk_tree_model_get (GtkTreeModel *tree_model, GtkTreeIter *iter, ...);
gboolean gtk_tree_model_get_iter_first(GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean gtk_tree_model_get_iter_from_string (GtkTreeModel *tree_model, GtkTreeIter *iter, const gchar *path_string);
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
GtkTreeModel *gtk_tree_view_get_model (GtkTreeView *tree_view);
GtkTreeSelection *gtk_tree_view_get_selection (GtkTreeView *tree_view);
GtkWidget *gtk_tree_view_new (void);
void gtk_tree_view_set_headers_visible (GtkTreeView *tree_view, gboolean headers_visible);
void gtk_tree_view_set_model (GtkTreeView *tree_view, GtkTreeModel *model);
gboolean gtk_widget_activate (GtkWidget *widget);
void gtk_widget_destroy (GtkWidget *widget);
void gtk_widget_hide (GtkWidget *widget);
GtkWidget* gtk_widget_get_ancestor (GtkWidget *widget, GType widget_type);
gboolean gtk_widget_get_has_tooltip (GtkWidget *widget);
GtkWidget * gtk_widget_get_parent (GtkWidget *widget);
GtkWidget* gtk_widget_get_toplevel (GtkWidget *widget);
void gtk_widget_grab_focus (GtkWidget *widget);
gboolean gtk_widget_is_focus (GtkWidget *widget);
void gtk_widget_set_has_tooltip (GtkWidget *widget, gboolean has_tooltip);
void gtk_widget_set_size_request (GtkWidget *widget,gint width,gint height);
void gtk_widget_set_tooltip_text (GtkWidget *widget, const gchar *text);
void gtk_widget_show (GtkWidget *widget);
void gtk_widget_show_all (GtkWidget *widget);
void gtk_window_get_size (GtkWindow *window,gint *width,gint *height);
void gtk_window_iconify (GtkWindow *window);
gboolean gtk_window_is_active (GtkWindow *window);
void gtk_window_set_default_size (GtkWindow *window, gint width, gint height);
void gtk_window_set_icon (GtkWindow *window, GdkPixbuf *icon);
void gtk_window_set_title (GtkWindow *window,const gchar *title);
void gtk_window_maximize (GtkWindow *window);

#ifdef __cplusplus
}
#endif
