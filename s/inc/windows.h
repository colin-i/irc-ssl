
#include "in.h"
#define h_addr h_addr_list[0]
struct hostent {
 char *h_name;
 char **h_aliases;
 short h_addrtype;
 short h_length;
 char **h_addr_list;
};//this is same 32 size like on linux because short is 4 bytes aligned

#define INFINITE 0xffffffff

typedef unsigned short u_short;

typedef unsigned char BYTE;
typedef char CHAR;
typedef unsigned long DWORD;
typedef void* HANDLE;
typedef unsigned long long UINT_PTR;
typedef unsigned long long ULONG_PTR;
typedef unsigned short WORD;

typedef ULONG_PTR DWORD_PTR;
typedef const CHAR *LPCSTR;

//incomplete
typedef void* LPSECURITY_ATTRIBUTES;


#define IPPROTO_IP 0
#define AF_INET 2
#define SOCK_STREAM 1
//#define SD_BOTH 2

typedef UINT_PTR SOCKET;
#define WINAPI __stdcall

#define WSAAPI WINAPI

typedef struct WSAData {
 WORD wVersion;
 WORD wHighVersion;
 unsigned short iMaxSockets;
 unsigned short iMaxUdpDg;
 char *lpVendorInfo;
 char szDescription[256 +1];
 char szSystemStatus[128 +1];
} WSADATA, *LPWSADATA;

#define __MINGW_NAME_AW(func) func ##A
#define __MSABI_LONG(x) x ## l
#define MAKEWORD(a,b) ((WORD) (((BYTE) (((DWORD_PTR) (a)) & 0xff)) | ((WORD) ((BYTE) (((DWORD_PTR) (b)) & 0xff))) << 8))
#define NO_ERROR __MSABI_LONG(0)

#define CreateEvent __MINGW_NAME_AW(CreateEvent)

#ifdef __cplusplus
extern "C" {
#endif

BOOL CloseHandle(HANDLE hObject);
HANDLE CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes,BOOL bManualReset,BOOL bInitialState,LPCSTR lpName);
BOOL SetEvent(HANDLE hEvent);
DWORD WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds);

int WSAAPI closesocket(SOCKET s);
int WSAAPI connect(SOCKET s,const struct sockaddr* name,int namelen);
struct hostent* WSAAPI gethostbyname(const char* name);
u_short WSAAPI htons(u_short hostshort);
int WSAAPI recv(SOCKET s,char* buf,int len,int flags);
int WSAAPI send(SOCKET s,const char* buf,int len,int flags);
SOCKET WSAAPI socket(int af,int type,int protocol);
int WSAAPI shutdown(SOCKET s,int how);

int WSAAPI WSACleanup();
int WSAAPI WSAStartup(WORD wVersionRequested,LPWSADATA lpWSAData);

#ifdef __cplusplus
}
#endif
