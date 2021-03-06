#ifndef __SDIRS_H
#define __SDIRS_H

// Server directories.
struct sdirs
{
	char *base;
	char *dedup;
	char *data;
	char *clients;
	char *client;

	char *working;
	char *finishing;
	char *current;

	char *timestamp;
	char *changed;
	char *unchanged;
	char *cmanifest;
	char *phase1data;

	char *lock;
	char *lockfile;
	uint8_t gotlock;

	// Legacy directories.
	char *currentdata;
	char *manifest;
	char *datadirtmp;
	char *phase2data;
	char *unchangeddata;
	char *cincexc;
	char *deltmppath;
};

extern struct sdirs *sdirs_alloc(void);
extern int sdirs_init(struct sdirs *sdirs, struct config *conf);
extern void sdirs_free(struct sdirs *sdirs);

#endif
