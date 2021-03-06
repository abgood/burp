#ifndef _BURPCONFIG_LEGACY_H
#define _BURPCONFIG_LEGACY_H

/* Burp common configuration defines */
/* Graham says: probably most of this stuff can be deleted - it is debris
   left from Bacula */

#undef  TRUE
#undef  FALSE
#define TRUE  1
#define FALSE 0

#ifndef ETIME
#define ETIME ETIMEDOUT
#endif

#define ioctl_req_t long unsigned int

#ifdef PROTOTYPES
# define __PROTO(p)     p
#else
# define __PROTO(p)     ()
#endif

#define ASSERT(x)

/* Allow printing of NULL pointers */
#define NPRT(x) (x)?(x):_("*None*")
#define NPRTB(x) (x)?(x):""

#if defined(HAVE_WIN32)

#define WIN32_REPARSE_POINT  1   /* Can be any number of "funny" directories except the next two */
#define WIN32_MOUNT_POINT    2   /* Directory link to Volume */
#define WIN32_JUNCTION_POINT 3   /* Directory link to a directory */

void InitWinAPIWrapper();

#define  OSDependentInit()    InitWinAPIWrapper()

#define sbrk(x)  0

#define clear_thread_id(x) memset(&(x), 0, sizeof(x))

#if defined(BUILDING_DLL)
#  define DLL_IMP_EXP   _declspec(dllexport)
#elif defined(USING_DLL)
#  define DLL_IMP_EXP   _declspec(dllimport)
#else
#  define DLL_IMP_EXP
#endif

#if defined(USING_CATS)
#  define CATS_IMP_EXP   _declspec(dllimport)
#else
#  define CATS_IMP_EXP
#endif

#else  /* HAVE_WIN32 */

#define clear_thread_id(x) x = 0

#define DLL_IMP_EXP
#define CATS_IMP_EXP

#define  OSDependentInit()

#endif /* HAVE_WIN32 */


#ifdef ENABLE_NLS
   #include <libintl.h>
   #include <locale.h>
   #ifndef _
      #define _(s) gettext((s))
   #endif /* _ */
   #ifndef N_
      #define N_(s) (s)
   #endif /* N_ */
#else /* !ENABLE_NLS */
   #undef _
   #undef N_
   #undef textdomain
   #undef bindtextdomain
   #undef setlocale

   #ifndef _
      #define _(s) (s)
   #endif
   #ifndef N_
      #define N_(s) (s)
   #endif
   #ifndef textdomain
      #define textdomain(d)
   #endif
   #ifndef bindtextdomain
      #define bindtextdomain(p, d)
   #endif
   #ifndef setlocale
      #define setlocale(p, d)
   #endif
#endif /* ENABLE_NLS */


/* Use the following for strings not to be translated */
#define NT_(s) (s)   

/* This should go away! ****FIXME***** */
#define MAXSTRING 500

/* Maximum length to edit time/date */
#define MAX_TIME_LENGTH 50

/* Maximum Name length including EOS */
#define MAX_NAME_LENGTH 128

/* Maximume number of user entered command args */
#define MAX_CMD_ARGS 30

/* All tape operations MUST be a multiple of this */
#define TAPE_BSIZE 1024

#ifdef DEV_BSIZE 
#define B_DEV_BSIZE DEV_BSIZE
#endif

#if !defined(B_DEV_BSIZE) & defined(BSIZE)
#define B_DEV_BSIZE BSIZE
#endif

#ifndef B_DEV_BSIZE
#define B_DEV_BSIZE 512
#endif

/*
 * Set to time limit for other end to respond to
 *  authentication.  Normally 10 minutes is *way*
 *  more than enough. The idea is to keep the Director
 *  from hanging because there is a dead connection on
 *  the other end.
 */
#define AUTH_TIMEOUT 60 * 10

/*
 * Default network buffer size
 */
#define DEFAULT_NETWORK_BUFFER_SIZE (64 * 1024)

#define FT_LNK_H      1  // hard link to file already saved.
#define FT_REG        3  // Regular file.
#define FT_LNK_S      4  // Soft Link.
#define FT_DIR        5  // Directory.
#define FT_SPEC       6  // Special file -- chr, blk, fifo, sock.
#define FT_NOFOLLOW   8  // Could not follow link.
#define FT_NOSTAT     9  // Could not stat file.
#define FT_NOFSCHG   14  // Different file system, prohibited.
#define FT_NOOPEN    15  // Could not open directory.
#define FT_RAW       16  // Raw block device.
#define FT_FIFO      17  // Raw fifo device.
#define FT_REPARSE   21  // Win NTFS reparse point.
#define FT_JUNCTION  26  // Win32 Junction point.

/* Size of File Address stored in STREAM_SPARSE_DATA. Do NOT change! */
#define SPARSE_FADDR_SIZE (sizeof(uint64_t))

/* Size of crypto length stored at head of crypto buffer. Do NOT change! */
#define CRYPTO_LEN_SIZE ((int)sizeof(uint32_t))


/* This is for dumb compilers/libraries like Solaris. Linux GCC
 * does it correctly, so it might be worthwhile
 * to remove the isascii(c) with ifdefs on such
 * "smart" systems.
 */
#define B_ISSPACE(c) (isascii((int)(c)) && isspace((int)(c)))
#define B_ISALPHA(c) (isascii((int)(c)) && isalpha((int)(c)))
#define B_ISUPPER(c) (isascii((int)(c)) && isupper((int)(c)))
#define B_ISDIGIT(c) (isascii((int)(c)) && isdigit((int)(c)))

/* For multiplying by 10 with shift and addition */
#define B_TIMES10(d) ((d<<3)+(d<<1))


typedef void (HANDLER)();
typedef int (INTHANDLER)();

#ifdef SETPGRP_VOID
# define SETPGRP_ARGS(x, y) /* No arguments */
#else
# define SETPGRP_ARGS(x, y) (x, y)
#endif

#ifndef S_ISLNK
#define S_ISLNK(m) (((m) & S_IFM) == S_IFLNK)
#endif

/* Added by KES to deal with Win32 systems */
#ifndef S_ISWIN32
#define S_ISWIN32 020000
#endif

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif

#ifndef MODE_RW
#define MODE_RW 0666
#endif

#if defined(HAVE_WIN32)
typedef int64_t   boffset_t;
#else
typedef off_t     boffset_t;
#endif

#define debug_level	0

/* Use our strdup with smartalloc */
#ifdef HAVE_WXCONSOLE
/* Groan, WxWidgets has its own way of doing NLS so cleanup */
#ifndef ENABLE_NLS
#undef _
#undef setlocale
#undef textdomain
#undef bindtextdomain
#endif  
#endif

/* This probably should be done on a machine by machine basis, but it works */
/* This is critical for the smartalloc routines to properly align memory */
#define ALIGN_SIZE (sizeof(double))
#define BALIGN(x) (((x) + ALIGN_SIZE - 1) & ~(ALIGN_SIZE -1))


/* =============================================================
 *               OS Dependent defines
 * ============================================================= 
 */

#ifndef HAVE_FSEEKO
/* Bad news. This OS cannot handle 64 bit fseeks and ftells */
#define fseeko fseek
#define ftello ftell
#endif

#if defined (__digital__) && defined (__unix__)
/* Tru64 - it does have fseeko and ftello , but since ftell/fseek are also 64 bit */
/* take this 'shortcut' */
#define fseeko fseek
#define ftello ftell
#undef  ioctl_req_t
#define ioctl_req_t int
#endif


#ifdef __alpha__
#define OSF 1
#undef  ioctl_req_t
#define ioctl_req_t int
#endif

#ifdef HAVE_SUN_OS
   /*
    * On Solaris 2.5, threads are not timesliced by default, so we need to
    * explictly increase the conncurrency level.
    */
#include <thread.h>
#define set_thread_concurrency(x)  thr_setconcurrency(x)
extern int thr_setconcurrency(int);
#define SunOS 1
#undef  ioctl_req_t
#define ioctl_req_t int

#else


/* Not needed on most systems */
#define set_thread_concurrency(x)

#endif

#if defined(HAVE_DARWIN_OS) || defined(HAVE_OSF1_OS)
/* Apparently someone forgot to wrap getdomainname as a C function */
extern "C" int getdomainname(char *name, int len);
#endif



#if defined(HAVE_WIN32)
#define DEFAULT_CONFIGDIR "C:\\Documents and Settings\\All Users\\Application Data\\Burp"
#define PathSeparator '\\'

//inline bool IsPathSeparator(int ch) { return ch == '/' || ch == '\\'; }
inline char *first_path_separator(char *path) { return strpbrk(path, "/\\"); }
inline const char *first_path_separator(const char *path) { return strpbrk(path, "/\\"); }

#else
#define PathSeparator '/'
/* Define Winsock functions if we aren't on Windows */

#define WSA_Init() 0 /* 0 = success */
#define WSACleanup() 0 /* 0 = success */

//inline bool IsPathSeparator(int ch) { return ch == '/'; }
inline char *first_path_separator(char *path) { return strchr(path, '/'); }
inline const char *first_path_separator(const char *path) { return strchr(path, '/'); }
#endif


/* HP-UX 11 specific workarounds */

#ifdef HAVE_HPUX_OS
# undef h_errno
extern int h_errno;
/* the {get,set}domainname() functions exist in HPUX's libc.
 * the configure script detects that correctly.
 * the problem is no system headers declares the prototypes for these functions
 * this is done below
 */
extern "C" int getdomainname(char *name, int namelen);
extern "C" int setdomainname(char *name, int namelen);
#undef  ioctl_req_t
#define ioctl_req_t int
#endif /* HAVE_HPUX_OS */


#ifdef HAVE_OSF1_OS
extern "C" int fchdir(int filedes);
extern "C" long gethostid(void);
extern "C" int mknod ( const char *path, int mode, dev_t device );
#undef  ioctl_req_t
#define ioctl_req_t int
#endif


/* Disabled because it breaks internationalisation...
#undef HAVE_SETLOCALE
#ifdef HAVE_SETLOCALE
#include <locale.h>
#else
#define setlocale(x, y) ("ANSI_X3.4-1968")
#endif
#ifdef HAVE_NL_LANGINFO
#include <langinfo.h>
#else
#define nl_langinfo(x) ("ANSI_X3.4-1968")
#endif
*/

#endif /* _BURP_H */
