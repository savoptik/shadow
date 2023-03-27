/*
 * SPDX-FileCopyrightText: 1990 - 1994, Julianne Frances Haugh
 * SPDX-FileCopyrightText: 1996 - 2000, Marek Michałkiewicz
 * SPDX-FileCopyrightText: 2001 - 2005, Tomasz Kłoczko
 * SPDX-FileCopyrightText: 2005 - 2008, Nicolas François
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * is_valid_user_name(), is_valid_group_name() - check the new user/group
 * name for validity;
 * return values:
 *   true  - OK
 *   false - bad name
 */

#include <config.h>

#ident "$Id$"

#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include "defines.h"
#include "getdef.h"
#include "chkname.h"
#include "prototypes.h"
#include "pwio.h"
#include "groupio.h"

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

	if (valid_field (name, ":\n") != 0)
		return false;

	/*
	 * Don't allow digit at the begining of user/group names.
	 */
	if (('\0' == *name) || (('0' <= *name) && ('9' >= *name))) {
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

	return result;
}

int allow_bad_names = false;

static bool is_valid_name (const char *name)
{

	const char *name_re = get_name_regexp ();

	if (name_re)
		return is_valid_name_regexp (name, name_re);

	if (allow_bad_names) {
		return true;
	}

	/*
	 * User/group names must match [a-z_][a-z0-9_-]*[$]
	 */

	if (('\0' == *name) ||
	    !((('a' <= *name) && ('z' >= *name)) || ('_' == *name))) {
		return false;
	}

	while ('\0' != *++name) {
		if (!(( ('a' <= *name) && ('z' >= *name) ) ||
		      ( ('0' <= *name) && ('9' >= *name) ) ||
		      ('_' == *name) ||
		      ('-' == *name) ||
		      ( ('$' == *name) && ('\0' == *(name + 1)) )
		     )) {
			return false;
		}
	}

	return true;
}

static size_t min (size_t a, size_t b)
{
	return a < b ? a : b;
}

bool is_valid_user_name (const char *name)
{
	size_t max_len;
	/*
	 * User names are limited by whatever utmp can
	 * handle and the settings in login.defs.
	 */
	max_len = min (getdef_unum ("USERNAME_MAX", USER_NAME_MAX_LENGTH),
					USER_NAME_MAX_LENGTH);
	if (strlen (name) > max_len) {
		return false;
	}

	return is_valid_name (name);
}

bool is_valid_group_name (const char *name)
{
	size_t max_len;
	/*
	 * Arbitrary limit for group names.
	 */
	if (GROUP_NAME_MAX_LENGTH <= 0)
		return false;

	max_len = min (getdef_unum ("GROUPNAME_MAX", GROUP_NAME_MAX_LENGTH),
					GROUP_NAME_MAX_LENGTH);

	if (strlen (name) > max_len) {
		return false;
	}

	return is_valid_name (name);
}

