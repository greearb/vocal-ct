
#ifndef __VOCAL__MISC_H__
#define __VOCAL__MISC_H__

// Released into the public domain. --Ben Greear Sep 6, 2003

#include <string>
#include <sys/time.h>

using namespace std;

void vhexDump(const char* msg, int len, string& retval,
              bool show_decode = true, bool add_newlines = true);


void debugMemUsage(const char* msg, const char* fname);


#if defined(sparc) || defined(__OpenBSD__)

// Fix sparc compile
typedef uint64_t u64; // hack, so we may include kernel's ethtool.h
typedef uint32_t u32; // hack, so we may include kernel's ethtool.h
typedef uint16_t u16; // ditto
typedef uint8_t u8;   // ditto

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

#define __u8 uint8
#define __u16 uint16
#define __u32 uint32
#define __u64 uint64

#else
// 15/1/04 fpi
// WorkAround Wi32
#ifndef WIN32

typedef __uint64_t u64; // hack, so we may include kernel's ethtool.h
typedef __uint32_t u32; // hack, so we may include kernel's ethtool.h
typedef __uint16_t u16; // ditto
typedef __uint8_t u8;   // ditto

typedef __uint64_t uint64;
typedef __uint32_t uint32;
typedef __uint16_t uint16;
typedef __uint8_t uint8;

#define __u8 uint8
#define __u16 uint16
#define __u32 uint32
#define __u64 uint64

#else

typedef UINT64 u64; // hack, so we may include kernel's ethtool.h
typedef DWORD  u32; // hack, so we may include kernel's ethtool.h
typedef WORD   u16; // ditto
typedef BYTE   u8;   // ditto

typedef DWORD uint32;
typedef WORD uint16;
typedef BYTE uint8;


typedef ULARGE_INTEGER uint64;

#define __u8  BYTE
#define __u16 WORD
#define __u32 DWORD
#define __u64 ULARGE_INTEGER

#endif
#endif

uint64 vgetCurMs();
uint64 vgetCurUs();
uint64 vgetCurNs();
struct timeval vgetCurTv();
struct timeval vtimediff(struct timeval *a, struct timeval *b);
/** Convert from miliseconds */
struct timeval vms_to_tv(uint64 ms);

/** Convert to miliseconds */
uint64 vtv_to_ms(const struct timeval& tv);

/** Convert to micro-seconds */
uint64 vtv_to_us(const struct timeval& tv);

/** Convert to nano-seconds */
uint64 vtv_to_ns(const struct timeval& tv);

int vparseInt(const char* s);
bool visNumber(const char* s);

string itoa(int i);
string itoa(uint16 i);

//void vusleep(int milliseconds);


#endif
