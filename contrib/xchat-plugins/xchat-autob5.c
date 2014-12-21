#define USE_PLUGIN

#include <stdio.h>
#include "xchat.h"
#include "plugin.h"
#include "../../include/hz.h"

extern	struct module *module_find (char *name);

struct	xp_signal	chanmsg_sig;
int	(*chanmsg_next)	(void *, void *, void *, void *, void *, char);
int autob5_chanmsg (struct server *serv, char * channel, char *from, char *text, void *a, char c);

struct	xp_signal	privmsg_sig;
int	(*privmsg_next)	(void *, void *, void *, void *, void *, char);
int autob5_privmsg (struct server *serv, char * channel, char *from, char *text, void *a, char c);

char	*name = "xchat-autob5";
char	*desc = "This is a gb->big5 code convert module for Xchat!";


int	module_init (int ver, struct module *mod, struct session *sess)
{
	/* This check *MUST* be done first */
	if (ver != MODULE_IFACE_VER)
		return 1;
	
	if (module_find (name) != NULL) {
		/* We are already loaded */
		PrintText(sess, "Module xchat-autob5 already loaded\n");
		return 1;
	}
	PrintText(sess, "Loaded module xchat-autob5\n");
	mod->name = name;
	mod->desc = desc;
		
	chanmsg_sig.signal = XP_CHANMSG;
	chanmsg_sig.callback = XP_CALLBACK(autob5_chanmsg);
	chanmsg_sig.naddr = &chanmsg_next;
	chanmsg_sig.mod = mod;

	privmsg_sig.signal = XP_PRIVMSG;
	privmsg_sig.callback = XP_CALLBACK(autob5_privmsg);
	privmsg_sig.naddr = &privmsg_next;
	privmsg_sig.mod = mod;
	
	hook_signal(&chanmsg_sig);
	hook_signal(&privmsg_sig);
	gb2big_init();
	
	return 0;
}

void	module_cleanup (struct module *mod, struct session *sess)
{
	PrintText(sess, "xchat-autob5 module unloading\n");
}

int	autob5_chanmsg (struct server *serv, char *channel, char *from, char *text, void *a, char c)
{
	int len;
	int msg_code;

	char * ps;
#ifdef DEBUG
	struct session *sess;
	char msg[512];

	sess= serv->front_session;
#endif
	len=strlen(text);

#ifdef DEBUG
	snprintf(msg,510, "String: %s, Len: %d",text,len);
	PrintText(sess, msg);
#endif
	msg_code=j_code(text,len);
	switch(msg_code){
		case BIG5_CODE:
#ifdef DEBUG
			PrintText(sess," GB\n");
#endif
			break;
		case GB_CODE:
			ps=gb2big(text,&len,0);
			strncpy(text,ps,len);
#ifdef DEBUG
			PrintText(sess," Big\n");
#endif
			break;
		default:
			;
	}

	XP_CALLNEXT(chanmsg_next, serv, channel, from, text, a, c);
}

int	autob5_privmsg (struct server *serv, char *from, char *ip, char *text, void *a, char c)
{
	int len;
	int msg_code;

	char * ps;
#ifdef DEBUG
	struct session *sess;
	char msg[512];

	sess= serv->front_session;
#endif
	len=strlen(text);

#ifdef DEBUG
	snprintf(msg,510, "String: %s, Len: %d",text,len);
	PrintText(sess, msg);
#endif
	msg_code=j_code(text,len);
	switch(msg_code){
		case BIG5_CODE:
#ifdef DEBUG
			PrintText(sess," GB\n");
#endif
			break;
		case GB_CODE:
			ps=gb2big(text,&len,0);
			strncpy(text,ps,len);
#ifdef DEBUG
			PrintText(sess," Big\n");
#endif
			break;
		default:
			;
	}

	XP_CALLNEXT(privmsg_next, serv, from, ip, text, a, c);
}
