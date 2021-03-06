#include "include.h"

#include "../../server/backup_phase1.h"

// Used on resume, this just reads the phase1 file and sets up the counters.
static int read_phase1(gzFile zp, struct config *conf)
{
	int ars=0;
	struct sbuf *p1b;
	if(!(p1b=sbuf_alloc(conf))) return -1;
	while(1)
	{
		sbuf_free_contents(p1b);
		if((ars=sbufl_fill_phase1(NULL, zp, p1b, conf->p1cntr)))
		{
			// ars==1 means it ended ok.
			if(ars<0)
			{
				sbuf_free(p1b);
				return -1;
			}
			return 0;
		}
		do_filecounter(conf->p1cntr, p1b->path.cmd, 0);

		if(p1b->path.cmd==CMD_FILE
		  || p1b->path.cmd==CMD_ENC_FILE
		  || p1b->path.cmd==CMD_METADATA
		  || p1b->path.cmd==CMD_ENC_METADATA
		  || p1b->path.cmd==CMD_EFS_FILE)
			do_filecounter_bytes(conf->p1cntr,
				(unsigned long long)p1b->statp.st_size);
	}
	sbuf_free(p1b);
	// not reached
	return 0;
}

static int do_forward(FILE *fp, gzFile zp, struct iobuf *result,
	struct iobuf *target, int isphase1, int seekback, int do_counters,
	int same, struct dpthl *dpthl, struct config *cconf)
{
	int ars=0;
	off_t pos=0;
	static struct sbuf *sb=NULL;

	if(!sb && !(sb=sbuf_alloc(cconf)))
		goto error;

	while(1)
	{
		// If told to 'seekback' to the immediately previous
		// entry, we need to remember the position of it.
		if(target && fp && seekback && (pos=ftello(fp))<0)
		{
			logp("Could not ftello in %s(): %s\n", __FUNCTION__,
				strerror(errno));
			goto error;
		}

		sbuf_free_contents(sb);

		if(isphase1)
			ars=sbufl_fill_phase1(fp, zp, sb, cconf->cntr);
		else
			ars=sbufl_fill(fp, zp, sb, cconf->cntr);

		// Make sure we end up with the highest datapth we can possibly
		// find - set_dpthl_from_string() will only set it if it is
		// higher.
		if(sb->burp1->datapth.buf
		  && set_dpthl_from_string(dpthl,
			sb->burp1->datapth.buf, cconf))
		{
			logp("unable to set datapath: %s\n",
				sb->burp1->datapth.buf);
			goto error;
		}

		if(ars)
		{
			// ars==1 means it ended ok.
			if(ars<0)
			{
				if(result->buf)
				{
					logp("Error after %s in %s()\n",
						result->buf, __FUNCTION__);
				}
				goto error;
			}
			return 0;
		}

		// If seeking to a particular point...
		if(target->buf && iobuf_pathcmp(target, &sb->path)<=0)
		{
			// If told to 'seekback' to the immediately previous
			// entry, do it here.
			if(fp && seekback && fseeko(fp, pos, SEEK_SET))
			{
				logp("Could not fseeko in %s(): %s\n",
					__FUNCTION__, strerror(errno));
				goto error;
			}
			return 0;
		}

		if(do_counters)
		{
			if(same) do_filecounter_same(cconf->cntr, sb->path.cmd);
			else do_filecounter_changed(cconf->cntr, sb->path.cmd);
			if(sb->burp1->endfile.buf)
			{
				unsigned long long e=0;
				e=strtoull(sb->burp1->endfile.buf, NULL, 10);
				do_filecounter_bytes(cconf->cntr, e);
				do_filecounter_recvbytes(cconf->cntr, e);
			}
		}

		iobuf_free_content(result);
		iobuf_copy(result, &sb->path);
		sb->path.buf=NULL;
	}

error:
	sbuf_free_contents(sb);
	return -1;
}

static int forward_fp(FILE *fp, struct iobuf *result, struct iobuf *target,
	int isphase1, int seekback, int do_counters, int same,
	struct dpthl *dpthl, struct config *cconf)
{
	return do_forward(fp, NULL, result, target, isphase1, seekback,
		do_counters, same, dpthl, cconf);
}

static int forward_zp(gzFile zp, struct iobuf *result, struct iobuf *target,
	int isphase1, int seekback, int do_counters, int same,
	struct dpthl *dpthl, struct config *cconf)
{
	return do_forward(NULL, zp, result, target, isphase1, seekback,
		do_counters, same, dpthl, cconf);
}

int do_resume(gzFile p1zp, FILE *p2fp, FILE *ucfp, struct dpthl *dpthl, struct config *cconf)
{
	int ret=0;
	struct iobuf *p1b=NULL;
	struct iobuf *p2b=NULL;
	struct iobuf *p2btmp=NULL;
	struct iobuf *ucb=NULL;

	if(!(p1b=iobuf_alloc())
	  || !(p2b=iobuf_alloc())
	  || !(p2btmp=iobuf_alloc())
	  || !(ucb=iobuf_alloc()))
		return -1;

	logp("Begin phase1 (read previous file system scan)\n");
	if(read_phase1(p1zp, cconf)) goto error;

	gzrewind(p1zp);

	logp("Setting up resume positions...\n");
	// Go to the end of p2fp.
	if(forward_fp(p2fp, p2btmp, NULL,
		0, /* not phase1 */
		0, /* no seekback */
		0, /* no counters */
		0, /* changed */
		dpthl, cconf)) goto error;
	rewind(p2fp);
	// Go to the beginning of p2fp and seek forward to the p2btmp entry.
	// This is to guard against a partially written entry at the end of
	// p2fp, which will get overwritten.
	if(forward_fp(p2fp, p2b, p2btmp,
		0, /* not phase1 */
		0, /* no seekback */
		1, /* do_counters */
		0, /* changed */
		dpthl, cconf)) goto error;
	logp("  phase2:    %s\n", p2b->buf);

	// Now need to go to the appropriate places in p1zp and unchanged.
	// The unchanged file needs to be positioned just before the found
	// entry, otherwise it ends up having a duplicated entry.
	if(forward_zp(p1zp, p1b, p2b,
		1, /* phase1 */
		0, /* no seekback */
		0, /* no counters */
		0, /* ignored */
		dpthl, cconf)) goto error;
	logp("  phase1:    %s\n", p1b->buf);

	if(forward_fp(ucfp, ucb, p2b,
		0, /* not phase1 */
		1, /* seekback */
		1, /* do_counters */
		1, /* same */
		dpthl, cconf)) goto error;
	logp("  unchanged: %s\n", ucb->buf);

	// Now should have all file pointers in the right places to resume.
	if(incr_dpthl(dpthl, cconf)) goto error;

	if(cconf->send_client_counters)
	{
		if(send_counters(cconf)) goto error;
	}

	goto end;
error:
	ret=-1;
end:
	iobuf_free(p1b);
	iobuf_free(p2b);
	iobuf_free(p2btmp);
	iobuf_free(ucb);
	logp("End phase1 (read previous file system scan)\n");
	return ret;
}
