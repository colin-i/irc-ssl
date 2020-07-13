
#include "inc/null.h"
#include "inc/bool.h"

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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#include <gtk/gtk.h>
#pragma GCC diagnostic pop
#else
#include "inc/gtk.h"
#endif

static GtkTextBuffer *buffer;
static SSL *ssl=nullptr;static int plain_socket=-1;static GThread*con_th=nullptr;
#define ssl_con_try "Trying with SSL.\n"
#define ssl_con_er "No SSL. Trying unencrypted.\n"
#define irc_bsz 512

/* ---------------------------------------------------------- *
 * create_socket() creates the socket & TCP-connect to server *
 * ---------------------------------------------------------- */
static int create_socket(const char*indata) {
	int sockfd;
	char hostname[256];
	int port;
	struct hostent *host;
	struct sockaddr_in dest_addr;
	  /* ---------------------------------------------------------- *
	   * the hostname from the text                                 *
	   * ---------------------------------------------------------- */
	size_t hstsz=strlen(indata);
	if(hstsz<sizeof(hostname)){
		memcpy(hostname, indata, hstsz);
		hostname[hstsz]='\0';
		  /* ---------------------------------------------------------- *
		   * if the hostname contains a colon :, we got a port number   *
		   * ---------------------------------------------------------- */
		char    portnum[6] = "6667";
		char*tmp_ptr;size_t sz=0;
		if(
		((tmp_ptr=strchr(hostname, ':'))==nullptr)
		||
		((sz=strlen(tmp_ptr+1))<sizeof(portnum))
		) {
			if(sz>0){
				memcpy(portnum, tmp_ptr+1, sz );portnum[sz]='\0';
				*tmp_ptr = '\0';
			}
			if ( (host = gethostbyname(hostname)) != nullptr ) {
				  /* ---------------------------------------------------------- *
				   * create the basic TCP socket                                *
				   * ---------------------------------------------------------- */
				sockfd = socket(AF_INET, SOCK_STREAM, 0);
				if(sockfd!=-1){
					dest_addr.sin_family=AF_INET;
					port = atoi(portnum);//stdlib
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
						printf("Error: Cannot connect to host %s on port %d.\n",hostname, port);
						close(sockfd);
					}
				}else puts("socket failed");
			}
			else
			printf("Error: Cannot resolve hostname %s.\n",  hostname);
		}
	}
	return -1;
}
static void main_text(const char*b,int s){
	GtkTextIter it;
	gtk_text_buffer_get_end_iter(buffer,&it);
	gtk_text_buffer_insert(buffer,&it,b,s);
}
static void irc_start(){
	//PASS abc\n
	const char*i1="NICK don\nUSER guest tolmoon tolsun :Ronnie Reagan\n\n";
	if(ssl!=nullptr)SSL_write(ssl,i1,(int)strlen(i1));
	else send(plain_socket,i1,strlen(i1),0);
	char buf[irc_bsz];
	for(;;){
		int sz;
		if(ssl!=nullptr)sz=SSL_read(ssl, buf, irc_bsz-1);
		else sz=recv(plain_socket,buf,irc_bsz-1,0);
		if(sz<=0)break;
		buf[sz]='\0';char*b=buf;
		for(;;){
			char*n=strstr(b,"\n");
			if(n!=nullptr){
				char aux=n[1];n[1]='\0';
				int s=n+1-b;
				main_text(b,s);
				n[1]=aux;
				if(strncmp(b,"PING",4)==0){
					b[1]='O';
					if(ssl!=nullptr)sz=SSL_write(ssl,b,(int)strlen(b));
					else sz=send(plain_socket,b,strlen(b),0);
				}
				b=n+1;sz-=s;continue;
			}
			main_text(b,sz);
			break;
		}
	}
}
	
static void proced_plain(const char*dest_url){
	plain_socket=create_socket(dest_url);
	if(plain_socket!=-1){
		irc_start();
		close(plain_socket);plain_socket=-1;
	}
}

static BOOL proced(const char*dest_url){
	const SSL_METHOD *method;
	SSL_CTX *ctx;
	int server;
	BOOL result=FALSE;
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
		ssl = SSL_new(ctx);
		if(ssl!=nullptr){
			  /* ---------------------------------------------------------- *
			   * Make the underlying TCP socket connection                  *
			   * ---------------------------------------------------------- */
			server = create_socket(dest_url);
			if(server != -1){
				printf( "Successfully made the TCP connection to: %s.\n", dest_url);
				  /* ---------------------------------------------------------- *
				   * Attach the SSL session to the socket descriptor            *
				   * ---------------------------------------------------------- */
				if(SSL_set_fd(ssl, server)==1){
					  /* ---------------------------------------------------------- *
					   * Try to SSL-connect here, returns 1 for success             *
					   * ---------------------------------------------------------- */
					main_text(ssl_con_try,sizeof(ssl_con_try)-1);
					//is waiting until timeout if not SSL// || printf("No SSL")||1
					if ( SSL_connect(ssl) == 1){
						printf(/*outbio ,*/ "Successfully enabled SSL/TLS session to: %s.\n", dest_url);
						//cert = SSL_get_peer_certificate(ssl);
						//certname = X509_NAME_new();
						//certname = X509_get_subject_name(cert);
						//X509_NAME_print_ex(stdout,/*outbio ,*/ certname, 0, 0);
						  /* ---------------------------------------------------------- *
						   * Start                                                      *
						   * ---------------------------------------------------------- */
						irc_start();
		/* ---------------------------------------------------------- *
		* Free the structures we don't need anymore, and close       *
		* -----------------------------------------------------------*/
					}else{
						printf("Error: Could not build a SSL session to: %s.\n", dest_url);
						result=TRUE;
					}
				}else puts("SSL_set_fd failed");
				close(server);
			}
			//X509_free(cert);
			SSL_free(ssl);ssl=nullptr;
		}
		SSL_CTX_free(ctx);
	}
	return result;
}
	
static gpointer worker (gpointer data)
{
	if(proced((const char*)data)){
		main_text(ssl_con_er,sizeof(ssl_con_er)-1);
		proced_plain((const char*)data);
	}
	con_th=nullptr;
	return nullptr;
}

static void save_combo_box(GtkTreeModel*list,GtkTreeIter*it){
	char out[sizeof(HOMEDIR)+sizeof(PACKAGE_NAME)+4];
	memcpy(out,HOMEDIR,sizeof(HOMEDIR)-1);
	out[sizeof(HOMEDIR)-1]='.';
	memcpy(out+sizeof(HOMEDIR),PACKAGE_NAME,sizeof(PACKAGE_NAME)-1);
	memcpy(out+sizeof(HOMEDIR)+sizeof(PACKAGE_NAME)-1,"info",4);
	out[sizeof(HOMEDIR)+sizeof(PACKAGE_NAME)+3]='\0';
	int f=open(out,O_WRONLY|O_TRUNC);
	if(f!=-1){
		gboolean valid;gchar*text;
		int i=0;
		gtk_tree_model_get_iter_first (list, it);
		do{
			gtk_tree_model_get (list, it, 0, &text, -1);
			if(i!=0)if(write(f,",",1)!=1){g_free(text);break;}
			size_t sz=strlen(text);
			if((size_t)write(f,text,sz)!=sz){g_free(text);break;}
			g_free(text);
			i++;
			valid = gtk_tree_model_iter_next( list, it);
		}while(valid);
		close(f);
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
	save_combo_box(list_store,&iter);
	gtk_combo_box_set_active(box, i);
}

static void enter_callback( GtkWidget *widget){//,gpointer data
	const char* t=gtk_entry_get_text ((GtkEntry*)widget);
	if(strlen(t)>0){
		while(con_th!=nullptr){
			if(ssl!=nullptr)SSL_shutdown(ssl);
			else if(plain_socket!=-1)shutdown(plain_socket,2);
			sleep(1);
		}
		set_combo_box_text((GtkComboBox*)gtk_widget_get_ancestor(widget,gtk_combo_box_text_get_type()),t);
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wcast-qual"
		con_th=g_thread_new("a",worker,(gpointer)t);
		#pragma GCC diagnostic pop
		g_thread_unref(con_th);
	}
}
static void
activate (GtkApplication* app,
          int*        user_data)
{
	/* Declare variables */
	GtkWidget *window;
	GtkWidget *text_view;
	GtkWidget *scrolled_window;
	  /* Create a window with a title, and a default size */
	window = gtk_application_window_new (app);
	gtk_window_set_title ((GtkWindow*) window, "IRC");
	if(user_data[0]!=0)
	 gtk_window_set_default_size ((GtkWindow*) window, user_data[0], user_data[1]);
	//gtk_window_maximize((GtkWindow*)window);
	  /* The text buffer represents the text being edited */
	buffer = gtk_text_buffer_new (nullptr);
	  /* Text view is a widget in which can display the text buffer. 
	   * The line wrapping is set to break lines in between words.
	   */
	text_view = gtk_text_view_new_with_buffer (buffer);
	gtk_text_view_set_editable((GtkTextView*) text_view, FALSE);
	gtk_text_view_set_wrap_mode ((GtkTextView*)text_view, GTK_WRAP_WORD);
	  /* Create the scrolled window. Usually nullptr is passed for both parameters so 
	   * that it creates the horizontal/vertical adjustments automatically. Setting 
	   * the scrollbar policy to automatic allows the scrollbars to only show up 
	   * when needed. 
	   */
	scrolled_window = gtk_scrolled_window_new (nullptr, nullptr);
	gtk_scrolled_window_set_policy ((GtkScrolledWindow*) scrolled_window, 
	                                  GTK_POLICY_AUTOMATIC, 
	                                  GTK_POLICY_AUTOMATIC); 
	  /* The function directly below is used to add children to the scrolled window 
	   * with scrolling capabilities (e.g text_view), otherwise, 
	   * gtk_scrolled_window_add_with_viewport() would have been used
	   */
	gtk_container_add ((GtkContainer*) scrolled_window, 
	                                         text_view);
	gtk_container_set_border_width ((GtkContainer*)scrolled_window, 5);
	GtkWidget*en=gtk_combo_box_text_new_with_entry();
	GtkWidget*entext=gtk_bin_get_child((GtkBin*)en);
	gtk_entry_set_text ((GtkEntry*)entext,":");
	g_signal_connect_data (entext, "activate",G_CALLBACK (enter_callback),nullptr,nullptr,(GConnectFlags) 0);
	//
	GtkWidget*box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_pack_start((GtkBox*)box,en,FALSE,FALSE,0);
	gtk_box_pack_start((GtkBox*)box,scrolled_window,TRUE,TRUE,0);
	gtk_container_add ((GtkContainer*)window, box);
	gtk_widget_show_all (window);
}

static void decr(int*argc,char**argv,int i){
	for(int j=i+1;j<*argc;j++){
		argv[j-1]=argv[j];
	}
	*argc=*argc-1;
}

int
main (int    argc,
      char **argv)
{
	  /* ---------------------------------------------------------- *
	   * initialize SSL library and register algorithms             *
	   * ---------------------------------------------------------- */
	if(OPENSSL_init_ssl(OPENSSL_INIT_NO_LOAD_SSL_STRINGS,nullptr)==1){
		GtkApplication *app;
		app = gtk_application_new (nullptr, G_APPLICATION_FLAGS_NONE);
		//if(app!=nullptr){
		g_application_add_main_option((GApplication*)app,"dimensions",'d',G_OPTION_FLAG_IN_MAIN,G_OPTION_ARG_STRING,"Window size","WIDTHxHEIGHT");
		int dim[2]={0,0};
		for(int i=0;i<argc;i++){
			char*a=argv[i];
			BOOL is_long=strncmp(a,"--dimensions",12)==0;
			if(is_long||(strncmp(a,"-d",2)==0)){
				char*b=nullptr;
				if(is_long){
					a=argv[i]+12;
					if(*a=='='){a++;b=strchr(a,'x');}
					else{
						i++;if(i<argc){
							a=argv[i];
							b=strchr(a,'x');
							if(b!=nullptr)decr(&argc,argv,i);
						}
					}
				}else{
					a=argv[i]+2;
					i++;if(i<argc){
						a=argv[i];
						b=strchr(a,'x');
						if(b!=nullptr)decr(&argc,argv,i);
					}
				}
				if(b!=nullptr){
					*b='\0';b++;
					dim[0]=atoi(a);
					dim[1]=atoi(b);
					decr(&argc,argv,i);
					break;
				}
			}
		}
		g_signal_connect_data (app, "activate", G_CALLBACK (activate), dim, nullptr,(GConnectFlags) 0);//obj>gsignal gobject-2.0
		//  if(han>0)
		g_application_run ((GApplication*)app, argc, argv);//gio.h>gapplication.h gio-2.0
		g_object_unref(buffer);
		g_object_unref (app);//#include gobject.h gobject-2.0
	}
}
