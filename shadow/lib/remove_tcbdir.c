/*
 * SPDX-FileCopyrightText: 1991 - 1994, Julianne Frances Haugh
 * SPDX-FileCopyrightText: 1996 - 2000, Marek Michałkiewicz
 * SPDX-FileCopyrightText: 2000 - 2006, Tomasz Kłoczko
 * SPDX-FileCopyrightText: 2007 - 2012, Nicolas François
 * SPDX-FileCopyrightText: 2023, Mikhail Efremov
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <tcb.h>
#include <unistd.h>

#include "config.h"

#include "defines.h"
#include "prototypes.h"
#include "getdef.h"
#include "shadowio.h"
#include "tcbfuncs.h"

#include "shadowlog_internal.h"

/* This function can't be in tcbfuncs.c because of remove_tree() usage */

bool remove_tcbdir (const char *user_name, uid_t user_id)
{
	char *buf = NULL;
	bool ret = true;
	const char *prefix_dir = get_root_prefix ();

	if (!getdef_bool ("USE_TCB")) {
		return 0;
	}

	if (asprintf(&buf, "%s" TCB_DIR "/%s", prefix_dir, user_name) < 0) {
		fprintf(shadow_logfd, "%s: Can't allocate memory, "
				"tcb entry for %s not removed.\n",
				shadow_progname, user_name);
		return false;
	}

	if (shadowtcb_drop_priv () == SHADOWTCB_FAILURE) {
		fprintf (shadow_logfd, _("%s: Cannot drop privileges: %s\n"),
		         shadow_progname, strerror (errno));
		shadowtcb_gain_priv ();
		free (buf);
		return false;
	}

	/* Only remove directory contents with dropped privileges.
	 * We will regain them and remove the user's tcb directory afterwards.
	 */
	if (remove_tree (buf, false) != 0) {
		fprintf (shadow_logfd, _("%s: Cannot remove the content of %s: %s\n"),
		         shadow_progname, buf, strerror (errno));
		shadowtcb_gain_priv ();
		free (buf);
		return false;
	}
	shadowtcb_gain_priv ();
	free (buf);
	if (shadowtcb_remove (prefix_dir, user_name) == SHADOWTCB_FAILURE) {
		fprintf (shadow_logfd, _("%s: Cannot remove tcb files for %s: %s\n"),
		         shadow_progname, user_name, strerror (errno));
		ret = false;
	}
	return ret;
}

