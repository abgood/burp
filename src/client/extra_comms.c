#include "include.h"

#ifndef HAVE_WIN32
#include <sys/utsname.h>
#endif

static const char *server_supports(const char *feat, const char *wanted)
{
	return strstr(feat, wanted);
}

static const char *server_supports_autoupgrade(const char *feat)
{
	// 1.3.0 servers did not list the features, but the only feature
	// that was supported was autoupgrade.
	if(!strcmp(feat, "extra_comms_begin ok")) return "ok";
	return server_supports(feat, ":autoupgrade:");
}

int extra_comms(struct config *conf,
	enum action *action, char **incexc, long *name_max)
{
	int ret=-1;
	char *feat=NULL;
	const char *cp=NULL;
	struct iobuf *rbuf=NULL;

	if(!(rbuf=iobuf_alloc())) goto end;

	if(async_write_str(CMD_GEN, "extra_comms_begin"))
	{
		logp("Problem requesting extra_comms_begin\n");
		goto end;
	}
	// Servers greater than 1.3.0 will list the extra_comms
	// features they support.
	if(async_read(rbuf))
	{
		logp("Problem reading response to extra_comms_begin\n");
		goto end;
	}
	feat=rbuf->buf;
	if(rbuf->cmd!=CMD_GEN
	  || strncmp(feat,
		"extra_comms_begin ok", strlen("extra_comms_begin ok")))
	{
		iobuf_log_unexpected(rbuf, __FUNCTION__);
		goto end;
	}

	// Can add extra bits here. The first extra bit is the
	// autoupgrade stuff.
	if(server_supports_autoupgrade(rbuf->buf)
	  && conf->autoupgrade_dir
	  && conf->autoupgrade_os
	  && (ret=autoupgrade_client(conf)))
		goto end;

	// :srestore: means that the server wants to do a restore.
	if(server_supports(feat, ":srestore:"))
	{
		if(conf->server_can_restore)
		{
			logp("Server is initiating a restore\n");
			if((ret=incexc_recv_client_restore(incexc, conf)))
				goto end;
			if(*incexc)
			{
				if((ret=parse_incexcs_buf(conf, *incexc)))
					goto end;
				*action=ACTION_RESTORE;
				log_restore_settings(conf, 1);
			}
		}
		else
		{
			logp("Server wants to initiate a restore\n");
			logp("Client configuration says no\n");
			if(async_write_str(CMD_GEN, "srestore not ok"))
				goto end;
		}
	}

	if((cp=server_supports(feat, ":name_max=")))
		*name_max=strtol(cp+strlen(":name_max="), NULL, 10);

	if(conf->orig_client)
	{
		char str[512]="";
		snprintf(str, sizeof(str), "orig_client=%s", conf->orig_client);
		if(!server_supports(feat, ":orig_client:"))
		{
			logp("Server does not support switching client.\n");
			goto end;
		}
		if((ret=async_write_str(CMD_GEN, str))
		  || (ret=async_read_expect(CMD_GEN, "orig_client ok")))
		{
			logp("Problem requesting %s\n", str);
			goto end;
		}
		logp("Switched to client %s\n", conf->orig_client);
	}

	// :sincexc: is for the server giving the client the
	// incexc config.
	if(*action==ACTION_BACKUP
	  || *action==ACTION_BACKUP_TIMED
	  || *action==ACTION_TIMER_CHECK)
	{
		if(!incexc && server_supports(feat, ":sincexc:"))
		{
			logp("Server is setting includes/excludes.\n");
			if((ret=incexc_recv_client(incexc, conf)))
				goto end;
			if(*incexc && (ret=parse_incexcs_buf(conf,
				*incexc))) goto end;
		}
	}

	if(server_supports(feat, ":counters:"))
	{
		if(async_write_str(CMD_GEN, "countersok"))
			goto end;
		conf->send_client_counters=1;
	}

	// :incexc: is for the client sending the server the
	// incexc config so that it better knows what to do on
	// resume.
	if(server_supports(feat, ":incexc:")
	  && (ret=incexc_send_client(conf)))
		goto end;

	if(server_supports(feat, ":uname:"))
	{
		const char *clientos=NULL;
#ifdef HAVE_WIN32
#ifdef _WIN64
		clientos="Windows 64bit";
#else
		clientos="Windows 32bit";
#endif
#else
		struct utsname utsname;
		if(!uname(&utsname))
			clientos=(const char *)utsname.sysname;
#endif
		if(clientos)
		{
			char msg[128]="";
			snprintf(msg, sizeof(msg),
				"uname=%s", clientos);
			if(async_write_str(CMD_GEN, msg))
				goto end;
		}
	}

	if((ret=async_write_str(CMD_GEN, "extra_comms_end"))
	  || (ret=async_read_expect(CMD_GEN, "extra_comms_end ok")))
	{
		logp("Problem requesting extra_comms_end\n");
		goto end;
	}

end:
	iobuf_free(rbuf);
	return ret;
}
