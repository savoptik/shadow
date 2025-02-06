// SPDX-FileCopyrightText: 1990-1994, Julianne Frances Haugh
// SPDX-FileCopyrightText: 1996-2000, Marek Michałkiewicz
// SPDX-FileCopyrightText: 2001-2005, Tomasz Kłoczko
// SPDX-FileCopyrightText: 2005-2008, Nicolas François
// SPDX-FileCopyrightText: 2023-2025, Alejandro Colomar <alx@kernel.org>
// SPDX-License-Identifier: BSD-3-Clause


/*
 * is_valid_user_name(), is_valid_group_name() - check the new user/group
 * name for validity;
 * return values:
 *   true  - OK
 *   false - bad name
 * errors:
 *   EINVAL	Invalid name characters or sequences
 *   EOVERFLOW	Name longer than maximum size
 */


#include <config.h>

#ident "$Id$"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>

#include "defines.h"
#include "getdef.h"
#include "chkname.h"
#include "string/strcmp/streq.h"
#include "prototypes.h"
#include "pwio.h"
#include "groupio.h"


#ifndef  LOGIN_NAME_MAX
# define LOGIN_NAME_MAX  256
#endif

#define IS_UNIQ_NAME(db_type, db_pref, name) \
       const struct db_type *db_pref; \
       (void) db_pref##_rewind (); \
       while ((db_pref = db_pref##_next ()) != NULL) { \
               if (strcasecmp (name, db_pref->db_pref##_name) == 0) \
                       return false; \
       } \
       return true;

bool is_uniq_user (const char *name)
{
       IS_UNIQ_NAME (passwd, pw, name);
}

bool is_uniq_group (const char *name)
{
       IS_UNIQ_NAME (group, gr, name);
}


static size_t min (size_t a, size_t b)
{
	return a < b ? a : b;
}


size_t
login_name_max_size(void)
{
	long  conf;
	size_t  maxsize;

	conf = sysconf(_SC_LOGIN_NAME_MAX);

	if (conf == -1)
		maxsize = LOGIN_NAME_MAX;
	else
		maxsize = conf;

	/*
	 * The _SC_LOGIN_NAME_MAX value includes space for the NUL byte,
	 * so we must subtract 1 from it.
	 * Decreasing LOGIN_NAME_MAX by 1 is a good thing as well.
	 */
	maxsize--;

	return min(getdef_unum ("USERNAME_MAX", maxsize),
					maxsize);
}


const char *get_name_regexp (void)
{
	const char *name_re = getdef_str("REGEXP_NAME");

	if (!name_re || name_re[0] == '\0')
		return NULL;

	return name_re;
}

static bool is_valid_name_regexp (const char *name, const char *regexp)
{
	regex_t preg;
	int errcode;
	bool result = false;

	if (valid_field (name, ":\n") != 0) {
		errno = EINVAL;
		return false;
	}

	/*
	 * Don't allow digit at the begining of user/group names.
	 */
	if (('\0' == *name) || (('0' <= *name) && ('9' >= *name))) {
		errno = EINVAL;
		return false;
	}

	errcode = regcomp(&preg, regexp, REG_NOSUB | REG_NEWLINE);
	if (errcode) {
		char errbuf[64];
		regerror(errcode, &preg, errbuf, sizeof (errbuf));
		fprintf (stderr, "regexp error: %s\n", errbuf);
		goto out;
	}

	if (regexec(&preg, name, 0, NULL, 0) == 0)
		result = true;

out:
	regfree (&preg);
	if (!result)
		errno = EINVAL;

	return result;
}

static bool
is_valid_name(const char *name)
{

	const char *name_re = get_name_regexp ();

	if (name_re)
		return is_valid_name_regexp (name, name_re);

	/*
	 * User/group names must match [a-z_][a-z0-9_-]*[$]
	 */

	if (('\0' == *name) ||
	    !((('a' <= *name) && ('z' >= *name)) || ('_' == *name))) {
		errno = EINVAL;
		return false;
	}

	while ('\0' != *++name) {
		if (!(( ('a' <= *name) && ('z' >= *name) ) ||
		      ( ('0' <= *name) && ('9' >= *name) ) ||
		      ('_' == *name) ||
		      ('-' == *name) ||
		      ( ('$' == *name) && ('\0' == *(name + 1)) )
		     ))
		{
			errno = EINVAL;
			return false;
		}
	}

	return true;
}


bool
is_valid_user_name(const char *name)
{
	if (strlen(name) > login_name_max_size()) {
		errno = EOVERFLOW;
		return false;
	}

	return is_valid_name(name);
}


bool
is_valid_group_name(const char *name)
{
	size_t max_len;
	/*
	 * Arbitrary limit for group names.
	 */
	if (GROUP_NAME_MAX_LENGTH <= 0) {
		errno = EINVAL;
		return false;
	}

	max_len = min (getdef_unum ("GROUPNAME_MAX", GROUP_NAME_MAX_LENGTH),
					GROUP_NAME_MAX_LENGTH);

	if (strlen (name) > max_len) {
		errno = EOVERFLOW;
		return false;
	}

	return is_valid_name (name);
}
