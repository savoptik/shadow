/*
 * SPDX-FileCopyrightText: 2009 - 2010, Nicolas François
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "config.h"

#ident "$Id:$"

#ifdef USE_PAM
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <security/pam_appl.h>
#include <security/pam_userpass.h>

#include "alloc/calloc.h"
#include "attr.h"
#include "prototypes.h"
#include "shadowlog.h"
#include "string/memset/memzero.h"

/*
 * Change non interactively the user's password using PAM.
 *
 * Return 0 on success, 1 on failure.
 */
int do_pam_passwd_non_interactive (const char *pam_service,
                                    const char *username,
                                    const char* password)
{
	pam_handle_t *pamh = NULL;
	pam_userpass_t userpass;
	struct pam_conv conv = {pam_userpass_conv, &userpass};
	int status;

	userpass.user = username;
	userpass.pass = password;

	status = pam_start (pam_service, username, &conv, &pamh);
	if (status != PAM_SUCCESS) {
		fprintf (log_get_logfd(),
		         _("%s: (user %s) pam_start failed with code %d\n"),
				 pam_service, username, status);
		return 1;
	}

	status = pam_chauthtok (pamh, 0);
	if (status != PAM_SUCCESS) {
		fprintf (log_get_logfd(),
		         _("%s: (user %s) pam_chauthtok() failed, error:\n"
		           "%s\n"),
		         pam_service, username, pam_strerror (pamh, status));
		pam_end(pamh, status);
		return 1;
	}

	status = pam_end (pamh, status);
    if (status != PAM_SUCCESS) {
		fprintf(log_get_logfd(), "%s: (user %s) pam_end failed with code %d\n", 
				pam_service, username, status);
		return 1;
	}

	return 0;
}
#else				/* !USE_PAM */
extern int ISO_C_forbids_an_empty_translation_unit;
#endif				/* !USE_PAM */
