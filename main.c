
#include <gtk/gtk.h>
/*struct _GtkTextIter {
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
};*/

#include "inc/string.h"
#include "inc/unistd.h"
#include "inc/stdlib.h"
#include "inc/stdio.h"
#include "inc/in.h"//netinet/in.h

#include "inc/netdb.h"
#include "inc/socket.h"//sys/socket.h

#include <openssl/ssl.h>

/* ---------------------------------------------------------- *
 * create_socket() creates the socket & TCP-connect to server *
 * ---------------------------------------------------------- */
int create_socket(char url_str[]/*, BIO *out*/) {
  int sockfd;
  char hostname[256];
  int           port;
  struct hostent *host;
  struct sockaddr_in dest_addr;

  /* ---------------------------------------------------------- *
   * Remove the final / from url_str, if there is one           *
   * ---------------------------------------------------------- */
 // if(url_str[strlen(url_str)] == '/')
   // url_str[strlen(url_str)] = '\0';

  /* ---------------------------------------------------------- *
   * the first : ends the protocol string, i.e. http            *
   * ---------------------------------------------------------- */
  //strncpy(proto, url_str, (strchr(url_str, ':')-url_str));

  /* ---------------------------------------------------------- *
   * the hostname starts after the "://" part                   *
   * ---------------------------------------------------------- */
char*hst=strstr(url_str, "://")+3;
size_t hstsz=strlen(hst);
if(hstsz<sizeof(hostname)){
  memcpy(hostname, hst, hstsz);
hostname[hstsz]='\0';

  /* ---------------------------------------------------------- *
   * if the hostname contains a colon :, we got a port number   *
   * ---------------------------------------------------------- */
  char    portnum[6] = "443";
  //char      proto[6] = "";

  if(strchr(hostname, ':')) {
    char*tmp_ptr = strchr(hostname, ':')+1;
    /* the last : starts the port number, if avail, i.e. 8443 */
size_t sz=strlen(tmp_ptr);
if(sz>sizeof(portnum)-1)return -1;
    memcpy(portnum, tmp_ptr, sz );portnum[sz]='\0';
    tmp_ptr[-1] = '\0';
  }

  if ( (host = gethostbyname(hostname)) != NULL ) {

  /* ---------------------------------------------------------- *
   * create the basic TCP socket                                *
   * ---------------------------------------------------------- */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
if(sockfd!=-1){
  dest_addr.sin_family=AF_INET;
  port = atoi(portnum);//stdlib
  dest_addr.sin_port=htons(port);
  dest_addr.sin_addr.s_addr = *(long*)(host->h_addr_list[0]);

  /* ---------------------------------------------------------- *
   * Zeroing the rest of the struct                             *
   * ---------------------------------------------------------- */
//  memset(&(dest_addr.sin_zero), '\0', 8);//string
//"setting it to zero doesn't seem to be actually necessary"
  //tmp_ptr = inet_ntoa(dest_addr.sin_addr);

  /* ---------------------------------------------------------- *
   * Try to make the host connect here                          *
   * ---------------------------------------------------------- */
  if ( connect(sockfd, (struct sockaddr *) &dest_addr,
                              sizeof(struct sockaddr)) != -1 ) {
  return sockfd;
}
else{
//    BIO_
printf("Error: Cannot connect to host %s on port %d.\n",
             hostname, port);
close(sockfd);
}
}else puts("socket failed");
}else
//    BIO_
printf("Error: Cannot resolve hostname %s.\n",  hostname);
}
return -1;
}

static void proced(GtkTextBuffer *buffer,char*dest_url){
	GtkTextIter it;

  //BIO              *certbio = NULL;
  //BIO               *outbio = NULL;
  //X509                *cert = NULL;
  //X509_NAME       *certname = NULL;
  const SSL_METHOD *method;
  SSL_CTX *ctx;
  SSL *ssl;
  int server;
  int ret, i;

  /* ---------------------------------------------------------- *
   * These function calls initialize openssl for correct work.  *
   * ---------------------------------------------------------- */
//  OpenSSL_add_all_algorithms();
//  ERR_load_BIO_strings();
//  ERR_load_crypto_strings();
//  SSL_load_error_strings();
//SSL_library_init();

  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  //certbio = BIO_new(BIO_s_file());
  //outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  /* ---------------------------------------------------------- *
   * Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
   * ---------------------------------------------------------- */
  method = SSLv23_client_method();

  /* ---------------------------------------------------------- *
   * Try to create a new SSL context                            *
   * ---------------------------------------------------------- */
ctx = SSL_CTX_new(method);
  if ( ctx != NULL){
//    BIO_
//puts(/*outbio ,*/ "Unable to create a new SSL context structure.");

  /* ---------------------------------------------------------- *
   * Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
   * ---------------------------------------------------------- */
  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

  /* ---------------------------------------------------------- *
   * Create new SSL connection state object                     *
   * ---------------------------------------------------------- */
  ssl = SSL_new(ctx);
if(ssl!=NULL){
//puts("Unable to create a new SSL structure."):return;

  /* ---------------------------------------------------------- *
   * Make the underlying TCP socket connection                  *
   * ---------------------------------------------------------- */
  server = create_socket(dest_url/*, outbio*/);
  if(server != -1){
//puts("Unable to make the TCP connection.");return;
//}
//    BIO_
printf(/*outbio ,*/ "Successfully made the TCP connection to: %s.\n", dest_url);

  /* ---------------------------------------------------------- *
   * Attach the SSL session to the socket descriptor            *
   * ---------------------------------------------------------- */
  if(SSL_set_fd(ssl, server)==1){

  /* ---------------------------------------------------------- *
   * Try to SSL-connect here, returns 1 for success             *
   * ---------------------------------------------------------- */
  if ( SSL_connect(ssl) == 1 ){
//    BIO_
//    BIO_
printf(/*outbio ,*/ "Successfully enabled SSL/TLS session to: %s.\n", dest_url);

  /* ---------------------------------------------------------- *
   * Get the remote certificate into the X509 structure         *
   * ---------------------------------------------------------- */
  //cert = SSL_get_peer_certificate(ssl);
  //if (cert == NULL)
//    BIO_
//printf(/*outbio ,*/ "Error: Could not get a certificate from: %s.\n", dest_url);
  //else
//    BIO_
//printf(/*outbio ,*/ "Retrieved the server's certificate from: %s.\n", dest_url);

  /* ---------------------------------------------------------- *
   * extract various certificate information                    *
   * -----------------------------------------------------------*/
  //certname = X509_NAME_new();
  //certname = X509_get_subject_name(cert);

  /* ---------------------------------------------------------- *
   * display the cert subject here                              *
   * -----------------------------------------------------------*/
//  BIO_
//printf(/*outbio ,*/ "Displaying the certificate subject data:\n");
  //X509_NAME_print_ex(stdout,/*outbio ,*/ certname, 0, 0);
//  BIO_
//printf(/*outbio ,*/ "\n");

const char*i1="PASS abc\nNICK don\nUSER guest tolmoon tolsun :Ronnie Reagan\n\n";
//int sz;//ssize_t sz;
SSL_write(ssl,i1,strlen(i1));

#define z 512
char buf[z];
for(;;){
ssize_t sz=SSL_read(ssl, buf, z-1);
if(sz<=0)break;
buf[sz]='\0';char*b=buf;
for(;;){
	char*n=strstr(b,"\n");
	if(n!=NULL){
	char aux=n[1];n[1]='\0';
	gtk_text_buffer_get_end_iter(buffer,&it);
	int s=n+1-b;
	gtk_text_buffer_insert(buffer,&it,b,s);
	//printf("%s",b);
	n[1]=aux;
	if(strncmp(b,"PING",4)==0){
		b[1]='O';
		sz=SSL_write(ssl,b,strlen(b));
	}
	b=n+1;sz-=s;continue;
	}
	gtk_text_buffer_get_end_iter(buffer,&it);
	gtk_text_buffer_insert(buffer,&it,b,sz);
	//printf("%s",b);
	break;
}
}

  /* ---------------------------------------------------------- *
   * Free the structures we don't need anymore                  *
   * -----------------------------------------------------------*/
}else printf(/*outbio ,*/ "Error: Could not build a SSL session to: %s.\n", dest_url);
}else puts("SSL_set_fd failed");
  close(server);
}
  //X509_free(cert);
  SSL_free(ssl);
}
  SSL_CTX_free(ctx);
}

//  BIO_
//printf(/*outbio ,*/ "Finished SSL/TLS connection with server: %s.\n", dest_url);

}

static gpointer worker (gpointer data)
{
 proced((GtkTextBuffer *)data,"https://127.0.0.1:6697");   
  return NULL;
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
  /* Declare variables */
  GtkWidget *window;
  GtkWidget *text_view;
  GtkWidget *scrolled_window;

  GtkTextBuffer *buffer;


  /* Create a window with a title, and a default size */
  window = gtk_application_window_new (app);
  gtk_window_set_title ((GtkWindow*) window, "TextView Example");
//  gtk_window_set_default_size ((GTK_WINDOW) window, 220, 200);
gtk_window_maximize((GtkWindow*)window);

  /* The text buffer represents the text being edited */
  buffer = gtk_text_buffer_new (NULL);
  

  /* Text view is a widget in which can display the text buffer. 
   * The line wrapping is set to break lines in between words.
   */
  text_view = gtk_text_view_new_with_buffer (buffer);
gtk_text_view_set_editable((GtkTextView*) text_view, FALSE);
  gtk_text_view_set_wrap_mode ((GtkTextView*) text_view, GTK_WRAP_WORD); 


  /* Create the scrolled window. Usually NULL is passed for both parameters so 
   * that it creates the horizontal/vertical adjustments automatically. Setting 
   * the scrollbar policy to automatic allows the scrollbars to only show up 
   * when needed. 
   */
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
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
 
  
  gtk_container_add ((GtkContainer*)window, scrolled_window);

/*     Create a red Button    */
//  GtkWidget*  b = gtk_button_new_with_label("Red");
    //gtk_widget_set_name(button1, "myButton_red");
//    gtk_widget_set_size_request(b, 160, 130);
//g_signal_connect (b, "clicked", G_CALLBACK (callback), (gpointer) buffer);

//GtkWidget*grid=gtk_grid_new();
//gtk_grid_attach(GTK_GRID(grid),scrolled_window,0,0,2,2);
//gtk_grid_attach(GTK_GRID(grid),b,0,2,2,1);
//gtk_container_add (GTK_CONTAINER (window), grid);

  gtk_widget_show_all (window);

GThread*th=g_thread_new("a",worker,(gpointer)buffer);
g_thread_unref(th);
}

int
main (int    argc,
      char **argv)
{
  /* ---------------------------------------------------------- *
   * initialize SSL library and register algorithms             *
   * ---------------------------------------------------------- */
//  if(
//SSL_library_init() always returns "1"
if(OPENSSL_init_ssl(OPENSSL_INIT_NO_LOAD_SSL_STRINGS,NULL)==1){
// < 0){
//    BIO_
//puts(/*outbio ,*/ "Could not initialize the OpenSSL library !");
//return;
//}

  GtkApplication *app;
  //int status;

  app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
//if(app!=NULL){
//typedef void (*GCallback) (void);

/*gulong      g_signal_connect_data           (gpointer instance,
                                             const gchar *detailed_signal,
                                             GCallback c_handler,
                                             gpointer data,
                                             GClosureNotify destroy_data,
                                             GConnectFlags connect_flags);*/
//  gulong han=
g_signal_connect_data (app, "activate", G_CALLBACK (activate), NULL, NULL,/*(GConnectFlags)*/ 0);//obj>gsignal gobject-2.0
//  if(han>0)
/*status*/g_application_run ((GApplication*)app, argc, argv);//gio.h>gapplication.h gio-2.0
  g_object_unref (app);//#include gobject.h gobject-2.0
//}
  //return status;
}
}
