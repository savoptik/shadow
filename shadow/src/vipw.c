/*
  vipw, vigr  edit the password or group file
  with -s will edit shadow or gshadow file
 
  Copyright (c) 1997       , Guy Maor <maor@ece.utexas.edu>
  Copyright (c) 1999 - 2000, Marek Michałkiewicz
  Copyright (c) 2002 - 2006, Tomasz Kłoczko
  Copyright (c) 2007 - 2008, Nicolas François
  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.  */

#include <config.h>

#ident "$Id: vipw.c 3006 2009-05-25 19:51:23Z nekral-guest $"

#include <errno.h>
#include <getopt.h>
#ifdef WITH_SELINUX                                                            
#include <selinux/selinux.h>                                                   
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
#ifdef SHADOWTCB
#include <tcb.h>
#include "tcbfuncs.h"
#endif
#include "getdef.h"
#include "commonio.h"
#include "defines.h"
#include "groupio.h"
#include "nscd.h"
#include "prototypes.h"
#include "pwio.h"
#include "sgroupio.h"
#include "shadowio.h"
/*@-exitarg@*/
#include "exitcodes.h"

#define MSG_WARN_EDIT_OTHER_FILE _( \
	"You have modified %s.\n"\
	"You may need to modify %s for consistency.\n"\
	"Please use the command '%s' to do so.\n")

/*
 * Global variables
 */
static const char *progname, *filename, *fileeditname;
static bool filelocked = false;
static bool createedit = false;
static int (*unlock) (void);
static bool quiet = false;
static bool securemode = false;
static char *user = NULL;

/* local function prototypes */
static void usage (void);
static int create_backup_file (FILE *, const char *, struct stat *);
static void vipwexit (const char *msg, int syserr, int ret);
static void vipwedit (const char *, int (*)(void), int (*)(void));

/*
 * usage - display usage message and exit
 */
static void usage (void)
{
	(void) 
	fputs (_("Usage: vipw [options]\n"
	         "\n"
	         "Options:\n"
	         "  -g, --group                   edit group database\n"
	         "  -h, --help                    display this help message and exit\n"
	         "  -p, --passwd                  edit passwd database\n"
	         "  -q, --quiet                   quiet mode\n"
	         "  -s, --shadow                  edit shadow or gshadow database\n"
	         "\n"), stderr);
	exit (E_USAGE);
}

/*
 *
 */
static int create_backup_file (FILE * fp, const char *backup, struct stat *sb)
{
	struct utimbuf ub;
	FILE *bkfp;
	int c;
	mode_t mask;

	mask = umask (077);
	bkfp = fopen (backup, "w");
	(void) umask (mask);
	if (NULL == bkfp) {
		return -1;
	}

	c = 0;
	if (fseeko (fp, 0, SEEK_SET) == 0)
		while ((c = getc (fp)) != EOF) {
			if (putc (c, bkfp) == EOF) {
				break;
			}
		}
	if ((EOF != c) || (ferror (fp) != 0) || (fflush (bkfp) != 0)) {
		fclose (bkfp);
		unlink (backup);
		return -1;
	}
	if (fsync (fileno (bkfp)) != 0) {
		(void) fclose (bkfp);
		unlink (backup);
		return -1;
	}
	if (fclose (bkfp) != 0) {
		unlink (backup);
		return -1;
	}

	ub.actime = sb->st_atime;
	ub.modtime = sb->st_mtime;
	if (   (utime (backup, &ub) != 0)
	    || (chmod (backup, sb->st_mode) != 0)
	    || (chown (backup, sb->st_uid, sb->st_gid) != 0)) {
		unlink (backup);
		return -1;
	}
	return 0;
}

#ifdef SHADOWTCB
static int prep_new(char **to_rename, char *fileedit, const char *file)
{
	FILE *f;
	struct stat st;

	if (!(f = fopen(fileedit, "r"))) return 0;
	if (unlink(fileedit)) return 0;
	if (!s_drop_priv()) return 0;
	if (stat(file, &st)) return 0;
	if (asprintf(to_rename, "%s+", file) < 0) {
		fclose(f);
		return 0;
	}
	if (create_backup_file(f, *to_rename, &st)) return 0;

	return 1;
}
#endif

/*
 *
 */
static void vipwexit (const char *msg, int syserr, int ret)
{
	int err = errno;

	if (createedit) {
		if (unlink (fileeditname) != 0) {
			fprintf (stderr, _("%s: failed to remove %s\n"), progname, fileeditname);
			/* continue */
		}
	}
	if (filelocked) {
		if ((*unlock) () == 0) {
			fprintf (stderr, _("%s: failed to unlock %s\n"), progname, fileeditname);
			SYSLOG ((LOG_ERR, "failed to unlock %s", fileeditname));
			/* continue */
		}
	}
	if (NULL != msg) {
		fprintf (stderr, "%s: %s", progname, msg);
	}
	if (0 != syserr) {
		fprintf (stderr, ": %s", strerror (err));
	}
	(void) fputs ("\n", stderr);
	if (!quiet) {
		fprintf (stdout, _("%s: %s is unchanged\n"), progname,
			 filename);
	}
	exit (ret);
}

#ifndef DEFAULT_EDITOR
#define DEFAULT_EDITOR "vi"
#endif
#define SCRATCHDIR ":tmp"

/*
 *
 */
static void
vipwedit (const char *file, int (*file_lock) (void), int (*file_unlock) (void))
{
	const char *editor;
	pid_t pid;
	struct stat st1, st2;
	int status;
	FILE *f;
	char filebackup[1024], fileedit[1024];
	char *to_rename;

	snprintf (filebackup, sizeof filebackup, "%s-", file);
#ifdef SHADOWTCB
	if (securemode) {
		if (mkdir(TCB_DIR "/" SCRATCHDIR, 0700) && errno != EEXIST) {
			fprintf(stderr, "%s when trying to mkdir " TCB_DIR "/" SCRATCHDIR
					"\nare you sure you're the admin here? :^)\n", strerror(errno));
			exit(1);
		}
		snprintf(fileedit, sizeof fileedit, TCB_DIR "/" SCRATCHDIR "/.vipw.shadow.%s", user);
	} else
#endif
		snprintf (fileedit, sizeof fileedit, "%s.edit", file);
	unlock = file_unlock;
	filename = file;
	fileeditname = fileedit;
#ifdef SHADOWTCB
	if (securemode && !s_drop_priv()) {
		fprintf(stderr, "Unable to open %s\n", file);
		exit(1);
	}
#endif
	if (access (file, F_OK) != 0) {
		vipwexit (file, 1, 1);
	}
#ifdef SHADOWTCB
	if (securemode && !s_gain_priv()) {
		fprintf(stderr, "Unable to gain privs\n");
		exit(1);
	}
#endif
#ifdef WITH_SELINUX
	/* if SE Linux is enabled then set the context of all new files
	   to be the context of the file we are editing */
	if (is_selinux_enabled ()) {
		security_context_t passwd_context=NULL;
		int ret = 0;
		if (getfilecon (file, &passwd_context) < 0) {
			vipwexit (_("Couldn't get file context"), errno, 1);
		}
		ret = setfscreatecon (passwd_context);
		freecon (passwd_context);
		if (0 != ret) {
			vipwexit (_("setfscreatecon () failed"), errno, 1);
		}
	}
#endif
	if (file_lock () == 0) {
		vipwexit (_("Couldn't lock file"), errno, 5);
	}
	filelocked = true;

#ifdef SHADOWTCB
	if (securemode && !s_drop_priv()) {
		fprintf(stderr, "Unable to open %s\n", file);
		exit(1);
	}
#endif
	/* edited copy has same owners, perm */
	if (stat (file, &st1) != 0) {
		vipwexit (file, 1, 1);
	}
	f = fopen (file, "r");
	if (NULL == f) {
		vipwexit (file, 1, 1);
	}
#ifdef SHADOWTCB
	if (securemode && !s_gain_priv()) {
		fprintf(stderr, "Unable to gain privs\n");
		exit(1);
	}
#endif
	if (create_backup_file (f, fileedit, &st1) != 0) {
		vipwexit (_("Couldn't make backup"), errno, 1);
	}
	(void) fclose (f);
	createedit = true;

	editor = getenv ("VISUAL");
	if (NULL == editor) {
		editor = getenv ("EDITOR");
	}
	if (NULL == editor) {
		editor = DEFAULT_EDITOR;
	}

	pid = fork ();
	if (-1 == pid) {
		vipwexit ("fork", 1, 1);
	} else if (0 == pid) {
		/* use the system() call to invoke the editor so that it accepts
		   command line args in the EDITOR and VISUAL environment vars */
		char *buf;

		buf = (char *) malloc (strlen (editor) + strlen (fileedit) + 2);
		snprintf (buf, strlen (editor) + strlen (fileedit) + 2,
			  "%s %s", editor, fileedit);
		if (system (buf) != 0) {
			fprintf (stderr, "%s: %s: %s\n", progname, editor,
				 strerror (errno));
			exit (1);
		} else {
			exit (0);
		}
	}

	for (;;) {
		pid = waitpid (pid, &status, WUNTRACED);
		if ((pid != -1) && (WIFSTOPPED (status) != 0)) {
			/* The child (editor) was suspended.
			 * Suspend vipw. */
			kill (getpid (), WSTOPSIG(status));
			/* wake child when resumed */
			kill (pid, SIGCONT);
		} else {
			break;
		}
	}

	if (   (-1 == pid)
	    || (WIFEXITED (status) == 0)
	    || (WEXITSTATUS (status) != 0)) {
		vipwexit (editor, 1, 1);
	}

	if (stat (fileedit, &st2) != 0) {
		vipwexit (fileedit, 1, 1);
	}
	if (st1.st_mtime == st2.st_mtime) {
		vipwexit (0, 0, 0);
	}
#ifdef WITH_SELINUX                                                            
	/* unset the fscreatecon */                                             
	if (is_selinux_enabled ()) {
		if (setfscreatecon (NULL)) {
			vipwexit (_("setfscreatecon () failed"), errno, 1);
		}
	}
#endif

	/*
	 * XXX - here we should check fileedit for errors; if there are any,
	 * ask the user what to do (edit again, save changes anyway, or quit
	 * without saving). Use pwck or grpck to do the check.  --marekm
	 */
	createedit = false;
#ifdef SHADOWTCB
	if (securemode) {
		if (!prep_new(&to_rename, fileedit, file)) {
			fprintf(stderr, _("%s: can't restore %s: %s (your changes are in %s)\n"),
					progname, file, strerror(errno), fileedit);
			vipwexit(0,0,1);
		}
	} else
#endif
		to_rename = fileedit;
	unlink (filebackup);
	link (file, filebackup);
	if (rename (to_rename, file) == -1) {
		fprintf (stderr,
		         _("%s: can't restore %s: %s (your changes are in %s)\n"),
		         progname, file, strerror (errno), fileedit);
		vipwexit (0, 0, 1);
	}
#ifdef SHADOWTCB
	if (securemode && !s_gain_priv()) {
		fprintf(stderr, "Unable to gain privs\n");
		exit(1);
	}
#endif
	if ((*file_unlock) () == 0) {
		fprintf (stderr, _("%s: failed to unlock %s\n"), progname, fileeditname);
		SYSLOG ((LOG_ERR, "failed to unlock %s", fileeditname));
		/* continue */
	}
	SYSLOG ((LOG_INFO, "file %s edited", fileeditname));
}

int main (int argc, char **argv)
{
	bool editshadow = false;
	char *a;
	bool do_vipw;

	(void) setlocale (LC_ALL, "");
	(void) bindtextdomain (PACKAGE, LOCALEDIR);
	(void) textdomain (PACKAGE);

	progname = ((a = strrchr (*argv, '/')) ? a + 1 : *argv);
	do_vipw = (strcmp (progname, "vigr") != 0);

	OPENLOG (do_vipw ? "vipw" : "vigr");

	{
		/*
		 * Parse the command line options.
		 */
		int c;
		static struct option long_options[] = {
			{"group", no_argument, NULL, 'g'},
			{"help", no_argument, NULL, 'h'},
			{"passwd", no_argument, NULL, 'p'},
			{"quiet", no_argument, NULL, 'q'},
			{"shadow", no_argument, NULL, 's'},
			{NULL, 0, NULL, '\0'}
		};
		while ((c =
			getopt_long (argc, argv, "ghpqs",
				     long_options, NULL)) != -1) {
			switch (c) {
			case 'g':
				do_vipw = false;
				break;
			case 'h':
				usage ();
				break;
			case 'p':
				do_vipw = true;
				break;
			case 'q':
				quiet = true;
				break;
			case 's':
				editshadow = true;
				break;
			default:
				usage ();
			}
		}
	}

#ifdef SHADOWTCB
	if (do_vipw && editshadow && getdef_bool("USE_TCB")) {
		securemode = true;
		user = argv[optind];
		if (!user) {
			usage();
		}
		if (!tcb_user(user))
			exit(1);
	}
#endif

	if (do_vipw) {
		if (editshadow) {
			vipwedit (spw_dbname(), spw_lock, spw_unlock);
			printf (MSG_WARN_EDIT_OTHER_FILE,
			        spw_dbname(),
			        PASSWD_FILE,
			        "vipw");
		} else {
			vipwedit (PASSWD_FILE, pw_lock, pw_unlock);
			if (spw_file_present ()) {
				printf (MSG_WARN_EDIT_OTHER_FILE,
				        PASSWD_FILE,
#ifdef SHADOWTCB
				        "/etc/tcb/*/shadow",
#else
				        SHADOW_FILE,
#endif
				        "vipw -s");
			}
		}
	} else {
#ifdef SHADOWGRP
		if (editshadow) {
			vipwedit (SGROUP_FILE, sgr_lock, sgr_unlock);
			printf (MSG_WARN_EDIT_OTHER_FILE,
			        SGROUP_FILE,
			        GROUP_FILE,
			        "vigr");
		} else {
#endif
			vipwedit (GROUP_FILE, gr_lock, gr_unlock);
#ifdef SHADOWGRP
			if (sgr_file_present ()) {
				printf (MSG_WARN_EDIT_OTHER_FILE,
				        GROUP_FILE,
				        SGROUP_FILE,
				        "vigr -s");
			}
		}
#endif
	}

	nscd_flush_cache ("passwd");
	nscd_flush_cache ("group");

	return E_SUCCESS;
}

