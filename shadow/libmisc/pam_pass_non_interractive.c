/*
 * Copyright (c)        2009, Nicolas François
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the copyright holders or contributors may not be used to
 *    endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>

#ident "$Id:$"

#ifdef USE_PAM
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <security/pam_appl.h>
#include <security/pam_userpass.h>
#include "prototypes.h"

/*
 * Change non interactively the user's password using PAM.
 *
 * Return 0 on success, 1 on failure.
 */
int do_pam_passwd_non_interractive (const char *pam_service,
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
		fprintf (stderr,
		         _("%s: (user %s) pam_start failed with code %d\n"),
				 pam_service, username, status);
		return 1;
	}

	status = pam_chauthtok (pamh, 0);
	if (status != PAM_SUCCESS) {
		fprintf (stderr,
		         _("%s: (user %s) pam_chauthtok() failed, error:\n"
		           "%s\n"),
		         pam_service, username, pam_strerror (pamh, status));
		pam_end(pamh, status);
		return 1;
	}

	status = pam_end (pamh, status);
    if (status != PAM_SUCCESS) {
		fprintf(stderr, "%s: (user %s) pam_end failed with code %d\n", 
				pam_service, username, status);
		return 1;
	}

	return 0;
}
#else				/* !USE_PAM */
extern int errno;		/* warning: ANSI C forbids an empty source file */
#endif				/* !USE_PAM */
