#pragma once

#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <poll.h>
#else
#include <WinSock2.h>
#endif
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#ifdef WIN32
typedef long long ssize_t;
typedef int mode_t;
#endif

#define ST_VERSION          "1.9"
#define ST_VERSION_MAJOR    1
#define ST_VERSION_MINOR    9

#ifndef ETIME
#define ETIME ETIMEDOUT
#endif

#ifndef ST_UTIME_NO_TIMEOUT
#define ST_UTIME_NO_TIMEOUT ((st_utime_t) -1LL)
#endif

#ifndef ST_UTIME_NO_WAIT
#define ST_UTIME_NO_WAIT 0
#endif

#define ST_EVENTSYS_DEFAULT 0
#define ST_EVENTSYS_SELECT  1
#define ST_EVENTSYS_POLL    2
#define ST_EVENTSYS_ALT     3

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long  st_utime_t;
typedef struct _st_thread* st_thread_t;
typedef struct _st_cond*    st_cond_t;
typedef struct _st_mutex*   st_mutex_t;
typedef struct _st_netfd*   st_netfd_t;

extern int st_init(void);
extern int st_getfdlimit(void);

extern int st_set_eventsys(int eventsys);
extern int st_get_eventsys(void);
extern const char* st_get_eventsys_name(void);

extern st_thread_t st_thread_self(void);
extern void st_thread_exit(void* retval);
extern int st_thread_join(st_thread_t thread, void** retvalp);
extern void st_thread_interrupt(st_thread_t thread);
extern st_thread_t st_thread_create(void* (*start)(void* arg), void* arg, int joinable, int stack_size);
extern int st_randomize_stacks(int on);
extern int st_set_utime_function(st_utime_t (*func)(void));

extern st_utime_t st_utime(void);
extern st_utime_t st_utime_last_clock(void);
extern int st_timecache_set(int on);
extern time_t st_time(void);
extern int st_usleep(st_utime_t usecs);
extern int st_sleep(int secs);
extern st_cond_t st_cond_new(void);
extern int st_cond_destroy(st_cond_t cvar);
extern int st_cond_timedwait(st_cond_t cvar, st_utime_t timeout);
extern int st_cond_wait(st_cond_t cvar);
extern int st_cond_signal(st_cond_t cvar);
extern int st_cond_broadcast(st_cond_t cvar);
extern st_mutex_t st_mutex_new(void);
extern int st_mutex_destroy(st_mutex_t lock);
extern int st_mutex_lock(st_mutex_t lock);
extern int st_mutex_unlock(st_mutex_t lock);
extern int st_mutex_trylock(st_mutex_t lock);

extern int st_key_create(int* keyp, void (*destructor)(void*));
extern int st_key_getlimit(void);
extern int st_thread_setspecific(int key, void* value);
extern void* st_thread_getspecific(int key);

extern st_netfd_t st_netfd_open(int osfd);
extern st_netfd_t st_netfd_open_socket(int osfd);
extern void st_netfd_free(st_netfd_t fd);
extern int st_netfd_close(st_netfd_t fd);
extern int st_netfd_fileno(st_netfd_t fd);
extern void st_netfd_setspecific(st_netfd_t fd, void* value, void (*destructor)(void*));
extern void* st_netfd_getspecific(st_netfd_t fd);
extern int st_netfd_serialize_accept(st_netfd_t fd);
extern int st_netfd_poll(st_netfd_t fd, int how, st_utime_t timeout);

extern int st_poll(struct pollfd* pds, int npds, st_utime_t timeout);
extern st_netfd_t st_accept(st_netfd_t fd, struct sockaddr* addr, int* addrlen, st_utime_t timeout);
extern int st_connect(st_netfd_t fd, const struct sockaddr* addr, int addrlen, st_utime_t timeout);
extern ssize_t st_read(st_netfd_t fd, void* buf, size_t nbyte, st_utime_t timeout);
extern ssize_t st_read_fully(st_netfd_t fd, void* buf, size_t nbyte, st_utime_t timeout);
extern ssize_t st_write(st_netfd_t fd, const void* buf, size_t nbyte, st_utime_t timeout);
extern int st_recvfrom(st_netfd_t fd, void* buf, int len, struct sockaddr* from, int* fromlen, st_utime_t timeout);
extern int st_sendto(st_netfd_t fd, const void* msg, int len, const struct sockaddr* to, int tolen, st_utime_t timeout);
extern st_netfd_t st_open(const char* path, int oflags, mode_t mode);

#ifdef __cplusplus
}
#endif