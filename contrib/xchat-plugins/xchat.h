#ifndef SKIPCONFIG
#include "../../config.h"
#endif

#ifndef PACKAGE				/* fixes prob compiling on RH5.2, *shrug* */
#define PACKAGE "xchat"
#endif
#ifndef VERSION
#define VERSION "1.5.3"
#endif

#ifdef USE_MYGLIB
#include "../fe-text/myglib.h"
#else
#include <glib.h>
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "history.h"

#ifndef HAVE_SNPRINTF
#define snprintf g_snprintf
#endif

#ifdef USE_DEBUG
#define malloc xchat_malloc
#define realloc xchat_realloc
#define free xchat_free
#define strdup xchat_strdup
#endif

#ifdef SOCKS
#ifdef __sgi
#include <sys/time.h>
#define INCLUDE_PROTOTYPES 1
#endif
#include <socks.h>
#endif

#ifdef USE_OPENSSL
#include <openssl/ssl.h>		  /* SSL_() */
#endif

#ifdef __EMX__						  /* for o/s 2 */
#define OFLAGS O_BINARY
#else
#define OFLAGS 0
#endif

#define FONTNAMELEN		127
#define PATHLEN			255
#define NICKLEN			64				/* including the NULL, so 63 really */
#define CHANLEN			202

#define safe_strcpy(dest,src,len)	strncpy(dest,src,len); \
												dest[len-1] = 0;

struct nbexec
{
	int myfd, childfd;
	int childpid;
	int tochannel;					  /* making this int keeps the struct 4-byte aligned */
	int iotag;
	struct session *sess;
};

struct xchatprefs
{
	char nick1[64];
	char nick2[64];
	char nick3[64];
	char realname[127];
	char username[127];
	char nick_suffix[4];			/* Only ever holds a one-character string. */
	char awayreason[256];
	char quitreason[256];
	char font_normal[FONTNAMELEN + 1];
	char dialog_font_normal[FONTNAMELEN + 1];
	char doubleclickuser[256];
	char sounddir[PATHLEN + 1];
	char soundcmd[PATHLEN + 1];
	char background[PATHLEN + 1];
	char background_dialog[PATHLEN + 1];
	char dccdir[PATHLEN + 1];
	char bluestring[64];
	char dnsprogram[72];
	char hostname[127];
	char trans_file[256];
	char logmask[256];

	char proxy_host[64];
	int proxy_port;
	int proxy_type;		/* 0 = disabled, 1 = socks4, 3 = wingate */

	int first_dcc_send_port;
	int last_dcc_send_port;

	int tint_red;
	int tint_green;
	int tint_blue;

	int dialog_tint_red;
	int dialog_tint_green;
	int dialog_tint_blue;

	int tabs_position;
	int max_auto_indent;
	int indent_pixels;
	int dialog_indent_pixels;
	int dcc_blocksize;
	int max_lines;
	int notify_timeout;
	int dcctimeout;
	int dccstalltimeout;
	int mainwindow_left;
	int mainwindow_top;
	int mainwindow_width;
	int mainwindow_height;
	int dccpermissions;
	int recon_delay;
	int bantype;
	int userlist_sort;
	int nu_color;
	int bt_color;
	unsigned long local_ip;
	unsigned long dcc_ip;

	unsigned int use_trans;
	unsigned int nopanel;
	unsigned int autosave;
	unsigned int autodialog;
	unsigned int autosave_url;
	unsigned int autoreconnect;
	unsigned int autoreconnectonfail;
	unsigned int invisible;
	unsigned int servernotice;
	unsigned int wallops;
	unsigned int skipmotd;
	unsigned int away;
	unsigned int autorejoin;
	unsigned int colorednicks;
	unsigned int nochanmodebuttons;
	unsigned int nouserlistbuttons;
   unsigned int showhostname_in_userlist;
	unsigned int nickcompletion;
	unsigned int old_nickcompletion;
	unsigned int tabchannels;
	unsigned int nopaned;
	unsigned int autodccchat;
	unsigned int autodccsend;
	unsigned int autoresume;
	unsigned int noautoopendccsendwindow;
	unsigned int noautoopendccrecvwindow;
	unsigned int noautoopendccchatwindow;
	unsigned int transparent;
	unsigned int tint;
	unsigned int dialog_transparent;
	unsigned int dialog_tint;
	unsigned int stripcolor;
	unsigned int timestamp;
	unsigned int fastdccsend;
	unsigned int skipserverlist;
	unsigned int filterbeep;
	unsigned int beepmsg;
	unsigned int privmsgtab;
	unsigned int logging;
	unsigned int newtabstofront;
	unsigned int dccwithnick;
	unsigned int hilitenotify;
	unsigned int hidever;
	unsigned int ip_from_server;
	unsigned int panelize_hide;
	unsigned int panel_vbox;
	unsigned int raw_modes;
	unsigned int no_server_logs;
	unsigned int show_away_once;
	unsigned int show_away_message;
	unsigned int nouserhost;
	unsigned int use_server_tab;
	unsigned int style_namelistgad;
	unsigned int style_inputbox;
	unsigned int windows_as_tabs;
	unsigned int use_fontset;
	unsigned int indent_nicks;
	unsigned int show_separator;
	unsigned int thin_separator;
	unsigned int dialog_indent_nicks;
	unsigned int dialog_show_separator;
	unsigned int treeview;
	unsigned int auto_indent;
	unsigned int wordwrap;
	unsigned int dialog_wordwrap;
	unsigned int mail_check;
	unsigned int double_buffer;
	unsigned int throttle;
	unsigned int fudgeservernotice;
	unsigned int topicbar;
	unsigned int hideuserlist;
	unsigned int hidemenu;
	unsigned int perlwarnings;
	unsigned int lagometer;
	unsigned int hebrew;
	unsigned int limitedtabhighlight;
	unsigned int pingtimeout;
	unsigned int show_invite_in_front_session;
	unsigned int show_notify_in_front_session;
	unsigned int whois_on_notifyonline;
	unsigned int inputgad_superfocus;
	unsigned int nickgad;
	unsigned int channelbox;

	unsigned int ctcp_number_limit;	/*flood */
	unsigned int ctcp_time_limit;	/*seconds of floods */

	unsigned int msg_number_limit;	/*same deal */
	unsigned int msg_time_limit;

};

struct session
{
	struct server *server;
	GSList *userlist;
	char channel[CHANLEN];
	char waitchannel[CHANLEN];		  /* waiting to join this channel */
	char willjoinchannel[CHANLEN];	  /* /join done for this channel */
	char channelkey[64];			  /* XXX correct max length? */
	int limit;						  /* channel user limit */
	int channelflags;				  /* channel flags (bitmap) */
	int logfd;

	char lastnick[NICKLEN];			  /* last nick you /msg'ed */

	struct history history;

	int ops;								/* num. of ops in channel */
	int hops;						  /* num. of half-oped users */
	int voices;							/* num. of voiced people */
	int total;							/* num. of users in channel */

	char *quitreason;

	struct session *lastlog_sess;
	struct setup *setup;
	struct nbexec *running_exec;

	struct session_gui *gui;		/* initialized by fe_new_window */

	int userlisthidden;
	int is_shell:1;
	int is_tab:1;			/* is this a tab or toplevel window? */
	int is_dialog:1;
	int is_server:1;		/* for use_server_tab feature */
	int new_data:1;		/* new data avail? (red tab) */
	int nick_said:1;		/* your nick mentioned? (green tab) */
	int ignore_date:1;
	int ignore_mode:1;
	int ignore_names:1;
	int end_of_names:1;
	int doing_who:1;		/* /who sent on this channel */
 	int highlight_tab:1;	/* Highlight the channel tab (red-style) */
};

typedef struct session session;

struct server
{
	int port;
	int sok;
#ifdef USE_OPENSSL
	SSL *ssl;
	int ssl_do_connect_tag;
	int use_ssl:1;					  /* is server SSL capable? */
	int accept_invalid_cert:1;	  /* ignore result of server's cert. verify */
#endif
	int childread;
	int childwrite;
	int childpid;
	int iotag;
	int bartag;
	int recondelay_tag;				/* reconnect delay timeout */
	char hostname[128];				/* real ip number */
	char servername[128];			/* what the server says is its name */
	char password[86];
	char nick[NICKLEN];
	char linebuf[522];				/* RFC says 512 including \r\n */
	int pos;
	int nickcount;

	GSList *outbound_queue;
	time_t last_send;
	int bytes_sent;				  /* this second only */

	struct session *front_session;

	struct server_gui *gui;		  /* initialized by fe_new_server */

	int ctcp_counter;				  /*flood */
	time_t ctcp_last_time;

	int msg_counter;				  /*counts the msg tab opened in a certain time */
	time_t msg_last_time;

	/*time_t connect_time;*/				/* when did it connect? */
	time_t lag_sent;
	time_t ping_recv;					/* when we last got a ping reply */

	int motd_skipped:1;
	int connected:1;
	int connecting:1;
	int no_login:1;
	int skip_next_who:1;			  /* used for "get my ip from server" */
	int inside_whois:1;
	int doing_who:1;				  /* /dns has been done */
	int end_of_motd:1;			  /* end of motd reached (logged in) */
	int sent_quit:1;				  /* sent a QUIT already? */
	int is_newtype:1;		/* undernet and dalnet need /list >0,<10000 */
	int six_modes:1;		/* undernet allows 6 modes per line */
};

typedef struct server server;

typedef int (*cmd_callback) (struct session * sess, char *tbuf, char *word[],
									  char *word_eol[]);

struct commands
{
	char *name;
	cmd_callback callback;
	char needserver;
	char needchannel;
	char *help;
};

struct away_msg
{
	struct server *server;
	char nick[NICKLEN];
	char *message;
};
