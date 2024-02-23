
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
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#include "inc/stat.h"
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

#include "icon.h"

static GtkTextView *text_view;static GtkWidget*home_page;static GtkListStore*channels;
static SSL *ssl=nullptr;static int plain_socket=-1;
static int con_th=-1;//static GThread*con_th=nullptr;
static BOOL close_intention;
#define ssl_con_try "Trying with SSL.\n"
#define ssl_con_plain "Trying unencrypted.\n"
#define irc_bsz 64
//"there are 510 characters maximum allowed for the command and its parameters"
#define irc_term "\r\n"
#define irc_term_sz sizeof(irc_term)-1

//#define _POSIX_HOST_NAME_MAX 255
#define hostname_sz 256//arranging
//at rfc level, for whois queries
//and "Servers are uniquely identified by their name, which has a maximum length of sixty three (63)"
//Back in ancient times (v1 era) the maximum was bumped up from 63 to 64 for user hostnames (for some reason).
#define hostnamerfc_sz 64
#define hostnamerfcnul_sz hostnamerfc_sz+1
#define hostnamerfc_scan "%64s"
#define chan_sz 50
//"up to fifty (50) characters"
#define channul_sz chan_sz+1
#define channame_scan "%50s"
#define name_sz 9
#define prefix_name_sz 1+name_sz
//"max. 9 characters"
#define namenul_sz name_sz+1
#define name_scan1 "%9"
#define name_scan name_scan1 "s"
#define mod_scan "%4s"
//not official: "Ident userid limited to 10 chars" //I think that includes ~
#define user_sz 10
#define usernul_sz user_sz+1
#define user_scan "%10s"

//hostname -.0..9a..z
#define nickname_start(a) ('A'<=*a&&*a<='}')
//nick format: A..}
//             -0..9; but not at [0]
//~ for owners, +q, tilde ascii is after }
//& for admins, +a
//@ for full operators, +o
//% for half operators, +h
//+ for voiced users, +v

#define password_sz 256
#define password_con "PASS %s" irc_term
#define nickname_con "NICK %s" irc_term
static char*info_path_name=nullptr;
#define not_a_nick_chan_host_start     "*"
#define home_string not_a_nick_chan_host_start "Home"
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

//QTO
//XC
#define help_text "Most of the parameters are set at start.\n\
Launch the program with --help argument for more info.\n\
Send irc commands from the " home_string " tab. Other tabs are sending " priv_msg_str " messages.\n\
\n\
Keyboard\n\
Ctrl+T = Tabs popup\n\
Shift+Ctrl+C = Close tab\n\
Ctrl+Q = Shutdown connection\n\
Ctrl+O = Open organizer\n\
Shift+Ctrl+X = Exit program\n\
\n\
Connection format:\n\
[[nickname" parse_host_delim "]password" parse_host_left "]hostname[" parse_host_delim "port1[" parse_host_ports_delim "portN][" parse_host_ports_micro "portM...][" parse_host_ports_macro "portP...]]\n\
A " parse_host_ports_macro_text " (" parse_host_ports_macro ") will override the connection type. Before " parse_host_ports_macro_text " are " con_nr_s " ports and after " parse_host_ports_macro_text " are " con_nr_u " ports.\n\
Escape " parse_host_left " in password with the uri format (\"%40\").\n\
e.g. newNick" parse_host_delim "a%40c" parse_host_left "127.0.0.1" parse_host_delim "7000" parse_host_ports_macro "6660" parse_host_ports_delim "6665" parse_host_ports_micro "6669"

struct data_len{
	const char*data;size_t len;
};
static pthread_t threadid;static sigset_t threadset;
static GtkWidget*chan_menu;
static GtkWidget*name_on_menu;static GtkWidget*name_off_menu;
static unsigned int alert_counter=0;
static GtkCheckMenuItem*show_time;static GtkCheckMenuItem*channels_counted;
//#define LIST_ITEM_OR_ORG_ID1 0
enum {
  LIST_ITEM = 0,
  N_COLUMNS
};//connections org,channels

#define number_of_args 23
//QWERTYUIOP
//ASDFgHJkL
// ZXCVbNM

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
	GtkWidget*organizer;
	BOOL handle_command_line_callback_was_executed;

	const char*proced_text;
	BOOL proced_switch;
	unsigned char proced_n;
	char*proced_hostname;

	GtkComboBox*organizer_dirs;GtkWidget*organizer_removeentry;
	                           GtkWidget*organizer_removerule;
	                           GtkToggleButton*organizer_del_confirmation;
	GtkNotebook*organizer_notebook;
	GtkWidget*organizer_entry_widget;GtkWidget*organizer_entry_widget_timestamp;
	GtkListStore*organizer_entry_names;
	GtkWidget*organizer_bot;
	BOOL organizer_can_add_names;
};
static int autoconnect=-1;static BOOL autoconnect_pending=FALSE;
static GSList*con_group;

static char chantypes[5];
static char chanmodes[7];
static char chanmodessigns[7];
static unsigned int maximummodes;

#define RPL_NONE -1
#define RPL_ISUPPORT 5
#define RPL_LUSERCHANNELS 254
#define RPL_AWAY 301
#define RPL_WHOISUSER 311
#define RPL_WHOISSERVER 312
#define RPL_WHOISIDLE 317
#define RPL_ENDOFWHOIS 318
#define RPL_WHOISCHANNELS 319
#define RPL_WHOISSPECIAL 320
#define RPL_WHOISACCOUNT 330
#define RPL_LIST 322
#define RPL_LISTEND 323
#define RPL_TOPIC 332
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define RPL_WHOISHOST 378
#define RPL_WHOISMODES 379
#define ERR_UNKNOWNERROR 400

#define digits_in_uint 10
#define digits_in_uintnul digits_in_uint+1
#define digits_in_posInt 10
#define digits_in_long 20
#define digits_in_longnul digits_in_long+1
//2 at 64, $((2**63))...

#define show_from_clause(a,b,c) if(icmpAmemBstrlownotempty(a,b))show_msg=c;
#define show_between_clause(a) if(show_msg!=a)showmsg=FALSE;
#define show_to_clause(a) if(show_msg==a)show_msg=RPL_NONE;else showmsg=FALSE;
static int show_msg=RPL_NONE;
static int log_file=-1;
static char*dummy=nullptr;
static char**ignores;
static BOOL can_send_data=FALSE;
#define chans_small_str "channels"
#define names_small_str "names"
#define counting_the_list_size (sizeof(chans_small_str)>sizeof(names_small_str)?sizeof(chans_small_str):sizeof(names_small_str))
#define list_end_str " listed\n"
#define autojoin_str "autojoin"
#define join_str  "JOIN"
#define names_str "NAMES"
#define whois_str "WHOIS"
#define counting_the_chanRel_size (sizeof(names_str)>sizeof(join_str)?sizeof(names_str):sizeof(join_str))
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
#define user_error not_a_nick_chan_host_start "Error"
#define user_error2 user_error "2"
#define user_topic not_a_nick_chan_host_start "Topic"
#define user_info not_a_nick_chan_host_start "Info"

#define homelocal ".local"
#define homeshare "share"
#define proj "sirc"
#define folder_separator "/"
#define folderseparator *folder_separator
#define removed_string " removed"
#define remove_ignored " ignored (maybe is not empty)"

#define dirback ".."
#define org_c "chans"
#define org_u "users"
#define org_g "global"
//#define LIST_ITEM_OR_ORG_ID1 0
enum {
  ORG_ID1     = 0,
  ORG_ID2     = 1,
  ORG_GEN     = 2,
  ORG_IDLE    = 3,
  ORG_SERVER  = 4,
  ORG_INDEX   = 5,
  ORG_CONV    = 6,
  ORG_N
};
struct org_col{
	int pos;
	GtkTreeSortable*sort;
};
#define org_new_names not_a_nick_chan_host_start "New"
struct organizer_from_storage{
	GtkComboBoxText*box;
	const char*server;
};
#define movestart not_a_nick_chan_host_start "Move"

#define LIST_ITEM_OR_ORG_ID1 0

#define new_line "\n"

struct name_pack{
	gchar*name;
	struct stk_s*ps;
};

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
#define addattextmain_n(a,b) addattextmain(a new_line,(b+1))
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
	gtk_text_buffer_insert(text_buffer,&it,new_line,1);
	//
	addattextview_scroll(b,v);
}
#define addatchans(n,msg,p) addattextv(contf_get_textv(p),n,msg)
static void addatnames(const char*n,const char*msg,GtkWidget*p,char*the_other_person){//the_other_person is null if  n  is the_other_person
	addattextv((GtkTextView*)gtk_bin_get_child((GtkBin*)p),n,msg);
	if(log_file!=-1){
		if(the_other_person==nullptr){
			write(log_file,n,strlen(n));
			write(log_file," " not_a_nick_chan_host_start,1+sizeof(not_a_nick_chan_host_start)-1);
		}else{
			if(*n!=*not_a_nick_chan_host_start){//send message
				write(log_file,not_a_nick_chan_host_start " ",sizeof(not_a_nick_chan_host_start)-1+1);
			}else{//info/error message
				write(log_file,n,strlen(n));
				write(log_file," ",1);
			}
			write(log_file,the_other_person,strlen(the_other_person));
		}
		char buf[1+digits_in_long+1+1];
		write(log_file,buf,(size_t)sprintf(buf," %ld ",time(nullptr)));//sizeof(time_t)==8?" %lld ":
		write(log_file,msg,strlen(msg));
		write(log_file,new_line,sizeof(new_line)-1);
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
//#define main_text_sn(a) main_text_s(a new_line)
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

			//dest_addr.sin_addr.s_addr = *(unsigned long*)((void*)(host->h_addr_list[0]));
			//  memset(&(dest_addr.sin_zero), '\0', 8);//string
			//"setting it to zero doesn't seem to be actually necessary"
			memcpy((char *) &dest_addr.sin_addr, (char *) host->h_addr,host->h_length);

			dest_addr.sin_port=htons(port);

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
		gchar*p=(char*)
#ifdef FN_G_MEMDUP2
		g_memdup2
#else
		g_memdup
#endif
		(indata+i,psz+1);
		p[psz]='\0';
		gchar*up=g_uri_unescape_string(p,nullptr);
		g_free(p);
		if(strlen(up)>=password_sz){g_free(up);return FALSE;}
		strcpy(psw,up);g_free(up);
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
		gchar*text;
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
		gchar*text;
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
static void send_channel_related(char*antet,char*item_text,size_t i){
	char buf[counting_the_chanRel_size+chan_sz+irc_term_sz];
	size_t a=strlen(antet);
	memcpy(buf,antet,a);buf[a]=' ';a++;
	memcpy(buf+a,item_text,i);
	memcpy(buf+a+i,irc_term,irc_term_sz);
	send_data(buf,a+irc_term_sz+i);
}
static void send_join(char*item_text,size_t i){
	send_channel_related((char*)join_str,item_text,i);
}
static gboolean chan_join(GtkTreeView *tree,GdkEvent*ignored,GtkNotebook*notebook){
	(void)ignored;
	GtkTreeSelection *sel=gtk_tree_view_get_selection(tree);
	GtkTreeIter iterator;
	if(gtk_tree_selection_get_selected (sel,nullptr,&iterator)){//can be no channel
		gchar*item_text;
		gtk_tree_model_get ((GtkTreeModel*)channels, &iterator, LIST_ITEM, &item_text, -1);
		for(size_t i=0;;i++){
			if(item_text[i]==' '){
				item_text[i]='\0';
				if(chan_not_joined(item_text,notebook))send_join(item_text,i);
				break;
			}
		}
		g_free(item_text);
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

static BOOL is_channel(const char*c){//prefixless names from notebook menu or from privmsg/invite incomings
	for(int i=0;;i++)if(chantypes[i]==*c)return TRUE;
		else if(chantypes[i]=='\0')return FALSE;
}
#define nickname_prefixless(a) nickname_start(a)?a:a+1

static BOOL to_organizer_folder(BOOL is_remove,BOOL remove){//for the moment this is the only chdir in the program
	char*h=getenv("HOME");
	if(h!=nullptr){
		if(chdir(h)!=-1){
			if(is_remove==FALSE){
				if(chdir(homelocal)==-1){
					if(mkdir(homelocal,0700)==-1)return FALSE;
					if(chdir(homelocal)==-1)return FALSE;
				}
				if(chdir(homeshare)==-1){
					if(mkdir(homeshare,0700)==-1)return FALSE;
					if(chdir(homeshare)==-1)return FALSE;
				}
				if(chdir(proj)==-1){
					if(mkdir(proj,0700)==-1)return FALSE;
					if(chdir(proj)==-1)return FALSE;
				}
				return TRUE;
			}else{
				if(access(homelocal folder_separator homeshare folder_separator proj,F_OK)==0){
					printf("%s%c%s%c%s%c%s",h,folderseparator,homelocal,folderseparator,homeshare,folderseparator,proj);
					BOOL a;
					if(remove==FALSE){
						puts("");a=TRUE;
					}else{
						if(rmdir(homelocal folder_separator homeshare folder_separator proj)==0){puts(removed_string);a=TRUE;}
						else{puts(remove_ignored);a=FALSE;}
					}
					if(a){
						if(access(homelocal folder_separator homeshare,F_OK)==0){
							printf("%s%c%s%c%s",h,folderseparator,homelocal,folderseparator,homeshare);
							if(remove==FALSE){
								puts("");a=TRUE;
							}else{
								if(rmdir(homelocal folder_separator homeshare)==0){puts(removed_string);a=TRUE;}
								else{puts(remove_ignored);a=FALSE;}
							}
							if(a){
								if(access(homelocal,F_OK)==0){
									printf("%s%c%s",h,folderseparator,homelocal);
									if(remove==FALSE){
										puts("");
									}else{
										if(rmdir(homelocal)==0) puts(removed_string);
										else puts(remove_ignored);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
}
#define to_organizer_folder_go to_organizer_folder(FALSE,FALSE)
#define to_organizer_folder_server(s) to_organizer_folder_go&&chdir(s)==0
#define to_organizer_folder_server_go to_organizer_folder_server(server_name(ps))
static const gchar*server_name(struct stk_s*ps){
	const gchar*n_main=gtk_notebook_get_menu_label_text(ps->notebook,home_page);
	n_main+=sizeof(not_a_nick_chan_host_start)-1;
	return n_main;
	//char*n_main=ps->proced_hostname;//at reconect is not ok and is set in another thread
}

static BOOL org_nick_iter(GtkNotebook*nb,char*name,GtkTreeModel**tm,GtkTreeIter*it){
	gint last=gtk_notebook_page_num(nb,gtk_notebook_get_nth_page(nb,-1));
	for(int tab=1;tab<=last;tab++){
		GtkWidget*sc=gtk_notebook_get_nth_page(nb,tab);//scroll
		if(gtk_label_get_use_markup((GtkLabel*)gtk_notebook_get_tab_label(nb,sc))){//only at global, is too many checks at remove to add at local, where it can be in multiple channels
			GtkWidget*tv=gtk_bin_get_child((GtkBin*)sc);
			*tm=gtk_tree_view_get_model((GtkTreeView*)tv);

			gboolean valid=gtk_tree_model_get_iter_first (*tm, it);
			while(valid){
				gchar*nick;
				gtk_tree_model_get (*tm, it, ORG_ID1, &nick, -1);
				int cmp=strcmp(nickname_prefixless(nick),name);
				g_free(nick);
				if(cmp==0)return TRUE;
				valid = gtk_tree_model_iter_next(*tm, it);
			}
		}
	}
	return FALSE;
}
static unsigned int org_conv_total(){
	GDir*entries=g_dir_open(".",0,nullptr);
	unsigned int n=0;
	if(entries!=nullptr){//something like a fast EACCES permission change, tested
		while(g_dir_read_name(entries)!=nullptr)n++;
		g_dir_close(entries);
	}
	return n;
}
static BOOL org_save_conv(char*user,char*text,const char*server){
	size_t sz=strlen(text);
	if(sz!=0){
		if(to_organizer_folder_server(server)&&chdir(org_u)==0){
			if( chdir(user)==0||(mkdir(user,0700)==0&&chdir(user)==0) ){
				unsigned int n=org_conv_total();
				char buf[digits_in_uintnul];sprintf(buf,"%u",n);
				FILE*dest=fopen(buf,"wb");
				if(dest!=nullptr){
					BOOL ret=fwrite(text,sz,1,dest)==1;
					fclose(dest);
					return ret;
				}
			}
		}
	}
	return FALSE;
}
static void name_closed(GtkTextView*tv,struct name_pack*nm){
//signal at child destroy will not get notebook tab name at that point, and notebook page-removed same
//destory is working at reconnect to another server for previous conversation. and for close everything is ok
	//g_object_unref(tv);//is ok is not required, will be critical, and if is unowned(not this case) is "A floating object was finalized"
	struct stk_s*ps=nm->ps;
	if(ps->organizer!=nullptr){
		const gchar*n_o=gtk_notebook_get_menu_label_text(ps->organizer_notebook,ps->organizer_entry_widget);
		if(*n_o!=*org_new_names){
			size_t n_o_sz=strchr(n_o,*not_a_nick_chan_host_start)-n_o;
			const gchar*n_main=server_name(ps);
			if(memcmp(n_o,n_main,n_o_sz)==0){//name can be in organizer server then
				GtkTreeModel*tm;GtkTreeIter iter;
				if(org_nick_iter(ps->organizer_notebook,nm->name,&tm,&iter)){
					GtkTextBuffer*b=gtk_text_view_get_buffer(tv);
					GtkTextIter start;GtkTextIter end;
					gtk_text_buffer_get_bounds (b, &start, &end);
					gchar*text = gtk_text_buffer_get_slice (b, &start, &end, TRUE);
					//save to file
					if(org_save_conv(nm->name,text,n_main)){
						//increment iter count
						gint count;
						gtk_tree_model_get (tm, &iter, ORG_CONV, &count, -1);
						count++;
						gtk_list_store_set((GtkListStore*)tm,&iter,ORG_CONV,count,-1);
					}
					g_free(text);
				}
			}
		}
	}
	g_free(nm->name);
	free(nm);
}

#define destroy "destroy"
static GtkWidget* name_join_nb(char*t,struct stk_s*ps){
	struct name_pack*n=(struct name_pack*)malloc(sizeof(struct stk_s*));
	if(n!=nullptr){
		n->name=g_strdup(t);
		n->ps=ps;
		GtkWidget*tv;
		GtkWidget*scrl=container_frame_name_out(&tv);
		g_signal_connect_data (tv,destroy,G_CALLBACK (name_closed),n,nullptr,(GConnectFlags)0);
		GtkWidget*close;GtkWidget*mn=add_new_tab(scrl,t,&close,ps->notebook,name_on_menu,TRUE);
		g_signal_connect_data (close, "clicked",G_CALLBACK (close_name),mn,nullptr,G_CONNECT_SWAPPED);//not "(GClosureNotify)gtk_widget_destroy" because at restart clear will be trouble
		return scrl;
	}
	return nullptr;
}
static void name_join_main(GtkTreeView*tree,struct stk_s*ps){
	GtkTreeSelection *sel=gtk_tree_view_get_selection(tree);
	GtkTreeIter iterator;
	if(gtk_tree_selection_get_selected (sel,nullptr,&iterator)){//this is required at org_chat call only
		gchar*item_text;
		gtk_tree_model_get (gtk_tree_view_get_model(tree), &iterator, LIST_ITEM_OR_ORG_ID1, &item_text, -1);
		char*a=nickname_prefixless(item_text);
		if(name_join_isnew(ps,a))
			gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,name_join_nb(a,ps)));
		g_free(item_text);
	}
}
static gboolean name_join(GtkTreeView*tree,GdkEvent*ignored,struct stk_s*ps){
	(void)ignored;
	name_join_main(tree,ps);
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
#define name_on_pan(c) page_from_str(c,name_on_menu)
#define name_off_pan(c) page_from_str(c,name_off_menu)
#define name_to_list(c) contf_get_list(chan_pan(c))
static void chan_change_nr_gain(GtkTreeIter*iter,char*chn,unsigned int nr){
	GtkTreeIter it=*iter;
	if(gtk_tree_model_iter_previous( (GtkTreeModel*)channels, &it)==FALSE)return;
	for(;;){
		gchar*text;
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
		gchar*text;
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
static BOOL get_chan_counted(const char*chan,char*c,GtkTreeIter*it,gchar**text){
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
static BOOL get_chan_alpha(const char*chan,char*c,GtkTreeIter*it,gchar**text){
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
	char c[chan_sz+1+digits_in_uint+1];gchar*text;
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
static BOOL listing_test(GtkWidget*a,GtkListStore*b){
	if(gtk_widget_get_has_tooltip(a)==FALSE){
		gtk_list_store_clear(b);gtk_widget_set_has_tooltip(a,TRUE);
		return TRUE;
	}
	return FALSE;
}
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

		//this was only stopping blank names clear, but now will stop organizer otherwise
		//gtk_widget_set_tooltip_text(pan,listing_info("names"));//is also a NAMES flag here

		GtkWidget*close;GtkWidget*lb=add_new_tab(pan,chan,&close,ps->notebook,chan_menu,FALSE);
		g_signal_connect_data (close, "clicked",G_CALLBACK (close_channel),lb,nullptr,G_CONNECT_SWAPPED);//but will close the notebook page only on PART received from the server
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
		gchar*text;
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
	gchar*txt;
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
	gchar*txt;
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
	gchar*text;
	int n=gtk_tree_model_iter_n_children((GtkTreeModel*)lst,nullptr);
	if(n>0){
		gtk_tree_model_iter_nth_child((GtkTreeModel*)lst, &it, nullptr, n-1);
		do{
			gtk_tree_model_get ((GtkTreeModel*)lst, &it, 0, &text, -1);
			if(nickname_start(text)==FALSE||strcmp(t,text)>0){
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
static BOOL rank_compare(char*intruder,int intruder_rank,char*defender){
	int defender_pos=strchr(chanmodessigns,*defender)-chanmodessigns;//example: ~&@%+ ~ is 0, + is 4
	if(intruder_rank<defender_pos)return TRUE;//is reversed order
	else if(intruder_rank==defender_pos)return strcmp(intruder,defender+1)<0;
	return FALSE;
}
static void add_name_highuser(GtkListStore*lst,char*t){
	GtkTreeIter it;
	GtkTreeIter i;
	gchar*text;
	if(gtk_tree_model_get_iter_first((GtkTreeModel*)lst, &it)){
		int pos=strchr(chanmodessigns,*t)-chanmodessigns;//example: ~&@%+ ~ is 0, + is 4
		do{
			gtk_tree_model_get ((GtkTreeModel*)lst, &it, 0, &text, -1);
			if(nickname_start(text)||rank_compare(t+1,pos,text)){
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
#define add_name_organizer_macro(l,i,n,x)\
	gtk_list_store_append(l,&i);\
	gtk_list_store_set(l, &i, ORG_ID1, n, ORG_IDLE, 0x7fFFffFF, ORG_INDEX, x, -1)
	//here is at new entries ORG_CONV 0
static void add_name_organizer(char*name,struct stk_s*ps){
	if(ps->organizer!=nullptr){
		if(ps->organizer_can_add_names){
			GtkListStore*new_entries=ps->organizer_entry_names;
			GtkTreeIter it;//=GtkTreeIter();
			gint n=gtk_tree_model_iter_n_children((GtkTreeModel*)new_entries,nullptr);
			add_name_organizer_macro(new_entries,it,name,n);
		}
	}
}
static void add_name(GtkListStore*lst,char*t,gpointer ps){
	add_name_organizer(t,(struct stk_s*)ps);//show with prefix
	if(nickname_start(t)){add_name_lowuser(lst,t);return;}
	add_name_highuser(lst,t);
}

static char*server_channel_base(char*channel,size_t channel_size,const char*h){
	size_t hs=strlen(h);
	char*z=(char*)malloc(hs+1+channel_size+1);
	if(z!=nullptr){
		memcpy(z   ,h,hs);
		z[hs]=*not_a_nick_chan_host_start;
		hs++;
		memcpy(z+hs,channel,channel_size);
		z[hs+channel_size]='\0';
	}
	return z;
}
static char*server_channel(struct stk_s*ps,char*channel,size_t channel_size){
	return server_channel_base(channel,channel_size,server_name(ps));
}

static void pars_names_org(struct stk_s*ps,char*serv_chan){
	gtk_notebook_set_tab_label_text(ps->organizer_notebook,ps->organizer_entry_widget,serv_chan);
	gtk_notebook_set_menu_label_text(ps->organizer_notebook,ps->organizer_entry_widget,serv_chan);
}
static void pars_names_org_inits(struct stk_s* ps,char*a){
	ps->organizer_can_add_names=TRUE;
	pars_names_org(ps,a);
	gtk_widget_set_sensitive(ps->organizer_bot,TRUE);
	char buf[digits_in_longnul];
	sprintf(buf,"%ld",time(nullptr));
	gtk_label_set_text((GtkLabel*)ps->organizer_entry_widget_timestamp,buf);
}
static void pars_names(GtkWidget*pan,char*b,size_t s,struct stk_s* ps,char*channm){
	GtkListStore*lst=contf_get_list(pan);
	if(listing_test(pan,lst)){//if first from a series of names until endNames
		if(ps->organizer!=nullptr){
			char*a=server_channel(ps,channm,strlen(channm));
			if(a!=nullptr){
				gchar*text=gtk_combo_box_text_get_active_text((GtkComboBoxText*)ps->organizer_dirs);
				if(text!=nullptr){//can be a blank organizer too
					if(strcmp(text,a)==0){
						gtk_list_store_clear(ps->organizer_entry_names);//required in all cases
						pars_names_org_inits(ps,a);
					}else ps->organizer_can_add_names=FALSE;
					g_free(text);
				}else pars_names_org_inits(ps,a);
				free(a);
			}else ps->organizer_can_add_names=FALSE;
		}
	}
	size_t j=0;
	for(size_t i=0;i<s;i++){
		if(b[i]==' '){b[i]='\0';add_name(lst,b+j,ps);b[i]=' ';j=i+1;}
	}
	add_name(lst,b+j,ps);
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
			char buf[prefix_name_sz+1];*buf=chanmodessigns[pos];
			strcpy(buf+1,n);
			add_name_highuser(lst,buf);
			return;
		}
		prevmod=get_iter_modes(lst,&it,n,TRUE);
		if(prevmod!='\0'){
			if(pos<(strchr(chanmodessigns,prevmod)-chanmodessigns)){
				gtk_list_store_remove(lst,&it);
				char buf[prefix_name_sz+1];*buf=chanmodessigns[pos];
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
					int sz=sprintf(downgraded,whois_str " %s" irc_term,n);
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
	else addatnames(usednick,text,pg,(char*)a);
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
	GList*list=gtk_container_get_children((GtkContainer*)name_on_menu);
	if(list!=nullptr){
		GList*lst=list;
		GtkNotebook*nb=ps->notebook;
		for(;;){
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(n,d)==0){
				GtkWidget*scrl=get_pan_from_menu(menu_item);
				addatnames(frontname,msg,scrl,nullptr);
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
			GtkWidget*scrl=name_join_nb(n,ps);addatnames(frontname,msg,scrl,nullptr);
			GtkNotebook*nb=ps->notebook;
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
	if(pg!=nullptr)addatnames(user_error,msg,pg,str);//after user quits, ERR_NOSUCHNICK when sending and is not
	else{
		pg=name_on_pan(str);
		if(pg!=nullptr)addatnames(user_error2,msg,pg,str);//after user changed nick, the program is not notified, and ERR_NOSUCHNICK comes in
	}
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
				addatnames(user_info,msg,get_pan_from_menu(menu_item),n);
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
	char buf[digits_in_posInt+counting_the_list_size+sizeof(list_end_str)];
	size_t n=(size_t)sprintf(buf,"%u %s" list_end_str,gtk_tree_model_iter_n_children(contf_get_model(w),nullptr),a);
	if(w==home_page)addattextmain(buf,n);
	else addatchans(user_info,buf,w);
}

static void org_move_indexed(GtkTreeModel*m,gint pos){
	GtkTreeIter it;
	gint i;GtkSortType s;
	gboolean is_sorted=gtk_tree_sortable_get_sort_column_id ((GtkTreeSortable*)m, &i, &s);//unsorted is still indexed
	if(is_sorted)gtk_tree_sortable_set_sort_column_id((GtkTreeSortable*)m,ORG_INDEX,GTK_SORT_ASCENDING);
	if(gtk_tree_model_iter_nth_child(m, &it, nullptr, pos)){
		do{
			gtk_list_store_set((GtkListStore*)m, &it, ORG_INDEX, pos, -1);
			pos++;
		}while(gtk_tree_model_iter_next(m, &it));
	}
	if(is_sorted)gtk_tree_sortable_set_sort_column_id((GtkTreeSortable*)m,i,s);
}

static const char*org_getchan(struct stk_s*ps){
	const gchar*text=gtk_notebook_get_menu_label_text(ps->organizer_notebook,ps->organizer_entry_widget);//Move is active for this label text
	const char*a=strchr(text,*not_a_nick_chan_host_start);//invalid conversion from const char* to char*   char*a=strchr(text,*not_a_nick_chan_host_start) but man strchr: char *strchr
	return a+1;
}
#define delim_init 10
static BOOL append_lineSz_tofile(char*text,size_t sz,const char*fname){
	FILE*f=fopen(fname,"ab");
	if(f!=nullptr){
		BOOL ret=FALSE;
		if(fwrite(text,sz,1,f)==1){
			char a='\n';
			if(fwrite(&a,1,1,f)==1)
				ret=TRUE;
		}
		fclose(f);
		return ret;
	}
	return FALSE;
}
#define append_line_tofile(l,f) append_lineSz_tofile(l,strlen(l),f)
static BOOL delete_line_fromfile_pref(const char*text,const char*fname,BOOL raw){
	FILE*f=fopen(fname,"r+b");
	if(f!=nullptr){//from here, in case file is manipulated somewhere else, the drive data can be overwrote or increased but not decreased.
		BOOL ret=FALSE;
		size_t len=strlen(text);
		size_t sz=delim_init;
		char*mem=(char*)malloc(delim_init);
		if(mem!=nullptr){
			while(getdelim(&mem,&sz,'\n',f)!=-1){
				char*inplace;
				if(raw)inplace=mem;
				else inplace=nickname_prefixless(mem);
				if(memcmp(text,inplace,len)==0){
					if(inplace[len]=='\n'){
						//move from this file location everything back len bytes
						long here=ftell(f);
						if(here!=-1){//if someone compiles for 32 and EINVAL, tested with truncate -s 2147483647 , and the error is at ftell not at getdelim or fseek
							long back=here-1-len-(inplace-mem);// - prefix if was the case
							fseek(f,0,SEEK_END);
							long tel=ftell(f);
							if(tel!=-1){//same
								long moved=tel-here;
								if(moved==0){
									if(back==0){
										if(unlink(fname)==0)//this is another descriptor, can be permission in the meantime
											ret=TRUE;
										//there's nothing to write, fclose will do nothing next
									}else{
										ftruncate(fileno(f),back);
										ret=TRUE;
									}
								}else{
									fseek(f,here,SEEK_SET);
									void*m=malloc(moved);
									if(m!=nullptr){
										fread(m,moved,1,f);
										fseek(f,back,SEEK_SET);
										fwrite(m,moved,1,f);
										ftruncate(fileno(f),back+moved);
										free(m);
										ret=TRUE;
									}
								}
							}
						}
						break;
					}
				}
			}
			free(mem);
		}
		fclose(f);
		return ret;
	}
	return FALSE;
}
#define delete_line_fromfile(text,fname) delete_line_fromfile_pref(text,fname,TRUE)

static void org_modchanged(GtkNotebook*nb,GtkListStore*list,GtkTreeIter*iter,const char*chan){
	if(chdir(org_c)==0){
		if(chdir(chan)==0){
			gint tab;gint pos;gint ix;
			gtk_tree_model_get((GtkTreeModel*)list, iter, 3,&tab, 4,&pos, 5,&ix, -1);

			GtkWidget*sc=gtk_notebook_get_nth_page(nb,tab);
			const gchar*rule=gtk_notebook_get_menu_label_text(nb,sc);

			GtkWidget*tv=gtk_bin_get_child((GtkBin*)sc);
			GtkTreeModel*tm=gtk_tree_view_get_model((GtkTreeView*)tv);
			gtk_tree_model_iter_nth_child(tm, iter, nullptr, pos);
			gchar*old_nick_with_or_without_prefix;
			gtk_tree_model_get(tm,iter, ORG_ID1,&old_nick_with_or_without_prefix, -1);

			if(delete_line_fromfile(old_nick_with_or_without_prefix,rule)){//delete user from channel list
				g_free(old_nick_with_or_without_prefix);
				gtk_list_store_remove((GtkListStore*)tm,iter);
				org_move_indexed(tm,ix);
			}
		}
	}
}
static void org_names_end(struct stk_s* ps){//nick uniqueness
	if(ps->organizer!=nullptr){
		if(ps->organizer_can_add_names){
			//second is for prefix, third for is_global to not change prefix there    , tab      ,pos       , and index at delete
			GtkListStore*list=gtk_list_store_new(6,G_TYPE_STRING,G_TYPE_INT,G_TYPE_INT,G_TYPE_INT,G_TYPE_INT,G_TYPE_INT);
			gtk_tree_sortable_set_sort_column_id((GtkTreeSortable*)list,0,GTK_SORT_ASCENDING);

			GtkTreeIter it;gchar*nick;gboolean valid;GtkTreeIter iter;
			gchar*nick_new;gint nick_new_pref;
			GtkNotebook*nb=ps->organizer_notebook;
			gint last=gtk_notebook_page_num(nb,gtk_notebook_get_nth_page(nb,-1));
			for(int tab=1;tab<=last;tab++){
				GtkWidget*sc=gtk_notebook_get_nth_page(nb,tab);//scroll
				GtkWidget*tv=gtk_bin_get_child((GtkBin*)sc);
				GtkTreeModel*tm=gtk_tree_view_get_model((GtkTreeView*)tv);
				gboolean is_global=gtk_label_get_use_markup((GtkLabel*)gtk_notebook_get_tab_label(nb,sc));

				valid=gtk_tree_model_get_iter_first (tm, &it);int pos=0;
				while(valid){
					gint index;
					gtk_tree_model_get (tm, &it, ORG_ID1, &nick, ORG_INDEX, &index, -1);//ORG_ID2, &user,-1);
					//char n_u[prefix_name_sz+1+prefix_name_sz+1];
					//sprintf(n_u,"%s" defaultstart "%s",nick,user==nullptr?"":user);
					gtk_list_store_append(list, &iter);
					if nickname_start(nick) {nick_new=nick;nick_new_pref=0;}
					else{nick_new=nick+1;nick_new_pref=*nick;}//not at is_global
					gtk_list_store_set(list,&iter, 0,nick_new, 1,nick_new_pref, 2,is_global, 3,tab, 4,pos, 5,index, -1);
					g_free(nick);//g_free(user);
					valid = gtk_tree_model_iter_next(tm, &it);pos++;
				}
			}
			GtkListStore*new_entries=ps->organizer_entry_names;
			valid=gtk_tree_model_get_iter_first ((GtkTreeModel*)new_entries, &it);//then End of NAMES list can come without users, tested for unexistent channel
			while(valid){
				gtk_tree_model_get((GtkTreeModel*)new_entries, &it, ORG_ID1, &nick, -1);
				if nickname_start(nick){nick_new=nick;nick_new_pref=0;}
				else{nick_new=nick+1;nick_new_pref=*nick;}

				gtk_list_store_append(list, &iter);
				gtk_list_store_set(list,&iter,0,nick_new,-1);

				//check only for previous if is same, is sorted
				GtkTreeIter new_iter=iter;//delete this new iter and let prev which has_pref,is_global,tab,pos
				if(gtk_tree_model_iter_previous((GtkTreeModel*)list, &iter)){
					gchar*nickprev;
					gtk_tree_model_get((GtkTreeModel*)list,&iter, 0,&nickprev, -1);
					if(strcmp(nick_new,nickprev)==0){
						gtk_list_store_remove(list,&new_iter);

						gint old_pref;gint is_global;
						gtk_tree_model_get((GtkTreeModel*)list, &iter, 1,&old_pref, 2,&is_global, -1);

						if(is_global||old_pref==nick_new_pref){//is global with no prefix or local but same/no prefix
							g_free(nickprev);g_free(nick);
							valid=gtk_list_store_remove(new_entries,&it);//iter is next valid row
							//ignore ORG_INDEX? it will look like a bug, 0,1,2  1 is at rules, 0,2  move from rules to first tab  0,2,2
							//I don't need to move entry from rules back to new entries so don't care
							//but someone who has only 2 lists and want to move between them will care
							//adding that the prefix will be lost from global to local, as a side effect, remove the entry at moving back to new entries
							continue;
						//only when prefix can be changed   and  when prefix was changed, if one is changing many times can be moved from localNO to globalNO
						}else if(to_organizer_folder_server_go)org_modchanged(nb,list,&iter,org_getchan(ps));
						//       to remove from stored                                     //for to remove
					}
					g_free(nickprev);
				}

				g_free(nick);
				valid = gtk_tree_model_iter_next((GtkTreeModel*)new_entries, &it);
			}
			g_object_unref(list);
		}
	}
}
static void names_end(GtkWidget*p,char*chan,gpointer ps){
	org_names_end((struct stk_s*)ps);
	counting_the_list(p,names_small_str);
	char c[chan_sz+1+digits_in_uint+1];
	GtkTreeIter it;gchar*text;
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
		int z=gtk_tree_model_iter_n_children((GtkTreeModel*)list,nullptr);
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
		counting_the_list(home_page,chans_small_str);
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

static void whois_update(GtkNotebook*nb,int col,char*nick,char*text){
	gint nb_page_index=gtk_notebook_get_current_page(nb);
	GtkWidget*current=gtk_notebook_get_nth_page(nb,nb_page_index);//scroll
	GtkWidget*tv=gtk_bin_get_child((GtkBin*)current);
	GtkTreeModel*tm=gtk_tree_view_get_model((GtkTreeView*)tv);
	GtkTreeIter it;
	gboolean valid=gtk_tree_model_get_iter_first (tm, &it);
	while(valid){//compare at first?yes, is coming from online, no rule to freeze organizer in the meantime yet
		gchar*n;
		gtk_tree_model_get (tm, &it, ORG_ID1, &n, -1);
		if(strcmp(nickname_prefixless(n),nick)==0){
			g_free(n);
			gtk_list_store_set((GtkListStore*)tm,&it, col,text, -1);
			return;
		}
		g_free(n);
		valid = gtk_tree_model_iter_next(tm, &it);
	}
}
static void whois_update_nr(GtkNotebook*nb,char*nick,int seconds){
	gint nb_page_index=gtk_notebook_get_current_page(nb);
	GtkWidget*current=gtk_notebook_get_nth_page(nb,nb_page_index);//scroll
	GtkWidget*tv=gtk_bin_get_child((GtkBin*)current);
	GtkTreeModel*tm=gtk_tree_view_get_model((GtkTreeView*)tv);
	GtkTreeIter it;
	gboolean valid=gtk_tree_model_get_iter_first (tm, &it);
	while(valid){//compare at first? same
		gchar*n;
		gtk_tree_model_get (tm, &it, ORG_ID1, &n, -1);
		if(strcmp(nickname_prefixless(n),nick)==0){
			g_free(n);
			gtk_list_store_set((GtkListStore*)tm,&it, ORG_IDLE,seconds, -1);
			return;
		}
		g_free(n);
		valid = gtk_tree_model_iter_next(tm, &it);
	}
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
		char channm_simple[channul_sz];//this is extra but is good at compiler warnings
		char nicknm[namenul_sz];
		char username[usernul_sz];
		char hostname[hostnamerfcnul_sz];
		#define specialsz 32
		#define specialsz_scan "%31s"
		char special[specialsz];
		char c;
		BOOL is_privmsg=strcmp(com,priv_msg_str)==0;
		if(is_privmsg||strcmp(com,not_msg_str)==0){
			if(nick_extract(a,nicknm)){
				if(is_channel(b)){
					if(sscanf(b,channame_scan " %c",channm,&c)==2)pars_pmsg_chan(nicknm,channm,b+strlen(channm)+2,((struct stk_s*)ps)->notebook);
				}else if(sscanf(b,name_scan " %c",channm,&c)==2)pars_pmsg_name(nicknm,b+strlen(channm)+2,(struct stk_s*)ps,is_privmsg,nicknm);
			}
		}else if(strcmp(com,join_str)==0){
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
			if(nick_extract(a,nicknm)&&sscanf(b,"%*s " channame_scan,channm_simple)==1){
				char buf[name_sz+sizeof(invite_str)+chan_sz];//invite_str size includes null
				sprintf(buf,"%s" invite_str "%s",nicknm,channm_simple);
				pars_pmsg_name(nicknm,buf,(struct stk_s*)ps,TRUE,"*Invite");
			}
		}else if(strlen(com)!=3)showmsg=FALSE;
		else{
			showmsg=TRUE;
			int d=atoi(com);//If no valid conversion could be performed, it returns zero;below,d==0

			if(d==RPL_WHOISUSER){
				show_between_clause(RPL_WHOISUSER) //then, show is true only at manual command whois
				if(((struct stk_s*)ps)->organizer!=nullptr){
					int s=sscanf(b,"%*s " name_scan " " user_scan,nicknm,username);//%*[~] is ok only when ~ is
					if(s==2)whois_update(((struct stk_s*)ps)->organizer_notebook,ORG_ID2,nicknm,*username!='~'?username:username+1);
				}
			}else if(d==RPL_WHOISSERVER){// 	RFC1459 	<nick> <server> :<server_info>
				show_between_clause(RPL_WHOISUSER)
				if(((struct stk_s*)ps)->organizer!=nullptr){
					int s=sscanf(b,"%*s " name_scan hostnamerfc_scan,nicknm,hostname);
					if(s==2)whois_update(((struct stk_s*)ps)->organizer_notebook,ORG_SERVER,nicknm,hostname);
				}
			}else if(d==RPL_WHOISIDLE){
				show_between_clause(RPL_WHOISUSER)
				if(((struct stk_s*)ps)->organizer!=nullptr){
					int seconds;
					int s=sscanf(b,"%*s " name_scan " %u",nicknm,&seconds);
					if(s==2)whois_update_nr(((struct stk_s*)ps)->organizer_notebook,nicknm,seconds);
				}
			}else if(d==RPL_WHOISCHANNELS){
				show_between_clause(RPL_WHOISUSER)
				b=strchr(b,' ');
				if(b!=nullptr){
					b++;if(sscanf(b,name_scan " %c",nicknm,&c)==2)
						pars_wmod(nicknm,b+strlen(nicknm)+2);
				}
			}else if(d==RPL_WHOISSPECIAL){
				show_between_clause(RPL_WHOISUSER)
				if(((struct stk_s*)ps)->organizer!=nullptr){
					int s=sscanf(b,"%*s " name_scan "  :identifies as " specialsz_scan,nicknm,special);//from https://scp-wiki.wikidot.com/chat-guide
					if(s==2)whois_update(((struct stk_s*)ps)->organizer_notebook,ORG_GEN,nicknm,special);
				}
			}else if(d==RPL_WHOISACCOUNT){// "logged in as"
				show_between_clause(RPL_WHOISUSER)
			}else if(d==RPL_AWAY){
				show_between_clause(RPL_WHOISUSER)
			}else if(d==RPL_WHOISHOST){
				show_between_clause(RPL_WHOISUSER)
			}else if(d==RPL_WHOISMODES){
				show_between_clause(RPL_WHOISUSER)
			}else if(d==RPL_ENDOFWHOIS){
				show_to_clause(RPL_WHOISUSER)

			}else if(d==RPL_LIST){//if -f 0 or the option, this is rare
				show_between_clause(RPL_LIST)
				unsigned int e;
				//if its >nr ,c is not 2
				if(sscanf(b,"%*s " channame_scan " %u",channm,&e)==2)
					if(test_to_add_chan(((struct stk_s*)ps),(int)e)){
						listing_test(home_page,channels);
						pars_chan(channm,e,((struct stk_s*)ps)->chans_max);
					}
			}
			//not on ircnet: else if(d==321)//RPL_LISTSTART
			else if(d==RPL_LISTEND){
				show_to_clause(RPL_LIST)
				list_end();

			}else if(d==RPL_NAMREPLY){
				show_between_clause(RPL_NAMREPLY)
				if(sscanf(b,"%*s %*c " channame_scan,channm)==1){
					GtkWidget*p=chan_pan(channm);
					if(p!=nullptr){
						b=strchr(b,':');//join #q:w is error
						if(b!=nullptr)pars_names(p,b+1,s-(size_t)(b+1-a),(struct stk_s*)ps,channm);
					}
				}
			}else if(d==RPL_ENDOFNAMES){
				show_to_clause(RPL_NAMREPLY)
				if(sscanf(b,"%*s " channame_scan,channm)==1){
					GtkWidget*p=chan_pan(channm);
					if(p!=nullptr)names_end(p,channm,ps);//at a join
				}

			}else if(d==RPL_TOPIC){
				if(sscanf(b,name_scan " " channame_scan " %c",nicknm,channm,&c)==3)
					addatchans(user_topic,b+strlen(nicknm)+1+strlen(channm)+2,chan_pan(channm));
			}else if(d==RPL_ISUPPORT){
				char*e=strstr(b,"PREFIX=");
				if(e!=nullptr){
					sscanf(e+7,"(%6[^)])%6s",chanmodes,chanmodessigns);
					maximummodes=strlen(chanmodessigns);
				}
				e=strstr(b,"CHANTYPES=");
				if(e!=nullptr){
					char chan_types[5];
					if(sscanf(e+10,"%4s",chan_types)==1){
						int i=0;
						for(;;){
							if(chan_types[i]==*not_a_nick_chan_host_start)//will be collision with homepage at least, and at least at organizer update decider
								break;
							i++;
							if(chan_types[i]=='\0'){
								strcpy(chantypes,chan_types);
								break;
							}
						}
					}
				}
			}else if(d==RPL_LUSERCHANNELS){
				send_autojoin((struct stk_s*)ps);
				//this not getting after first recv
				//another solution can be after 376 RPL_ENDOFMOTD
				//or after 1 second, not beautiful
				send_list
			}else if(d>=ERR_UNKNOWNERROR){//Error Replies.
				//switch(d){
					//porbably deprecated
					//case 436://ERR_NICKCOLLISION
					//case 464://ERR_PASSWDMISMATCH
					//
					//rare,not tried
					//case 463://ERR_NOPERMFORHOST
					//case 465://ERR_YOUREBANNEDCREEP
					//
					//case 432://ERR_ERRONEUSNICKNAME
					//case 433://ERR_NICKNAMEINUSE
					//	action_to_close();//why was to close at these two?
					//	break;
				//	default:
				b=strchr(b,' ');
				if(b!=nullptr){
					b++;if(sscanf(b,channame_scan " %c",channm,&c)==2)
						pars_err(channm,b+strlen(channm)+2);
				}
				//}
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
	unsigned char n=ps->proced_n;
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
static gboolean proced_connecting(gpointer b){
//on main thread
	chantypes[0]='\0';//no # ? will be one if CHANTYPES= , else channels are names, wrong but not fatal
	chanmodes[0]='\0';
	chanmodessigns[0]='\0';//when straight at whois modes
	maximummodes=0;

	struct stk_s*ps=(struct stk_s*)b;
	clear_old_chat(ps->notebook);
	addattextmain(ps->proced_text,-1);
	unsigned char n;
	if(ps->proced_switch){
		GSList*lst=con_group;
		n=con_nr_max;
		for(;;){
			if(gtk_check_menu_item_get_active((GtkCheckMenuItem*)lst->data))break;
			lst=lst->next;n--;
		}
	}else n=_con_nr_s;
	ps->proced_n=n;

	if(ps->organizer!=nullptr){
		const gchar*c=gtk_notebook_get_menu_label_text(ps->organizer_notebook,ps->organizer_entry_widget);//or at index 0
		if(strcmp(c,ps->proced_hostname)!=0)
			gtk_widget_set_sensitive(ps->organizer_bot,FALSE);//server was changed
	}

	char hostname[1+hostname_sz]={not_a_nick_chan_host_start};//only first is set in case of arrays
	strcpy(hostname+1,ps->proced_hostname);
	gtk_notebook_set_menu_label_text(ps->notebook,home_page,hostname);
	gtk_notebook_set_tab_label_text(ps->notebook,home_page,hostname);

	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
static gboolean proced_disconnected(gpointer b){
	struct stk_s*ps=(struct stk_s*)b;
	gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,home_page));
	addattextmain(ps->proced_text,-1);
	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
static void proced(struct stk_s*ps){
	char hostname[hostname_sz];
	char psw[password_sz];char nkn[namenul_sz];
	unsigned short*ports;size_t port_last;size_t swtch;
	if(parse_host_str(ps->text,hostname,psw,nkn,&ports,&port_last,&swtch,ps)) {
		ps->proced_text="Connecting...\n";
		ps->proced_switch=swtch==not_a_switch;
		ps->proced_hostname=hostname;
		g_idle_add(proced_connecting,ps);
		int out;sigwait(&threadset,&out);

		proced_core(ps,hostname,psw,nkn,ports,port_last,swtch);
		free(ports);

		ps->proced_text="Disconnected.\n";
		g_idle_add(proced_disconnected,ps);
		sigwait(&threadset,&out);
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
				if(i){if(write(f,new_line,1)!=1){g_free(text);break;}}
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

//is new netry or is not
static BOOL set_combo_box_text(GtkComboBox * box,const char*txt)
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
			return FALSE;
		}    
		g_free(item_text);
		i++; 
		valid = gtk_tree_model_iter_next( list_store, &iter);
	}
	gtk_combo_box_text_append_text((GtkComboBoxText*)box,txt);
	gtk_combo_box_set_active(box, i);
	return TRUE;
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
		if(set_combo_box_text((GtkComboBox*)((struct stk_s*)ps)->cbt,t)){
			save_combo_box(gtk_combo_box_get_model((GtkComboBox*)((struct stk_s*)ps)->cbt));
		}
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
		info_path_name[i]=folderseparator;
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
static BOOL icmpAmemBstrlownotempty(const char*s1,const char*s2){
	char b=s2[0];
	for(size_t i=0;;){
		char a=s1[i];
		if(a!=b){
			a+='a'-'A';
			if(a!=b)return FALSE;
		}
		i++;
		b=s2[i];
		if(b=='\0')return TRUE;
	}
}
#define is_home(a) *a==*home_string
static void send_activate(GtkEntry*entry,struct stk_s*ps){
	GtkEntryBuffer*t=gtk_entry_get_buffer(entry);
	const gchar*text=gtk_entry_buffer_get_text(t);
	size_t sz=strlen(text);
	GtkWidget*pg=gtk_notebook_get_nth_page(ps->notebook,gtk_notebook_get_current_page(ps->notebook));
	const char*a=gtk_notebook_get_menu_label_text(ps->notebook,pg);
	if(sz!=0){//nothing to send else
		if(is_home(a)){
			show_from_clause(text,"list",RPL_LIST)
			else show_from_clause(text,"names",RPL_NAMREPLY)
			else show_from_clause(text,"whois",RPL_WHOISUSER)
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
	}else if(is_home(a)){//enters there for visibility
		addattextmain_n("",0);
	}
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
	gchar*text = gtk_text_buffer_get_slice (buffer, &start, &end, TRUE);
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
		const gchar*text=gtk_entry_get_text((GtkEntry*)l->data);
		g_list_free(l);
		if(strlen(text)!=0){//else is like the placeholder
			*chan_min=atoi(text);
			send_list_if
		}
	}
	gtk_widget_destroy((GtkWidget*)dialog);
}
static gboolean to_placeholder(GtkEntry*en,GdkEventKey*event){
	if(event->type==GDK_KEY_PRESS){
		if('0'<=event->keyval&&event->keyval<='9'){
			GtkEntryBuffer*buf=gtk_entry_get_buffer(en);
			//gtk_entry_buffer_insert_text(buf,gtk_entry_buffer_get_length(buf), //coming back there is not relevant at this point
			//gtk_entry_buffer_set_text(buf, //this is delete text and isert at 0
			gtk_entry_buffer_insert_text(buf,0,(const gchar*)&event->keyval,1);//this is the faster
			gtk_widget_grab_focus((GtkWidget*)en);
			gtk_editable_set_position((GtkEditable*)en, -1);//the text is selected all, need to move cursor to end
			return TRUE;
		}
	}
	return FALSE;
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

	GtkWidget*wg=gtk_dialog_get_widget_for_response((GtkDialog*)dialog, GTK_RESPONSE_OK);
	g_signal_connect_data (wg, "key-press-event",G_CALLBACK (to_placeholder),entry,nullptr,G_CONNECT_SWAPPED);//this will swap with first arg, not second GdkEventKey
	gtk_widget_grab_focus(wg);//this to see the placeholder

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
static void gather_parse(size_t*sum,gchar*mem,struct ajoin**ons){
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
static void gather_free(size_t sum,gchar*mem,struct ajoin*ins){
	if(sum>0){
		g_free(mem);
		for(size_t i=0;i<sum;i++)if(ins[i].chans!=&dummy)
			free(ins[i].chans);
		free(ins);
	}
}

static void deciderfn(struct stk_s*ps){
	const char*b=gtk_notebook_get_menu_label_text(ps->notebook,gtk_notebook_get_nth_page(ps->notebook,gtk_notebook_get_current_page(ps->notebook)));
	if(is_channel(b)){
		size_t bs=strlen(b);
		char*z=server_channel(ps,(char*)b,bs);
		if(z!=nullptr){
			int current=gtk_combo_box_get_active(ps->organizer_dirs);
			if(set_combo_box_text(ps->organizer_dirs,z)==FALSE){//is an existent entry
				if(gtk_combo_box_get_active(ps->organizer_dirs)==current){//is same entry already selected
					gtk_widget_set_sensitive(ps->organizer_bot,FALSE);//will be TRUE when names comes in
					send_channel_related((char*)names_str,(char*)b,bs);
				}
			}else gtk_widget_set_sensitive(ps->organizer_removeentry,TRUE);
			free(z);
		}
	}
	else{
		#define org_pop_er "Must be in a channel"
		addattextmain_n(org_pop_er,sizeof(org_pop_er)-1);
	}
}
static void org_clear_rules(GtkNotebook*nb){
	gint last=gtk_notebook_page_num(nb,gtk_notebook_get_nth_page(nb,-1));
	for(int i=1;i<=last;i++){
		GtkWidget*sc=gtk_notebook_get_nth_page(nb,i);//scroll
		GtkWidget*tv=gtk_bin_get_child((GtkBin*)sc);
		GtkTreeModel*tm=gtk_tree_view_get_model((GtkTreeView*)tv);
		gtk_list_store_clear((GtkListStore*)tm);
	}
}
static void org_repopulate(BOOL is_global,GtkNotebook*nb){
	gint last=gtk_notebook_page_num(nb,gtk_notebook_get_nth_page(nb,-1));
	for(int i=1;i<=last;i++){
		GtkWidget*sc=gtk_notebook_get_nth_page(nb,i);//scroll
		gboolean tab_is_global=gtk_label_get_use_markup((GtkLabel*)gtk_notebook_get_tab_label(nb,sc));
		if(is_global==tab_is_global){
			const gchar*rulename=gtk_notebook_get_menu_label_text(nb,sc);
			FILE*f=fopen(rulename,"rb");
			if(f!=nullptr){
				size_t sz=delim_init;
				char*mem=(char*)malloc(delim_init);
				if(mem!=nullptr){
					GtkWidget*tv=gtk_bin_get_child((GtkBin*)sc);
					GtkListStore*tm=(GtkListStore*)gtk_tree_view_get_model((GtkTreeView*)tv);
					int pos=0;
					while(getdelim(&mem,&sz,'\n',f)!=-1){
						mem[strlen(mem)-1]='\0';
						GtkTreeIter iter;
						add_name_organizer_macro(tm,iter,mem,pos);
						pos++;
					}
					free(mem);
				}
				fclose(f);
			}
		}
	}
}
static void org_repopulate_conv(GtkNotebook*nb){
	gint last=gtk_notebook_page_num(nb,gtk_notebook_get_nth_page(nb,-1));
	for(int i=1;i<=last;i++){
		GtkWidget*sc=gtk_notebook_get_nth_page(nb,i);//scroll
		if(gtk_label_get_use_markup((GtkLabel*)gtk_notebook_get_tab_label(nb,sc))){//conv only at global
			GtkWidget*tv=gtk_bin_get_child((GtkBin*)sc);
			GtkTreeModel*tm=gtk_tree_view_get_model((GtkTreeView*)tv);
			GtkTreeIter iter;
			gboolean valid = gtk_tree_model_get_iter_first (tm, &iter);
			if(valid){
				do{
					gchar*nick;
					gtk_tree_model_get (tm, &iter, ORG_ID1, &nick, -1);
					if(g_file_test(nick,G_FILE_TEST_IS_DIR)){
						if(chdir(nick)==0){
							unsigned int n=org_conv_total();
							gtk_list_store_set((GtkListStore*)tm,&iter, ORG_CONV, n, -1);
							if(chdir("..")!=0){g_free(nick);return;}
						}else{g_free(nick);return;}
					}
					g_free(nick);
					valid = gtk_tree_model_iter_next(tm, &iter);
				}while(valid);
			}
		}
	}
}
static void org_changed(GtkComboBoxText *combo_box,struct stk_s*ps)//, gpointer user_data)
{
	gint current_pos=gtk_combo_box_get_active ((GtkComboBox*)combo_box);
	if(current_pos!=-1){//this is the case when last entry is deleted
		if(to_organizer_folder_go){//is possible to be in another folder
			gchar*text=gtk_combo_box_text_get_active_text (combo_box);
			//not this check, is the server folder there//if(*text!=chan..){//only if the folder is malevolently changed(this case is at list repopulation)
			char*chan=strchr(text,*not_a_nick_chan_host_start);
			//not this check, is the channel folder there//if(chan!=nullptr){//only if the folder is malevolently changed(this case is at list repopulation)
			*chan='\0';
			if(chdir(text)==0||(mkdir(text,0700)==0&&chdir(text)==0)){
				if(chdir(org_g)==0||(mkdir(org_g,0700)==0&&chdir(org_g)==0)){
					//empty current global/local
					org_clear_rules(ps->organizer_notebook);

					//retake global lists
					org_repopulate(TRUE,ps->organizer_notebook);

					if(chdir(dirback)==0){
						if(chdir(org_c)==0||(mkdir(org_c,0700)==0&&chdir(org_c)==0)){
							chan++;
							if(chdir(chan)==0||(mkdir(chan,0700)==0&&chdir(chan)==0)){
								//retake local lists
								org_repopulate(FALSE,ps->organizer_notebook);

								if(chdir(dirback)==0&&chdir(dirback)==0){
									if(chdir(org_u)==0||(mkdir(org_u,0700)==0&&chdir(org_u)==0)){
										//users conversations, here or at retake but there must malloc and form ../users/nick
										org_repopulate_conv(ps->organizer_notebook);
										//populate main tab
										send_channel_related((char*)names_str,chan,strlen(chan));
									}
								}
							}
						}
					}
				}
			}
			g_free(text);
		}
	}
	gtk_widget_set_sensitive(ps->organizer_bot,FALSE);//will be TRUE when names comes in
}

static BOOL org_delconf(struct stk_s*ps){
	if(gtk_toggle_button_get_active(ps->organizer_del_confirmation)){
		GtkWidget*dialog=gtk_message_dialog_new((GtkWindow*)ps->organizer,GTK_DIALOG_DESTROY_WITH_PARENT/*modal will be at dialog_run*/,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Delete the list?");
		gint response=gtk_dialog_run((GtkDialog*)dialog);
		gtk_widget_destroy (dialog);
		return response==GTK_RESPONSE_YES;
	}
	return TRUE;
}

static void folder_rm(){//at lists is important to not let the file because can have duplicates if reparsed
	GDir*entries=g_dir_open(".",0,nullptr);
	if(entries!=nullptr){
		for(;;){
			const gchar*file=g_dir_read_name(entries);
			if(file==nullptr)break;
			unlink(file);//the list is not if no user is in the list, that will be -1 //!=0){r=-1;break;}
		}
		g_dir_close(entries);
	}//else return -1;
	//return 0;
}
static BOOL folder_enter_rm(const gchar*folder){
	if(chdir(folder)==0){
		folder_rm();
		if(chdir("..")!=0)return FALSE;
		rmdir(folder);
	}
	return TRUE;
}
static BOOL iterate_folders_enter_rm(){
	GDir*entries=g_dir_open(".",0,nullptr);
	if(entries!=nullptr){
		for(;;){
			const gchar*dir=g_dir_read_name(entries);
			if(dir==nullptr)break;
			if(g_file_test(dir,G_FILE_TEST_IS_DIR))
				if(folder_enter_rm(dir)==FALSE){g_dir_close(entries);return FALSE;}
		}
		g_dir_close(entries);
	}//else return -1;
	return TRUE;
}
static void iterate_folders_enter(int (*f)(const char*, void*),void*data){
	GDir*entries=g_dir_open(".",0,nullptr);
	if(entries!=nullptr){
		for(;;){
			const gchar*dir=g_dir_read_name(entries);
			if(dir==nullptr)break;
			if(g_file_test(dir,G_FILE_TEST_IS_DIR)){
				if(chdir(dir)==0){
					if(f(dir,data)!=0)break;//{r=-1;break;}//has inner chdirs
					if(chdir("..")!=0)break;//{r=-1;break;}
				}else break;//{r=-1;break;}
			}
		}
		g_dir_close(entries);
	}//else return -1;
}
static BOOL iterate_folders_enter2(int (*f)(const char*),const char*data){
	GDir*entries=g_dir_open(".",0,nullptr);
	if(entries!=nullptr){
		for(;;){
			const gchar*dir=g_dir_read_name(entries);
			if(dir==nullptr)break;
			if(g_file_test(dir,G_FILE_TEST_IS_DIR)){
				if(chdir(dir)==0){
					if(f(data)!=0){g_dir_close(entries);return FALSE;}//has inner chdirs
					if(chdir("..")!=0){g_dir_close(entries);return FALSE;}
				}else break;//{r=-1;break;}
			}
		}
		g_dir_close(entries);
	}//else return -1;
	return TRUE;
}
static void iterate_folders(void (*f)(const char*, void*),void*data){
	GDir*entries=g_dir_open(".",0,nullptr);
	if(entries!=nullptr){
		for(;;){
			const gchar*dir=g_dir_read_name(entries);
			if(dir==nullptr)break;
			if(g_file_test(dir,G_FILE_TEST_IS_DIR)){
				f(dir,data);//!=0){r=-1;break;}
			}
		}
		g_dir_close(entries);
	}//else return -1;
}

static BOOL org_removechan_global(gchar*server){
	if(chdir(org_g)==0){
		folder_rm();//remove global lists
		if(chdir(dirback)==0){
			rmdir(org_g);
			if(chdir(org_u)==0){
				if(iterate_folders_enter_rm()){
					if(chdir(dirback)==0){
						rmdir(org_u);
						if(chdir(dirback)==0){
							rmdir(server);
							return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}
static void org_removechan(struct stk_s*ps){
	if(org_delconf(ps)){//is for preventing double click deletes
		if(to_organizer_folder_go){//is possible to be in another folder
			GtkComboBox *combo_box=ps->organizer_dirs;
			gchar*text=gtk_combo_box_text_get_active_text((GtkComboBoxText*)combo_box);
			char*chan=strchr(text,*not_a_nick_chan_host_start);
			*chan='\0';chan++;
			if(chdir(text)==0&&chdir(org_c)==0&&chdir(chan)==0){
				folder_rm();//remove local lists
				if(chdir(dirback)==0){
					rmdir(chan);
					if(chdir(dirback)==0){
						if(rmdir(org_c)!=0||org_removechan_global(text)){//test to see if was not the last channel in server   or   remove restof the data
							gtk_combo_box_text_remove((GtkComboBoxText*)combo_box,gtk_combo_box_get_active (combo_box));
							if(gtk_tree_model_iter_n_children(gtk_combo_box_get_model(combo_box),nullptr)==0){//if was last overall
								gtk_widget_set_sensitive(ps->organizer_removeentry,FALSE);
								pars_names_org(ps,(char*)org_new_names);
								gtk_list_store_clear(ps->organizer_entry_names);
								org_clear_rules(ps->organizer_notebook);
							}else{//there is segmentation at the moment if clicked again and nothing selected
								gtk_combo_box_set_active(combo_box,0);
							}
						}
					}
				}
			}
			g_free(text);
		}
	}
}

void organizer_tab_column_click(struct org_col*st){
	gint pos=st->pos;
	GtkTreeSortable*sort=st->sort;
	gint i;GtkSortType s;
	gboolean b=gtk_tree_sortable_get_sort_column_id (sort, &i, &s);
	// It returns TRUE unless the sort_column_id is GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID or GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID.
	if(b){
		if(i==pos){
			if(s==GTK_SORT_DESCENDING)
				gtk_tree_sortable_set_sort_column_id(sort,pos,GTK_SORT_ASCENDING);
			else
				gtk_tree_sortable_set_sort_column_id(sort,pos,GTK_SORT_DESCENDING);
		}else{
			gtk_tree_sortable_set_sort_column_id(sort,pos,GTK_SORT_DESCENDING);
		}
	}else{
		gtk_tree_sortable_set_sort_column_id(sort,pos,GTK_SORT_DESCENDING);
	}
}
static void organizer_tab_column_add(GtkTreeView*tree,char*name,int pos,GtkTreeSortable*sort){
	struct org_col*s=(struct org_col*)malloc(sizeof(struct org_col));
	if(s!=nullptr){
		s->pos=pos;s->sort=sort;
		GtkCellRenderer *renderer= gtk_cell_renderer_text_new();
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(name, renderer, "text", pos, nullptr);//"value" at progress. btw is sorting G_TYPE_INT ok
		g_signal_connect_data(column,"clicked",G_CALLBACK(organizer_tab_column_click),(gpointer)s,(GClosureNotify)free,G_CONNECT_SWAPPED);
		gtk_tree_view_column_set_expand (column,TRUE);
		gtk_tree_view_column_set_clickable(column,TRUE);
		gtk_tree_view_column_set_resizable(column,TRUE);
		gtk_tree_view_append_column(tree, column);
	}
}
static GtkListStore* organizer_tab_add(GtkNotebook*nb,char*title,GtkWidget**child_out,gboolean is_global){
	//                                                       nick             user           gender         idle in minutes server         id of append number of conversations
	GtkTreeSortable*sort;
	if(is_global)
		sort=(GtkTreeSortable*)gtk_list_store_new(ORG_N,   G_TYPE_STRING,   G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT,     G_TYPE_STRING, G_TYPE_INT,  G_TYPE_INT);//is already sortable
	else
		sort=(GtkTreeSortable*)gtk_list_store_new(ORG_N-1, G_TYPE_STRING,   G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT,     G_TYPE_STRING, G_TYPE_INT);
	//any filter can come here
	//GtkTreeModel*sort=gtk_tree_model_sort_new_with_model(list);
	//g_object_unref(list);

	//GtkTreeIter it;gtk_list_store_append(list,&it);gtk_list_store_set(list, &it, 0, "a", 1, "x", 2,"x",3,1,-1);gtk_list_store_append(list,&it);gtk_list_store_set(list, &it, 0, "b", 1, "x", 2,"x",3,2,-1);

	GtkWidget*treeV=gtk_tree_view_new_with_model((GtkTreeModel*)sort);
	g_object_unref(sort);

	organizer_tab_column_add((GtkTreeView*)treeV,(char*)"Id",ORG_ID1,sort);
	organizer_tab_column_add((GtkTreeView*)treeV,(char*)"AKA",ORG_ID2,sort);
	organizer_tab_column_add((GtkTreeView*)treeV,(char*)"Gender",ORG_GEN,sort);
	organizer_tab_column_add((GtkTreeView*)treeV,(char*)"Idle",ORG_IDLE,sort);
	organizer_tab_column_add((GtkTreeView*)treeV,(char*)"Server",ORG_SERVER,sort);
	organizer_tab_column_add((GtkTreeView*)treeV,(char*)"Index",ORG_INDEX,sort);
	if(is_global)
		organizer_tab_column_add((GtkTreeView*)treeV,(char*)"Conversations",ORG_CONV,sort);

	GtkWidget*scroll = gtk_scrolled_window_new(nullptr, nullptr);
	if(child_out!=nullptr)//only at restore but that is the highest usage anyway
		*child_out=scroll;
	gtk_container_add((GtkContainer*)scroll,treeV);
	GtkWidget*tab;
	if(is_global){
		tab = gtk_label_new (nullptr);
		gchar *markup= g_markup_printf_escaped ("<u>\%s</u>", title);
		gtk_label_set_markup (((GtkLabel*)tab), markup);
		g_free(markup);
	}else{
		tab=gtk_label_new(title);
	}
	gtk_notebook_append_page_menu(nb,scroll,tab,gtk_label_new(title));
	return (GtkListStore*)sort;
}

static void organizer_populate_dirs_chans(const char*dir,void*s){
	char*nm=server_channel_base((char*)dir,strlen(dir),((struct organizer_from_storage*)s)->server);
	if(nm!=nullptr){
		gtk_combo_box_text_append_text(((struct organizer_from_storage*)s)->box,nm);
		free(nm);
	}
}
static int organizer_populate_dirs(const char*dir,void*box){
	if(chdir("chans")==0){
		struct organizer_from_storage s={(GtkComboBoxText*)box,dir};
		iterate_folders(organizer_populate_dirs_chans,&s);//==0){
		return chdir("..");
	}
	return 0;
}

#define localrules "_local"
#define globalrules "_global"
static BOOL org_storerule(const char*text,size_t sz,gboolean is_global){//this way or search at start for files
	if(to_organizer_folder_go){
		const char*fname;if(is_global)fname=globalrules;
		else fname=localrules;
		return append_lineSz_tofile((char*)text,sz,fname);
	}
	return FALSE;
}

static int org_deleterule_global(const gchar*rule){
	if(chdir(org_g)==0){
		FILE*users=fopen(rule,"rb");
		if(users!=nullptr){
			fseek(users,0,SEEK_END);
			long end=ftell(users);
			if(end!=-1){
				char*mem=(char*)malloc(end+1);
				if(mem!=nullptr){
					fseek(users,0,SEEK_SET);
					fread(mem,end,1,users);
					mem[end]='\0';
					fclose(users);
					unlink(rule);   //remove the rule
					if(chdir(dirback)!=0){free(mem);return -1;}
					if(chdir(org_u)==0){// and at users remove conversations from what names rule has
						char*name=mem;
						for(;;){
							char*next=strchr(name,'\n');
							if(next==nullptr)break;
							*next='\0';
							if(folder_enter_rm(name)==FALSE){free(mem);return -1;}
							next++;
							name=next;
						}
						free(mem);
					}else free(mem);
				}else fclose(users);
			}else fclose(users);
		}
		return chdir(dirback);//this is for org_u (or for org_g)
	}
	return 0;
}
static int org_deleterule_local(const gchar*rule){
	if(chdir(org_c)==0){
		GDir*channels=g_dir_open(".",0,nullptr);
		if(channels!=nullptr){
			for(;;){
				const gchar*chan=g_dir_read_name(channels);
				if(chan==nullptr)break;
				if(chdir(chan)==0){
					unlink(rule);
					if(chdir(dirback)!=0){g_dir_close(channels);return -1;}
				}
			}
			g_dir_close(channels);
		}
		return chdir(dirback);
	}
	return 0;
}
static BOOL org_deleterule(GtkLabel*label){
	if(to_organizer_folder_go){
		const gchar*rule=gtk_label_get_text(label);
		gboolean is_global=gtk_label_get_use_markup(label);
		if(is_global){
			if(iterate_folders_enter2(org_deleterule_global,rule))//delete list/list_conversations at all servers
				return delete_line_fromfile(rule,globalrules);
		}
		else if(iterate_folders_enter2(org_deleterule_local,rule))//delete list at all servers+channels
			return delete_line_fromfile(rule,localrules);
	}
	return FALSE;
}

static void org_addrule(struct stk_s*ps){
	GtkWidget*dialog= gtk_dialog_new_with_buttons ("Add Rule",(GtkWindow*)ps->organizer,(GtkDialogFlags)(GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL),
			    "_Cancel",GTK_RESPONSE_CANCEL,"_OK",GTK_RESPONSE_OK,nullptr);
	GtkWidget*content=gtk_dialog_get_content_area((GtkDialog*)dialog);
	GtkWidget*options=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

	GtkWidget*name=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

	GtkWidget*nm=gtk_label_new("Name");
	gtk_box_pack_start((GtkBox*)name,nm,FALSE,FALSE,0);
	GtkWidget*entry = gtk_entry_new();
	gtk_box_pack_start((GtkBox*)name, entry, TRUE, TRUE, 0);
	gtk_box_pack_start((GtkBox*)options,name,TRUE,TRUE,0);

	GtkWidget*scope=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	GtkWidget*r1=gtk_radio_button_new_with_label(nullptr,"Local");//no mnemonics here
	gtk_box_pack_start((GtkBox*)scope,r1,FALSE,FALSE,0);
	GtkWidget*r2=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)r1,"Global");
	gtk_box_pack_start((GtkBox*)scope,r2,FALSE,FALSE,0);

	gtk_box_pack_start((GtkBox*)options,scope,FALSE,FALSE,0);
	gtk_box_pack_start((GtkBox*)content,options,TRUE,TRUE,0);
	gtk_widget_show_all (dialog);//this to see the new widgets
	int response=gtk_dialog_run((GtkDialog*)dialog);
	if(response==GTK_RESPONSE_OK){
		const gchar*text=gtk_entry_get_text((GtkEntry*)entry);
		size_t sz=strlen(text);
		if(sz!=0){
			gboolean is_global=gtk_toggle_button_get_active((GtkToggleButton*)r2);
			if(org_storerule(text,sz,is_global)){
				GtkWidget*s;
				organizer_tab_add(ps->organizer_notebook,(char*)text,&s,is_global);
				gtk_widget_show_all(s);//to see the tab
				gtk_widget_set_sensitive(ps->organizer_removerule,TRUE);
			}
		}
	}
	gtk_widget_destroy (dialog);
}
static void org_removerule(GtkWidget*thisone,struct stk_s*ps){
	GtkNotebook*nb=ps->organizer_notebook;
	gint index=gtk_notebook_get_current_page(nb);
	if(index>0){//first page is with New
		if(org_delconf(ps)){//is for preventing double click deletes
			GtkWidget*current=gtk_notebook_get_nth_page(nb,index);//scroll
			GtkWidget*label=gtk_notebook_get_tab_label(nb,current);
			if(org_deleterule((GtkLabel*)label)){
				gtk_notebook_remove_page(nb,index);
				if(index==1){//maybe was last
					if(gtk_notebook_page_num(nb,gtk_notebook_get_nth_page(nb,-1))==0){
						gtk_widget_set_sensitive(thisone,FALSE);
					}
				}
			}
		}
	}
}
//rules added
static BOOL org_restorerule(const char*name,GtkNotebook*nb,BOOL is_global){
	//already at organizer folder here
	FILE*f=fopen(name,"rb");
	if(f!=nullptr){
		BOOL ret=FALSE;
		size_t sz=delim_init;
		char*mem=(char*)malloc(delim_init);
		if(mem!=nullptr){
			while(getdelim(&mem,&sz,'\n',f)!=-1){
				mem[strlen(mem)-1]='\0';//even at last, that is the way we added
				organizer_tab_add(nb,mem,nullptr,is_global);
				ret=TRUE;
			}
			free(mem);
		}
		fclose(f);
		return ret;
	}
	return FALSE;
}

static BOOL org_query_append_str(char**mem,size_t*sz,char*fast_append,size_t*all_size,char separator){
	char extra=separator!=0?1:0;
	size_t appended=strlen(fast_append);
	size_t new_size=*sz+appended+extra;
	if(new_size>*all_size){
		char*m=(char*)realloc(*mem,new_size);
		if(m==nullptr)return FALSE;
		*mem=m;
		*all_size=new_size;
	}
	if(separator!=0){
		(*mem)[*sz]=separator;
	}
	memcpy(*mem+*sz+extra,fast_append,appended);
	*sz=new_size;
	return TRUE;
}
#define org_query_send if(org_query_append_str(&command,&sz,(char*)irc_term,&all_size,0))send_data(command,sz);
static void org_query(GtkNotebook*nb){
	GtkWidget*current=gtk_notebook_get_nth_page(nb,gtk_notebook_get_current_page(nb));//scroll
	GtkWidget*tv=gtk_bin_get_child((GtkBin*)current);
	GtkTreeModel*tm=gtk_tree_view_get_model((GtkTreeView*)tv);

	//get current order
	//-1(GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID) is critical without a func and -2(GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID) will not sort like original, so ORG_INDEX is required
	gint pos;GtkSortType type;
	gboolean orig_sort=gtk_tree_sortable_get_sort_column_id((GtkTreeSortable*)tm, &pos, &type);

	//sort by ORG_SERVER
	gtk_tree_sortable_set_sort_column_id((GtkTreeSortable*)tm,ORG_SERVER,GTK_SORT_ASCENDING);

	//sending whois server n1,..,nN
	GtkTreeIter iter;
	gboolean valid = gtk_tree_model_get_iter_first (tm, &iter);
	if(valid){//don't send wrong command if no names received
		size_t all_size=0;
		char*command=nullptr;
		char*current_server=nullptr;BOOL current_server_is_solid=FALSE;
		size_t sz;
		BOOL ok;
		do{
			gchar*server;gchar*nick;
			gtk_tree_model_get (tm, &iter, ORG_ID1, &nick, ORG_SERVER, &server, -1);//are these allocs NULL if not ok? so a simple access violation on NULL will be next
			if(current_server==nullptr||strcmp(current_server,server==nullptr?"":server)!=0){
				if(command!=nullptr){
					org_query_send
					if(current_server_is_solid)g_free(current_server);
				}
				sz=0;
				if(server==nullptr){
					current_server=(char*)"";current_server_is_solid=FALSE;
				}else{
					current_server=server;current_server_is_solid=TRUE;
				}
				if(ok=org_query_append_str(&command,&sz,(char*)whois_str,&all_size,0)){
					if(ok=org_query_append_str(&command,&sz,current_server,&all_size,' ')){
						if nickname_start(nick) {
							ok=org_query_append_str(&command,&sz,nick,&all_size,' ');
						}else ok=org_query_append_str(&command,&sz,nick+1,&all_size,' ');
					}
				}
				g_free(nick);
			}else{
				if nickname_start(nick) {
					ok=org_query_append_str(&command,&sz,nick,&all_size,',');
				}else ok=org_query_append_str(&command,&sz,nick+1,&all_size,',');
				g_free(nick);
				g_free(server);//can be NULL if not set
			}
			if(ok==FALSE)break;
			valid = gtk_tree_model_iter_next(tm, &iter);
		}while(valid);
		if(current_server!=nullptr){
			if(current_server_is_solid)g_free(current_server);
			if(command!=nullptr){
				org_query_send
				free(command);
			}
		}
	}

	//sort back
	if(orig_sort==FALSE){
		gtk_tree_sortable_set_sort_column_id((GtkTreeSortable*)tm,ORG_INDEX,GTK_SORT_ASCENDING);
	}else{
		gtk_tree_sortable_set_sort_column_id((GtkTreeSortable*)tm,pos,type);
	}
}

static void org_chat(struct stk_s*ps){
	GtkWidget*current=gtk_notebook_get_nth_page(ps->organizer_notebook,gtk_notebook_get_current_page(ps->organizer_notebook));//scroll
	GtkWidget*tv=gtk_bin_get_child((GtkBin*)current);
	name_join_main((GtkTreeView*)tv,ps);//will add an if inside the function
	gtk_window_present(ps->main_win);
}

static void org_move_files_locToGlob(const char*chan,char*nick){
	GDir*entries=g_dir_open(".",0,nullptr);
	if(entries!=nullptr){
		for(;;){
			const gchar*dir=g_dir_read_name(entries);
			if(dir==nullptr)break;
			if(strcmp(dir,chan)!=0){
				if(chdir(dir)==0){
					GDir*rules=g_dir_open(".",0,nullptr);
					if(rules!=nullptr){
						const gchar*rule;
						for(;;){
							rule=g_dir_read_name(rules);
							if(rule==nullptr)break;
							delete_line_fromfile_pref(nick,rule,FALSE);
						}
						g_dir_close(rules);
					}
					if(chdir(dirback)!=0)break;
				}
			}
		}
		g_dir_close(entries);
	}//else return FALSE;
}
static BOOL org_move_files(struct stk_s*ps,GtkWidget*prev_tab,gint prev_index,GtkWidget*current_tab,gint current_index,char*nick,gboolean is_global_previous,gboolean is_global){
	if(to_organizer_folder_server_go){//used at least for a simple name remove (local->new_entries) to double_name+conversations remove(global->local)
		GtkNotebook*nb=ps->organizer_notebook;
		if(is_global_previous&&(is_global==FALSE)){//global->0/local
			if(chdir(org_u)==0){
				if(chdir(nick)==0){//is prefixless at global
					GDir*entries=g_dir_open(".",0,nullptr);
					if(entries!=nullptr){
						for(;;){
							const gchar*file=g_dir_read_name(entries);
							if(file==nullptr)break;
							unlink(file);
						}
						g_dir_close(entries);
					}//else return FALSE;
					if(chdir(dirback)==0){
						rmdir(nick);
					}else return FALSE;
				}//else return FALSE;
				if(chdir(dirback)!=0)return FALSE;
			}//else return FALSE;
		}
		//see at global first, because is nearer than local at folders
		if(is_global_previous||is_global){
			if(chdir(org_g)==0){
				//delete
				if(is_global_previous)if(delete_line_fromfile(nick,gtk_notebook_get_menu_label_text(nb,prev_tab))==FALSE)return FALSE;
				//write
				if(is_global)if(append_line_tofile(nickname_prefixless(nick),gtk_notebook_get_menu_label_text(nb,current_tab))==FALSE)return FALSE;
				//back
				if(chdir(dirback)!=0)return FALSE;
			}//else return FALSE;
		}
		//and at local
		if(is_global_previous==FALSE||is_global==FALSE){
			if(chdir(org_c)==0){
				const char*chan=org_getchan(ps);
				if(chdir(chan)==0){
					//delete
					BOOL from_local=is_global_previous==FALSE&&prev_index!=0;
					if(from_local)if(delete_line_fromfile(nick,gtk_notebook_get_menu_label_text(nb,prev_tab))==FALSE)return FALSE;
					//write
					if(is_global==FALSE&&current_index!=0)if(append_line_tofile(nick,gtk_notebook_get_menu_label_text(nb,current_tab))==FALSE)return FALSE;
					//as a side effect, local->global, must be server unique, then search in other server channels for uniqueness
					if(from_local&&is_global){
						if(chdir(dirback)!=0)return FALSE;//back
						org_move_files_locToGlob(chan,nickname_prefixless(nick));//)==FALSE)return FALSE;
					}
				}//else return FALSE;
			}//else return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
#define org_move_scan "%u" not_a_nick_chan_host_start "%u"
static void org_move(GtkButton*button,struct stk_s*ps){
	GtkNotebook*nb=ps->organizer_notebook;
	const gchar*user=gtk_button_get_label(button);
	//the attributions are not required if user click wrong, but this case is rare
	gint nb_page_index=gtk_notebook_get_current_page(nb);
	GtkWidget*current=gtk_notebook_get_nth_page(nb,nb_page_index);//scroll
	GtkWidget*tv=gtk_bin_get_child((GtkBin*)current);
	GtkTreeModel*tm=gtk_tree_view_get_model((GtkTreeView*)tv);
	GtkTreeIter iterator;
	if(*user==*movestart){
		GtkTreeSelection *sel=gtk_tree_view_get_selection((GtkTreeView*)tv);
		gboolean selected=gtk_tree_selection_get_selected (sel,nullptr,&iterator);
		if(selected){
			gchar*item_text;
			gint tree_index=get_pos_from_model(tm,&iterator);
			char*space=(char*)malloc(digits_in_posInt+1+digits_in_posInt+1);
			if(space!=nullptr){
				sprintf(space,org_move_scan,nb_page_index,tree_index);
				gtk_button_set_label(button,space);
				free(space);
			}
		}
	}else{
		gint tab,pos;
		sscanf(user,org_move_scan,&tab,&pos);
		if(tab!=nb_page_index){//this is same place
			GtkWidget*previous=gtk_notebook_get_nth_page(nb,tab);
			if(previous!=nullptr){//can be deleted between the clicks
				GtkWidget*tvprev=gtk_bin_get_child((GtkBin*)previous);
				GtkTreeModel*tmprev=gtk_tree_view_get_model((GtkTreeView*)tvprev);
				if(gtk_tree_model_iter_nth_child(tmprev,&iterator,nullptr,pos)){//can be deleted between the clicks
					gchar*a;gchar*b;gchar*c;gint d;gchar*e;
					gint f,g;//indexes are not ok with holes, at sorting
					gboolean prev_glob=gtk_label_get_use_markup((GtkLabel*)gtk_notebook_get_tab_label(nb,previous));
					if(prev_glob)
						gtk_tree_model_get(tmprev,&iterator,ORG_ID1,&a,ORG_ID2,&b,ORG_GEN,&c,ORG_IDLE,&d,ORG_SERVER,&e,ORG_INDEX,&f,ORG_CONV,&g,-1);
					else
						gtk_tree_model_get(tmprev,&iterator,ORG_ID1,&a,ORG_ID2,&b,ORG_GEN,&c,ORG_IDLE,&d,ORG_SERVER,&e,ORG_INDEX,&f,-1);
					gboolean current_glob=gtk_label_get_use_markup((GtkLabel*)gtk_notebook_get_tab_label(nb,current));
					if(org_move_files(ps,previous,tab,current,nb_page_index,a,prev_glob,current_glob)){
						gtk_list_store_remove((GtkListStore*)tmprev,&iterator);
						if(tab!=0)org_move_indexed(tmprev,f);//index sort is not relevant there, comment at org_names_end
						if(nb_page_index!=0){//can't move back, there is a comment about this at org_names_end
							gboolean is_global=gtk_label_get_use_markup((GtkLabel*)gtk_notebook_get_tab_label(nb,current));//at global, no channel prefix for the user, attention if wanting to go back at local with no prefix
							char*current_nick;
							if(is_global)current_nick=nickname_prefixless(a);
							else current_nick=a;

							gint n=gtk_tree_model_iter_n_children(tm,nullptr);
							gtk_list_store_append((GtkListStore*)tm,&iterator);
							if(current_glob){
								if(prev_glob)gtk_list_store_set((GtkListStore*)tm, &iterator, ORG_ID1,current_nick,ORG_ID2,b,ORG_GEN,c,ORG_IDLE,d,ORG_SERVER,e,ORG_INDEX,n,ORG_CONV,g,-1);
								else gtk_list_store_set((GtkListStore*)tm, &iterator, ORG_ID1,current_nick,ORG_ID2,b,ORG_GEN,c,ORG_IDLE,d,ORG_SERVER,e,ORG_INDEX,n,ORG_CONV,0,-1);
							}else{
								gtk_list_store_set((GtkListStore*)tm, &iterator, ORG_ID1,current_nick,ORG_ID2,b,ORG_GEN,c,ORG_IDLE,d,ORG_SERVER,e,ORG_INDEX,n,-1);
							}
							//and select the moved item
							GtkTreePath * path = gtk_tree_model_get_path ( tm , &iterator );
							gtk_tree_view_set_cursor((GtkTreeView*)tv,path,nullptr,FALSE);
							gtk_tree_path_free(path);
						}
					}
					g_free(a);g_free(b);g_free(c);g_free(e);
				}
			}
		}
		gtk_button_set_label(button,movestart);
	}
}

static void organizer_populate(GtkWidget*window,struct stk_s*ps){
	GtkWidget*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	GtkWidget*top=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

	GtkWidget*decider=gtk_button_new_with_label("Update");
	g_signal_connect_data (decider, "clicked",G_CALLBACK(deciderfn),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_box_pack_start((GtkBox*)top,decider,FALSE,FALSE,0);

	GtkNotebook*nb = (GtkNotebook*)gtk_notebook_new ();  ps->organizer_notebook=nb;//here, attention at org_changed

	GtkWidget*dirs=gtk_combo_box_text_new();	ps->organizer_dirs=(GtkComboBox*)dirs;
	gtk_box_pack_start((GtkBox*)top,dirs,TRUE,TRUE,0);

	iterate_folders_enter(organizer_populate_dirs,dirs);//read from stored data

	GtkWidget*buttonspack=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	GtkWidget*buttons=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

	GtkWidget*remove_chan=gtk_button_new_with_label("X");	ps->organizer_removeentry=remove_chan;
	g_signal_connect_data (remove_chan, "clicked",G_CALLBACK(org_removechan),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_box_pack_start((GtkBox*)buttons,remove_chan,FALSE,FALSE,0);
	GtkWidget*add_folder=gtk_button_new_with_label("+");
	g_signal_connect_data (add_folder, "clicked",G_CALLBACK(org_addrule),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_box_pack_start((GtkBox*)buttons,add_folder,FALSE,FALSE,0);
	GtkWidget*remove_folder=gtk_button_new_with_label("-");    ps->organizer_removerule=remove_folder;
	g_signal_connect_data (remove_folder, "clicked",G_CALLBACK(org_removerule),ps,nullptr,(GConnectFlags)0);
	gtk_box_pack_start((GtkBox*)buttons,remove_folder,FALSE,FALSE,0);
	gtk_box_pack_start((GtkBox*)buttonspack,buttons,FALSE,FALSE,0);

	GtkWidget*del_confirmation=gtk_check_button_new_with_mnemonic("_Delete confirmation");//Alt+D
	gtk_toggle_button_set_active((GtkToggleButton*)del_confirmation,TRUE);
	gtk_box_pack_start((GtkBox*)buttonspack,del_confirmation,FALSE,FALSE,0);
	ps->organizer_del_confirmation=(GtkToggleButton*)del_confirmation;

	gtk_box_pack_start((GtkBox*)top,buttonspack,FALSE,FALSE,0);
	gtk_box_pack_start((GtkBox*)box,top,FALSE,FALSE,0);

	gtk_notebook_popup_enable(nb);
	ps->organizer_entry_names=organizer_tab_add(nb,(char*)org_new_names,&ps->organizer_entry_widget,FALSE);
	//and add rest of the rules if are defined
	BOOL stored_rules=org_restorerule(localrules,nb,FALSE);
	if(org_restorerule(globalrules,nb,TRUE)==FALSE&&stored_rules==FALSE)gtk_widget_set_sensitive (remove_folder,FALSE);
	//
	gtk_box_pack_start((GtkBox*)box,(GtkWidget*)nb,TRUE,TRUE,0);

	GtkWidget*bot=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);     ps->organizer_bot=bot;//used if repopulation and set active, changed callback

	g_signal_connect_data (dirs, "changed",G_CALLBACK(org_changed),ps,nullptr,(GConnectFlags)0);
	if(gtk_tree_model_iter_n_children(gtk_combo_box_get_model((GtkComboBox*)dirs),nullptr)==0){
		//is after repopulation
		gtk_widget_set_sensitive (remove_chan,FALSE);
		gtk_widget_set_sensitive(bot,FALSE);//is set inside changed callback
	}
	else{//set active and chdir inside here after rules restore
		gtk_combo_box_set_active((GtkComboBox*)dirs,0);
	}

	GtkWidget*move=gtk_button_new_with_label(movestart);
	g_signal_connect_data (move, "clicked",G_CALLBACK(org_move),ps,nullptr,(GConnectFlags)0);
	gtk_box_pack_start((GtkBox*)bot,move,FALSE,FALSE,0);
	GtkWidget*query=gtk_button_new_with_label("Query");
	g_signal_connect_data (query, "clicked",G_CALLBACK(org_query),nb,nullptr,G_CONNECT_SWAPPED);
	gtk_box_pack_start((GtkBox*)bot,query,FALSE,FALSE,0);
	GtkWidget*chat=gtk_button_new_with_label("Chat");
	g_signal_connect_data (chat, "clicked",G_CALLBACK(org_chat),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_box_pack_start((GtkBox*)bot,chat,FALSE,FALSE,0);
	ps->organizer_entry_widget_timestamp=gtk_label_new(nullptr);
	gtk_box_pack_start((GtkBox*)bot,ps->organizer_entry_widget_timestamp,FALSE,FALSE,0);
	gtk_box_pack_start((GtkBox*)box,bot,FALSE,FALSE,0);

	gtk_container_add ((GtkContainer*)window, box);
}

static void mainclose_names(GtkNotebook*nb,GtkWidget*men){
	GList*list=gtk_container_get_children((GtkContainer*)men);
	if(list!=nullptr){
		GList*lst=list;
		for(;;){
			GtkWidget*nb_page=get_pan_from_menu(list->data);
			GtkWidget*txv=gtk_bin_get_child((GtkBin*)nb_page);

			g_signal_emit_by_name(txv,destroy);//can call normally but there is a malloc data for each one that is passed as func data, must find a way that way?
			//and disconnect, if that malloc data, can disconnect the function only next
			g_signal_handlers_disconnect_matched(txv,G_SIGNAL_MATCH_ID,g_signal_lookup(destroy, gtk_text_view_get_type()),0,nullptr,nullptr,nullptr);

			list=g_list_next(list);
			if(list==nullptr)break;
		}
		g_list_free(lst);
	}
}
static void everything_destroy_from_mainclose(struct stk_s*ps){
	gtk_widget_destroy(menuwithtabs);
	if(ps->organizer!=nullptr){
		//need to save conversations and remove those handlers
		mainclose_names(ps->notebook,name_on_menu);
		mainclose_names(ps->notebook,name_off_menu);
		gtk_window_close((GtkWindow*)ps->organizer);
	}
}
static void organizer_destroy_from_selfclose(struct stk_s*ps){
	//here is also coming from main close
	ps->organizer=nullptr;
}
static void organizer_popup(struct stk_s*ps){
	if(ps->organizer==nullptr){
		if(to_organizer_folder_go){
			GtkWidget *win = gtk_application_window_new ((GtkApplication*)ps->app);
			ps->organizer=win;
			//GtkWidget *dialog = gtk_dialog_new_with_buttons("Organizer",  nullptr, (GtkDialogFlags)0,  "_Done",GTK_RESPONSE_NONE,nullptr);//still is on top

			gtk_window_set_title((GtkWindow*)win, "Organizer");
			g_signal_connect_data(win,"destroy",G_CALLBACK(organizer_destroy_from_selfclose),ps,nullptr,G_CONNECT_SWAPPED);

			int w;int h;
			gtk_window_get_size(ps->main_win,&w,&h);w*=0xf;
			gtk_window_set_default_size((GtkWindow*)win,w/0x10,h);//h is not doing right for this width

			organizer_populate(win,ps);

			gtk_widget_show_all(win);
			//gtk_window_unmaximize((GtkWindow*)win);//at this dims will be automaximized, at dims/2 will not be automaximized  //is not working here
		}
	}else gtk_window_present((GtkWindow*)ps->organizer);
}

static gboolean prog_key_press (struct stk_s*ps, GdkEventKey  *event){
	if(event->type==GDK_KEY_PRESS){
		if((event->state&GDK_CONTROL_MASK)!=0){
			unsigned int K=gdk_keyval_to_upper(event->keyval);
			if((event->state&GDK_SHIFT_MASK)==0){
				if(K==GDK_KEY_T){
					GList*lst=gtk_container_get_children((GtkContainer*)menuwithtabs);
					GList*list=lst;
					for(;;){
						list=g_list_next(list);
						if(list==nullptr)break;
						gtk_widget_destroy((GtkWidget*)list->data);
					}
					g_list_free(lst);
					reload_tabs(chan_menu,menuwithtabs,ps->notebook);//sorted from here
					reload_tabs(name_on_menu,menuwithtabs,ps->notebook);
					reload_tabs(name_off_menu,menuwithtabs,ps->notebook);
					gtk_menu_popup_at_widget((GtkMenu*)menuwithtabs,(GtkWidget*)ps->notebook,GDK_GRAVITY_NORTH_WEST,GDK_GRAVITY_NORTH_WEST,nullptr);
				}else if(K==GDK_KEY_Q)action_to_close();
				else if(K==GDK_KEY_O)organizer_popup(ps);
			}else{
				if(K==GDK_KEY_C){
					GtkWidget*pg=gtk_notebook_get_nth_page(ps->notebook,gtk_notebook_get_current_page(ps->notebook));
					if(is_home(gtk_notebook_get_menu_label_text(ps->notebook,pg))==FALSE)gtk_button_clicked((GtkButton*)tab_close_button(ps->notebook,pg));
				}else if(K==GDK_KEY_X)g_application_quit(ps->app);
			}
		}else if(event->keyval==GDK_KEY_Up&&gtk_widget_is_focus(ps->sen_entry)){
			if(send_entry_list_cursor!=send_entry_list->head){
				send_entry_list_cursor=send_entry_list_cursor==nullptr?
					send_entry_list->tail
					:send_entry_list_cursor->prev;
				gtk_entry_set_text((GtkEntry*)ps->sen_entry,(const char*)send_entry_list_cursor->data);
			}
			return TRUE;//lost focus other way
		}else if(event->keyval==GDK_KEY_Down&&gtk_widget_is_focus(ps->sen_entry)){
			if(send_entry_list_cursor!=nullptr){
				send_entry_list_cursor=send_entry_list_cursor->next;
				GtkEntryBuffer*buf=gtk_entry_get_buffer((GtkEntry*)ps->sen_entry);
				gtk_entry_buffer_delete_text(buf,0,-1);
				if(send_entry_list_cursor!=nullptr)gtk_entry_buffer_insert_text(buf,0,(const char*)send_entry_list_cursor->data,-1);
			}
			return TRUE;//is trying to switch focus
		}
	}
	return FALSE;//propagation seems fine, for "other handlers"
}

static void
activate (GtkApplication* app,
          struct stk_s*ps)
{
	ps->app=(GApplication*)app;
	/* Create a window with a title, and a default size */
	GtkWidget *window = gtk_application_window_new (app);
	menuwithtabs=gtk_menu_new();
	//
	g_signal_connect_data (window,"destroy",G_CALLBACK(everything_destroy_from_mainclose),ps,nullptr,G_CONNECT_SWAPPED);
	ps->organizer=nullptr;
	//moved to everything_destroy_from_mainclose: g_signal_connect_data (window,"destroy",G_CALLBACK(gtk_widget_destroy),menuwithtabs,nullptr,G_CONNECT_SWAPPED);
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
	menu_item = gtk_menu_item_new_with_label ("Organizer");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (organizer_popup),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	menu_item = gtk_menu_item_new_with_label ("Copy to Clipboard");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (clipboard_tev),ps->notebook,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
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
	menu_item = gtk_menu_item_new_with_label ("Channel Minimum Users");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (chan_reMin),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	show_time=(GtkCheckMenuItem*)gtk_check_menu_item_new_with_label("Show Message Timestamp");
	if(ps->timestamp)gtk_check_menu_item_set_active(show_time,TRUE);
	gtk_menu_shell_append ((GtkMenuShell*)menu,(GtkWidget*)show_time);gtk_widget_show((GtkWidget*)show_time);
	//
	channels_counted=(GtkCheckMenuItem*)gtk_check_menu_item_new_with_label("Sort Channels by Number");
	gtk_check_menu_item_set_active(channels_counted,TRUE);
	g_signal_connect_data (channels_counted, "toggled",G_CALLBACK(channels_sort),nullptr,nullptr,(GConnectFlags)0);
	gtk_menu_shell_append ((GtkMenuShell*)menu,(GtkWidget*)channels_counted);gtk_widget_show((GtkWidget*)channels_counted);
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
	gtk_widget_show_all (window);
	ps->main_win=(GtkWindow*)window;
	//
	GtkWidget*info=gtk_image_new_from_icon_name ("dialog-information",GTK_ICON_SIZE_MENU);
	gtk_notebook_set_action_widget(ps->notebook,info,GTK_PACK_END);
	g_signal_connect_data (ps->notebook, "switch-page",G_CALLBACK (nb_switch_page),ps->sen_entry,nullptr,(GConnectFlags)0);//this,before show,was critical;
	info_path_name_restore((GtkComboBoxText*)en,entext,ps);
	g_signal_connect_data (window, "key-press-event",G_CALLBACK (prog_key_press),ps,nullptr,G_CONNECT_SWAPPED);
	//
	if(ps->maximize)gtk_window_maximize((GtkWindow*)window);//problem? maybe is since dual monitor and only after reboot. gtk_test_widget_wait_for_draw(window) ?
	if(ps->minimize)gtk_window_iconify((GtkWindow*)window);
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
	puts("Would remove:");
	if(info_path_name!=nullptr){
		if(access(info_path_name,F_OK)==0){
			puts(info_path_name);
		}
	}
	to_organizer_folder(TRUE,FALSE);
	puts("yes ?");
	int e=getchar();
	if(e=='y'){
		e=getchar();
		if(e=='e'){
			e=getchar();
			if(e=='s'){
				if(unlink(info_path_name)==0)printf("%s" removed_string new_line,info_path_name);
				to_organizer_folder(TRUE,TRUE);
				return;
			}
		}
	}
	puts("expecting \"yes\"");
}

//struct stack_dict { GHashTable *values;  gsize magic;};
//#define GVSD(d)                 ((struct stack_dict *) (d))

static gint handle_local_options (struct stk_s* ps, GVariantDict*options){
	//1 dimensions_id
	char*result;
	if(g_variant_dict_lookup (options, ps->args[dimensions_id], "s", &result)){//missing argument is not reaching here
		char*b=strchr(result,'x');
		if(b!=nullptr){*b='\0';b++;}
		ps->dim[0]=atoi(result);
		ps->dim[1]=b!=nullptr?atoi(b):ps->dim[0];
		g_free(result);
	}else ps->dim[0]=-1;//this is default at gtk
	//2 connection_number_id
	int nr;
	if (g_variant_dict_lookup (options,ps->args[connection_number_id], "i", &nr)){//if 0 this is false here
		if(nr<con_nr_min||nr>con_nr_max){
			printf("%s must be from " con_nr_nrs " interval, \"%i\" given.\n",ps->args[connection_number_id],nr);
			return EXIT_FAILURE;//warning: this is same as not executing this function, if wanting that return, change this
		}
		ps->con_type=(unsigned char)nr;
	}else ps->con_type=default_connection_number;
	//3 right_id
	if (g_variant_dict_lookup (options,ps->args[right_id], "i", &ps->separator)==FALSE)//they are already G_OPTION_ARG_INT (gint)
	ps->separator=default_right;//passed to gtk_widget_set_size_request
	//4 refresh_id
	//if (g_variant_dict_lookup (options,ps->args[refresh_id], "i", &ps->refresh)==FALSE)//but at 0 is not ok
	//same for gpointer p;g_hash_table_lookup_extended(GVSD(options)->values,ps->args[refresh_id],nullptr,&p);//pointer(not getting the value)/0
	gchar*temp;
	if (g_variant_dict_lookup (options,ps->args[refresh_id], "s", &temp)){
		if(sscanf(temp,"%u",&ps->refresh)!=1){//EOF is not, "" is error catched before entering the dict
			puts("Refresh interval argument error");
			return EXIT_FAILURE;
		}
		g_free(temp);
	}else ps->refresh=default_refresh;//passed to g_timeout_add
	//5 timestamp_id
	ps->timestamp=g_variant_dict_contains(options,ps->args[timestamp_id]);
	//6 chan_min_id
	if (g_variant_dict_lookup (options,ps->args[chan_min_id], "i", &ps->chan_min)==FALSE)
		ps->chan_min=default_chan_min;
	//7 visible_id
	ps->visible=g_variant_dict_contains(options,ps->args[visible_id]);
	//8 hide_id
	ps->show_msgs=g_variant_dict_contains(options,ps->args[hide_id])==FALSE;
	//9 maximize_id
	ps->maximize=g_variant_dict_contains(options,ps->args[maximize_id]);
	//10 minimize_id
	ps->minimize=g_variant_dict_contains(options,ps->args[minimize_id]);
	//11 welcomeNotice_id
	ps->wnotice=g_variant_dict_contains(options,ps->args[welcomeNotice_id]);
	//12 chans_max_id
	if (g_variant_dict_lookup (options,ps->args[chans_max_id], "i", &ps->chans_max)==FALSE)
		ps->chans_max=default_chans_max;
	//13 send_history_id
	if (g_variant_dict_lookup (options,ps->args[send_history_id],"i",&ps->send_history)==FALSE)
		ps->send_history=default_send_history;

	//14 removeconf_id
	if(g_variant_dict_contains(options,ps->args[removeconf_id])/*true*/){
		remove_config();
		return EXIT_SUCCESS;
	}

	//these are after allocs where set to allocated mem or 0/nullptr
	ps->handle_command_line_callback_was_executed=TRUE;

	//15 nick_id
	if (g_variant_dict_lookup (options,ps->args[nick_id],"s",&ps->nick)==FALSE)
		ps->nick=nullptr;
	//16 welcome_id
	if(g_variant_dict_lookup(options,ps->args[welcome_id],"s",&ps->welcome)==FALSE)
		ps->welcome=nullptr;
	//17 user_id
	if(g_variant_dict_lookup(options,ps->args[user_id],"s",&ps->user_irc))//they are already G_OPTION_ARG_STRING
		ps->user_irc_free=TRUE;//-Wstring-compare tells the result is unspecified against a #define
	else{ps->user_irc=default_user;ps->user_irc_free=FALSE;}
	//18 log_id
	GVariant*v=g_variant_dict_lookup_value(options,ps->args[log_id],G_VARIANT_TYPE_STRING);
	if(v!=nullptr){
		const char*a=g_variant_get_string(v,nullptr);//return [transfer none]
		log_file=open(a,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
		g_variant_unref(v);
	}
	//19 ignore_id
	if(g_variant_dict_lookup(options,ps->args[ignore_id],"s",&ps->ignores_mem))
		gather_parse(&ps->ignores_sum,ps->ignores_mem,&ps->ignores);
	else ps->ignores_sum=0;
	//20 run_id
	if(g_variant_dict_lookup(options,ps->args[run_id],"s",&ps->execute_newmsg)==FALSE)
		ps->execute_newmsg=nullptr;
	//21 autojoin_id
	if(g_variant_dict_lookup(options,ps->args[autojoin_id],"s",&ps->ajoins_mem))
		parse_autojoin(ps);
	else ps->ajoins_sum=0;
	//22 password_id
	if (g_variant_dict_lookup (options,ps->args[password_id],"s",&ps->password)==FALSE)
		ps->password=nullptr;

	//23 autoconnect_id has a callback

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
		g_application_add_main_option((GApplication*)app,ps.args[ignore_id],ps.args_short[ignore_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Ignore private messages from nicknames. The format is the same as \"" autojoin_str "\".","\"I1,N1,N2...NN I2... ... IN...\"");
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
		ps.args[refresh_id]="refresh";ps.args_short[refresh_id]='f';//when ARG_INT and comming 0 it will not go further into the dict to know, ARG_STRING can do it
		g_application_add_main_option((GApplication*)app,ps.args[refresh_id],ps.args_short[refresh_id],G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Refresh channels interval in seconds. Default " INT_CONV_STR(default_refresh) ". Less than 1 to disable.","SECONDS");//-f -1 is valid less than 1
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
		ps.handle_command_line_callback_was_executed=FALSE;

		send_entry_list=g_queue_new();
		info_path_name_set(argv[0]);

		int exitcode=g_application_run ((GApplication*)app, argc, argv);//gio.h>gapplication.h gio-2.0
		g_object_unref (app);

		if(info_path_name!=nullptr)free(info_path_name);
		g_queue_free_full(send_entry_list,g_free);

		if(ps.handle_command_line_callback_was_executed==TRUE){//or !=EXIT_FAILURE, but can be many exit scenarios
			if(ps.nick!=nullptr)g_free(ps.nick);
			if(ps.welcome!=nullptr)g_free(ps.welcome);
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wcast-qual"
			if(ps.user_irc_free)g_free((gpointer)ps.user_irc);
			#pragma GCC diagnostic pop
			if(log_file!=-1)close(log_file);
			if(ps.execute_newmsg!=nullptr)g_free(ps.execute_newmsg);
			gather_free(ps.ajoins_sum,ps.ajoins_mem,ps.ajoins);
			gather_free(ps.ignores_sum,ps.ignores_mem,ps.ignores);
		}
	}else puts("openssl error");
	return EXIT_SUCCESS;
}

/*example entries for .sircinfo
irc.libera.chat:6697
chat.freenode.net:6697,7000,7070
@bucharest.ro.eu.undernet.org:6660-6669
chat.freenode.net:6697,7000,7070;6665-6667,8000-8002
@127.0.0.1
zonder:@irc.us.ircnet.net
@0.0.0.0
@127.0.0.1:6666-6667
@127.0.0.1:6697;6667-6669
*/
//example of rules for organizer:: local rules: staff/try . global rules: talks/far/have/no/male/not
