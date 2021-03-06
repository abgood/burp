#ifndef _CURRENT_BACKUPS_H
#define _CURRENT_BACKUPS_H

struct bu
{
	char *path;
	char *basename;
	char *data;
	char *delta;
	char *timestamp;
	char *forward_timestamp;
	unsigned long index;
	unsigned long forward_index;
	int hardlinked;
	int deletable;
	// transposed index - will set the oldest backup to 1.
	unsigned long trindex;
};

extern int recursive_hardlink(const char *src, const char *dst,
	struct config *conf);
extern void free_current_backups(struct bu **arr, int a);
extern int get_current_backups(struct sdirs *sdirs,
	struct bu **arr, int *a, int log);
extern int get_current_backups_str(const char *dir,
	struct bu **arr, int *a, int log);
extern int get_new_timestamp(struct sdirs *sdirs, struct config *cconf,
	char *buf, size_t s);
extern int read_timestamp(const char *path, char buf[], size_t len);
extern int write_timestamp(const char *timestamp, const char *tstmp);
extern int compress_file(const char *current, const char *file,
	struct config *cconf);
extern int compress_filename(const char *d, const char *file,
	const char *zfile, struct config *cconf);
extern int remove_old_backups(struct sdirs *sdirs, struct config *cconf);
extern int do_link(const char *oldpath, const char *newpath,
	struct stat *statp, struct config *conf, uint8_t overwrite);
extern int delete_backup(struct sdirs *sdirs, struct config *cconf,
	struct bu *arr, int a, int b);

#endif
