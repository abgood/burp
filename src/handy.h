#ifndef HANDY_H
#define HANDY_H

#include "prepend.h"

#include <openssl/evp.h>
#include <openssl/md5.h>
#include <zlib.h>

#include "bfile.h"

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

extern void close_fd(int *fd);
extern int close_fp(FILE **fp);
extern int gzclose_fp(gzFile *fp);
extern int is_dir(const char *path, struct dirent *d);
extern int is_dir_lstat(const char *path);
extern int mkpath(char **rpath, const char *limit);
extern int build_path(const char *datadir, const char *fname, char **rpath, const char *limit);

extern int open_file_for_send(BFILE *bfd, const char *fname, int64_t winattr, struct config *conf);
extern int close_file_for_send(BFILE *bfd);
extern int send_whole_file_gz(const char *fname, const char *datapth, int quick_read, unsigned long long *bytes, const char *encpassword, struct cntr *cntr, int compression, BFILE *bfd, FILE *fp, const char *extrameta, size_t elen, size_t datalen);
extern int set_non_blocking(int fd);
extern int set_blocking(int fd);
extern int do_rename(const char *oldpath, const char *newpath);
extern char *get_tmp_filename(const char *basis);
extern char *get_checksum_str(unsigned char *checksum);
extern void add_fd_to_sets(int fd, fd_set *read_set, fd_set *write_set, fd_set *err_set, int *max_fd);
extern int init_client_socket(const char *host, const char *port);
extern void reuseaddr(int fd);
extern void write_status(char phase, const char *path, struct config *conf);
extern int run_script_to_buf(const char **args, struct strlist *userargs,
	struct cntr *cntr, int do_wait, int logfunc, char **logbuf);
extern int run_script(const char **args, struct strlist *userargs,
	struct cntr *cntr, int do_wait, int logfunc);
extern char *comp_level(struct config *conf);
extern int chuser_and_or_chgrp(struct config *conf);
extern const char *getdatestr(time_t t);
extern const char *time_taken(time_t d);
extern int dpthl_is_compressed(int compressed, const char *datapath);
#ifndef HAVE_WIN32
extern void setup_signal(int sig, void handler(int sig));
#endif
extern void cmd_to_text(char cmd, char *buf, size_t len);
extern void print_all_cmds(void);

extern void log_restore_settings(struct config *cconf, int srestore);

extern long version_to_long(const char *version);

/* These receive_a_file() and send_file() functions are for use by extra_comms
   and the CA stuff, rather than backups/restores. */
extern int receive_a_file(const char *path, struct config *conf);
extern int send_a_file(const char *path, struct config *conf);

extern int split_sig(const char *buf, unsigned int s, char *weak, char *strong);
extern int split_sig_with_save_path(const char *buf, unsigned int s, char *weak, char *strong, char *save_path);

extern int build_path_w(const char *path);

extern int astrcat(char **buf, const char *append);

extern int recursive_delete(const char *d, const char *file, uint8_t delfiles);

extern int do_quick_read(const char *datapth, struct cntr *cntr);

extern int strncmp_w(const char *s1, const char *s2);

#endif
