
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

typedef uint64_t uint64;
typedef int64_t int64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

#else

#ifndef WIN32

typedef __uint64_t uint64;
typedef __int64_t int64;
typedef __uint32_t uint32;
typedef __uint16_t uint16;
typedef __uint8_t uint8;

#define closesocket close
#define vsleep sleep
#define vgettimeofday gettimeofday
#define VSTRERROR strerror(errno)
#define ERRNO errno
#define WSAEWOULDBLOCK EWOULDBLOCK

#else

typedef uint32_t uint32;
typedef int32_t int32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef uint64_t uint64;
typedef int64_t int64;

void vsleep(int secs);

#define SOL_IP IPPROTO_IP

#define SOCKADDR sockaddr
const char* vwinstrerror();
#define VSTRERROR vwinstrerror()
#define ERRNO WSAGetLastError()

extern uint64 vbaselineMs;
extern uint32 vtickBaseline;

int vgettimeofday(struct timeval* tv, void* null);

// Gets high resolution by spinning up to 15ms.  Don't call this often!!!
uint64 vgetRawCurMsSpin();
uint64 vcalcEpocOffset();
void vresyncBaselineClock(); /* re-calculates the basline using getRawCurMsSpin() */

/* Correctly synched with the 'real' time/date clock, but only exact to
 * about 15ms.  Set foo to non NULL if you want to recalculate the
 */
uint64 vgetCurMsFromClock();


#endif
#endif

//#define __u8  uint8
//#define __u16 uint16
//#define __u32 uint32
//#define __u64 uint64

const char* vtoStringIp(uint32 ip);
int vtoIpString(const char* ip, uint32& retval);

#define vgetCurMs() __dbg_vgetCurMs(__FILE__, __LINE__)
//#define vgetCurMs __vgetCurMs
uint64 __dbg_vgetCurMs(const char* file, int line);
uint64 __vgetCurMs();
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
string itoa(unsigned long);
string itoa(uint16 i);


int vsetPrio(int sk, uint16 tos, uint32 prio, const char* dbg);

/* Returns actual priority that was set, or < 0 on error */
/* NOTE:  Must set Priority after TOS, cause TOS clobbers priority */
int vsetPriorityHelper_priv(int sk, uint32 prio, const char* dbg);
int vsetTosHelper_priv(int sk, uint16 tos);

//void vusleep(int milliseconds);


#endif
