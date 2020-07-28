
typedef unsigned long long uint64_t;
typedef void OPENSSL_INIT_SETTINGS;
typedef void SSL_CTX;
typedef void SSL_METHOD;
typedef void SSL;
#define SSLv23_client_method TLS_client_method
#define SSL_OP_NO_SSLv2 0x0
#define OPENSSL_INIT_NO_LOAD_SSL_STRINGS 0x00100000L

#ifdef __cplusplus
extern "C" {
#endif

int OPENSSL_init_ssl(uint64_t opts, const OPENSSL_INIT_SETTINGS *settings);
void SSL_CTX_free(SSL_CTX *);
SSL_CTX *SSL_CTX_new(const SSL_METHOD *meth);
unsigned long SSL_CTX_set_options(SSL_CTX *ctx, unsigned long op);
int SSL_connect(SSL *ssl);
void SSL_free(SSL *ssl);
SSL *SSL_new(SSL_CTX *ctx);
int SSL_read(SSL *ssl, void *buf, int num);
int SSL_set_fd(SSL *s, int fd);
int SSL_shutdown(SSL *ssl);
int SSL_write(SSL *ssl, const void *buf, int num);
const SSL_METHOD *TLS_client_method(void);

#ifdef __cplusplus
}
#endif
