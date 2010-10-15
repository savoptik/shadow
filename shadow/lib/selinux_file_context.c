#include <config.h>

#include "prototypes.h"
#ifdef WITH_SELINUX
#include <selinux/selinux.h>

/*
 * selinux_file_context - Set the security context before any file or
 *                        directory creation.
 *
 *	selinux_file_context () should be called before any creation of file,
 *	symlink, directory, ...
 *
 *	Callers may have to Reset SELinux to create files with default
 *	contexts:
 *		setfscreatecon (NULL);
 */
int selinux_file_context (const char *dst_name)
{
	static bool selinux_checked = false;
	static bool selinux_enabled;
	security_context_t scontext = NULL;

	if (!selinux_checked) {
		selinux_enabled = is_selinux_enabled () > 0;
		selinux_checked = true;
	}

	if (selinux_enabled) {
		/* Get the default security context for this file */
		if (matchpathcon (dst_name, 0, &scontext) < 0) {
			if (security_getenforce () != 0) {
				return 1;
			}
		}
		/* Set the security context for the next created file */
		if (setfscreatecon (scontext) < 0) {
			if (security_getenforce () != 0) {
				return 1;
			}
		}
		freecon (scontext);
	}
	return 0;
}
#endif
