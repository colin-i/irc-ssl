
struct hostent
{
  char *h_name;
  char **h_aliases;
  int h_addrtype;
  int h_length;
  char **h_addr_list;

#define h_addr h_addr_list[0]

};

#ifdef __cplusplus
extern "C" {
#endif

struct hostent *gethostbyname(const char *name);

#ifdef __cplusplus
}
#endif
