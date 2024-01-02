
#include "inc/null.h"
#include "inc/bool.h"

#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#else
#include "inc/libgen.h"
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include "inc/fcntl.h"
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#else
#include "inc/netdb.h"
#endif
#ifdef HAVE_NETINET_IN_H
#include<netinet/in.h>
#else
#include "inc/in.h"
#endif
#ifdef HAVE_OPENSSL_SSL_H
#include <openssl/ssl.h>
#else
#include "inc/openssl.h"
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
#include "inc/pthread.h"
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#else
#include "inc/signal.h"
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#else
#include "inc/stdio.h"
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#include "inc/stdlib.h"
#endif
#ifdef HAVE_STRING_H
#include<string.h>
#else
#include "inc/string.h"
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#else
#include "inc/socket.h"
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#else
#include "inc/time.h"
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include "inc/unistd.h"
#endif

#ifdef HAVE_GTK_GTK_H
#pragma GCC diagnostic push//there are 5 more ignors in the program
#pragma GCC diagnostic ignored "-Weverything"
#include <gtk/gtk.h>
#pragma GCC diagnostic pop
#else
#include "inc/gtk.h"
#endif

static GtkTextView *text_view;static GtkWidget*home_page;static GtkListStore*channels;
static SSL *ssl=nullptr;static int plain_socket=-1;
static int con_th=-1;//static GThread*con_th=nullptr;
static BOOL close_intention;
#define ssl_con_try "Trying with SSL.\n"
#define ssl_con_plain "Trying unencrypted.\n"
#define irc_bsz 64
//"510"
#define irc_term "\r\n"
#define irc_term_sz sizeof(irc_term)-1
#define hostname_sz 512//arranging
#define password_sz 505+1//fitting
#define password_con "PASS %s" irc_term
#define nickname_con "NICK %s" irc_term
static char*info_path_name=nullptr;
#define home_string "*Home"
#define priv_msg_str "PRIVMSG"
#define not_msg_str "NOTICE"
#define mod_msg_str "MODE"
#define parse_host_left "@"
#define parse_host_delim ":"
#define parse_host_ports_delim "-"
#define parse_host_ports_micro ","
#define parse_host_ports_macro ";"
#define parse_host_ports_macro_text "semicolon"
#define STR_INDIR(x) #x
#define INT_CONV_STR(x) STR_INDIR(x)
#define _con_nr_su 1
#define _con_nr_us 2
#define _con_nr_s 3
#define _con_nr_u 4
#define con_nr_su "SSL or Unencrypted"
#define con_nr_us "Unencrypted or SSL"
#define con_nr_s "SSL"
#define con_nr_u "Unencrypted"
#define con_nr_min _con_nr_su
#define con_nr_max _con_nr_u
#define con_nr_nrs INT_CONV_STR(con_nr_min) "-" INT_CONV_STR(con_nr_max)
//#define con_nr_righttype1 _con_nr_us
//#define con_nr_righttype2 _con_nr_u
#define help_text "Most of the parameters are set at start.\n\
Launch the program with --help argument for more info.\n\
Send irc commands from the " home_string " tab. Other tabs are sending " priv_msg_str " messages.\n\
\n\
Keyboard\n\
Ctrl+T = Tabs popup\n\
Ctrl+C = Close tab\n\
Ctrl+Q = Shutdown connection\n\
Ctrl+X = Exit program\n\
\n\
Connection format:\n\
[[nickname" parse_host_delim "]password" parse_host_left "]hostname[" parse_host_delim "port1[" parse_host_ports_delim "portN][" parse_host_ports_micro "portM...][" parse_host_ports_macro "portP...]]\n\
A " parse_host_ports_macro_text " (" parse_host_ports_macro ") will override the connection type. Before " parse_host_ports_macro_text " are " con_nr_s " ports and after " parse_host_ports_macro_text " are " con_nr_u " ports.\n\
Escape " parse_host_left " in password with the uri format (\"%40\").\n\
e.g. newNick" parse_host_delim "a%40c" parse_host_left "127.0.0.1" parse_host_delim "7000" parse_host_ports_macro "6660" parse_host_ports_delim "6665" parse_host_ports_micro "6669"
#define chan_sz 50
#define channul_sz chan_sz+1
//"up to fifty (50) characters"
#define channame_scan "%50s"
#define name_sz 9
#define namenul_sz name_sz+1
#define name_scan1 "%9"
#define name_scan name_scan1 "s"
#define mod_scan "%4s"
struct data_len{
	const char*data;size_t len;
};
static pthread_t threadid;static sigset_t threadset;
static GtkWidget*chan_menu;
static GtkWidget*name_on_menu;static GtkWidget*name_off_menu;
static unsigned int alert_counter=0;
static GtkCheckMenuItem*show_time;static GtkCheckMenuItem*channels_counted;
enum {
  LIST_ITEM = 0,
  N_COLUMNS
};//connections org,channels
#define number_of_args 23
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
struct stk_s{
#pragma GCC diagnostic pop
	const char*args[number_of_args];
	int dim[2];GtkComboBoxText*cbt;GtkTreeView*tv;
	char*nick;const char*text;char*nknnow;
	int separator;
	GtkWidget*con_entry;gulong con_entry_act;GtkWidget*sen_entry;gulong sen_entry_act;
	int chans_max;//n_children is int
	int chan_min;//0 gtk parse handle arguments!
	int refresh;//same
	unsigned int refreshid;
	GtkNotebook*notebook;
	struct data_len*dl;
	char*welcome;
	const char*user_irc;
	GtkWidget*trv;unsigned long trvr;
	char*execute_newmsg;GtkWindow*main_win;
	int argc;char**argv;
	int active;
	struct ajoin*ajoins;char*ajoins_mem;size_t ajoins_sum;
	struct ajoin*ignores;char*ignores_mem;size_t ignores_sum;
	char*password;
	GtkListStore*org_tree_list;
	GApplication*app;
	unsigned int send_history;
	gboolean maximize;gboolean minimize;gboolean visible;
	gboolean timestamp;gboolean wnotice;
	BOOL user_irc_free;unsigned char con_type;BOOL show_msgs;
	char args_short[number_of_args];
};
static int autoconnect=-1;static BOOL autoconnect_pending=FALSE;
static GSList*con_group;
static const unsigned char icon16[]={
0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0x00,0x00,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff
,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8
,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xff,0xff,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8,0x00,0xa2,0xe8
//3842
};
static char chantypes[5]={'\0'};
static char chanmodes[7]={'\0'};
static char chanmodessigns[7]={'\0'};//& at whois
static unsigned int maximummodes=0;
#define RPL_NONE -1
#define RPL_LIST 322
#define RPL_NAMREPLY 353
#define show_from_clause(a,b,c) if(icmpAmemBstr(a,b))show_msg=c;
#define show_to_clause(a) if(show_msg==a)show_msg=RPL_NONE;
static int show_msg=RPL_NONE;
#define digits_in_uint 10
static int log_file=-1;
static char*dummy=nullptr;
static char**ignores;
static BOOL can_send_data=FALSE;
#define chans_str "channels"
#define names_str "names"
#define counting_the_list_size (sizeof(chans_str)>sizeof(names_str)?sizeof(chans_str):sizeof(names_str))
#define list_end_str " listed\n"
#define autojoin_str "autojoin"
enum{autoconnect_id,autojoin_id,dimensions_id,chan_min_id,chans_max_id,connection_number_id,hide_id,ignore_id,log_id,maximize_id,minimize_id,nick_id,password_id,refresh_id,right_id,run_id,send_history_id,timestamp_id,user_id,visible_id,welcome_id,welcomeNotice_id,removeconf_id};
struct ajoin{
	int c;//against get_active
	char**chans;
};
#define invite_str " invited you to join channel "
static GtkWidget*menuwithtabs;
#define size_t_max (((unsigned long int)1<<(8*sizeof(size_t)-1))-1)+((unsigned long int)1<<(8*sizeof(size_t)-1))
#define not_a_switch size_t_max
//is also odd, there are pairs, and text size is also let small
//at = and == is same asm against 1     and for other compilers can compare from bit sign and is faster
static GQueue*send_entry_list;static GList*send_entry_list_cursor=nullptr;
#define default_chan_min 250
#define default_chans_max 150
#define default_connection_number _con_nr_su
#define default_refresh 600
#define default_send_history 50
#define default_right 150
#define default_user "USER guest tolmoon tolsun :Ronnie Reagan"
#define mod_add_char '+'
#define mod_remove_char "-"
#define is_mod_add(a) *a==mod_add_char
#define visible_char "i"
#define visible_mod mod_remove_char visible_char
#define wait_recon 10
#define user_error "*Error"
#define user_topic "*Topic"
#define user_info "*Info"

#define contf_get_treev(pan) (GtkTreeView*)gtk_bin_get_child((GtkBin*)gtk_paned_get_child2((GtkPaned*)pan))
#define contf_get_model(pan) gtk_tree_view_get_model(contf_get_treev(pan))
#define contf_get_list(pan) (GtkListStore*)contf_get_model(pan)
#define contf_get_textv(pan) (GtkTextView*)gtk_bin_get_child((GtkBin*)gtk_paned_get_child1((GtkPaned*)pan))
static void addtimestamp(GtkTextBuffer*text_buffer,GtkTextIter*it){
	if(gtk_check_menu_item_get_active(show_time)){
		GDateTime*time_new_now=g_date_time_new_now_local();
		if(time_new_now!=nullptr){
			char tm[1+2+1+2+1+2+1+1];
			sprintf(tm,"<%u:%02u:%02u>",g_date_time_get_hour(time_new_now),g_date_time_get_minute(time_new_now),g_date_time_get_second(time_new_now));
			g_date_time_unref(time_new_now);
			gtk_text_buffer_insert(text_buffer,it,tm,-1);
		}
	}
}
static gboolean wait_iter_wrap(gpointer b){
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer ((GtkTextView*)b);
	GtkTextIter it;
	gtk_text_buffer_get_end_iter(text_buffer,&it);
	GdkRectangle rect;
	GdkRectangle r2;
	gtk_text_view_get_visible_rect((GtkTextView*)b,&rect);
	gtk_text_view_get_iter_location((GtkTextView*)b,&it,&r2);
	int y=r2.y-rect.height;
	if(y>0){
		GtkAdjustment*a=gtk_scrolled_window_get_vadjustment((GtkScrolledWindow*)gtk_widget_get_parent((GtkWidget*)b));
		gtk_adjustment_set_value(a,y);
	}
	return FALSE;
}
static BOOL addattextview_isbottom(GtkTextView*tv,GtkTextBuffer*text_buffer,GtkTextIter*it){
	gtk_text_buffer_get_end_iter(text_buffer,it);
	GdkRectangle rect;
	GdkRectangle r2;
	gtk_text_view_get_visible_rect(tv,&rect);
	gtk_text_view_get_iter_location(tv,it,&r2);
	return rect.y+rect.height >= r2.y;
}
//iter location is not wraped now
#define addattextview_scroll(scroll,tv) if(scroll)g_idle_add(wait_iter_wrap,tv)
static void addattextmain(const char*data,size_t len){
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (text_view);
	GtkTextIter it;
	BOOL b=addattextview_isbottom(text_view,text_buffer,&it);
	addtimestamp(text_buffer,&it);
	gtk_text_buffer_insert(text_buffer,&it,data,(int)len);
	addattextview_scroll(b,text_view);
}
#define addattextmain_struct(s) addattextmain(s->data,s->len)
static void addattextv(GtkTextView*v,const char*n,const char*msg){
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (v);
	GtkTextIter it;
	BOOL b=addattextview_isbottom(v,text_buffer,&it);
	//
	gtk_text_buffer_insert(text_buffer,&it,n,-1);
	addtimestamp(text_buffer,&it);
	gtk_text_buffer_insert(text_buffer,&it,": ",2);
	gtk_text_buffer_insert(text_buffer,&it,msg,-1);
	gtk_text_buffer_insert(text_buffer,&it,"\n",1);
	//
	addattextview_scroll(b,v);
}
#define addatchans(n,msg,p) addattextv(contf_get_textv(p),n,msg)
static void addatnames(const char*n,const char*msg,GtkWidget*p){
	addattextv((GtkTextView*)gtk_bin_get_child((GtkBin*)p),n,msg);
	if(log_file!=-1){
		write(log_file,n,strlen(n));
		char buf[1+20+1+1];//2 at 64, $((2**63))...
		write(log_file,buf,(size_t)sprintf(buf," %ld ",time(nullptr)));//sizeof(time_t)==8?" %lld ":
		write(log_file,msg,strlen(msg));
		write(log_file,irc_term,irc_term_sz);
	}
}
static gboolean textviewthreadsfunc(gpointer b){
	addattextmain_struct(((struct data_len*)b));
	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
static void main_text(const char*b,size_t s){
	struct data_len dl;dl.data=b;dl.len=s;
	g_idle_add(textviewthreadsfunc,&dl);
	int out;sigwait(&threadset,&out);
}
#define main_text_s(b) main_text(b,sizeof(b)-1)
static int recv_data(char*b,int sz){
	if(ssl!=nullptr)return SSL_read(ssl, b, sz);
	return read(plain_socket,b,(size_t)sz);
}
static void send_data(const char*str,size_t sz){
	if(ssl!=nullptr){SSL_write(ssl,str,(int)sz);return;}
	write(plain_socket,str,sz);
}
#define sendlist "LIST" irc_term
#define send_list send_data(sendlist,sizeof(sendlist)-1);
#define send_list_if if(can_send_data)send_list
static gboolean sendthreadsfunc(gpointer b){
	send_data(((struct data_len*)b)->data,((struct data_len*)b)->len);
	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
static void send_safe(const char*str,size_t sz){
	struct data_len dl;dl.data=str;dl.len=sz;
	g_idle_add(sendthreadsfunc,&dl);
	int out;sigwait(&threadset,&out);
}
static gboolean close_ssl_safe(gpointer ignore){(void)ignore;
//to call shutdown with peace
	SSL_free(ssl);ssl=nullptr;
	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
static gboolean close_plain(gpointer ignore){(void)ignore;
//to call shutdown with peace
	close(plain_socket);plain_socket=-1;
	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
#define close_plain_safe int a;g_idle_add(close_plain,nullptr);sigwait(&threadset,&a);
/* ---------------------------------------------------------- *
 * create_socket() creates the socket & TCP-connect to server *
 * ---------------------------------------------------------- */
static void create_socket(char*hostname,unsigned short port) {
	struct hostent *host = gethostbyname(hostname);
	struct sockaddr_in dest_addr;
	if ( host != nullptr ) {
		  /* ---------------------------------------------------------- *
		   * create the basic TCP socket                                *
		   * ---------------------------------------------------------- */
		plain_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(plain_socket!=-1){
			dest_addr.sin_family=AF_INET;
			dest_addr.sin_port=htons(port);
			dest_addr.sin_addr.s_addr = *(unsigned long*)((void*)(host->h_addr_list[0]));
			//  memset(&(dest_addr.sin_zero), '\0', 8);//string
			//"setting it to zero doesn't seem to be actually necessary"
			  /* ---------------------------------------------------------- *
			   * Try to make the host connect here                          *
			   * ---------------------------------------------------------- */
			if ( connect(plain_socket, (struct sockaddr *) &dest_addr,
				sizeof(struct sockaddr)) == -1 ) {
				main_text_s("Error: Cannot connect to host.\n");
				close_plain_safe
			}
		}else main_text_s("Error: Cannot open the socket.\n");
	}
	else
		main_text_s("Error: Cannot resolve hostname.\n");
}
static BOOL parse_host_str(const char*indata,char*hostname,char*psw,char*nkn,unsigned short**pr,size_t*pl,size_t*swtch,struct stk_s*ps) {
	size_t sz=strlen(indata);
	//
	const char*left=strchr(indata,*parse_host_left);BOOL nonick=TRUE;
	if(left!=nullptr){
		size_t lsz=(size_t)(left-indata);
		size_t i=lsz;
		while(i>0){
			i--;
			if(indata[i]==*parse_host_delim){
				if(i>=namenul_sz)return FALSE;
				if(i>0){
					memcpy(nkn,indata,i);nkn[i]='\0';nonick=FALSE;
				}
				i++;
				break;
			}
		}
		size_t psz=lsz-i;
		char*p=(char*)
#ifdef FN_G_MEMDUP2
		g_memdup2
#else
		g_memdup
#endif
		(indata+i,psz+1);
		p[psz]='\0';
		char*up=g_uri_unescape_string(p,nullptr);
		g_free(p);
		if(strlen(up)>=password_sz){free(up);return FALSE;}
		strcpy(psw,up);free(up);
		sz-=(size_t)(left+1-indata);indata=left+1;
	}else if(ps->password!=nullptr)strcpy(psw,ps->password);
	else *psw='\0';
	if(nonick){
		if(ps->nick!=nullptr)strcpy(nkn,ps->nick);
		else{
			const char def_n[]="guest_abc";
			memcpy(nkn,def_n,sizeof(def_n));
		}
	}
	ps->nknnow=nkn;//can be only at this go
	//
	const char*ptr=strchr(indata,*parse_host_delim);
	if(ptr!=nullptr)sz=(size_t)(ptr-indata);
	if(sz<hostname_sz){
		memcpy(hostname, indata, sz);
		hostname[sz]='\0';
		if(ptr==nullptr){
			*pr=(unsigned short*)malloc(2*sizeof(unsigned short));
			if(*pr==nullptr)return FALSE;
			(*pr)[0]=6667;(*pr)[1]=6667;*pl=0;*swtch=not_a_switch;
			return TRUE;
		}
		//at switch recon, can't test pl==switch because of :;port  /   pl<switch  :port;    =>    only not_a_switch = odd is ok
		ptr++;
		if(*ptr=='\0')return FALSE;//return here, else will return TRUE with two undefined
		size_t i=1;
		for(size_t j=0;ptr[j]!='\0';j++)if(ptr[j]==*parse_host_ports_micro||ptr[j]==*parse_host_ports_macro)i++;
		size_t max=i*2;
		unsigned short*por=(unsigned short*)malloc(max*sizeof(unsigned short));
		if(por!=nullptr){
			if(*ptr==*parse_host_ports_macro){//case when are induced unencrypted ports
				ptr++;
				if(*ptr=='\0'){free(por);return FALSE;}//case:   ;   return here, else will return TRUE with two undefined
				*swtch=0;
				max-=2;//because first two are not anymore
			}else *swtch=not_a_switch;
			size_t j=0;size_t k=0;
			for(;;){
				BOOL end=ptr[j]=='\0';BOOL sw=ptr[j]==*parse_host_ports_macro;
				if(ptr[j]==*parse_host_ports_micro||end||sw){
					int n=sscanf(ptr,"%hu" parse_host_ports_delim "%hu",&por[k],&por[k+1]);
					if(n==0){free(por);return FALSE;}//this is not same as EOF, a "" is an EOF
					if(n==1)por[k+1]=por[k];
					if(end){
						if(n==EOF)//this is the case: :port;    pl 0   switch 2      or :port,      EOF can also be an error but not here
							max-=2;
						*pl=max-2;*pr=por;return TRUE;
					}
					k+=2;
					if(sw){
						if(*swtch!=not_a_switch){free(por);return FALSE;}//this is the second switch?
						*swtch=k;
					}
					ptr=&ptr[j+1];j=0;continue;
				}
				j++;
			}
		}
	}
	return FALSE;
}
static void pars_chan_end(GtkTreeIter*it,char*channm,unsigned int nr){
	size_t ln=strlen(channm);channm[ln]=' ';sprintf(channm+ln+1,"%u",nr);
	gtk_list_store_set(channels, it, LIST_ITEM, channm, -1);
}
static void pars_chan_insert(GtkTreeIter*it,char*chan,unsigned int nr,int max){
	GtkTreeIter i;
	gtk_list_store_insert_before(channels,&i,it);
	pars_chan_end(&i,chan,nr);
	int n=gtk_tree_model_iter_n_children((GtkTreeModel*)channels,nullptr);
	if(n>max){
		gtk_tree_model_iter_nth_child((GtkTreeModel*)channels,&i,nullptr,n-1);
		gtk_list_store_remove(channels,&i);
	}
}
static int pars_chan_counted(char*chan,unsigned int nr,int max){
	GtkTreeIter it;int sum=0;
	gboolean valid=gtk_tree_model_get_iter_first ((GtkTreeModel*)channels, &it);
	while(valid){
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)channels, &it, 0, &text, -1);
		char*c=strchr(text,' ');*c='\0';
		unsigned int n=(unsigned int)atoi(c+1);
		int a=strcmp(chan,text);
		g_free(text);
		if(nr>n||(nr==n&&a<0)){
			pars_chan_insert(&it,chan,nr,max);
			return -1;
		}
		valid = gtk_tree_model_iter_next( (GtkTreeModel*)channels, &it);sum++;
	}
	return sum;
}
static int pars_chan_alpha(char*chan,unsigned int nr,int max){
	GtkTreeIter it;int n=0;
	gboolean valid=gtk_tree_model_get_iter_first ((GtkTreeModel*)channels, &it);
	while(valid){
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)channels, &it, 0, &text, -1);
		char*c=strchr(text,' ');*c='\0';
		int a=strcmp(chan,text);
		g_free(text);
		if(a<0){
			pars_chan_insert(&it,chan,nr,max);
			return -1;
		}
		valid = gtk_tree_model_iter_next( (GtkTreeModel*)channels, &it);n++;
	}
	return n;
}
static void pars_chan(char*chan,unsigned int nr,int max){
	int n;
	if(gtk_check_menu_item_get_active(channels_counted)){n=pars_chan_counted(chan,nr,max);}
	else n=pars_chan_alpha(chan,nr,max);
	if(n>=0&&n<max){
		GtkTreeIter it;
		gtk_list_store_append(channels,&it);
		pars_chan_end(&it,chan,nr);
	}
}
static GtkWidget*container_frame_name_out(GtkWidget**out){
	GtkWidget*text = gtk_text_view_new ();
	gtk_text_view_set_editable((GtkTextView*)text, FALSE);
	gtk_text_view_set_wrap_mode ((GtkTextView*)text, GTK_WRAP_WORD_CHAR);
	GtkWidget *scrolled_window = gtk_scrolled_window_new (nullptr, nullptr);
	gtk_scrolled_window_set_policy ((GtkScrolledWindow*) scrolled_window,
	                                  GTK_POLICY_EXTERNAL,//NEVER. but with will have the bigger value and cannot rewrap
	                                  GTK_POLICY_AUTOMATIC);
	gtk_container_add ((GtkContainer*) scrolled_window,text);
	gtk_container_set_border_width ((GtkContainer*)scrolled_window, 5);
	if(out!=nullptr)*out=text;
	return scrolled_window;
}
#define container_frame_name container_frame_name_out(nullptr);
static GtkWidget*container_frame(int sep,GCallback click,gpointer data){
	GtkWidget*scrolled_window=container_frame_name
	//
	GtkWidget *tree=gtk_tree_view_new();
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "text", LIST_ITEM, nullptr);
	//
	GtkListStore*ls= gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
	gtk_tree_view_set_headers_visible((GtkTreeView*)tree,FALSE);
	gtk_tree_view_append_column((GtkTreeView*)tree, column);
	gtk_tree_view_set_model((GtkTreeView*)tree, (GtkTreeModel*)ls);
	g_object_unref(ls);
	//
	g_signal_connect_data(tree,"button-release-event",click,data,nullptr,(GConnectFlags)0);
	//
	GtkWidget*scrolled_right = gtk_scrolled_window_new (nullptr, nullptr);
	gtk_container_add((GtkContainer*)scrolled_right,tree);
	//
	GtkWidget *pan=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_pack1((GtkPaned*)pan,scrolled_window,TRUE,TRUE);
	gtk_paned_pack2((GtkPaned*)pan,scrolled_right,FALSE,TRUE);
	gtk_widget_set_size_request (scrolled_right, sep, -1);
	return pan;
}
#define get_pan_from_menu(x) gtk_label_get_mnemonic_widget((GtkLabel*)gtk_bin_get_child ((GtkBin*)x))
static void page_show(GtkWidget*menuitem,GtkNotebook*nb){
	GtkWidget*pan=get_pan_from_menu(menuitem);
	gtk_notebook_set_current_page(nb,gtk_notebook_page_num(nb,pan));
}
static void close_channel(GtkLabel*t){
	char buf[5+chan_sz+irc_term_sz]="PART ";
	const char*a=gtk_label_get_text(t);
	size_t n=strlen(a);
	memcpy(buf+5,a,n);memcpy(&buf[5+n],irc_term,irc_term_sz);
	send_data(buf,5+irc_term_sz+n);
}
static GtkWidget*alert_widget(GtkWidget*box){
	GList*l=gtk_container_get_children((GtkContainer*)box);
	GtkWidget*img=(GtkWidget*)l->data;
	g_list_free(l);
	if(G_TYPE_FROM_INSTANCE(img)!=gtk_image_get_type())return nullptr;
	return img;
}
static void unalert(GtkNotebook*notebook,GtkWidget*box){
	GtkWidget*a=alert_widget(box);
	if(a!=nullptr){
		gtk_widget_destroy(a);
		alert_counter--;
		if(alert_counter==0)
			gtk_widget_hide(gtk_notebook_get_action_widget(notebook,GTK_PACK_END));
	}
}
static void close_name(GtkWidget*mn){
	GtkWidget*page=get_pan_from_menu(mn);
	GtkNotebook*nb=(GtkNotebook*)gtk_widget_get_ancestor(page,gtk_notebook_get_type());
	unalert(nb,gtk_notebook_get_tab_label(nb,page));
	gtk_notebook_remove_page(nb,gtk_notebook_page_num(nb,page));
	gtk_widget_destroy(mn);
}
static GtkWidget*add_new_tab_menuitem(GtkWidget*frame,const char*title,GtkNotebook*notebook,GtkWidget*menu){
	GtkWidget*menu_item = gtk_menu_item_new_with_label (title);
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (page_show),notebook,nullptr,(GConnectFlags)0);
	GtkWidget*lab=gtk_bin_get_child ((GtkBin*)menu_item);
	gtk_label_set_mnemonic_widget((GtkLabel*)lab,frame);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);
	gtk_widget_show(menu_item);
	return menu_item;
}
static GtkWidget*add_new_tab(GtkWidget*frame,char*title,GtkWidget**cls,GtkNotebook*notebook,GtkWidget*menu,BOOL is_name){
	gtk_widget_show_all (frame);
	GtkWidget*t=gtk_label_new (title);
	GtkWidget*close=gtk_button_new();
	gtk_button_set_relief((GtkButton*)close,GTK_RELIEF_NONE);
	GtkWidget*closeimg=gtk_image_new_from_icon_name ("window-close",GTK_ICON_SIZE_MENU);
	gtk_button_set_image((GtkButton*)close,closeimg);
	GtkWidget*box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_end((GtkBox*)box,close,FALSE,FALSE,0);
	gtk_box_pack_end((GtkBox*)box,t,TRUE,TRUE,0);
	gtk_widget_show_all(box);
	gtk_notebook_append_page_menu (notebook, frame, box, gtk_label_new (title));
	gtk_notebook_set_tab_reorderable(notebook, frame, TRUE);
	GtkWidget*menu_item=add_new_tab_menuitem(frame,title,notebook,menu);
	*cls=close;
	return is_name?menu_item:t;
}
static BOOL chan_not_joined(char*item_text,GtkNotebook*notebook){
	BOOL b=TRUE;
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	if(list!=nullptr){
		GList*lst=list;
		for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(item_text,d)==0){
				gtk_notebook_set_current_page(notebook,gtk_notebook_page_num(notebook,get_pan_from_menu(menu_item)));
				b=FALSE;
				break;
			}
			list=g_list_next(list);
			if(list==nullptr)break;
		}
		g_list_free(lst);
	}
	return b;
}
static void send_join(char*item_text,size_t i){
	char buf[5+chan_sz+irc_term_sz]="JOIN ";
	memcpy(buf+5,item_text,i);
	memcpy(buf+5+i,irc_term,irc_term_sz);
	send_data(buf,5+irc_term_sz+i);
}
static gboolean chan_join (GtkTreeView *tree,GdkEvent*ignored,GtkNotebook*notebook){
	(void)ignored;
	GtkTreeSelection *sel=gtk_tree_view_get_selection(tree);
	GtkTreeIter iterator;
	if(gtk_tree_selection_get_selected (sel,nullptr,&iterator)){//can be no channel
		char*item_text;
		gtk_tree_model_get ((GtkTreeModel*)channels, &iterator, LIST_ITEM, &item_text, -1);
		for(size_t i=0;;i++){
			if(item_text[i]==' '){
				item_text[i]='\0';
				if(chan_not_joined(item_text,notebook))send_join(item_text,i);
				break;
			}
		}
		free(item_text);
	}
	return FALSE;//not care about other events
}
static BOOL name_join_isnew(struct stk_s*ps,char*n){
	if(strcmp(ps->nknnow,n)==0){
		gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,home_page));
		return FALSE;
	}
	GList*list=gtk_container_get_children((GtkContainer*)name_on_menu);
	if(list!=nullptr){
		GList*lst=list;
		for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(n,d)==0){
				g_list_free(lst);
				gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,get_pan_from_menu(menu_item)));
				return FALSE;
			}
			list=g_list_next(list);
			if(list==nullptr)break;
		}
		g_list_free(lst);
	}
	return TRUE;
}
static GtkWidget* name_join_nb(char*t,GtkNotebook*nb){
	GtkWidget*scrl=container_frame_name
	GtkWidget*close;GtkWidget*mn=add_new_tab(scrl,t,&close,nb,name_on_menu,TRUE);
	g_signal_connect_data (close, "clicked",G_CALLBACK (close_name),mn,nullptr,G_CONNECT_SWAPPED);//not "(GClosureNotify)gtk_widget_destroy" because at restart clear will be trouble
	return scrl;
}
#define nickname_start(a) ('A'<=*a&&*a<='}')
static gboolean name_join(GtkTreeView*tree,GdkEvent*ignored,struct stk_s*ps){
	(void)ignored;
	GtkTreeSelection *sel=gtk_tree_view_get_selection(tree);
	GtkTreeIter iterator;
	gtk_tree_selection_get_selected (sel,nullptr,&iterator);
	char*item_text;
	gtk_tree_model_get (gtk_tree_view_get_model(tree), &iterator, LIST_ITEM, &item_text, -1);
	char*a=nickname_start(item_text)?item_text:item_text+1;
	if(name_join_isnew(ps,a))
		gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,name_join_nb(a,ps->notebook)));
	g_free(item_text);
	return FALSE;//not care about other events
}
static GtkWidget* page_from_str(char*c,GtkWidget*men){
	GtkWidget*pan=nullptr;
	GList*list=gtk_container_get_children((GtkContainer*)men);
	if(list!=nullptr){
		GList*lst=list;
		for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(c,d)==0){
				pan=get_pan_from_menu(menu_item);
				break;
			}
			list=g_list_next(list);
			if(list==nullptr)break;
		}
		g_list_free(lst);
	}
	return pan;
}
#define chan_pan(c) page_from_str(c,chan_menu)
#define name_off_pan(c) page_from_str(c,name_off_menu)
#define name_to_list(c) contf_get_list(chan_pan(c))
static void chan_change_nr_gain(GtkTreeIter*iter,char*chn,unsigned int nr){
	GtkTreeIter it=*iter;
	if(gtk_tree_model_iter_previous( (GtkTreeModel*)channels, &it)==FALSE)return;
	for(;;){
		char*text;
		char c[channul_sz];
		unsigned int n;
		gtk_tree_model_get ((GtkTreeModel*)channels, &it, 0, &text, -1);
		sscanf(text,channame_scan " %u",c,&n);
		if(n>nr)break;
		int a=strcmp(c,chn);
		g_free(text);
		if(n==nr&&a<0)break;
		if(gtk_tree_model_iter_previous((GtkTreeModel*)channels, &it)==FALSE){
			gtk_list_store_move_after(channels,iter,nullptr);
			return;
		}
	}
	gtk_list_store_move_after(channels,iter,&it);
}
static void chan_change_nr_loss(GtkTreeIter*iter,char*chn,unsigned int nr){
	GtkTreeIter it=*iter;
	if(gtk_tree_model_iter_next( (GtkTreeModel*)channels, &it)==FALSE)return;
	for(;;){
		char*text;
		char c[channul_sz];
		unsigned int n;
		gtk_tree_model_get ((GtkTreeModel*)channels, &it, 0, &text, -1);
		sscanf(text,channame_scan " %u",c,&n);
		if(nr>n)break;
		int a=strcmp(c,chn);
		g_free(text);
		if(nr==n&&a>0)break;
		if(gtk_tree_model_iter_next((GtkTreeModel*)channels, &it)==FALSE){
			gtk_list_store_move_before(channels,iter,nullptr);
			return;
		}
	}
	gtk_list_store_move_before(channels,iter,&it);
}
static BOOL get_chan_counted(const char*chan,char*c,GtkTreeIter*it,char**text){
	gboolean valid=gtk_tree_model_get_iter_first ((GtkTreeModel*)channels, it);
	while(valid){
		gtk_tree_model_get ((GtkTreeModel*)channels, it, 0, text, -1);
		sscanf(*text,channame_scan,c);
		if(strcmp(chan,c)==0)return TRUE;
		g_free(*text);
		valid = gtk_tree_model_iter_next( (GtkTreeModel*)channels, it);
	}
	return FALSE;
}
static BOOL get_chan_alpha(const char*chan,char*c,GtkTreeIter*it,char**text){
	gboolean valid=gtk_tree_model_get_iter_first ((GtkTreeModel*)channels, it);
	while(valid){
		gtk_tree_model_get ((GtkTreeModel*)channels, it, 0, text, -1);
		sscanf(*text,channame_scan,c);
		int a=strcmp(chan,c);
		if(a==0)return TRUE;
		g_free(*text);
		if(a<0)return FALSE;
		valid = gtk_tree_model_iter_next( (GtkTreeModel*)channels, it);
	}
	return FALSE;
}
static BOOL chan_change_nr(const char*chan,int v){
	GtkTreeIter it;
	//chan_min hidding
	char c[chan_sz+1+digits_in_uint+1];char*text;
	BOOL b;
	gboolean ac=gtk_check_menu_item_get_active(channels_counted);
	if(ac){b=get_chan_counted(chan,c,&it,&text);}
	else b=get_chan_alpha(chan,c,&it,&text);
	if(b){
		size_t s=strlen(c);size_t ss=s;
		unsigned int n;
		s++;sscanf(text+s,"%u",&n);
		g_free(text);
		n+=(unsigned int)v;
		if(ac){
			if(v>0)chan_change_nr_gain(&it,c,n);
			else chan_change_nr_loss(&it,c,n);
		}
		c[ss]=' ';
		s+=(size_t)sprintf(c+s,"%u",n);c[s]='\0';
		gtk_list_store_set(channels, &it, LIST_ITEM, c, -1);//-1 is for end of arguments
		return TRUE;
	}
	return FALSE;
}
#define listing_test(a,b) if(gtk_widget_get_has_tooltip(a)==FALSE){gtk_list_store_clear(b);gtk_widget_set_has_tooltip(a,TRUE);}
#define listing_info(a) "Adding " a "..."
static gboolean home_page_tooltip (GtkWidget*ignored,int ignored2,int ignored3,gboolean ignored4,GtkTooltip*tooltip){
	(void)ignored;(void)ignored2;(void)ignored3;(void)ignored4;
	//no gtk_tooltip_get_text, ...set_text is once
	gtk_tooltip_set_text(tooltip,listing_info("channels"));
	return TRUE;
}
#define test_to_add_chan(ps,a) ps->chan_min<=a
static void pars_join(char*chan,struct stk_s*ps){
	GtkWidget*pan=chan_pan(chan);
	if(pan==nullptr){//can be kick and let the channel window
		pan=container_frame(ps->separator,G_CALLBACK(name_join),ps);
		gtk_widget_set_tooltip_text(pan,listing_info("names"));//is also a NAMES flag here
		GtkWidget*close;GtkWidget*lb=add_new_tab(pan,chan,&close,ps->notebook,chan_menu,FALSE);
		g_signal_connect_data (close, "clicked",G_CALLBACK (close_channel),lb,nullptr,G_CONNECT_SWAPPED);
	}
	gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,pan));
	if(chan_change_nr(chan,1)==FALSE)if(test_to_add_chan(ps,1))pars_chan(chan,1,ps->chans_max);
}
static void pars_join_user(char*channm,char*nicknm){
	//if(p!=nullptr){
	GtkListStore*lst=name_to_list(channm);
	GtkTreeIter it;
	gtk_tree_model_iter_nth_child((GtkTreeModel*)lst, &it, nullptr, gtk_tree_model_iter_n_children((GtkTreeModel*)lst,nullptr)-1);//at least one, we already joined
	for(;;){
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)lst, &it, LIST_ITEM, &text, -1);
		if(strcmp(nicknm,text)>0||nickname_start(text)==FALSE){
			g_free(text);
			GtkTreeIter i;
			gtk_list_store_insert_after(lst,&i,&it);
			gtk_list_store_set(lst, &i, LIST_ITEM, nicknm, -1);
			chan_change_nr(channm,1);
			return;
		}
		g_free(text);
		if(gtk_tree_model_iter_previous( (GtkTreeModel*)lst,&it)==FALSE)break;
	}
	gtk_list_store_prepend(lst,&it);
	gtk_list_store_set(lst, &it, LIST_ITEM, nicknm, -1);
	chan_change_nr(channm,1);
	//}
}
static void pars_part(char*c,GtkNotebook*nb){
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	GList*lst=list;
	for(;;){
		GtkWidget*menu_item=(GtkWidget*)list->data;
		const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
		if(strcmp(c,d)==0){
			GtkWidget*pan=get_pan_from_menu(menu_item);
			unalert(nb,gtk_notebook_get_tab_label(nb,pan));
			gtk_notebook_remove_page(nb,gtk_notebook_page_num(nb,pan));
			gtk_widget_destroy(menu_item);
			chan_change_nr(c,-1);
			break;
		}
		list=g_list_next(list);
		if(list==nullptr)break;
	}
	g_list_free(lst);
}
static BOOL get_iter_unmodes(GtkListStore*lst,GtkTreeIter*it,char*nk){
	char*txt;
	gtk_tree_model_iter_nth_child((GtkTreeModel*)lst, it, nullptr, 
		gtk_tree_model_iter_n_children((GtkTreeModel*)lst,nullptr)-1);
	do{
		gtk_tree_model_get ((GtkTreeModel*)lst, it, 0, &txt, -1);
		if(nickname_start(txt)==FALSE){g_free(txt);return FALSE;}
		int a=strcmp(nk,txt);
		g_free(txt);
		if(a==0)return TRUE;
		else if(a>0)return FALSE;
	}while(gtk_tree_model_iter_previous( (GtkTreeModel*)lst, it));
	return FALSE;
}
static char get_iter_modes(GtkListStore*lst,GtkTreeIter*it,char*nk,BOOL notop){
	char*txt;
	gtk_tree_model_get_iter_first ((GtkTreeModel*)lst,it);
	gtk_tree_model_get ((GtkTreeModel*)lst,it, 0, &txt, -1);
	char lastmod=*txt^1;//to be dif at first compare
	unsigned int modes=0;
	for(;;){
		if(nickname_start(txt)){g_free(txt);return '\0';}
		if(*txt!=lastmod){
			modes++;lastmod=*txt;
			if(notop&&modes==maximummodes&&lastmod==*chanmodessigns){g_free(txt);return '\0';}
			//not at partquit&the 5th&1from5
		}
		int a=strcmp(nk,txt+1);
		g_free(txt);
		if(a==0)return lastmod;
		else if(modes==maximummodes&&a<0)return '\0';//quit/mistakes/whois
		if(gtk_tree_model_iter_next( (GtkTreeModel*)lst,it)==FALSE)return '\0';
		gtk_tree_model_get ((GtkTreeModel*)lst,it, 0, &txt, -1);
	}
}
static void pars_part_quit(char*nk,const char*cn,GtkListStore*lst){
	GtkTreeIter it;
	if(get_iter_unmodes(lst,&it,nk)||get_iter_modes(lst,&it,nk,FALSE)!='\0'){
		gtk_list_store_remove(lst,&it);chan_change_nr(cn,-1);
	}
}
static void pars_part_user(char*channm,char*nicknm){
	//if(p!=nullptr){
	GtkListStore*lst=name_to_list(channm);
	pars_part_quit(nicknm,channm,lst);
	//}
}
static BOOL nick_extract(char*a,char*n){
	return sscanf(a,":" name_scan1 "[^!]",n)==1;
}
static int nick_and_chan(char*a,char*b,char*n,char*c,char*nick){
	if(nick_extract(a,n)){
		if(*b==':')b++;//undernet,at ngircd no
		if(sscanf(b,channame_scan,c)==1){
			if(strcmp(nick,n)!=0)return 1;
			return 0;
		}
	}
	return -1;
}
static void add_name_lowuser(GtkListStore*lst,char*t){
	GtkTreeIter it;
	GtkTreeIter i;
	char*text;
	int n=gtk_tree_model_iter_n_children((GtkTreeModel*)lst,nullptr);
	if(n>0){
		gtk_tree_model_iter_nth_child((GtkTreeModel*)lst, &it, nullptr, n-1);
		do{
			gtk_tree_model_get ((GtkTreeModel*)lst, &it, 0, &text, -1);
			if(strcmp(t,text)>0||nickname_start(text)==FALSE){
				g_free(text);
				gtk_list_store_insert_after(lst,&i,&it);
				gtk_list_store_set(lst, &i, LIST_ITEM, t, -1);
				return;
			}
			g_free(text);
		}while(gtk_tree_model_iter_previous( (GtkTreeModel*)lst, &it));
	}
	gtk_list_store_prepend(lst,&it);
	gtk_list_store_set(lst, &it, LIST_ITEM, t, -1);
}
static void add_name_highuser(GtkListStore*lst,char*t){
	GtkTreeIter it;
	GtkTreeIter i;
	char*text;
	if(gtk_tree_model_get_iter_first((GtkTreeModel*)lst, &it)){
		do{
			gtk_tree_model_get ((GtkTreeModel*)lst, &it, 0, &text, -1);
			if(strcmp(t,text)<0||nickname_start(text)){
				g_free(text);
				gtk_list_store_insert_before(lst,&i,&it);
				gtk_list_store_set(lst, &i, LIST_ITEM, t, -1);
				return;
			}
			g_free(text);
		}while(gtk_tree_model_iter_next( (GtkTreeModel*)lst, &it));
	}
	gtk_list_store_append(lst,&it);
	gtk_list_store_set(lst, &it, LIST_ITEM, t, -1);
}
static void add_name(GtkListStore*lst,char*t){
	if(nickname_start(t)){add_name_lowuser(lst,t);return;}
	add_name_highuser(lst,t);
}
static void pars_names(GtkWidget*pan,char*b,size_t s){
	GtkListStore*lst=contf_get_list(pan);
	listing_test(pan,lst)
	size_t j=0;
	for(size_t i=0;i<s;i++){
		if(b[i]==' '){b[i]='\0';add_name(lst,b+j);b[i]=' ';j=i+1;}
	}
	add_name(lst,b+j);
}
static void pars_quit(char*nk){
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	GList*ls=list;
	for(;;){
		GtkWidget*menu_item=(GtkWidget*)list->data;
		GtkListStore*lst=contf_get_list(get_pan_from_menu(menu_item));
		pars_part_quit(nk,gtk_menu_item_get_label((GtkMenuItem*)menu_item),lst);
		list=g_list_next(list);
		if(list==nullptr)break;
	}
	g_list_free(ls);
}
static void pars_mod_set(GtkListStore*lst,char*n,int pos,BOOL plus){
	GtkTreeIter it;char prevmod;
	if(plus){
		if(get_iter_unmodes(lst,&it,n)){
			gtk_list_store_remove(lst,&it);
			char buf[1+name_sz+1];*buf=chanmodessigns[pos];
			strcpy(buf+1,n);
			add_name_highuser(lst,buf);
			return;
		}
		prevmod=get_iter_modes(lst,&it,n,TRUE);
		if(prevmod!='\0'){
			if(pos<(strchr(chanmodessigns,prevmod)-chanmodessigns)){
				gtk_list_store_remove(lst,&it);
				char buf[1+name_sz+1];*buf=chanmodessigns[pos];
				strcpy(buf+1,n);
				add_name_highuser(lst,buf);
			}
		}
	}else{
		prevmod=get_iter_modes(lst,&it,n,FALSE);
		if(prevmod!='\0'){
			int spos=strchr(chanmodessigns,prevmod)-chanmodessigns;
			if(spos<=pos){
				gtk_list_store_remove(lst,&it);
				add_name_lowuser(lst,n);
				if(chanmodessigns[spos+1]!='\0'){//can be downgraded
					char downgraded[6+name_sz+irc_term_sz+1];
					int sz=sprintf(downgraded,"WHOIS %s" irc_term,n);
					send_data(downgraded,(size_t)sz);
				}
			}
		}
	}
}
static void pars_mod_sens(BOOL plus,char*c,char*m,char*n){
	for(size_t i=0;m[i]!='\0';i++){
		char*modpos=strchr(chanmodes,m[i]);
		if(modpos!=nullptr){
			GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
			GList*ls=list;
			for(;;){
				GtkWidget*menu_item=(GtkWidget*)list->data;
				const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
				if(strcmp(c,d)==0){
					GtkListStore*lst=contf_get_list(get_pan_from_menu(menu_item));
					pars_mod_set(lst,n,modpos-chanmodes,plus);
					break;
				}
				list=g_list_next(list);
				if(list==nullptr)break;
			}
			g_list_free(ls);
			return;
		}
	}
}
static void pars_wmod(char*n,char*msg){
	size_t j=0;
	for(size_t i=0;;i++){
		if(msg[i]==' '){
			char*modpos=strchr(chanmodessigns,msg[j]);
			if(modpos!=nullptr){
				msg[i]='\0';
				GtkWidget*pan=chan_pan(&msg[j+1]);
				if(pan!=nullptr)pars_mod_set(contf_get_list(pan),n,modpos-chanmodessigns,TRUE);
				msg[i]=' ';
			}
			j=i+i;
		}else if(msg[i]=='\0'){
			char*modpos=strchr(chanmodessigns,msg[j]);
			if(modpos!=nullptr){
				GtkWidget*pan=chan_pan(&msg[j+1]);
				if(pan!=nullptr)pars_mod_set(contf_get_list(pan),n,modpos-chanmodessigns,TRUE);
			}
			return;
		}
	}
}
static void pars_mod(char*c,char*m,char*n){
	if(is_mod_add(m))pars_mod_sens(TRUE,c,m+1,n);
	else if(*m==*mod_remove_char)pars_mod_sens(FALSE,c,m+1,n);
}
static void pars_mod_self(struct stk_s*ps,char*mod){
	if(ps->visible){
		if(is_mod_add(mod)){
			mod++;
			for(size_t i=0;mod[i]!='\0';i++){
				if(mod[i]==*visible_char){
					char vidata[5+name_sz+3+irc_term_sz]=mod_msg_str " ";
					size_t c=strlen(ps->nknnow);
					memcpy(vidata+5,ps->nknnow,c);
					c+=5;
					memcpy(vidata+c," " visible_mod irc_term,3+irc_term_sz);
					send_data(vidata,c+3+irc_term_sz);
				}
			}
		}
	}
	ps->visible=FALSE;
}
static gboolean force_focus(gpointer e){
	gtk_widget_grab_focus((GtkWidget*)e);
	return FALSE;
}
static void nb_switch_page(GtkNotebook *notebook,GtkWidget *page,guint ignored,GtkEntry*e){
//swapped is not a,b,c,d->d,a,b,c it is d,b,c,a
(void)ignored;
	GtkWidget*box=gtk_notebook_get_tab_label(notebook,page);
	if(G_TYPE_FROM_INSTANCE(box)==gtk_box_get_type())unalert(notebook,box);
	g_idle_add(force_focus,e);
}
static void alert(GtkWidget*box,GtkNotebook*nb){
	GtkWidget*info=gtk_image_new_from_icon_name ("dialog-information",GTK_ICON_SIZE_MENU);
	gtk_box_pack_start((GtkBox*)box,info,FALSE,FALSE,0);
	gtk_widget_show(info);
	gtk_widget_show(gtk_notebook_get_action_widget(nb,GTK_PACK_END));
	alert_counter++;
}
static void prealert(GtkNotebook*nb,GtkWidget*child){
	if(gtk_notebook_get_current_page(nb)!=gtk_notebook_page_num(nb,child)){
		GtkWidget*box=gtk_notebook_get_tab_label(nb,child);
		if(alert_widget(box)==nullptr)alert(box,nb);
	}
}
static BOOL is_channel(const char*c){
	for(int i=0;;i++)if(chantypes[i]==*c)return TRUE;
		else if(chantypes[i]=='\0')return FALSE;
}
static void send_msg_type(char*usednick,const char*a,const char*text,GtkWidget*pg,const char*msg_irc_type){
	const char s_msg[]=" :";
	size_t len=strlen(msg_irc_type);size_t wid=sizeof(s_msg)-1;
	size_t dim=strlen(a);size_t sz=strlen(text);
	char*b=(char*)malloc(len+dim+wid+sz+irc_term_sz);
	if(b==nullptr)return;
	memcpy(b,msg_irc_type,len);
	memcpy(b+len,a,dim);size_t spc=len+dim;
	memcpy(b+spc,s_msg,wid);spc+=wid;
	memcpy(b+spc,text,sz);sz+=spc;
	if(is_channel(a))addatchans(usednick,text,pg);
	else addatnames(usednick,text,pg);
	memcpy(b+sz,irc_term,irc_term_sz);
	send_data(b,sz+irc_term_sz);
	free(b);
}
#define send_msg(usednick,a,text,pg) send_msg_type(usednick,a,text,pg,priv_msg_str " ")
static void pars_pmsg_chan(char*n,char*c,char*msg,GtkNotebook*nb){
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	GList*lst=list;
	for(;;){
		GtkWidget*menu_item=(GtkWidget*)list->data;
		const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
		if(strcmp(c,d)==0){
			GtkWidget*pan=get_pan_from_menu(menu_item);
			addatchans(n,msg,pan);
			prealert(nb,pan);
			break;
		}
		list=g_list_next(list);
		if(list==nullptr)break;
	}
	g_list_free(lst);
}
static BOOL talk_user(char*n){
	for(size_t i=0;;i++){
		if(ignores[i]==nullptr)return TRUE;
		if(strcmp(ignores[i],n)==0)return FALSE;
	}
}
#define exec_nm \
if(ps->execute_newmsg!=nullptr)\
	if(gtk_window_is_active(ps->main_win)==FALSE)\
		g_spawn_command_line_async(ps->execute_newmsg,nullptr);
static void pars_pmsg_name(char*n,char*msg,struct stk_s*ps,BOOL is_privmsg,const char*frontname){
	BOOL novel=TRUE;
	GtkNotebook*nb=ps->notebook;
	GList*list=gtk_container_get_children((GtkContainer*)name_on_menu);
	if(list!=nullptr){
		GList*lst=list;
		for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(n,d)==0){
				GtkWidget*scrl=get_pan_from_menu(menu_item);
				addatnames(frontname,msg,scrl);
				prealert(nb,scrl);
				if(is_privmsg)exec_nm
				novel=FALSE;
				break;
			}
			list=g_list_next(list);
			if(list==nullptr)break;
		}
		g_list_free(lst);
	}
	if(novel){
		if(talk_user(n)){
			GtkWidget*scrl=name_join_nb(n,nb);addatnames(frontname,msg,scrl);
			alert(gtk_notebook_get_tab_label(nb,scrl),nb);
			if(is_privmsg){
				if(ps->welcome!=nullptr){
					if(ps->wnotice)send_msg_type(ps->nknnow,n,ps->welcome,scrl,not_msg_str " ");
					else send_msg(ps->nknnow,n,ps->welcome,scrl);
				}
				exec_nm
			}
		}
	}
}
static void pars_err(char*str,char*msg){
	GtkWidget*pg=chan_pan(str);
	if(pg!=nullptr){//e.g. ERR_CHANNELISFULL
		addatchans(user_error,msg,pg);
		return;
	}
	pg=name_off_pan(str);
	if(pg!=nullptr)addatnames(user_error,msg,pg);
}
static void line_switch(char*n,GtkWidget*from,GtkWidget*to,const char*msg){
	GList*list=gtk_container_get_children((GtkContainer*)from);
	if(list!=nullptr){
		GList*lst=list;
		for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(n,d)==0){//there is a conv with this channel nick
				g_object_ref(menu_item);
				gtk_container_remove((GtkContainer*)from, menu_item);
				gtk_container_add((GtkContainer*)to, menu_item);
				g_object_unref(menu_item);//to 1
				addatnames(user_info,msg,get_pan_from_menu(menu_item));
				break;
			}
			list=g_list_next(list);
			if(list==nullptr)break;
		}
		g_list_free(lst);
	}
}
static void counting_the_list(GtkWidget*w,const char*a){
	gtk_widget_set_has_tooltip(w,FALSE);
	char buf[digits_in_uint+counting_the_list_size+sizeof(list_end_str)];
	size_t n=(size_t)sprintf(buf,"%u %s" list_end_str,gtk_tree_model_iter_n_children(contf_get_model(w),nullptr),a);
	if(w==home_page)addattextmain(buf,n);
	else addatchans(user_info,buf,w);
}
static void names_end(GtkWidget*p,char*chan){
	counting_the_list(p,names_str);
	char c[chan_sz+1+digits_in_uint+1];
	GtkTreeIter it;char*text;
	BOOL b;
	gboolean ac=gtk_check_menu_item_get_active(channels_counted);
	if(ac){b=get_chan_counted(chan,c,&it,&text);}
	else b=get_chan_alpha(chan,c,&it,&text);
	if(b){
		int n;
		size_t len=strlen(c);
		sscanf(text+len+1,"%u",&n);
		g_free(text);
		GtkListStore*list=contf_get_list(p);
		int z=gtk_tree_model_iter_n_children ((GtkTreeModel*)list,nullptr);
		int dif=z-n;
		if(dif==0)return;
		if(ac){
			if(dif>0)chan_change_nr_gain(&it,chan,(unsigned int)z);
			else if(dif<0)chan_change_nr_loss(&it,chan,(unsigned int)z);
		}
		sprintf(c+len," %u",z);
		gtk_list_store_set(channels, &it, LIST_ITEM, c, -1);
	}
}
static void list_end(){
	if(gtk_widget_get_has_tooltip(home_page))//can be zero channels and this
		counting_the_list(home_page,chans_str);
}
static void send_autojoin(struct stk_s*ps){
	for(size_t i=0;i<ps->ajoins_sum;i++)
		if(ps->ajoins[i].c==ps->active){
			for(size_t j=0;ps->ajoins[i].chans[j]!=nullptr;j++)
				send_join(ps->ajoins[i].chans[j],strlen(ps->ajoins[i].chans[j]));
			break;
		}
}
static void action_to_close(){
	close_intention=TRUE;
	if(ssl!=nullptr)SSL_shutdown(ssl);
	else if(plain_socket!=-1)shutdown(plain_socket,2);
}
static gboolean incsafe(gpointer ps){
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wcast-qual"
	char*a=(char*)((struct stk_s*)ps)->dl->data;
	#pragma GCC diagnostic pop
	size_t s=((struct stk_s*)ps)->dl->len;
	if(a[s-1]=='\n')s--;
	if(a[s-1]=='\r')s--;
	a[s]='\0';
	//
	BOOL showmsg=((struct stk_s*)ps)->show_msgs;
	char com[8];
	if(sscanf(a,"%*s %7s",com)==1){
		size_t ln=strlen(com);
		char*b=strchr(a,' ')+1+ln;if(*b==' ')b++;
		char channm[chan_sz+1+digits_in_uint+1];//+ to set the "chan nr" at join on the same string
		char nicknm[namenul_sz];
		char c;
		BOOL is_privmsg=strcmp(com,priv_msg_str)==0;
		if(is_privmsg||strcmp(com,not_msg_str)==0){
			if(nick_extract(a,nicknm)){
				if(is_channel(b)){
					if(sscanf(b,channame_scan " %c",channm,&c)==2)pars_pmsg_chan(nicknm,channm,b+strlen(channm)+2,((struct stk_s*)ps)->notebook);
				}else if(sscanf(b,name_scan " %c",channm,&c)==2)pars_pmsg_name(nicknm,b+strlen(channm)+2,(struct stk_s*)ps,is_privmsg,nicknm);
			}
		}else if(strcmp(com,"JOIN")==0){
			int resp=nick_and_chan(a,b,nicknm,channm,((struct stk_s*)ps)->nknnow);
			if(resp==0)pars_join(channm,(struct stk_s*)ps);
			else if(resp==1){pars_join_user(channm,nicknm);line_switch(nicknm,name_off_menu,name_on_menu,"User Join");}
		}else if(strcmp(com,"PART")==0){
			int resp=nick_and_chan(a,b,nicknm,channm,((struct stk_s*)ps)->nknnow);
			if(resp==0)pars_part(channm,((struct stk_s*)ps)->notebook);
			else if(resp==1)pars_part_user(channm,nicknm);
		}else if(strcmp(com,"KICK")==0){
			if(sscanf(b,channame_scan " " name_scan,channm,nicknm)==2)
				pars_part_user(channm,nicknm);
		}else if(strcmp(com,"QUIT")==0){
			if(nick_extract(a,nicknm)){
				pars_quit(nicknm);
				line_switch(nicknm,name_on_menu,name_off_menu,"User Quit");
			}
		}else if(strcmp(com,mod_msg_str)==0){
			char mod[1+3+1];//"limit of three (3) changes per command for modes that take a parameter."
			if(sscanf(b,channame_scan " " mod_scan " " name_scan,channm,mod,nicknm)==3)
				pars_mod(channm,mod,nicknm);
			else if(sscanf(b,"%*s :" mod_scan,mod)==1)pars_mod_self((struct stk_s*)ps,mod);
		}else if(strcmp(com,"INVITE")==0){
			if(nick_extract(a,nicknm)&&sscanf(b,"%*s " channame_scan,channm)==1){
				char buf[name_sz+sizeof(invite_str)+chan_sz];//invite_str size includes null
				sprintf(buf,"%s" invite_str "%s",nicknm,channm);//is ok channm is only chan_sz here
				pars_pmsg_name(nicknm,buf,(struct stk_s*)ps,TRUE,"*Invite");
			}
		}else if(strlen(com)!=3)showmsg=FALSE;
		else{
			showmsg=TRUE;
			int d=atoi(com);//If no valid conversion could be performed, it returns zero;below,d==0
			if(d==RPL_LIST){
				if(show_msg!=RPL_LIST)showmsg=FALSE;
				unsigned int e;
				//if its >nr ,c is not 2
				if(sscanf(b,"%*s " channame_scan " %u",channm,&e)==2)
					if(test_to_add_chan(((struct stk_s*)ps),(int)e)){
						listing_test(home_page,channels)
						pars_chan(channm,e,((struct stk_s*)ps)->chans_max);
					}
			}
			//not on ircnet: else if(d==321)//RPL_LISTSTART
			else if(d==323){//RPL_LISTEND
				show_to_clause(RPL_LIST)
				list_end();
			}else if(d==RPL_NAMREPLY){
				if(show_msg!=RPL_NAMREPLY)showmsg=FALSE;
				if(sscanf(b,"%*s %*c " channame_scan,channm)==1){
					GtkWidget*p=chan_pan(channm);
					if(p!=nullptr){
						b=strchr(b,':');//join #q:w is error
						if(b!=nullptr)pars_names(p,b+1,s-(size_t)(b+1-a));
					}
				}
			}else if(d==366){//RPL_ENDOFNAMES
				show_to_clause(RPL_NAMREPLY)
				if(sscanf(b,"%*s " channame_scan,channm)==1){
					GtkWidget*p=chan_pan(channm);
					if(p!=nullptr)names_end(p,channm);//at a join
				}
			}else if(d==332){//RPL_TOPIC
				if(sscanf(b,name_scan " " channame_scan " %c",nicknm,channm,&c)==3)
					addatchans(user_topic,b+strlen(nicknm)+1+strlen(channm)+2,chan_pan(channm));
			}else if(d==319){//RPL_WHOISCHANNELS
				b=strchr(b,' ');
				if(b!=nullptr){
					b++;if(sscanf(b,name_scan " %c",nicknm,&c)==2)
						pars_wmod(nicknm,b+strlen(nicknm)+2);
				}
			}else if(d==5){//RPL_ISUPPORT
				char*e=strstr(b,"PREFIX=");
				if(e!=nullptr){
					sscanf(e+7,"(%6[^)])%6s",chanmodes,chanmodessigns);
					maximummodes=strlen(chanmodessigns);
				}
				e=strstr(b,"CHANTYPES=");
				if(e!=nullptr)sscanf(e+10,"%4s",chantypes);
			}else if(d==254){//RPL_LUSERCHANNELS
				send_autojoin((struct stk_s*)ps);
				//this not getting after first recv
				//another solution can be after 376 RPL_ENDOFMOTD
				//or after 1 second, not beautiful
				send_list
			}else if(d>400){//Error Replies.
				switch(d){
					//porbably deprecated
					//case 436://ERR_NICKCOLLISION
					//case 464://ERR_PASSWDMISMATCH
					//
					//rare,not tried
					//case 463://ERR_NOPERMFORHOST
					//case 465://ERR_YOUREBANNEDCREEP
					//
					case 432://ERR_ERRONEUSNICKNAME
					case 433://ERR_NICKNAMEINUSE
						action_to_close();
						break;
					default:
						b=strchr(b,' ');
						if(b!=nullptr){
							b++;if(sscanf(b,channame_scan " %c",channm,&c)==2)
								pars_err(channm,b+strlen(channm)+2);
						}
				}
			}else if(d==0)showmsg=FALSE;//"abc"
		}
	}else showmsg=FALSE;
	if(showmsg){
		a[s]='\n';addattextmain(a,s+1);
	}
	pthread_kill(threadid,SIGUSR1);
	return FALSE;
}
static void incomings(char*a,size_t n,struct stk_s*ps){
	struct data_len dl;dl.data=a;dl.len=n;
	ps->dl=&dl;
	g_idle_add(incsafe,ps);
	int out;sigwait(&threadset,&out);
}
static gboolean refresh_callback( gpointer ignored){
	(void)ignored;
	send_list
	return TRUE;
}
static void start_old_clear(GtkWidget*w,GtkNotebook*nb){
	GList*list=gtk_container_get_children((GtkContainer*)w);
	if(list!=nullptr){
		GList*lst=list;
		for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			GtkWidget*pan=get_pan_from_menu(menu_item);
			gtk_notebook_remove_page(nb,gtk_notebook_page_num(nb,pan));
			gtk_widget_destroy(menu_item);
			list=g_list_next(list);
			if(list==nullptr)break;
		}
		g_list_free(lst);
	}
}
static GtkWidget*tab_close_button(GtkNotebook*nb,GtkWidget*pan){
	GtkWidget*box=gtk_notebook_get_tab_label(nb,pan);
	GList*l=gtk_container_get_children((GtkContainer*)box);
	GtkWidget*b=(GtkWidget*)g_list_last(l)->data;
	g_list_free(l);
	return b;
}
static void close_buttons_handler(GtkNotebook*nb,void(*fn)(gpointer,gulong)){
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	if(list!=nullptr){
		GList*ls=list;for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			GtkWidget*pan=get_pan_from_menu(menu_item);
			GtkWidget*b=tab_close_button(nb,pan);
			fn(b,g_signal_handler_find(b,G_SIGNAL_MATCH_ID,g_signal_lookup("clicked", gtk_button_get_type()),0, nullptr, nullptr, nullptr));
			//
			gtk_widget_set_has_tooltip(pan,FALSE);//in the middle of the messages
			//
			list=g_list_next(list);
			if(list==nullptr)break;
		}g_list_free(ls);
	}
}
static gboolean senstartthreadsfunc(gpointer ps){
	g_signal_handler_unblock(((struct stk_s*)ps)->sen_entry,((struct stk_s*)ps)->sen_entry_act);
	//
	if(((struct stk_s*)ps)->refresh>0)
		((struct stk_s*)ps)->refreshid=g_timeout_add(1000*(unsigned int)((struct stk_s*)ps)->refresh,refresh_callback,nullptr);
	//
	close_buttons_handler(((struct stk_s*)ps)->notebook,g_signal_handler_unblock);
	g_signal_handler_unblock(((struct stk_s*)ps)->trv,((struct stk_s*)ps)->trvr);
	//
	gtk_list_store_clear(channels);
	//
	pthread_kill( threadid, SIGUSR1);
	can_send_data=TRUE;
	return FALSE;
}
static gboolean senstopthreadsfunc(gpointer ps){
	can_send_data=FALSE;
	g_signal_handler_block(((struct stk_s*)ps)->sen_entry,((struct stk_s*)ps)->sen_entry_act);
	//
	if(((struct stk_s*)ps)->refresh>0)
		g_source_remove(((struct stk_s*)ps)->refreshid);
	//
	close_buttons_handler(((struct stk_s*)ps)->notebook,g_signal_handler_block);
	g_signal_handler_block(((struct stk_s*)ps)->trv,((struct stk_s*)ps)->trvr);
	//
	gtk_widget_set_has_tooltip(home_page,FALSE);//in the middle of the messages
	//
	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
static BOOL irc_start(char*psw,char*nkn,struct stk_s*ps){
	size_t fln=strlen(ps->user_irc);
	size_t nkn_len=strlen(nkn);
	size_t nln=sizeof(nickname_con)-3+nkn_len;
	size_t pln=*psw=='\0'?0:(size_t)snprintf(nullptr,0,password_con,psw);
	char*i1=(char*)malloc(pln+nln+fln+irc_term_sz);
	BOOL out_v=TRUE;
	if(i1!=nullptr){
		if(*psw!='\0')sprintf(i1,password_con,psw);
		sprintf(i1+pln,nickname_con,nkn);
		memcpy(i1+pln+nln,ps->user_irc,fln);
		memcpy(i1+pln+nln+fln,irc_term,irc_term_sz);
		send_safe(i1,pln+nln+fln+irc_term_sz);
		free(i1);
		char*buf=(char*)malloc(irc_bsz);int bsz=irc_bsz;
		if(buf!=nullptr){
			int sz=recv_data(buf,bsz);
			if(sz>0){//'the traditional "end-of-file" return'
				g_idle_add(senstartthreadsfunc,ps);
				int out;sigwait(&threadset,&out);
				do{
					if(sz==bsz&&buf[sz-1]!='\n'){
						void*re;
						do{
							re=realloc(buf,(size_t)bsz+irc_bsz);
							if(re==nullptr)break;
							buf=(char*)re;
							sz+=recv_data(buf+bsz,irc_bsz);
							bsz+=irc_bsz;
						}while(sz==bsz&&buf[sz-1]!='\n');
						if(re==nullptr)break;
					}
					char*b=buf;
					do{
						char*n=(char*)memchr(b,'\n',(size_t)sz);
						size_t s;
						if(n!=nullptr)s=(size_t)(n+1-b);
						else s=(size_t)sz;
						size_t number_of_times=4;
						if(s>4&&memcmp(b,"PING",number_of_times)==0){
							main_text(b,s);
							b[1]='O';
							send_safe(b,s);
						}else if(*b==':')incomings(b,s,ps);
						if(n!=nullptr)b=n+1;
						sz-=s;
					}while(sz>0);
					sz=recv_data(buf,bsz);
				}while(sz>0);
				g_idle_add(senstopthreadsfunc,ps);
				sigwait(&threadset,&out);
			}else out_v=FALSE;
			free(buf);
		}
	}
	return out_v;
}
static BOOL con_ssl(char*psw,char*nkn,struct stk_s*ps){
	const SSL_METHOD *method;
	SSL_CTX *ctx;
	BOOL r;
	main_text_s(ssl_con_try);
	method = SSLv23_client_method();//Set SSLv2 client hello, also announce SSLv3 and TLSv1
	ctx = SSL_CTX_new(method);
	if ( ctx != nullptr){
		SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);//Disabling SSLv2 will leave v3 and TSLv1 for negotiation
		ssl = SSL_new(ctx);
		if(ssl!=nullptr){
			if(SSL_set_fd(ssl, plain_socket)==1){
				//is waiting until timeout if not SSL// || printf("No SSL")||1
				if ( SSL_connect(ssl) == 1){
					main_text_s("Successfully enabled SSL/TLS session.\n");
					r=irc_start(psw,nkn,ps);
				}else r=FALSE;
			}else{main_text_s("Error: SSL_set_fd failed.\n");r=FALSE;}
			g_idle_add(close_ssl_safe,nullptr);
			int out;sigwait(&threadset,&out);
		}else r=FALSE;
		SSL_CTX_free(ctx);
	}else return FALSE;
	return r;
}
static BOOL con_plain(char*psw,char*nkn,struct stk_s*ps){
	main_text_s(ssl_con_plain);
	BOOL b=irc_start(psw,nkn,ps);
	return b;
}
static void clear_old_chat(GtkNotebook*nb){
	if(alert_counter>0){
		gtk_widget_hide(gtk_notebook_get_action_widget(nb,GTK_PACK_END));
		alert_counter=0;
	}
	start_old_clear(chan_menu,nb);
	start_old_clear(name_on_menu,nb);
	start_old_clear(name_off_menu,nb);
}
static void proced_core(struct stk_s*ps,char*hostname,char*psw,char*nkn,unsigned short*ports,size_t port_last,size_t swtch){
	unsigned char n;
	if(swtch==not_a_switch){
		GSList*lst=con_group;
		n=con_nr_max;
		for(;;){
			if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)lst->data))break;
			lst=lst->next;n--;
		}
	}else n=_con_nr_s;
	for(;;){
		size_t port_i=0;
		//if(swtch<=port_last&&(n==con_nr_righttype1||n==con_nr_righttype2))n--;//this is useless at :ssl;plain
		for(;;){
			if(swtch==port_i)n++;//this was moved up to let unencrypted only preferred
			unsigned short port1=ports[port_i];unsigned short port2=ports[port_i+1];
			for(;;){
				create_socket(hostname,port1);
				if(plain_socket != -1){
					BOOL r;
					if(n==_con_nr_su){
						r=con_ssl(psw,nkn,ps);
						if(r==FALSE){
							close_plain_safe
							create_socket(hostname,port1);
							if(plain_socket != -1)
								con_plain(psw,nkn,ps);
						}
					}else if(n==_con_nr_us){
						r=con_plain(psw,nkn,ps);
						if(r==FALSE){
							close_plain_safe
							create_socket(hostname,port1);
							if(plain_socket != -1)
								con_ssl(psw,nkn,ps);
						}
					}else if(n==_con_nr_s)con_ssl(psw,nkn,ps);
					else con_plain(psw,nkn,ps);//_con_nr_u
					close_plain_safe
				}
				if(close_intention)return;
				main_text_s("Will try to reconnect after " INT_CONV_STR(wait_recon) " seconds.\n");
				for(unsigned int i=0;i<wait_recon;i++){
					sleep(1);
					if(close_intention)return;
				}
				if(port1==port2)break;
				if(port1<port2)port1++;
				else port1--;
			}
			if(port_i==port_last)break;
			port_i+=2;
			//if(swtch==port_i)n++;
		}
	}
}
static void proced(struct stk_s*ps){
	char hostname[hostname_sz];
	char psw[password_sz];char nkn[namenul_sz];
	unsigned short*ports;size_t port_last;size_t swtch;
	if(parse_host_str(ps->text,hostname,psw,nkn,&ports,&port_last,&swtch,ps)) {
		main_text_s("Connecting...\n");
		clear_old_chat(ps->notebook);
		proced_core(ps,hostname,psw,nkn,ports,port_last,swtch);
		free(ports);
		main_text_s("Disconnected.\n");
		gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,home_page));
	}else main_text_s("Error: Wrong input. For format, press the vertical ellipsis button and then Help.\n");
}
static gpointer worker (gpointer ps)
{
	//int s = 
	pthread_sigmask(SIG_BLOCK, &threadset, nullptr);
	//if (s == 0)
	proced((struct stk_s*)ps);
	con_th=-1;//nullptr;
	return nullptr;
}
static void save_combo_box(GtkTreeModel*list){
//can be from add, from remove,from test org con menu nothing
	GtkTreeIter it;
	if(info_path_name!=nullptr){
		int f=open(info_path_name,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
		if(f!=-1){
			BOOL i=FALSE;
			gboolean valid=gtk_tree_model_get_iter_first (list, &it);
			while(valid){
				gchar*text;
				gtk_tree_model_get (list, &it, 0, &text, -1);
				if(i){if(write(f,"\n",1)!=1){g_free(text);break;}}
				else i=TRUE;
				size_t sz=strlen(text);
				if((size_t)write(f,text,sz)!=sz){g_free(text);break;}
				g_free(text);
				valid = gtk_tree_model_iter_next( list, &it);
			}
			close(f);
		}
	}
}

static void set_combo_box_text(GtkComboBox * box,const char*txt) 
{
	GtkTreeIter iter;
	gboolean valid;
	int i;
	GtkTreeModel * list_store = gtk_combo_box_get_model(box);
	// Go through model's list and find the text that matches, then set it active
	//column 0 with type G_TYPE_STRING, you would write: gtk_tree_model_get (model, iter, 0,
	i = 0; 
	valid = gtk_tree_model_get_iter_first (list_store, &iter);
	while (valid) {
		gchar *item_text;
		gtk_tree_model_get (list_store, &iter, 0, &item_text, -1);
		if (strcmp(item_text, txt) == 0) { 
			gtk_combo_box_set_active(box, i);
			g_free(item_text);
			return;
		}    
		g_free(item_text);
		i++; 
		valid = gtk_tree_model_iter_next( list_store, &iter);
	}
	gtk_combo_box_text_append_text((GtkComboBoxText*)box,txt);
	save_combo_box(list_store);
	gtk_combo_box_set_active(box, i);
}
static void ignores_init(struct stk_s*ps,int active){
	for(size_t i=0;i<ps->ignores_sum;i++){
		if(ps->ignores[i].c==active){
			ignores=ps->ignores[i].chans;
			return;
		}
	}
	ignores=&dummy;
}
static gboolean enter_recallback( gpointer ps){
	const char* t=gtk_entry_get_text ((GtkEntry*)((struct stk_s*)ps)->con_entry);
	if(strlen(t)>0){
		if(con_th==0){//con_th!=nullptr){
			action_to_close();
			g_timeout_add(1000,enter_recallback,ps);
			return FALSE;
		}
		set_combo_box_text((GtkComboBox*)((struct stk_s*)ps)->cbt,t);
		int active=gtk_combo_box_get_active((GtkComboBox*)((struct stk_s*)ps)->cbt);
		((struct stk_s*)ps)->text=t;((struct stk_s*)ps)->active=active;
		ignores_init((struct stk_s*)ps,active);
		close_intention=FALSE;
		con_th = pthread_create( &threadid, nullptr, worker,ps);
	}
	//unblock this ENTER
	g_signal_handler_unblock(((struct stk_s*)ps)->con_entry,((struct stk_s*)ps)->con_entry_act);
	return FALSE;
}
static void enter_callback( gpointer ps){
	//block this ENTER
	g_signal_handler_block(((struct stk_s*)ps)->con_entry,((struct stk_s*)ps)->con_entry_act);
	enter_recallback(ps);
}
static void info_path_name_set_val(const char*a,char*b,size_t i,size_t j){
	info_path_name=(char*)malloc(i+2+j+5);
	if(info_path_name!=nullptr){
		memcpy(info_path_name,a,i);
		info_path_name[i]='/';
		info_path_name[i+1]='.';
		char*c=info_path_name+i+2;
		memcpy(c,b,j);
		memcpy(c+j,"info",5);
	}
}
static void info_path_name_set(char*a){
	char*h=getenv("HOME");
	if(h!=nullptr){
		char*b=basename(a);
		size_t i=strlen(h);
		size_t j=strlen(b);
		info_path_name_set_val(h,b,i,j);//sizeof(HOMEDIR)-1
	}
}
static void info_path_name_restore(GtkComboBoxText*cbt,GtkWidget*entext,struct stk_s*ps){
	if(info_path_name!=nullptr){
		int f=open(info_path_name,O_RDONLY);
		if(f!=-1){
			size_t sz=(size_t)lseek(f,0,SEEK_END);
			if(sz>0){
				char*r=(char*)malloc(sz+1);
				if(r!=nullptr){
					lseek(f,0,SEEK_SET);
					read(f,r,sz);
					char*a=r;
					for(size_t i=0;i<sz;i++){
						if(r[i]=='\n'){
							r[i]='\0';
							gtk_combo_box_text_append_text(cbt,a);
							a=&r[i]+1;
						}
					}
					r[sz]='\0';
					gtk_combo_box_text_append_text(cbt,a);
					free(r);
					if(autoconnect!=-1){
						gtk_combo_box_set_active((GtkComboBox*)cbt,autoconnect);//void
						gtk_widget_activate(entext);
					}else gtk_combo_box_set_active((GtkComboBox*)cbt,0);
				}
			}
		}
	}
}
static int get_pos_from_model(GtkTreeModel*mod,GtkTreeIter*it){
	GtkTreePath * path = gtk_tree_model_get_path ( mod , it ) ;
	int i= (gtk_tree_path_get_indices ( path ))[0] ;
	gtk_tree_path_free(path);
	return i;
}
static int organize_connections_ini(GtkTreeView*tv,GtkTreeModel**mod,GtkTreeIter*it){
	GtkTreeSelection *sel=gtk_tree_view_get_selection(tv);
	gtk_tree_selection_get_selected (sel,mod,it);
	return get_pos_from_model(*mod,it);
}
static void organize_connections_dialog (GtkDialog *dialog, gint response, struct stk_s*ps){
	GtkTreeModel*mod;GtkTreeIter it;
	GtkTreeIter i2;
	if(response==1){
		int i = organize_connections_ini(ps->tv,&mod,&it);
		gtk_combo_box_text_remove(ps->cbt,i);
		if(gtk_list_store_remove ((GtkListStore*)mod,&it)==FALSE&&i==0)//GtkListStore *
			organize_connections_dialog (dialog, 0, ps);
	}
	else if(response==2){
		int i = organize_connections_ini(ps->tv,&mod,&it);
		i2=it;
		if(gtk_tree_model_iter_previous(mod,&i2)){
			gtk_list_store_swap((GtkListStore*)mod,&it,&i2);
			GtkTreeModel*mdl=gtk_combo_box_get_model((GtkComboBox*)ps->cbt);
			gtk_tree_model_iter_nth_child(mdl,&it,nullptr,i);
			i2=it;
			gtk_tree_model_iter_previous(mdl,&i2);
			gtk_list_store_swap((GtkListStore*)mdl,&it,&i2);
		}
	}
	else if(response==3){
		int i = organize_connections_ini(ps->tv,&mod,&it);
		i2=it;
		if(gtk_tree_model_iter_next(mod,&i2)){
			gtk_list_store_swap((GtkListStore*)mod,&it,&i2);
			GtkTreeModel*mdl=gtk_combo_box_get_model((GtkComboBox*)ps->cbt);
			gtk_tree_model_iter_nth_child(mdl,&it,nullptr,i);
			i2=it;
			gtk_tree_model_iter_next(mdl,&i2);
			gtk_list_store_swap((GtkListStore*)mdl,&it,&i2);
		}
	}
	else{// response==0 || X button is GTK_RESPONSE_DELETE_EVENT
		save_combo_box(gtk_combo_box_get_model((GtkComboBox*)ps->cbt));
		gtk_widget_destroy((GtkWidget*)dialog);
	}
}
static void cell_edited_callback(struct stk_s*ps,gchar *path,gchar *new_text){
	GtkTreeIter iter;
	gtk_tree_model_get_iter_from_string((GtkTreeModel*)ps->org_tree_list,&iter,path);
	gtk_list_store_set(ps->org_tree_list, &iter, LIST_ITEM, new_text, -1);
	int i=get_pos_from_model((GtkTreeModel*)ps->org_tree_list, &iter);
	GtkTreeModel*mdl=gtk_combo_box_get_model((GtkComboBox*)ps->cbt);
	gtk_tree_model_iter_nth_child(mdl,&iter,nullptr,i);
	gtk_list_store_set((GtkListStore*)mdl, &iter, LIST_ITEM, new_text, -1);
}
static void organize_connections (struct stk_s*ps){
	GtkTreeModel * list = gtk_combo_box_get_model((GtkComboBox*)ps->cbt);
	GtkTreeIter iterFrom;
	gboolean valid = gtk_tree_model_get_iter_first (list, &iterFrom);
	GtkWidget *dialog;
	if(valid){
		GtkWindow*top=(GtkWindow *)gtk_widget_get_toplevel ((GtkWidget *)ps->cbt);
		if(gtk_tree_model_iter_n_children (list,nullptr)>1)
			dialog = gtk_dialog_new_with_buttons ("Organize Connections",
			    top, (GtkDialogFlags)(GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL),
			    "Move _Up",2,"Move D_own",3,"D_elete",1,"_Done",0,nullptr);
		else
			dialog = gtk_dialog_new_with_buttons ("Organize Connections",
			    top, (GtkDialogFlags)(GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL),
			    "D_elete",1,"_Done",0,nullptr);
		GtkWidget *tree=gtk_tree_view_new();ps->tv=(GtkTreeView*)tree;
		//
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;
		GtkListStore *store;
		gtk_tree_view_set_headers_visible((GtkTreeView*)tree,FALSE);
		renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, nullptr);
		store= gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
		ps->org_tree_list=store;
		g_signal_connect_data (renderer, "edited",G_CALLBACK (cell_edited_callback),ps,nullptr,G_CONNECT_SWAPPED);
		column = gtk_tree_view_column_new_with_attributes("", renderer, "text", LIST_ITEM, nullptr);
		gtk_tree_view_append_column((GtkTreeView*)tree, column);
		gtk_tree_view_set_model((GtkTreeView*)tree, (GtkTreeModel*)store);
		g_object_unref(store);
		//
		GtkTreeIter iterTo;
		int i=0;
		do{
			gchar *item_text;
			gtk_tree_model_get (list, &iterFrom, 0, &item_text, -1);
			//
			gtk_list_store_append(store, &iterTo);
			gtk_list_store_set(store, &iterTo, LIST_ITEM, item_text, -1);
			//
			g_free(item_text);
			i++; 
			valid = gtk_tree_model_iter_next( list, &iterFrom);
		}while (valid);
		//
		int w;int h;
		gtk_window_get_size (top,&w,&h);
		gtk_window_set_default_size((GtkWindow*)dialog,w,h);
		GtkWidget*scrolled_window = gtk_scrolled_window_new (nullptr, nullptr);
		gtk_container_add((GtkContainer*)scrolled_window,tree);
		GtkWidget*box=gtk_dialog_get_content_area((GtkDialog*)dialog);
		gtk_box_pack_start((GtkBox*)box, scrolled_window, TRUE, TRUE, 0);
	}else{
		dialog = gtk_dialog_new_with_buttons ("Organize Connections",
			(GtkWindow *)gtk_widget_get_toplevel ((GtkWidget *)ps->cbt),
			(GtkDialogFlags)(GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL),
			"_Done",0,nullptr);
	}
	g_signal_connect_data (dialog, "response",G_CALLBACK (organize_connections_dialog),ps,nullptr,(GConnectFlags) 0);
	gtk_widget_show_all (dialog);
}
static gboolean prog_menu_popup (GtkMenu*menu,GdkEvent*evn){
	gtk_menu_popup_at_pointer(menu,evn);
	return FALSE;
}
static void help_popup(struct stk_s*ps){
	GtkWidget *dialog = gtk_dialog_new_with_buttons ("Help",
			    ps->main_win, (GtkDialogFlags)(GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL),
			    "_OK",GTK_RESPONSE_NONE,nullptr);
	int w;int h;
	gtk_window_get_size (ps->main_win,&w,&h);
	gtk_window_set_default_size((GtkWindow*)dialog,w,h);
	g_signal_connect_data (dialog,"response",G_CALLBACK (gtk_widget_destroy),
	                       nullptr,nullptr,(GConnectFlags)0);
	//
	GtkWidget*text;
	GtkWidget*scrolled_window = container_frame_name_out(&text);
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer ((GtkTextView*)text);
	gtk_text_buffer_set_text (text_buffer,help_text,sizeof(help_text)-1);
	//
	GtkTextIter it;
	gtk_text_buffer_get_end_iter(text_buffer,&it);
	gtk_text_buffer_insert(text_buffer,&it,"\n\nArguments:\n",-1);
	for(unsigned int i=0;i<number_of_args;i++){
		if(i>0)gtk_text_buffer_insert(text_buffer,&it," ",1);
		gtk_text_buffer_insert(text_buffer,&it,ps->args[i],-1);
		gtk_text_buffer_insert(text_buffer,&it,",",1);
		gtk_text_buffer_insert(text_buffer,&it,&ps->args_short[i],1);
	}
	//
	if(ps->argc==1)gtk_text_buffer_insert(text_buffer,&it,"\n\nNo argument was provided.\n",-1);
	else{
		gtk_text_buffer_insert(text_buffer,&it,"\n\nReceived argument",-1);
		if(ps->argc>2)gtk_text_buffer_insert(text_buffer,&it,"s",-1);
		gtk_text_buffer_insert(text_buffer,&it,":\n",-1);
		for(int i=1;i<ps->argc;i++){
			if(i>1)gtk_text_buffer_insert(text_buffer,&it," ",1);
			BOOL a=strchr(ps->argv[i],' ')!=nullptr;
			if(a)gtk_text_buffer_insert(text_buffer,&it,"\"",1);
			gtk_text_buffer_insert(text_buffer,&it,ps->argv[i],-1);
			if(a)gtk_text_buffer_insert(text_buffer,&it,"\"",1);
		}
	}
	//
	GtkWidget*box=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_box_pack_start((GtkBox*)box, scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show_all (dialog);
}
static BOOL icmpAmemBstr(const char*s1,const char*s2){
	for(size_t i=0;;i++){
		char a=s1[i];
		char c=a-s2[i];
		if(c!='\0'){
			if('A'<=a&&a<='Z'){if(a+('a'-'A')!=s2[i])return FALSE;}
			else if('a'<=a&&a<='z'){if(a-('a'-'A')!=s2[i])return FALSE;}
			else if(s2[i]=='\0')return TRUE;
			else return FALSE;
		}else if(a=='\0')return TRUE;
	}
}
#define is_home(a) *a==*home_string
static void send_activate(GtkEntry*entry,struct stk_s*ps){
	GtkEntryBuffer*t=gtk_entry_get_buffer(entry);
	const char*text=gtk_entry_buffer_get_text(t);
	//
	GtkWidget*pg=gtk_notebook_get_nth_page(ps->notebook,gtk_notebook_get_current_page(ps->notebook));
	const char*a=gtk_notebook_get_menu_label_text(ps->notebook,pg);
	if(is_home(a)){
		show_from_clause(text,"list",RPL_LIST)
		else show_from_clause(text,"names",RPL_NAMREPLY)
		size_t sz=strlen(text);
		char*b=(char*)malloc(sz+irc_term_sz);
		if(b==nullptr)return;
		memcpy(b,text,sz);
		memcpy(b+sz,irc_term,irc_term_sz);
		send_data(b,sz+irc_term_sz);
		free(b);
	}else send_msg(ps->nknnow,a,text,pg);
	if(ps->send_history>0){
		if(send_entry_list->length==ps->send_history)g_free(g_queue_pop_head(send_entry_list));
		g_queue_push_tail(send_entry_list,g_strdup(text));
		send_entry_list_cursor=nullptr;
	}
	gtk_entry_buffer_delete_text(t,0,-1);
}
#define menu_con_add_item(n,s,a,b,c,d)\
a = gtk_radio_menu_item_new_with_label (b, s);\
b = gtk_radio_menu_item_get_group((GtkRadioMenuItem*)a);\
if (d->con_type==n)gtk_check_menu_item_set_active ((GtkCheckMenuItem*)a, TRUE);\
gtk_menu_shell_append (c,a)
static void con_click(GtkWidget*en){
	gtk_widget_activate(en);
}
static void clipboard_tev(GtkNotebook*notebook){
	GtkWidget*pg=gtk_notebook_get_nth_page(notebook,gtk_notebook_get_current_page(notebook));
	const char*a=gtk_notebook_get_menu_label_text(notebook,pg);
	GtkTextBuffer *buffer;
	if(is_home(a))buffer = gtk_text_view_get_buffer(text_view);
	else if(is_channel(a))buffer=gtk_text_view_get_buffer(contf_get_textv(pg));
	else buffer=gtk_text_view_get_buffer((GtkTextView*)gtk_bin_get_child((GtkBin*)pg));
	GtkTextIter start;GtkTextIter end;
	gtk_text_buffer_get_bounds (buffer, &start, &end);
	char*text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
	//an allocated UTF-8 string
	gtk_clipboard_set_text (gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),text,-1);
	g_free(text);
}
static void channels_sort(){
	send_list_if
}
static void chan_reMin_response (GtkDialog *dialog,gint response,int*chan_min){
	if(response==GTK_RESPONSE_OK){
		GList*l=gtk_container_get_children((GtkContainer*)gtk_dialog_get_content_area(dialog));
		const char*text=gtk_entry_get_text((GtkEntry*)l->data);
		g_list_free(l);
		*chan_min=atoi(text);
		send_list_if
	}
	gtk_widget_destroy((GtkWidget*)dialog);
}
static void chan_reMin(struct stk_s*ps){
	GtkWidget *dialog = gtk_dialog_new_with_buttons ("Channel Minimum Users",
			    ps->main_win, (GtkDialogFlags)(GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL),
			    "_OK",GTK_RESPONSE_OK,nullptr);
	GtkWidget*entry = gtk_entry_new();
	char buf[digits_in_uint+1];
	sprintf(buf,"%u",ps->chan_min);
	gtk_entry_set_placeholder_text((GtkEntry*)entry,buf);
	g_signal_connect_data (dialog,"response",G_CALLBACK (chan_reMin_response),
	                       &ps->chan_min,nullptr,(GConnectFlags)0);
	GtkWidget*box=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_box_pack_start((GtkBox*)box, entry, TRUE, TRUE, 0);
	gtk_widget_show_all (dialog);
}
static void reload_tabs(GtkWidget*menu_from,GtkWidget*menu,GtkNotebook*notebook){
	GList*list=gtk_container_get_children((GtkContainer*)menu_from);
	if(list!=nullptr){
		GList*lst=list;
		for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			add_new_tab_menuitem(get_pan_from_menu(menu_item)
				,gtk_menu_item_get_label((GtkMenuItem*)menu_item),notebook,menu);
			list=g_list_next(list);
			if(list==nullptr)break;
		}
		g_list_free(lst);
	}
}
static gboolean prog_key_press (struct stk_s*ps, GdkEventKey  *event){
	if(event->type==GDK_KEY_PRESS){
		if((event->state&GDK_CONTROL_MASK)!=0){
			unsigned int K=gdk_keyval_to_upper(event->keyval);
			if(K==GDK_KEY_T){
				GList*lst=gtk_container_get_children((GtkContainer*)menuwithtabs);
				GList*list=lst;
				for(;;){
					list=g_list_next(list);
					if(list==nullptr)break;
					gtk_widget_destroy((GtkWidget*)list->data);
				}
				g_list_free(lst);
				reload_tabs(chan_menu,menuwithtabs,ps->notebook);
				reload_tabs(name_on_menu,menuwithtabs,ps->notebook);
				reload_tabs(name_off_menu,menuwithtabs,ps->notebook);
				gtk_menu_popup_at_widget((GtkMenu*)menuwithtabs,(GtkWidget*)ps->notebook,GDK_GRAVITY_NORTH_WEST,GDK_GRAVITY_NORTH_WEST,nullptr);
			}else if(K==GDK_KEY_C){
				GtkWidget*pg=gtk_notebook_get_nth_page(ps->notebook,gtk_notebook_get_current_page(ps->notebook));
				if(is_home(gtk_notebook_get_menu_label_text(ps->notebook,pg))==FALSE)gtk_button_clicked((GtkButton*)tab_close_button(ps->notebook,pg));
			}else if(K==GDK_KEY_Q)action_to_close();
			else if(K==GDK_KEY_X)g_application_quit(ps->app);
		}else if(event->keyval==GDK_KEY_Up&&gtk_widget_is_focus(ps->sen_entry)){
			if(send_entry_list_cursor!=send_entry_list->head){
				send_entry_list_cursor=send_entry_list_cursor==nullptr?
					send_entry_list->tail
					:send_entry_list_cursor->prev;
				gtk_entry_set_text((GtkEntry*)ps->sen_entry,(const char*)send_entry_list_cursor->data);
				return TRUE;//lost focus other way
			}
		}else if(event->keyval==GDK_KEY_Down&&gtk_widget_is_focus(ps->sen_entry)){
			if(send_entry_list_cursor!=nullptr){
				send_entry_list_cursor=send_entry_list_cursor->next;
				GtkEntryBuffer*buf=gtk_entry_get_buffer((GtkEntry*)ps->sen_entry);
				gtk_entry_buffer_delete_text(buf,0,-1);
				if(send_entry_list_cursor!=nullptr)gtk_entry_buffer_insert_text(buf,0,(const char*)send_entry_list_cursor->data,-1);
				return TRUE;//is trying to switch focus
			}
		}
	}
	return FALSE;//propagation seems fine
}
static void gather_parse(size_t*sum,char*mem,struct ajoin**ons){
	*sum=0;
	for(size_t i=0;;i++){
		BOOL b=mem[i]=='\0';
		if(mem[i]==' '||b){
			*sum=*sum+1;
			if(b)break;
			else mem[i]='\0';
		}
	}
	//
	struct ajoin*ins=(struct ajoin*)malloc((*sum)*sizeof(struct ajoin));
	if(ins==nullptr){*sum=0;g_free(mem);return;}
	*ons=ins;
	size_t j=0;size_t k=0;
	for(size_t i=0;;){
		for(;mem[j]!='\0';j++){
			if(mem[j]==','){
				mem[j]='\0';
				break;
			}
		}
		ins[i].c=atoi(&mem[k]);
		j++;k=j;
		size_t m=0;
		for(;;j++){
			BOOL b=mem[j]=='\0';
			if(mem[j]==','||b){
				m++;
				if(b)break;else mem[j]='\0';
			}
		}
		ins[i].chans=(char**)malloc(sizeof(char*)*(m+1));
		if(ins[i].chans==nullptr)ins[i].chans=&dummy;
		else{
			j=k;
			for(size_t l=0;;){
				if(mem[j]=='\0'){
					ins[i].chans[l]=&mem[k];
					l++;if(l==m){ins[i].chans[l]=nullptr;break;}
					k=j+1;
				}
				j++;
			}
		}
		i++;if(i==*sum)break;
		j++;k=j;
	}
}
static void gather_free(size_t sum,char*mem,struct ajoin*ins){
	if(sum>0){
		g_free(mem);
		for(size_t i=0;i<sum;i++)if(ins[i].chans!=&dummy)
			free(ins[i].chans);
		free(ins);
	}
}
static void
activate (GtkApplication* app,
          struct stk_s*ps)
{
	ps->app=(GApplication*)app;
	/* Create a window with a title, and a default size */
	GtkWidget *window = gtk_application_window_new (app);
	menuwithtabs=gtk_menu_new();
	g_signal_connect_data (window, "destroy",G_CALLBACK(gtk_widget_destroy),menuwithtabs,nullptr,G_CONNECT_SWAPPED);
	//
	gtk_window_set_title ((GtkWindow*) window, "IRC");
	if(ps->dim[0]!=-1)
		gtk_window_set_default_size ((GtkWindow*) window, ps->dim[0], ps->dim[1]);
	//
	GdkPixbuf*p=gdk_pixbuf_new_from_data (icon16,GDK_COLORSPACE_RGB,FALSE,8,16,16,3*16,nullptr,nullptr);
	gtk_window_set_icon((GtkWindow*)window, p);
	g_object_unref(p);
	//
	ps->notebook = (GtkNotebook*)gtk_notebook_new ();
	home_page=container_frame(ps->separator,G_CALLBACK(chan_join),ps->notebook);
	g_signal_connect_data (home_page, "query-tooltip",G_CALLBACK (home_page_tooltip),nullptr,nullptr,(GConnectFlags)0);
	text_view=contf_get_textv(home_page);
	ps->trv=(GtkWidget*)contf_get_treev(home_page);
	channels=(GtkListStore*)gtk_tree_view_get_model((GtkTreeView*)ps->trv);
	//__asm("int $3");
	GType trv_gt=gtk_button_get_type();
	guint trv_gsl=g_signal_lookup("button-release-event", trv_gt);
	ps->trvr=g_signal_handler_find(ps->trv,G_SIGNAL_MATCH_ID,trv_gsl,0,nullptr,nullptr,nullptr);
	g_signal_handler_block(ps->trv,ps->trvr);//warning without
	//
	gtk_notebook_set_scrollable(ps->notebook,TRUE);
	gtk_notebook_popup_enable(ps->notebook);
	gtk_notebook_append_page_menu (ps->notebook, home_page, gtk_label_new (home_string), gtk_label_new (home_string));//i dont like the display (at 2,3..) without the last parameter
	gtk_notebook_set_tab_reorderable(ps->notebook, home_page, TRUE);
	add_new_tab_menuitem(home_page,home_string,ps->notebook,menuwithtabs);
	//
	sigemptyset(&threadset);
	sigaddset(&threadset, SIGUSR1);
	GtkWidget*en=gtk_combo_box_text_new_with_entry();
	GtkWidget*entext=gtk_bin_get_child((GtkBin*)en);
	ps->con_entry=entext;//this for timeouts
	ps->con_entry_act=g_signal_connect_data (entext, "activate",G_CALLBACK (enter_callback),ps,nullptr,G_CONNECT_SWAPPED);
	ps->cbt=(GtkComboBoxText*)en;
	//
	GtkWidget*con=gtk_button_new();
	GtkWidget*conimg=gtk_image_new_from_icon_name ("go-next",GTK_ICON_SIZE_MENU);
	gtk_button_set_image((GtkButton*)con,conimg);
	g_signal_connect_data (con, "clicked",G_CALLBACK (con_click),entext,nullptr,G_CONNECT_SWAPPED);
	//
	GtkWidget *org=gtk_button_new_with_label("\u22EE");
	GtkWidget *menu = gtk_menu_new ();
	//
	GtkWidget *menu_item = gtk_menu_item_new_with_label ("Organize Connections");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (organize_connections),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	menu_item = gtk_menu_item_new_with_label ("Channels");
	chan_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu((GtkMenuItem*)menu_item,chan_menu);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	menu_item = gtk_menu_item_new_with_label ("Names Online");
	name_on_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu((GtkMenuItem*)menu_item,name_on_menu);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	menu_item = gtk_menu_item_new_with_label ("Names Offline");
	name_off_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu((GtkMenuItem*)menu_item,name_off_menu);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	show_time=(GtkCheckMenuItem*)gtk_check_menu_item_new_with_label("Show Message Timestamp");
	if(ps->timestamp)gtk_check_menu_item_set_active(show_time,TRUE);
	gtk_menu_shell_append ((GtkMenuShell*)menu,(GtkWidget*)show_time);gtk_widget_show((GtkWidget*)show_time);
	//
	GtkWidget*menu_con=gtk_menu_item_new_with_label("Connection Type");
	GtkMenuShell*menucon=(GtkMenuShell*)gtk_menu_new();
	con_group=nullptr;
	menu_con_add_item(_con_nr_su,con_nr_su,menu_item,con_group,menucon,ps);//0x31
	menu_con_add_item(_con_nr_us,con_nr_us,menu_item,con_group,menucon,ps);
	menu_con_add_item(_con_nr_s,con_nr_s,menu_item,con_group,menucon,ps);
	menu_con_add_item(_con_nr_u,con_nr_u,menu_item,con_group,menucon,ps);
	gtk_menu_item_set_submenu((GtkMenuItem*)menu_con,(GtkWidget*)menucon);
	gtk_menu_shell_append ((GtkMenuShell*)menu,menu_con);
	gtk_widget_show_all(menu_con);
	//
	menu_item = gtk_menu_item_new_with_label ("Copy to Clipboard");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (clipboard_tev),ps->notebook,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	channels_counted=(GtkCheckMenuItem*)gtk_check_menu_item_new_with_label("Sort Channels by Number");
	gtk_check_menu_item_set_active(channels_counted,TRUE);
	g_signal_connect_data (channels_counted, "toggled",G_CALLBACK(channels_sort),nullptr,nullptr,(GConnectFlags)0);
	gtk_menu_shell_append ((GtkMenuShell*)menu,(GtkWidget*)channels_counted);gtk_widget_show((GtkWidget*)channels_counted);
	//
	menu_item = gtk_menu_item_new_with_label ("Channel Minimum Users");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (chan_reMin),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	menu_item = gtk_menu_item_new_with_label ("Shutdown Connection");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (action_to_close),nullptr,nullptr,(GConnectFlags)0);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	menu_item = gtk_menu_item_new_with_label ("Help");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (help_popup),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	menu_item = gtk_menu_item_new_with_label ("Exit Program");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (g_application_quit),app,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	g_signal_connect_data (org, "button-press-event",G_CALLBACK (prog_menu_popup),menu,nullptr,G_CONNECT_SWAPPED);
	//
	ps->sen_entry=gtk_entry_new();
	ps->sen_entry_act=g_signal_connect_data(ps->sen_entry,"activate",G_CALLBACK(send_activate),ps,nullptr,(GConnectFlags)0);
	g_signal_handler_block(ps->sen_entry,ps->sen_entry_act);
	//
	GtkWidget*top=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_start((GtkBox*)top,en,TRUE,TRUE,0);
	gtk_box_pack_start((GtkBox*)top,con,FALSE,FALSE,0);
	gtk_box_pack_start((GtkBox*)top,org,FALSE,FALSE,0);
	GtkWidget*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_pack_start((GtkBox*)box,top,FALSE,FALSE,0);
	gtk_box_pack_start((GtkBox*)box,(GtkWidget*)ps->notebook,TRUE,TRUE,0);
	gtk_box_pack_start((GtkBox*)box,ps->sen_entry,FALSE,FALSE,0);
	gtk_container_add ((GtkContainer*)window, box);
	//
	if(ps->maximize)gtk_window_maximize((GtkWindow*)window);
	if(ps->minimize)gtk_window_iconify((GtkWindow*)window);
	//
	gtk_widget_show_all (window);
	ps->main_win=(GtkWindow*)window;
	//
	GtkWidget*info=gtk_image_new_from_icon_name ("dialog-information",GTK_ICON_SIZE_MENU);
	gtk_notebook_set_action_widget(ps->notebook,info,GTK_PACK_END);
	g_signal_connect_data (ps->notebook, "switch-page",G_CALLBACK (nb_switch_page),ps->sen_entry,nullptr,(GConnectFlags)0);//this,before show,was critical;
	info_path_name_restore((GtkComboBoxText*)en,entext,ps);
	g_signal_connect_data (window, "key-press-event",G_CALLBACK (prog_key_press),ps,nullptr,G_CONNECT_SWAPPED);
}
static void parse_autojoin(struct stk_s*ps){
	gather_parse(&ps->ajoins_sum,ps->ajoins_mem,&ps->ajoins);
	if(autoconnect_pending){
		GDateTime*time_new_now=g_date_time_new_now_local();
		if(time_new_now!=nullptr){
			long long s=g_date_time_to_unix(time_new_now);
			g_date_time_unref(time_new_now);
			s/=60*60*24;
			s%=ps->ajoins_sum;
			autoconnect=ps->ajoins[s].c;
		}
	}
}
static gboolean autoconnect_callback(const gchar *option_name,const gchar *value,gpointer data,GError **error){
	(void)option_name;(void)data;(void)error;
	if(value==nullptr)autoconnect_pending=TRUE;
	else autoconnect=atoi(value);
	return TRUE;
}
static void remove_config(){
	if(info_path_name!=nullptr){
		if(access(info_path_name,F_OK)==0){
			puts("Would remove:");
			puts(info_path_name);
			puts("yes ?");
			int e=getchar();
			if(e=='y'){
				e=getchar();
				if(e=='e'){
					e=getchar();
					if(e=='s'){
						if(unlink(info_path_name)==0)printf("%s removed\n",info_path_name);
						return;
					}
				}
			}
			puts("expecting \"yes\"");
		}
	}
}
static gint handle_local_options (struct stk_s* ps, GVariantDict*options){
	char*result;
	if(g_variant_dict_lookup (options, ps->args[dimensions_id], "s", &result)){//missing argument is not reaching here
		char*b=strchr(result,'x');
		if(b!=nullptr){*b='\0';b++;}
		ps->dim[0]=atoi(result);
		ps->dim[1]=b!=nullptr?atoi(b):ps->dim[0];
		g_free(result);
	}else ps->dim[0]=-1;//this is default at gtk
	//
	if (g_variant_dict_lookup (options,ps->args[nick_id],"s",&ps->nick)==FALSE)
		ps->nick=nullptr;
	//
	if (g_variant_dict_lookup (options,ps->args[right_id], "i", &ps->separator)==FALSE)
		ps->separator=default_right;
	//
	if (g_variant_dict_lookup (options,ps->args[refresh_id], "i", &ps->refresh)==FALSE)
		ps->refresh=default_refresh;
	//
	if(g_variant_dict_lookup(options,ps->args[welcome_id],"s",&ps->welcome)==FALSE)
		ps->welcome=nullptr;
	//
	ps->timestamp=g_variant_dict_contains(options,ps->args[timestamp_id]);
	//
	if(g_variant_dict_lookup(options,ps->args[user_id],"s",&ps->user_irc))
		ps->user_irc_free=TRUE;//-Wstring-compare tells the result is unspecified against a #define
	else{ps->user_irc=default_user;ps->user_irc_free=FALSE;}
	//
	if (g_variant_dict_lookup (options,ps->args[chan_min_id], "i", &ps->chan_min)==FALSE)
		ps->chan_min=default_chan_min;
	//
	ps->visible=g_variant_dict_contains(options,ps->args[visible_id]);
	//
	GVariant*v=g_variant_dict_lookup_value(options,ps->args[log_id],G_VARIANT_TYPE_STRING);
	if(v!=nullptr){
		const char*a=g_variant_get_string(v,nullptr);
		log_file=open(a,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
	}
	//
	if(g_variant_dict_lookup(options,ps->args[ignore_id],"s",&ps->ignores_mem))
		gather_parse(&ps->ignores_sum,ps->ignores_mem,&ps->ignores);
	else ps->ignores_sum=0;
	//
	if(g_variant_dict_lookup(options,ps->args[run_id],"s",&ps->execute_newmsg)==FALSE)
		ps->execute_newmsg=nullptr;
	//
	if(g_variant_dict_lookup(options,ps->args[autojoin_id],"s",&ps->ajoins_mem))
		parse_autojoin(ps);
	else ps->ajoins_sum=0;
	//
	if (g_variant_dict_lookup (options,ps->args[password_id],"s",&ps->password)==FALSE)
		ps->password=nullptr;
	//
	ps->show_msgs=g_variant_dict_contains(options,ps->args[hide_id])==FALSE;
	//
	ps->maximize=g_variant_dict_contains(options,ps->args[maximize_id]);
	//
	ps->minimize=g_variant_dict_contains(options,ps->args[minimize_id]);
	//
	ps->wnotice=g_variant_dict_contains(options,ps->args[welcomeNotice_id]);
	//
	if (g_variant_dict_lookup (options,ps->args[chans_max_id], "i", &ps->chans_max)==FALSE)
		ps->chans_max=default_chans_max;
	//
	if (g_variant_dict_lookup (options,ps->args[send_history_id],"i",&ps->send_history)==FALSE)
		ps->send_history=default_send_history;

	//these are after allocs where set to allocated mem or 0/nullptr

	if(g_variant_dict_contains(options,ps->args[removeconf_id])/*true*/){
		remove_config();
		return 0;
	}

	int nr;
	if (g_variant_dict_lookup (options,ps->args[connection_number_id], "i", &nr)){//if 0 this is false here
		if(nr<con_nr_min||nr>con_nr_max){
			printf("%s must be from " con_nr_nrs " interval, \"%i\" given.\n",ps->args[connection_number_id],nr);
			return 0;
		}
		ps->con_type=(unsigned char)nr;
	}else ps->con_type=default_connection_number;

	return -1;
}
int main (int    argc,
      char **argv)
{
	  /* ---------------------------------------------------------- *
	   * initialize SSL library and register algorithms             *
	   * ---------------------------------------------------------- */
	if(OPENSSL_init_ssl(OPENSSL_INIT_NO_LOAD_SSL_STRINGS,nullptr)==1){
		struct stk_s ps;
		GtkApplication *app;
		app = gtk_application_new (nullptr, G_APPLICATION_FLAGS_NONE);

		//if(app!=nullptr){
		//QWERTYUIOP
		//ASDFgHJkL
		// ZXCVbNM
		ps.args[autoconnect_id]="autoconnect";ps.args_short[autoconnect_id]='a';
		const GOptionEntry autoc[]={{ps.args[autoconnect_id],ps.args_short[autoconnect_id],G_OPTION_FLAG_IN_MAIN|G_OPTION_FLAG_OPTIONAL_ARG,G_OPTION_ARG_CALLBACK,(gpointer)autoconnect_callback,"[=INDEX] optional value: autoconnect to that index. Else, autoconnect to an autojoin connection (the reminder of unix days % autojoin total).","INDEX"}
			,{nullptr,'\0',0,(GOptionArg)0,nullptr,nullptr,nullptr}};
		g_application_add_main_option_entries((GApplication*)app,autoc);
		ps.args[autojoin_id]=autojoin_str;ps.args_short[autojoin_id]='j';
		g_application_add_main_option((GApplication*)app,ps.args[autojoin_id],ps.args_short[autojoin_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Autojoin channels on connection index. e.g. \"2,#a,#b 4,#b,#z\"","\"I1,C1,C2...CN I2... ... IN...\"");
		ps.args[dimensions_id]="dimensions";ps.args_short[dimensions_id]='d';
		g_application_add_main_option((GApplication*)app,ps.args[dimensions_id],ps.args_short[dimensions_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Window size","WIDTH[xHEIGHT]");
		ps.args[chan_min_id]="chan_min";ps.args_short[chan_min_id]='m';
		g_application_add_main_option((GApplication*)app,ps.args[chan_min_id],ps.args_short[chan_min_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_INT,"Minimum users to list a channel(at " STR_INDIR(RPL_LIST) "). Default " INT_CONV_STR(default_chan_min) ".","NR");
		ps.args[chans_max_id]="chans_max";ps.args_short[chans_max_id]='s';
		g_application_add_main_option((GApplication*)app,ps.args[chans_max_id],ps.args_short[chans_max_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_INT,"Maximum channels in the list. Default " INT_CONV_STR(default_chans_max) ".","NR");
		ps.args[connection_number_id]="connection_number";ps.args_short[connection_number_id]='c';
		g_application_add_main_option((GApplication*)app,ps.args[connection_number_id],ps.args_short[connection_number_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_INT,INT_CONV_STR(_con_nr_su) "=" con_nr_su ", " INT_CONV_STR(_con_nr_us) "=" con_nr_us ", " INT_CONV_STR(_con_nr_s) "=" con_nr_s ", " INT_CONV_STR(_con_nr_u) "=" con_nr_u ". Default value is " INT_CONV_STR(default_connection_number) ".",con_nr_nrs);
		ps.args[hide_id]="hide";ps.args_short[hide_id]='h';
		g_application_add_main_option((GApplication*)app,ps.args[hide_id],ps.args_short[hide_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_NONE,"Don't display activity messages at " home_string " tab (join,part,...).",nullptr);
		ps.args[ignore_id]="ignore";ps.args_short[ignore_id]='i';
		g_application_add_main_option((GApplication*)app,ps.args[ignore_id],ps.args_short[ignore_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Ignore private messages from nicknames. The format is te same as \"" autojoin_str "\".","\"I1,N1,N2...NN I2... ... IN...\"");
		ps.args[log_id]="log";ps.args_short[log_id]='l';
		g_application_add_main_option((GApplication*)app,ps.args[log_id],ps.args_short[log_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Log private chat to filename.","FILENAME");//_FILENAME
		ps.args[maximize_id]="maximize";ps.args_short[maximize_id]='z';
		g_application_add_main_option((GApplication*)app,ps.args[maximize_id],ps.args_short[maximize_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_NONE,"Maximize window at launch.",nullptr);
		ps.args[minimize_id]="minimize";ps.args_short[minimize_id]='y';
		g_application_add_main_option((GApplication*)app,ps.args[minimize_id],ps.args_short[minimize_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_NONE,"Minimize(iconify) window at launch.",nullptr);
		ps.args[nick_id]="nick";ps.args_short[nick_id]='n';
		g_application_add_main_option((GApplication*)app,ps.args[nick_id],ps.args_short[nick_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Default nickname","NICKNAME");
		ps.args[password_id]="password";ps.args_short[password_id]='p';
		g_application_add_main_option((GApplication*)app,ps.args[password_id],ps.args_short[password_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Default password (blank overwrite with \"" parse_host_left "host...\", the format is at the g.u.i. help)","PASSWORD");
		ps.args[refresh_id]="refresh";ps.args_short[refresh_id]='f';
		g_application_add_main_option((GApplication*)app,ps.args[refresh_id],ps.args_short[refresh_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_INT,"Refresh channels interval in seconds. Default " INT_CONV_STR(default_refresh) ". Less than 1 to disable.","SECONDS");
		ps.args[right_id]="right";ps.args_short[right_id]='r';
		g_application_add_main_option((GApplication*)app,ps.args[right_id],ps.args_short[right_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_INT,"Right pane size, default " INT_CONV_STR(default_right),"WIDTH");
		ps.args[run_id]="run";ps.args_short[run_id]='x';
		g_application_add_main_option((GApplication*)app,ps.args[run_id],ps.args_short[run_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"If window is not active, run command line at new private messages.","COMMAND");
		ps.args[send_history_id]="send_history";ps.args_short[send_history_id]='o';
		g_application_add_main_option((GApplication*)app,ps.args[send_history_id],ps.args_short[send_history_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_INT,"Send history length (up/down at send entry). Default " INT_CONV_STR(default_send_history) ".","NR");
		ps.args[timestamp_id]="timestamp";ps.args_short[timestamp_id]='t';
		g_application_add_main_option((GApplication*)app,ps.args[timestamp_id],ps.args_short[timestamp_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_NONE,"Show message timestamp.",nullptr);
		ps.args[user_id]="user";ps.args_short[user_id]='u';
		g_application_add_main_option((GApplication*)app,ps.args[user_id],ps.args_short[user_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"User message. Default \"" default_user "\"","STRING");
		ps.args[visible_id]="visible";ps.args_short[visible_id]='v';
		g_application_add_main_option((GApplication*)app,ps.args[visible_id],ps.args_short[visible_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_NONE,"Counter with " mod_msg_str " " visible_mod " if server sends default invisible.",nullptr);
		ps.args[welcome_id]="welcome";ps.args_short[welcome_id]='w';
		g_application_add_main_option((GApplication*)app,ps.args[welcome_id],ps.args_short[welcome_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Welcome message sent in response when someone starts a conversation.","TEXT");
		ps.args[welcomeNotice_id]="welcome-notice";ps.args_short[welcomeNotice_id]='e';
		g_application_add_main_option((GApplication*)app,ps.args[welcomeNotice_id],ps.args_short[welcomeNotice_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_NONE,"Welcome message sent as a " not_msg_str " instead of " priv_msg_str ".",nullptr);
		ps.args[removeconf_id]="remove-config";ps.args_short[removeconf_id]='q';
		g_application_add_main_option((GApplication*)app,ps.args[removeconf_id],ps.args_short[removeconf_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_NONE,"Remove configuration and exit.",nullptr);
		g_signal_connect_data (app, "handle-local-options", G_CALLBACK (handle_local_options), &ps, nullptr,G_CONNECT_SWAPPED);
		g_signal_connect_data (app, "activate", G_CALLBACK (activate), &ps, nullptr,(GConnectFlags) 0);
		//  if(han>0)
		ps.argc=argc;ps.argv=argv;
		send_entry_list=g_queue_new();

		info_path_name_set(argv[0]);
		g_application_run ((GApplication*)app, argc, argv);//gio.h>gapplication.h gio-2.0
		g_object_unref (app);

		g_queue_free_full(send_entry_list,g_free);
		if(ps.nick!=nullptr)g_free(ps.nick);
		if(ps.welcome!=nullptr)g_free(ps.welcome);
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wcast-qual"		
		if(ps.user_irc_free)g_free((gpointer)ps.user_irc);
		#pragma GCC diagnostic pop
		if(info_path_name!=nullptr)free(info_path_name);
		if(log_file!=-1)close(log_file);
		if(ps.execute_newmsg!=nullptr)g_free(ps.execute_newmsg);
		gather_free(ps.ajoins_sum,ps.ajoins_mem,ps.ajoins);
		gather_free(ps.ignores_sum,ps.ignores_mem,ps.ignores);
	}else puts("openssl error");
	return EXIT_SUCCESS;
}

/* example entries for .sircinfo
irc.libera.chat:6697
chat.freenode.net:6697,7000,7070
@192.168.4.3     
@bucharest.ro.eu.undernet.org:6660-6669
chat.freenode.net:6697,7000,7070;6665-6667,8000-8002
@192.168.4.1
zonder:@irc.us.ircnet.net
@0.0.0.0
@192.168.4.3:6666-6667
@192.168.4.3:6667;6666,6667-6667
*/
