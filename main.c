
#include "inc/null.h"
#include "inc/bool.h"

#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#else
#include "inc/libgen.h"
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#include "inc/limits.h"
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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include "inc/unistd.h"
#endif

#ifdef HAVE_GTK_GTK_H
#pragma GCC diagnostic push//there are 2 more ignors in the program
#pragma GCC diagnostic ignored "-Weverything"
#include <gtk/gtk.h>
#pragma GCC diagnostic pop
#else
#include "inc/gtk.h"
#endif

static GtkTextView *text_view;static GtkTextMark *text_mark_end;
static GtkWidget*home_page;static GtkListStore*channels;
#define text_mark_atend "a"
static SSL *ssl=nullptr;static int plain_socket=-1;
static int con_th=-1;//static GThread*con_th=nullptr;
static int portindex;static int portend;
#define ssl_con_try "Trying with SSL.\n"
#define ssl_con_no "Trying unencrypted.\n"
#define irc_bsz 64
//"510"
#define irc_term "\r\n"
#define irc_term_sz sizeof(irc_term)-1
#define hostname_sz 512//arranging
#define password_sz 505+1//fitting
#define password_con "PASS %s" irc_term
#define nickname_con "NICK %s" irc_term
static char*info_path_name=nullptr;
#define help_text "Most of the parameters are set at start.\n\
Launch the program with --help argument for more info.\n\
\n\
Connection format is [[nickname:]password@]hostname[:port1[-portn]].\n\
e.g. newNick:abc@127.0.0.1:6665-6669"
#define channm_sz 201
//"up to 200 characters"
#define channame_scan "%200s"
#define namenul_sz 10//9 char
#define name_scan1 "%9"
#define name_scan name_scan1 "s"
struct data_len{
	const char*data;size_t len;
};
static long threadid;static sigset_t threadset;
static GtkWidget*chan_menu;
static GtkWidget*name_menu;
#define send_prv1 "PRIVMSG "
#define send_prv2 " :"
#define home_string "^Home"
static unsigned int alert_counter;

enum {
  LIST_ITEM = 0,
  N_COLUMNS
};//connections org,channels
struct stk_s{
	const char*args[4];
	int dim[2];char*path;GtkComboBoxText*cbt;GtkTreeView*tv;
	char*nick;const char*text;char*nknnow;
	int separator;
	GtkWidget*con_entry;gulong con_entry_act;GtkWidget*sen_entry;gulong sen_entry_act;
	unsigned int refresh;unsigned int refreshid;
	GtkNotebook*notebook;
	struct data_len*dl;
};

#define contf_get_list(pan) (GtkListStore*)gtk_tree_view_get_model((GtkTreeView*)gtk_bin_get_child((GtkBin*)gtk_paned_get_child2((GtkPaned*)pan)))
#define contf_get_textv(pan) (GtkTextView*)gtk_bin_get_child((GtkBin*)gtk_paned_get_child1((GtkPaned*)pan))
#define textv_get_atend(tv) gtk_text_buffer_get_mark(gtk_text_view_get_buffer(tv),text_mark_atend)
static void addattextview(struct data_len*b){
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (text_view);
	GtkTextIter it;gtk_text_buffer_get_end_iter(text_buffer,&it);
	gtk_text_buffer_insert(text_buffer,&it,b->data,(int)b->len);
	/* now scroll to the end using marker */
	gtk_text_view_scroll_to_mark (text_view, text_mark_end, 0., FALSE, 0., 0.);
}
static void addattextv(GtkTextView*v,char*n,const char*msg){
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (v);
	GtkTextIter it;gtk_text_buffer_get_end_iter(text_buffer,&it);
	gtk_text_buffer_insert(text_buffer,&it,n,-1);
	gtk_text_buffer_insert(text_buffer,&it,"> ",2);
	gtk_text_buffer_insert(text_buffer,&it,msg,-1);
	gtk_text_buffer_insert(text_buffer,&it,"\n",1);
	gtk_text_view_scroll_to_mark (v, textv_get_atend(v), 0., FALSE, 0., 0.);
}
#define addatchans(n,msg,p) addattextv(contf_get_textv(p),n,msg)
#define addatnames(n,msg,p) addattextv((GtkTextView*)gtk_bin_get_child((GtkBin*)p),n,msg)
static gboolean textviewthreadsfunc(gpointer b){
	addattextview((struct data_len*)b);
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
	if(ssl!=nullptr)SSL_write(ssl,str,(int)sz);
	else write(plain_socket,str,sz);
}
#define sendlist "LIST" irc_term
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
//
static BOOL parse_host_str(const char*indata,char*hostname,char*psw,char*nkn,int*sens,struct stk_s*ps) {
	size_t sz=strlen(indata);
	//
	const char*left=strchr(indata,'@');BOOL nonick=TRUE;
	if(left!=nullptr){
		size_t lsz=(size_t)(left-indata);
		size_t i=lsz;
		while(i>0){
			i--;
			if(indata[i]==':'){
				if(i>=namenul_sz)return FALSE;
				if(i>0){
					memcpy(nkn,indata,i);nkn[i]='\0';nonick=FALSE;
				}
				i++;
				break;
			}
		}
		size_t psz=lsz-i;
		if(psz>=password_sz)return FALSE;
		memcpy(psw,indata+i,psz);psw[psz]='\0';
		sz-=(size_t)(left+1-indata);indata=left+1;
	}else *psw='\0';
	if(nonick){
		if(ps->nick!=nullptr)memcpy(nkn,ps->nick,strlen(ps->nick)+1);
		else{
			const char def_n[]="guest_abc";
			memcpy(nkn,def_n,sizeof(def_n));
		}
	}
	ps->nknnow=nkn;//can be only at this go
	//
	const char*ptr=strchr(indata,':');
	if(ptr!=nullptr)sz=(size_t)(ptr-indata);
	if(sz<hostname_sz){
		memcpy(hostname, indata, sz);
		hostname[sz]='\0';
		if(ptr==nullptr){
			portindex=6667;portend=6667;
			return TRUE;
		}
		ptr++;
		const char*mid=strchr(ptr,'-');
		if(mid==nullptr){
			portindex=atoi(ptr);portend=portindex;
			return portindex<0x10000;
		}
		char portnum[6];
		size_t p1sz=(size_t)(mid-ptr);
		if(p1sz>=6)return FALSE;
		memcpy(portnum,ptr,p1sz);portnum[p1sz]='\0';
		portindex=atoi(portnum);
		portend=atoi(mid+1);
		if(portend>0xffFF)return FALSE;
		if(portindex<=portend)*sens=1;
		else *sens=-1;
		return TRUE;
	}
	return FALSE;
}
/* ---------------------------------------------------------- *
 * create_socket() creates the socket & TCP-connect to server *
 * ---------------------------------------------------------- */
static int create_socket(char*hostname,int port) {
	int sockfd;
	struct hostent *host;
	struct sockaddr_in dest_addr;
	if ( (host = gethostbyname(hostname)) != nullptr ) {
		  /* ---------------------------------------------------------- *
		   * create the basic TCP socket                                *
		   * ---------------------------------------------------------- */
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd!=-1){
			dest_addr.sin_family=AF_INET;
			dest_addr.sin_port=htons(port);
			dest_addr.sin_addr.s_addr = *(unsigned long*)((void*)(host->h_addr_list[0]));
			//  memset(&(dest_addr.sin_zero), '\0', 8);//string
			//"setting it to zero doesn't seem to be actually necessary"
			  /* ---------------------------------------------------------- *
			   * Try to make the host connect here                          *
			   * ---------------------------------------------------------- */
			if ( connect(sockfd, (struct sockaddr *) &dest_addr,
				sizeof(struct sockaddr)) != -1 ) {
				return sockfd;
			}
			else{
				main_text_s("Error: Cannot connect to host.\n");
				close(sockfd);
			}
		}else main_text_s("Error: Cannot open the socket.\n");
	}
	else
		main_text_s("Error: Cannot resolve hostname.\n");
	return -1;
}
static void pars_chan_end(GtkTreeIter*it,char*channm,unsigned int nr){
	size_t ln=strlen(channm);channm[ln]=' ';sprintf(channm+ln+1,"%u",nr);
	gtk_list_store_set(channels, it, LIST_ITEM, channm, -1);
}
static void pars_chan(char*chan,unsigned int nr){
	GtkTreeIter it;
	gboolean valid=gtk_tree_model_get_iter_first ((GtkTreeModel*)channels, &it);
	while(valid){
		char c[channm_sz];unsigned int n;GtkTreeIter i;
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)channels, &it, 0, &text, -1);
		sscanf(text,channame_scan " %u",c,&n);
		int a=strcmp(chan,c);
		g_free(text);
		if(nr>n||(nr==n&&a<0)){
			gtk_list_store_insert_before(channels,&i,&it);
			pars_chan_end(&i,chan,nr);
			return;
		}
		valid = gtk_tree_model_iter_next( (GtkTreeModel*)channels, &it);
	}
	gtk_list_store_append(channels,&it);
	pars_chan_end(&it,chan,nr);
}
static GtkWidget*container_frame(int sep,GCallback click,gpointer ps){
	  /* Text view is a widget in which can display the text buffer.
	   * The line wrapping is set to break lines in between words.
	   */
	GtkTextView*text = (GtkTextView*)gtk_text_view_new ();
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (text);
	  /* The text buffer represents the text being edited */
	GtkTextIter text_iter_end;
	gtk_text_buffer_get_end_iter (text_buffer, &text_iter_end);
	gtk_text_buffer_create_mark (text_buffer,
	                             text_mark_atend,
	                             &text_iter_end,
	                             FALSE);
	gtk_text_view_set_editable(text, FALSE);
	gtk_text_view_set_wrap_mode (text, GTK_WRAP_WORD);
	  /* Create the scrolled window. Usually nullptr is passed for both parameters so 
	   * that it creates the horizontal/vertical adjustments automatically. Setting 
	   * the scrollbar policy to automatic allows the scrollbars to only show up 
	   * when needed. 
	   */
	GtkWidget *scrolled_window = gtk_scrolled_window_new (nullptr, nullptr);
	gtk_scrolled_window_set_policy ((GtkScrolledWindow*) scrolled_window,
	                                  GTK_POLICY_AUTOMATIC,
	                                  GTK_POLICY_AUTOMATIC);
	  /* The function directly below is used to add children to the scrolled window 
	   * with scrolling capabilities (e.g text_view), otherwise, 
	   * gtk_scrolled_window_add_with_viewport() would have been used
	   */
	gtk_container_add ((GtkContainer*) scrolled_window,
	                                       (GtkWidget*) text);
	gtk_container_set_border_width ((GtkContainer*)scrolled_window, 5);
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
	g_signal_connect_data(tree,"button-release-event",click,ps,nullptr,(GConnectFlags)0);
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
static GtkWidget*container_frame_name(){
	GtkTextView*text = (GtkTextView*)gtk_text_view_new ();
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (text);
	GtkTextIter text_iter_end;
	gtk_text_buffer_get_end_iter (text_buffer, &text_iter_end);
	gtk_text_buffer_create_mark (text_buffer,
	                             text_mark_atend,
	                             &text_iter_end,
	                             FALSE);
	gtk_text_view_set_editable(text, FALSE);
	gtk_text_view_set_wrap_mode (text, GTK_WRAP_WORD);
	GtkWidget *scrolled_window = gtk_scrolled_window_new (nullptr, nullptr);
	gtk_scrolled_window_set_policy ((GtkScrolledWindow*) scrolled_window,
	                                  GTK_POLICY_AUTOMATIC,
	                                  GTK_POLICY_AUTOMATIC);
	gtk_container_add ((GtkContainer*) scrolled_window,
	                                       (GtkWidget*) text);
	gtk_container_set_border_width ((GtkContainer*)scrolled_window, 5);
	return scrolled_window;
}
#define get_pan_from_menu(x) gtk_label_get_mnemonic_widget((GtkLabel*)gtk_bin_get_child ((GtkBin*)x))
static void chan_popup(GtkWidget*menuitem,GtkNotebook*nb){
	GtkWidget*pan=get_pan_from_menu(menuitem);
	gtk_notebook_set_current_page(nb,gtk_notebook_page_num(nb,pan));
}
static void close_channel(GtkLabel*t){
	char buf[4+channm_sz+irc_term_sz]="PART ";
	const char*a=gtk_label_get_text(t);
	size_t n=strlen(a);
	memcpy(buf+5,a,n);memcpy(&buf[5+n],irc_term,irc_term_sz);
	send_data(buf,5+irc_term_sz+n);
}
static void close_name(GtkWidget*mn){
	gtk_widget_destroy(get_pan_from_menu(mn));
	gtk_widget_destroy(mn);
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
	//
	GtkWidget*menu_item = gtk_menu_item_new_with_label (title);
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (chan_popup),notebook,nullptr,(GConnectFlags)0);
	GtkWidget*lab=gtk_bin_get_child ((GtkBin*)menu_item);
	gtk_label_set_mnemonic_widget((GtkLabel*)lab,frame);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);
	gtk_widget_show(menu_item);
	*cls=close;
	return is_name?menu_item:t;
}
static gboolean chan_join (GtkTreeView *tree){
	GtkTreeSelection *sel=gtk_tree_view_get_selection(tree);
	GtkTreeIter iterator;
	if(gtk_tree_selection_get_selected (sel,nullptr,&iterator)){
		char buf[4+channm_sz+irc_term_sz]="JOIN ";char*item_text;
		gtk_tree_model_get ((GtkTreeModel*)channels, &iterator, LIST_ITEM, &item_text, -1);
		for(size_t i=0;;i++){
			if(item_text[i]==' '){
				memcpy(buf+5,item_text,i);
				memcpy(buf+5+i,irc_term,irc_term_sz);
				send_data(buf,5+irc_term_sz+i);
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
	GList*list=gtk_container_get_children((GtkContainer*)name_menu);
	if(list!=nullptr){
		GList*lst=list;
		do{
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(n,d)==0){
				g_list_free(lst);
				gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,get_pan_from_menu(menu_item)));
				return FALSE;
			}
		}while((list=g_list_next(list))!=nullptr);
		g_list_free(lst);
	}
	return TRUE;
}
static GtkWidget* name_join_nb(char*t,GtkNotebook*nb){
	GtkWidget*scrl=container_frame_name();
	GtkWidget*close;GtkWidget*mn=add_new_tab(scrl,t,&close,nb,name_menu,TRUE);
	g_signal_connect_data (close, "clicked",G_CALLBACK (close_name),mn,nullptr,G_CONNECT_SWAPPED);//not "(GClosureNotify)gtk_widget_destroy" because at restart clear will be trouble
	return scrl;
}
static gboolean name_join(GtkTreeView*tree,GdkEvent*ignored,struct stk_s*ps){
	(void)ignored;
	GtkTreeSelection *sel=gtk_tree_view_get_selection(tree);
	GtkTreeIter iterator;
	gtk_tree_selection_get_selected (sel,nullptr,&iterator);
	char*item_text;
	gtk_tree_model_get (gtk_tree_view_get_model(tree), &iterator, LIST_ITEM, &item_text, -1);
	char*a=*item_text!='@'?item_text:item_text+1;
	if(name_join_isnew(ps,a)){
		gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,name_join_nb(a,ps->notebook)));
		gtk_widget_grab_focus(ps->sen_entry);
	}
	free(item_text);
	return FALSE;//not care about other events
}
static GtkWidget* chan_pan(char*c){
	GtkWidget*pan=nullptr;
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	if(list!=nullptr){
		GList*lst=list;
		do{
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(c,d)==0){
				pan=get_pan_from_menu(menu_item);
				break;
			}
		}while((list=g_list_next(list))!=nullptr);
		g_list_free(lst);
	}
	return pan;
}
static GtkWidget*name_to_pan(const char*n){
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	if(list!=nullptr){
		GList*lst=list;
		do{
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(d,n)==0)
				return get_pan_from_menu(menu_item);
		}while((list=g_list_next(list))!=nullptr);
		g_list_free(lst);
	}
	return nullptr;
}
#define name_to_list(c) contf_get_list(name_to_pan(c))
static void chan_change_nr_swap(GtkTreeIter*iter,int pos,int val,char*chn,unsigned int nr){
	val*=-1;
	pos+=val;
	int startpos=pos;
	GtkTreeIter it;
	for(;;){
		if(pos<0)break;//gtk warning
		if(gtk_tree_model_iter_nth_child((GtkTreeModel*)channels,&it,nullptr,pos)==FALSE){
			gtk_tree_model_iter_nth_child((GtkTreeModel*)channels,&it,nullptr,pos-1);
			break;//the other way
		}
		char*text;
		char c[channm_sz];
		unsigned int n;
		gtk_tree_model_get ((GtkTreeModel*)channels, &it, 0, &text, -1);
		sscanf(text,channame_scan " %u",c,&n);
		int a=strcmp(c,chn);
		g_free(text);
		if(val<0){if(n>nr||(n==nr&&a<0))break;}
		else if(nr>n||(nr==n&&a>0))break;
		pos+=val;
	}
	if(pos!=startpos)gtk_list_store_swap (channels, &it, iter);
}
static BOOL chan_change_nr(const char*chan,int v){
	GtkTreeIter it;
	gtk_tree_model_get_iter_first ((GtkTreeModel*)channels, &it);
	gboolean valid;int pos=0;for(;;){
		char c[channm_sz+1+10];
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)channels, &it, 0, &text, -1);
		sscanf(text,channame_scan,c);
		if(strcmp(chan,c)==0){
			size_t s=strlen(c);size_t ss=s;
			unsigned int n;
			s++;sscanf(text+s,"%u",&n);
			n+=(unsigned int)v;
			chan_change_nr_swap(&it,pos,v,c,n);
			c[ss]=' ';
			s+=(size_t)sprintf(c+s,"%u",n);c[s]=0;
			gtk_list_store_set(channels, &it, LIST_ITEM, c, -1);
			g_free(text);
			return TRUE;
		}
		g_free(text);
		valid = gtk_tree_model_iter_next( (GtkTreeModel*)channels, &it);
		if(valid==FALSE)break;
		pos++;
	}
	return FALSE;
}
static void pars_join(char*chan,struct stk_s*ps){
	GtkWidget*pan=chan_pan(chan);
	if(pan==nullptr){//can be kick and let the channel window
		pan=container_frame(ps->separator,G_CALLBACK(name_join),ps);
		gtk_widget_set_tooltip_text(pan,chan);//is also a NAMES flag here
		GtkWidget*close;GtkWidget*lb=add_new_tab(pan,chan,&close,ps->notebook,chan_menu,FALSE);
		g_signal_connect_data (close, "clicked",G_CALLBACK (close_channel),lb,nullptr,G_CONNECT_SWAPPED);
	}
	gtk_notebook_set_current_page(ps->notebook,gtk_notebook_page_num(ps->notebook,pan));
	if(chan_change_nr(chan,1)==FALSE)pars_chan(chan,1);
}
static void pars_join_user(char*channm,char*nicknm){
	//if(p!=nullptr){
	GtkListStore*lst=name_to_list(channm);
	GtkTreeIter it;
	gtk_tree_model_iter_nth_child((GtkTreeModel*)lst, &it, nullptr, gtk_tree_model_iter_n_children((GtkTreeModel*)lst,nullptr)-1);//at least one, we already joined
	for(;;){
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)lst, &it, LIST_ITEM, &text, -1);
		if(strcmp(nicknm,text)>0||*text=='@'){
			g_free(text);
			GtkTreeIter i;
			gtk_list_store_insert_after(lst,&i,&it);
			gtk_list_store_set(lst, &i, LIST_ITEM, nicknm, -1);
			chan_change_nr(channm,1);
			return;
		}
		g_free(text);
		if(gtk_tree_model_iter_next( (GtkTreeModel*)lst,&it)==FALSE)break;
	}
	gtk_list_store_prepend(lst,&it);
	gtk_list_store_set(lst, &it, LIST_ITEM, nicknm, -1);
	chan_change_nr(channm,1);
	//}
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
static void pars_part(char*c,GtkNotebook*nb){
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	GList*lst=list;
	do{
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
	}while((list=g_list_next(list))!=nullptr);
	g_list_free(lst);
}
static void pars_part_quit(char*nk,const char*cn,GtkListStore*lst){
	GtkTreeIter it;
	gtk_tree_model_get_iter_first ((GtkTreeModel*)lst, &it);
	do{
		char*txt;
		gtk_tree_model_get ((GtkTreeModel*)lst, &it, 0, &txt, -1);
		if(*txt!='@'){
			for(;;){
				int a=strcmp(nk,txt);
				g_free(txt);
				if(a<=0){
					if(a==0){
						gtk_list_store_remove(lst,&it);
						chan_change_nr(cn,-1);
					}
					break;
				}
				if(gtk_tree_model_iter_next( (GtkTreeModel*)lst, &it)==FALSE)break;
				gtk_tree_model_get ((GtkTreeModel*)lst, &it, 0, &txt, -1);
			}
			break;
		}
		int a=strcmp(nk,txt+1);
		g_free(txt);
		if(a<=0){
			if(a==0){
				gtk_list_store_remove(lst,&it);
				chan_change_nr(cn,-1);
			}
			break;
		}
	}while(gtk_tree_model_iter_next( (GtkTreeModel*)lst, &it));
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
static int nick_and_chan(char*a,char*b,const char*bb,char*n,char*c,char*nick){
	if(nick_extract(a,n)){
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wformat-nonliteral"
		if(sscanf(b,bb,c)==1){
		#pragma GCC diagnostic pop
			if(strcmp(nick,n)!=0)return 1;
			return 0;
		}
	}
	return -1;
}
static void add_name(GtkListStore*lst,char*t){
	GtkTreeIter it;
	gboolean valid=gtk_tree_model_get_iter_first ((GtkTreeModel*)lst, &it);
	while(valid){
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)lst, &it, 0, &text, -1);
		int a=strcmp(t,text);
		g_free(text);
		GtkTreeIter i;
		if(a<0){
			gtk_list_store_insert_before(lst,&i,&it);
			gtk_list_store_set(lst, &i, LIST_ITEM, t, -1);
			return;
		}
		valid = gtk_tree_model_iter_next( (GtkTreeModel*)lst, &it);
	}
	gtk_list_store_append(lst,&it);
	gtk_list_store_set(lst, &it, LIST_ITEM, t, -1);
}
static void pars_names(GtkWidget*pan,char*b,size_t s){
	GtkListStore*lst=contf_get_list(pan);
	if(gtk_widget_get_has_tooltip(pan)==FALSE){
		gtk_list_store_clear(lst);
		gtk_widget_set_has_tooltip(pan,TRUE);
	}
	size_t j=0;
	for(size_t i=0;i<s;i++){
		if(b[i]==' '){b[i]=0;add_name(lst,b+j);j=i+1;}
	}
	add_name(lst,b+j);
}
static void pars_quit(char*nk){
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	GList*ls=list;
	do{
		GtkWidget*menu_item=(GtkWidget*)list->data;
		GtkListStore*lst=contf_get_list(get_pan_from_menu(menu_item));
		pars_part_quit(nk,gtk_menu_item_get_label((GtkMenuItem*)menu_item),lst);
	}while((list=g_list_next(list))!=nullptr);
	g_list_free(ls);
}
static void pars_mod_sens_plus(GtkListStore*lst,char*n){
	int c=gtk_tree_model_iter_n_children ((GtkTreeModel*)lst,nullptr);
	GtkTreeIter it;
	gtk_tree_model_iter_nth_child((GtkTreeModel*)lst,&it,nullptr,c-1);
	for(;;){
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)lst, &it, LIST_ITEM, &text, -1);
		if(strcmp(n,text)==0){
			g_free(text);
			char buf[namenul_sz+1];*buf='@';
			strcpy(buf+1,n);
			gtk_list_store_set(lst, &it, LIST_ITEM, buf, -1);
			GtkTreeIter i=it;
			if(gtk_tree_model_iter_previous( (GtkTreeModel*)lst,&i)){
				do{
					gtk_tree_model_get ((GtkTreeModel*)lst, &i, LIST_ITEM, &text, -1);
					if(*text=='@'){
						for(;;){
							if(strcmp(n,text+1)>0){
								g_free(text);
								gtk_list_store_move_after(lst,&it,&i);
								return;
							}
							g_free(text);
							if(gtk_tree_model_iter_previous( (GtkTreeModel*)lst,&i)==FALSE){
								gtk_list_store_move_after(lst,&it,nullptr);
								return;
							}
							gtk_tree_model_get ((GtkTreeModel*)lst, &i, LIST_ITEM, &text, -1);
						}
					}
					g_free(text);
				}while(gtk_tree_model_iter_previous( (GtkTreeModel*)lst,&i));
				gtk_list_store_move_after(lst,&it,nullptr);
			}
			return;
		}
		g_free(text);
		if(gtk_tree_model_iter_previous( (GtkTreeModel*)lst,&it)==FALSE/*||*text=='@'*/){/*g_free(text);*/return;}//guard only loop
	}
}
static void pars_mod_sens_minus(GtkListStore*lst,char*n){
	GtkTreeIter it;
	gtk_tree_model_get_iter_first ((GtkTreeModel*)lst, &it);
	for(;;){
		char*text;
		gtk_tree_model_get ((GtkTreeModel*)lst, &it, LIST_ITEM, &text, -1);
		if(strcmp(n,text+1)==0){
			for(size_t j=0;text[j]!='\0';j++)text[j]=text[j+1];
			gtk_list_store_set(lst, &it, LIST_ITEM, text, -1);
			g_free(text);
			GtkTreeIter i=it;
			if(gtk_tree_model_iter_next( (GtkTreeModel*)lst,&i)){
				do{
					gtk_tree_model_get ((GtkTreeModel*)lst, &i, LIST_ITEM, &text, -1);
					if(*text!='@'){
						for(;;){
							if(strcmp(n,text)<0){
								g_free(text);
								gtk_list_store_move_before(lst,&it,&i);
								return;
							}
							g_free(text);
							if(gtk_tree_model_iter_next( (GtkTreeModel*)lst,&i)==FALSE){
								gtk_list_store_move_before(lst,&it,nullptr);
								return;
							}
							gtk_tree_model_get ((GtkTreeModel*)lst, &i, LIST_ITEM, &text, -1);
						}
					}
					g_free(text);
				}while(gtk_tree_model_iter_next( (GtkTreeModel*)lst,&i));
				gtk_list_store_move_before(lst,&it,nullptr);
			}
			return;
		}
		g_free(text);
		if(gtk_tree_model_iter_next( (GtkTreeModel*)lst,&it)==FALSE/*||*text!='@'*/){/*g_free(text);*/break;}//guard only loop
	}
}
static void pars_mod_sens(BOOL plus,char*c,char*m,char*n){
	for(size_t i=1;m[i]!='\0';i++){
		if(m[i]=='o'){
			GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
			GList*ls=list;
			do{
				GtkWidget*menu_item=(GtkWidget*)list->data;
				const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
				if(strcmp(c,d)==0){
					GtkListStore*lst=contf_get_list(get_pan_from_menu(menu_item));
					GtkTreeIter it;
					gtk_tree_model_get_iter_first ((GtkTreeModel*)lst, &it);
					gboolean valid;do{
						char*text;
						gtk_tree_model_get ((GtkTreeModel*)lst, &it, LIST_ITEM, &text, -1);
						int a=strcmp(n,plus?text:text+1);
						g_free(text);
						if(a==0){
							if(plus)pars_mod_sens_plus(lst,n);
							else pars_mod_sens_minus(lst,n);
							break;
						}
						valid=gtk_tree_model_iter_next( (GtkTreeModel*)lst,&it);
					}while(valid);
					break;
				}
			}while((list=g_list_next(list))!=nullptr);
			g_list_free(ls);
			return;
		}
	}
}
static void pars_mod(char*c,char*m,char*n){
	if(*m=='+')pars_mod_sens(TRUE,c,m,n);
	else if(*m=='-')pars_mod_sens(FALSE,c,m,n);
}
static void nb_switch_page(GtkNotebook *notebook,GtkWidget *page){//,guint page_num,gpointer user_data){
	GtkWidget*box=gtk_notebook_get_tab_label(notebook,page);
	if(G_TYPE_FROM_INSTANCE(box)==gtk_box_get_type())unalert(notebook,box);
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
#define is_channel(c) *c=='#'||*c=='&'
static void pars_pmsg_chan(char*n,char*c,char*msg,GtkNotebook*nb){
	GList*list=gtk_container_get_children((GtkContainer*)chan_menu);
	GList*lst=list;
	do{
		GtkWidget*menu_item=(GtkWidget*)list->data;
		const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
		if(strcmp(c,d)==0){
			GtkWidget*pan=get_pan_from_menu(menu_item);
			addatchans(n,msg,pan);
			prealert(nb,pan);
			break;
		}
	}while((list=g_list_next(list))!=nullptr);
	g_list_free(lst);
}
static void pars_pmsg_name(char*n,char*c,char*msg,GtkNotebook*nb){
	BOOL novel=TRUE;
	GList*list=gtk_container_get_children((GtkContainer*)name_menu);
	if(list!=nullptr){
		GList*lst=list;
		do{
			GtkWidget*menu_item=(GtkWidget*)list->data;
			const char*d=gtk_menu_item_get_label((GtkMenuItem*)menu_item);
			if(strcmp(n,d)==0){
				GtkWidget*scrl=get_pan_from_menu(menu_item);
				addatnames(c,msg,scrl);
				prealert(nb,scrl);
				novel=FALSE;
				break;
			}
		}while((list=g_list_next(list))!=nullptr);
		g_list_free(lst);
	}
	if(novel){
		GtkWidget*scrl=name_join_nb(n,nb);addatnames(c,msg,scrl);
		alert(gtk_notebook_get_tab_label(nb,scrl),nb);
	}
}
static gboolean incsafe(gpointer ps){
	addattextview(((struct stk_s*)ps)->dl);
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wcast-qual"
	char*a=(char*)((struct stk_s*)ps)->dl->data;
	#pragma GCC diagnostic pop
	size_t s=((struct stk_s*)ps)->dl->len;
	s-=a[s-2]=='\r'?2:1;
	a[s]='\0';
	//
	char com[5];
	if(sscanf(a,"%*s %7s",com)==1){
		size_t ln=strlen(com);
		char*b=strchr(a,' ')+1+ln;if(*b==' ')b++;
		char channm[channm_sz+1+10];//+ to set the "chan nr" at join on the same string
		char nicknm[namenul_sz];
		if(strcmp(com,"PRIVMSG")==0){
			if(nick_extract(a,nicknm)){
				int c;
				if(is_channel(b)){
					if(sscanf(b,channame_scan " %c",channm,(char*)&c)==2)pars_pmsg_chan(nicknm,channm,b+strlen(channm)+2,((struct stk_s*)ps)->notebook);
				}else if(sscanf(b,name_scan " %c",channm,(char*)&c)==2)pars_pmsg_name(nicknm,channm,b+strlen(channm)+2,((struct stk_s*)ps)->notebook);
			}
		}else if(strcmp(com,"JOIN")==0){
			int resp=nick_and_chan(a,b,":" channame_scan,nicknm,channm,((struct stk_s*)ps)->nknnow);
			if(resp==0)pars_join(channm,(struct stk_s*)ps);
			else if(resp==1)pars_join_user(channm,nicknm);
		}else if(strcmp(com,"PART")==0){
			int resp=nick_and_chan(a,b,channame_scan,nicknm,channm,((struct stk_s*)ps)->nknnow);
			if(resp==0)pars_part(channm,((struct stk_s*)ps)->notebook);
			else if(resp==1)pars_part_user(channm,nicknm);
		}else if(strcmp(com,"KICK")==0){
			if(sscanf(b,channame_scan " " name_scan,channm,nicknm)==2)
				pars_part_user(channm,nicknm);
		}else if(strcmp(com,"QUIT")==0){
			if(nick_extract(a,nicknm))pars_quit(nicknm);
		}else if(strcmp(com,"MODE")==0){
			char mod[1+3+1];//"limit of three (3) changes per command for modes that take a parameter."
			if(sscanf(b,channame_scan " %4s " name_scan,channm,mod,nicknm)==3)
				pars_mod(channm,mod,nicknm);
		}else{
			int d=atoi(com);
			if(d==322){//list
				unsigned int e;
				//if its >nr ,c is not 2
				if(sscanf(b,"%*s " channame_scan " %u",channm,&e)==2)
					pars_chan(channm,e);
			}else if(d==321)gtk_list_store_clear(channels);//list start
			else if(d==353){//names
				if(sscanf(b,"%*s %*c " channame_scan,channm)==1){
					GtkWidget*p=name_to_pan(channm);
					if(p!=nullptr){
						b=strchr(b,':');//join #q:w is error
						if(b!=nullptr)pars_names(p,b+1,s-(size_t)(b+1-a));
					}
				}
			}else if(d==366){//names end
				if(sscanf(b,"%*s " channame_scan,channm)==1){
					GtkWidget*p=name_to_pan(channm);
					if(p!=nullptr)gtk_widget_set_has_tooltip(p,FALSE);
				}
			}
		}
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
	send_data(sendlist,sizeof(sendlist)-1);
	return TRUE;
}
static int start_old_clear(GtkWidget*w,GtkNotebook*nb){
	GList*list=gtk_container_get_children((GtkContainer*)w);
	if(list!=nullptr){
		GList*lst=list;
		do{
			GtkWidget*menu_item=(GtkWidget*)list->data;
			GtkWidget*pan=get_pan_from_menu(menu_item);
			gtk_notebook_remove_page(nb,gtk_notebook_page_num(nb,pan));
			gtk_widget_destroy(menu_item);
		}while((list=g_list_next(list))!=nullptr);
		g_list_free(lst);
		return 1;
	}
	return 0;
}
static gboolean senstartthreadsfunc(gpointer ps){
	GtkNotebook*nb=((struct stk_s*)ps)->notebook;
	g_signal_handler_unblock(((struct stk_s*)ps)->sen_entry,((struct stk_s*)ps)->sen_entry_act);
	//
	if(((struct stk_s*)ps)->refresh>0)
		((struct stk_s*)ps)->refreshid=g_timeout_add(1000*((struct stk_s*)ps)->refresh,refresh_callback,nullptr);
	//
	if(start_old_clear(chan_menu,nb)+start_old_clear(name_menu,nb)>0)
		gtk_widget_hide(gtk_notebook_get_action_widget(nb,GTK_PACK_END));
	//
	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
static gboolean senstopthreadsfunc(gpointer ps){
	g_signal_handler_block(((struct stk_s*)ps)->sen_entry,((struct stk_s*)ps)->sen_entry_act);
	//
	if(((struct stk_s*)ps)->refresh>0)
		g_source_remove(((struct stk_s*)ps)->refreshid);
	//
	pthread_kill( threadid, SIGUSR1);
	return FALSE;
}
static void irc_start(char*psw,char*nkn,struct stk_s*ps){
	g_idle_add(senstartthreadsfunc,ps);
	int out;sigwait(&threadset,&out);
	const char format[]="USER guest tolmoon tolsun :Ronnie Reagan" irc_term;
	size_t fln=sizeof(format)-1;
	size_t nln=(size_t)snprintf(nullptr,0,nickname_con,nkn);
	size_t pln=*psw=='\0'?0:(size_t)snprintf(nullptr,0,password_con,psw);
	char*i1=(char*)malloc(pln+nln+fln);
	if(i1!=nullptr){
		if(*psw!='\0')sprintf(i1,password_con,psw);
		sprintf(i1+pln,nickname_con,nkn);
		memcpy(i1+pln+nln,format,fln);
		send_safe(i1,pln+nln+fln);
		send_safe(sendlist,sizeof(sendlist)-1);
		//
		char*buf=(char*)malloc(irc_bsz);int bsz=irc_bsz;
		if(buf!=nullptr){
			for(;;){
				int sz=recv_data(buf,bsz);
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
				if(sz<=0)break;
				char*b=buf;
				for(;;){
					char*n=(char*)memchr(b,'\n',(size_t)sz);
					if(n!=nullptr){
						size_t s=(size_t)(n+1-b);
						if(s>4&&memcmp(b,"PING",4)==0){
							main_text(b,s);
							b[1]='O';
							send_safe(b,s);
						}else if(*b==':')incomings(b,s,ps);
						b=n+1;sz-=s;
						continue;
					}
					main_text(b,(size_t)sz);
					break;
				}
			}
			free(buf);
		}
		free(i1);
	}
	g_idle_add(senstopthreadsfunc,ps);
	sigwait(&threadset,&out);
}
static void proced(struct stk_s*ps){
	const SSL_METHOD *method;
	SSL_CTX *ctx;
	int server;
	  /* ---------------------------------------------------------- *
	   * Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
	   * ---------------------------------------------------------- */
	method = SSLv23_client_method();
	  /* ---------------------------------------------------------- *
	   * Try to create a new SSL context                            *
	   * ---------------------------------------------------------- */
	ctx = SSL_CTX_new(method);
	if ( ctx != nullptr){
		  /* ---------------------------------------------------------- *
		   * Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
		   * ---------------------------------------------------------- */
		SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
		  /* ---------------------------------------------------------- *
		   * Create new SSL connection state object                     *
		   * ---------------------------------------------------------- */
		SSL*withssl = SSL_new(ctx);
		if(withssl!=nullptr){
			char hostname[hostname_sz];
			int sens;char psw[password_sz];char nkn[namenul_sz];
			if(parse_host_str(ps->text,hostname,psw,nkn,&sens,ps)) {
				for(;;){
					ssl=withssl;				
					  /* ---------------------------------------------------------- *
					   * Make the underlying TCP socket connection                  *
					   * ---------------------------------------------------------- */
					server = create_socket(hostname,portindex);
					if(server != -1){
						  /* ---------------------------------------------------------- *
						   * Attach the SSL session to the socket descriptor            *
						   * ---------------------------------------------------------- */
						if(SSL_set_fd(withssl, server)==1){
							  /* ---------------------------------------------------------- *
							   * Try to SSL-connect here, returns 1 for success             *
							   * ---------------------------------------------------------- */
							main_text_s(ssl_con_try);
							//is waiting until timeout if not SSL// || printf("No SSL")||1
							if ( SSL_connect(withssl) == 1){//this at reconnects, is not ready for ssl conect,disc,plain connect
								main_text_s("Successfully enabled SSL/TLS session.\n");
								//cert = SSL_get_peer_certificate(ssl);
								//certname = X509_NAME_new();
								//certname = X509_get_subject_name(cert);
								//X509_NAME_print_ex(stdout,/*outbio ,*/ certname, 0, 0);
								  /* ---------------------------------------------------------- *
								   * Start                                                      *
								   * ---------------------------------------------------------- */
								irc_start(psw,nkn,ps);
								close(server);break;
							}else{
								ssl=nullptr;
								//main_text_s("Error: Could not build a SSL session.\n");
								main_text_s(ssl_con_no);
								plain_socket=create_socket(hostname,portindex);
								if(plain_socket!=-1){
									close(server);
									irc_start(psw,nkn,ps);
									close(plain_socket);plain_socket=-1;
									break;
								}
							}
						}else main_text_s("Error: SSL_set_fd failed.\n");
						close(server);
					}
					if(portindex==portend)break;
					portindex+=sens;
				}
			}else main_text_s("Error: Wrong input. For format, press the vertical ellipsis button and then Help.\n");
			main_text_s("Disconnected.\n");
  			/* ---------------------------------------------------------- *
  			* Free the structures we don't need anymore, and close       *
  			* -----------------------------------------------------------*/
			//X509_free(cert);
			SSL_free(withssl);ssl=nullptr;
		}
		SSL_CTX_free(ctx);
	}
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
		int f=open(info_path_name,O_WRONLY|O_TRUNC);
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
static gboolean enter_recallback( gpointer ps){
	const char* t=gtk_entry_get_text ((GtkEntry*)((struct stk_s*)ps)->con_entry);
	if(strlen(t)>0){
		if(con_th==0){//con_th!=nullptr){
			portindex=portend;
			if(ssl!=nullptr)SSL_shutdown(ssl);
			else if(plain_socket!=-1)shutdown(plain_socket,2);
			g_timeout_add(1000,enter_recallback,ps);
			return FALSE;
		}
		set_combo_box_text((GtkComboBox*)gtk_widget_get_ancestor(((struct stk_s*)ps)->con_entry,gtk_combo_box_text_get_type()),t);
		((struct stk_s*)ps)->text=t;
		/*#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wcast-qual"
		con_th=g_thread_new(nullptr,worker,ps);
		#pragma GCC diagnostic pop
		g_thread_unref(con_th);*/
		con_th = pthread_create( &threadid, nullptr, worker,ps);
	}
	//unblock this ENTER
	g_signal_handler_unblock(((struct stk_s*)ps)->con_entry,((struct stk_s*)ps)->con_entry_act);
	return FALSE;
}
static gboolean enter_callback( gpointer ps){
	//block this ENTER
	g_signal_handler_block(((struct stk_s*)ps)->con_entry,((struct stk_s*)ps)->con_entry_act);
	return enter_recallback(ps);
}
static BOOL info_path_name_set_val(const char*a,char*b,size_t i,size_t j){
	info_path_name=(char*)malloc(i+j+6);
	if(info_path_name!=nullptr){
		memcpy(info_path_name,a,i);
		info_path_name[i]='.';
		char*c=info_path_name+i+1;
		memcpy(c,b,j);
		memcpy(c+j,"info",5);
		return TRUE;
	}
	return FALSE;
}
static BOOL info_path_name_set(char*a){
	char*c=realpath(a,nullptr);
	if(c!=nullptr){
		char*b=basename(a);
		size_t i=sizeof(BDIR)-1;
		size_t j=strlen(c);
		size_t k=strlen(b);
		BOOL answer;
		if(i+k==j&&memcmp(c,BDIR,i)==0&&memcmp(c+i,b,k)==0)answer=info_path_name_set_val(HOMEDIR,b,sizeof(HOMEDIR)-1,k);
		else answer=info_path_name_set_val(c,b,j-k,k);
		free(c);
		return answer;
	}
	return FALSE;
}
static BOOL info_path_name_restore(GtkComboBoxText*cbt,char*nm){
	if(info_path_name_set(nm)){
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
					gtk_combo_box_set_active((GtkComboBox*)cbt,0);
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}
static int organize_connections_ini(GtkTreeView*tv,GtkTreeModel**mod,GtkTreeIter*it){
	GtkTreeSelection *sel=gtk_tree_view_get_selection(tv);
	gtk_tree_selection_get_selected (sel,mod,it);
	GtkTreePath * path = gtk_tree_model_get_path ( *mod , it ) ;
	int i= (gtk_tree_path_get_indices ( path ))[0] ;
	gtk_tree_path_free(path);
	return i;
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
	else{// if(response==0){
		save_combo_box(gtk_combo_box_get_model((GtkComboBox*)ps->cbt));
		gtk_widget_destroy((GtkWidget*)dialog);
	}
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
			    "Move _Up",2,"Move D_own",3,"_Delete",1,"Do_ne",0,nullptr);
		else
			dialog = gtk_dialog_new_with_buttons ("Organize Connections",
			    top, (GtkDialogFlags)(GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL),
			    "_Delete",1,"Do_ne",0,nullptr);
		GtkWidget *tree=gtk_tree_view_new();ps->tv=(GtkTreeView*)tree;
		//
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *column;
		GtkListStore *store;
		gtk_tree_view_set_headers_visible((GtkTreeView*)tree,FALSE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("", renderer, "text", LIST_ITEM, nullptr);
		gtk_tree_view_append_column((GtkTreeView*)tree, column);
		store= gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
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
			"Do_ne",0,nullptr);
	}
	g_signal_connect_data (dialog, "response",G_CALLBACK (organize_connections_dialog),ps,nullptr,(GConnectFlags) 0);
	gtk_widget_show_all (dialog);
}
static gboolean prog_menu_popup (GtkMenu*menu,GdkEvent*evn){
	gtk_menu_popup_at_pointer(menu,evn);
	return FALSE;
}
static void help_popup(GtkWindow*top){
	GtkWidget *dialog = gtk_dialog_new_with_buttons ("Organize Connections",
			    top, (GtkDialogFlags)(GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL),
			    "OK",GTK_RESPONSE_NONE,nullptr);
	int w;int h;
	gtk_window_get_size (top,&w,&h);
	gtk_window_set_default_size((GtkWindow*)dialog,w,h);
	g_signal_connect_data (dialog,"response",G_CALLBACK (gtk_widget_destroy),
	                       dialog,nullptr,G_CONNECT_SWAPPED);
	//
	GtkWidget*scrolled_window = gtk_scrolled_window_new (nullptr, nullptr);
	GtkWidget*text = gtk_text_view_new ();
	gtk_text_view_set_editable((GtkTextView*)text, FALSE);
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer ((GtkTextView*)text);
	gtk_text_buffer_set_text (text_buffer,help_text,sizeof(help_text)-1);
	gtk_container_add ( (GtkContainer*)scrolled_window, text);
	GtkWidget*box=gtk_dialog_get_content_area((GtkDialog*)dialog);
	gtk_box_pack_start((GtkBox*)box, scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show_all (dialog);
}
static void send_activate(GtkEntry*entry,struct stk_s*ps){
	GtkEntryBuffer*t=gtk_entry_get_buffer(entry);
	const char*text=gtk_entry_buffer_get_text(t);
	size_t sz=strlen(text);
	//
	GtkWidget*pg=gtk_notebook_get_nth_page(ps->notebook,gtk_notebook_get_current_page(ps->notebook));
	const char*a=gtk_notebook_get_menu_label_text(ps->notebook,pg);
	char*b;
	BOOL priv=*a!=*home_string;
	if(priv){
		size_t len=sizeof(send_prv1)-1;size_t wid=sizeof(send_prv2)-1;
		size_t dim=strlen(a);
		b=(char*)malloc(len+dim+wid+sz+irc_term_sz);
		if(b==nullptr)return;
		memcpy(b,send_prv1,len);
		memcpy(b+len,a,dim);size_t spc=len+dim;
		memcpy(b+spc,send_prv2,wid);spc+=wid;
		memcpy(b+spc,text,sz);sz+=spc;
		if(is_channel(a))addatchans(ps->nknnow,text,pg);
		else addatnames(ps->nknnow,text,pg);
	}else{
		b=(char*)malloc(sz+irc_term_sz);
		if(b==nullptr)return;
		memcpy(b,text,sz);
	}
	memcpy(b+sz,irc_term,irc_term_sz);
	send_data(b,sz+irc_term_sz);
	free(b);
	gtk_entry_buffer_delete_text(t,0,-1);
}
static void
activate (GtkApplication* app,
          struct stk_s*ps)
{
	  /* Create a window with a title, and a default size */
	GtkWidget *window = gtk_application_window_new (app);
	gtk_window_set_title ((GtkWindow*) window, "IRC");
	if(ps->dim[0]!=-1)
		gtk_window_set_default_size ((GtkWindow*) window, ps->dim[0], ps->dim[1]);
	//
	home_page=container_frame(ps->separator,G_CALLBACK(chan_join),nullptr);
	text_view=contf_get_textv(home_page);
	text_mark_end=textv_get_atend(text_view);
	channels=contf_get_list(home_page);
	//
	ps->notebook = (GtkNotebook*)gtk_notebook_new ();
	gtk_notebook_set_scrollable(ps->notebook,TRUE);
	gtk_notebook_popup_enable(ps->notebook);
	gtk_notebook_append_page_menu (ps->notebook, home_page, gtk_label_new (home_string), gtk_label_new (home_string));//i dont like the display (at 2,3..) without the last parameter
	gtk_notebook_set_tab_reorderable(ps->notebook, home_page, TRUE);
	//
	sigemptyset(&threadset);
	sigaddset(&threadset, SIGUSR1);
	GtkWidget*en=gtk_combo_box_text_new_with_entry();
	GtkWidget*entext=gtk_bin_get_child((GtkBin*)en);
	if(info_path_name_restore((GtkComboBoxText*)en,ps->path))gtk_entry_set_text ((GtkEntry*)entext,":");
	ps->con_entry=entext;//this for timeouts
	ps->con_entry_act=g_signal_connect_data (entext, "activate",G_CALLBACK (enter_callback),ps,nullptr,G_CONNECT_SWAPPED);
	//
	GtkWidget *org=gtk_button_new_with_label("\u22EE");ps->cbt=(GtkComboBoxText*)en;
	GtkWidget *menu = gtk_menu_new ();
	GtkWidget *menu_item = gtk_menu_item_new_with_label ("Organize Connections");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (organize_connections),ps,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	menu_item = gtk_menu_item_new_with_label ("Help");
	g_signal_connect_data (menu_item, "activate",G_CALLBACK (help_popup),window,nullptr,G_CONNECT_SWAPPED);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	menu_item = gtk_menu_item_new_with_label ("Channels");
	chan_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu((GtkMenuItem*)menu_item,chan_menu);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	g_signal_connect_data (org, "button-press-event",G_CALLBACK (prog_menu_popup),menu,nullptr,G_CONNECT_SWAPPED);
	//
	menu_item = gtk_menu_item_new_with_label ("Names");
	name_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu((GtkMenuItem*)menu_item,name_menu);
	gtk_menu_shell_append ((GtkMenuShell*)menu, menu_item);gtk_widget_show(menu_item);
	//
	ps->sen_entry=gtk_entry_new();
	ps->sen_entry_act=g_signal_connect_data(ps->sen_entry,"activate",G_CALLBACK(send_activate),ps,nullptr,(GConnectFlags)0);
	g_signal_handler_block(ps->sen_entry,ps->sen_entry_act);
	//
	GtkWidget*top=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_start((GtkBox*)top,en,TRUE,TRUE,0);
	gtk_box_pack_start((GtkBox*)top,org,FALSE,FALSE,0);
	GtkWidget*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_pack_start((GtkBox*)box,top,FALSE,FALSE,0);
	gtk_box_pack_start((GtkBox*)box,(GtkWidget*)ps->notebook,TRUE,TRUE,0);
	gtk_box_pack_start((GtkBox*)box,ps->sen_entry,FALSE,FALSE,0);
	gtk_container_add ((GtkContainer*)window, box);
	gtk_widget_show_all (window);
	//
	GtkWidget*info=gtk_image_new_from_icon_name ("dialog-information",GTK_ICON_SIZE_MENU);
	gtk_notebook_set_action_widget(ps->notebook,info,GTK_PACK_END);
	g_signal_connect_data (ps->notebook, "switch-page",G_CALLBACK (nb_switch_page),nullptr,nullptr,(GConnectFlags)0);//this,before show,was critical
}
static gint handle_local_options (struct stk_s* ps, GVariantDict*options){
	gchar*result;
	if (g_variant_dict_lookup (options, ps->args[0], "s", &result)){//missing argument is not reaching here
		char*b=strchr(result,'x');
		if(b!=nullptr){*b='\0';b++;}
		ps->dim[0]=atoi(result);
		ps->dim[1]=b!=nullptr?atoi(b):ps->dim[0];
		g_free(result);
	}else ps->dim[0]=-1;//this is default at gtk
	GVariant*v=g_variant_dict_lookup_value(options,ps->args[1],G_VARIANT_TYPE_STRING);
	if(v!=nullptr){
		ps->nick=g_variant_dup_string(v,nullptr);//get is not the same pointer as argv[2],is always utf-8
	}else ps->nick=nullptr;
	if (g_variant_dict_lookup (options,ps->args[2], "s", &result)){
		ps->separator=atoi(result);
		g_free(result);
	}else ps->separator=150;
	if (g_variant_dict_lookup (options,ps->args[3], "s", &result)){
		ps->refresh=(unsigned int)atoi(result);
		g_free(result);
	}else ps->refresh=60;
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
		ps.args[0]="dimensions";
		g_application_add_main_option((GApplication*)app,ps.args[0],'d',G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Window size","WIDTH[xHEIGHT]");
		ps.args[1]="nick";
		g_application_add_main_option((GApplication*)app,ps.args[1],'n',G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Default nickname","NICKNAME");
		ps.args[2]="right";
		g_application_add_main_option((GApplication*)app,ps.args[2],'r',G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Right pane size, default 150","WIDTH");
		ps.args[3]="refresh";
		g_application_add_main_option((GApplication*)app,ps.args[3],'f',G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Refresh channels interval in seconds. Default 60. 0 for no refresh.","SECONDS");
		ps.path=argv[0];
		g_signal_connect_data (app, "handle-local-options", G_CALLBACK (handle_local_options), &ps, nullptr,G_CONNECT_SWAPPED);
		g_signal_connect_data (app, "activate", G_CALLBACK (activate), &ps, nullptr,(GConnectFlags) 0);
		//  if(han>0)
		g_application_run ((GApplication*)app, argc, argv);//gio.h>gapplication.h gio-2.0
		if(ps.nick!=nullptr)g_free(ps.nick);
		g_object_unref (app);
		if(info_path_name!=nullptr)free(info_path_name);
	}else puts("openssl error");
}
