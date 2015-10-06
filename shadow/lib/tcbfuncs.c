#define _GNU_SOURCE
#include <config.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <grp.h>
#include <errno.h>
#include <sys/stat.h>
#include <tcb.h>

#include "getdef.h"
#include "shadowio.h"

#define LOCK_SUFFIX			".lock"

static char *stored_tcb_user = NULL;

int s_drop_priv()
{
	if (!getdef_bool("USE_TCB"))
		return 1;

	if (stored_tcb_user)
		return !tcb_drop_priv(stored_tcb_user);
	else
		return 0;
}

int s_gain_priv()
{
	if (!getdef_bool("USE_TCB"))
		return 1;
	return !tcb_gain_priv();
}

/*
 * In case something goes wrong, we return immediately, not polluting the
 * code with free().  All errors are fatal, so an application is expected
 * to exit soon.
 */

#define NOMEM { \
	fprintf(stderr, "Out of memory.\n"); \
	return 0; \
}

int tcb_user(const char *name)
{
	char *buf;
	int retval;

	if (!getdef_bool("USE_TCB"))
		/* The user should be in the traditional shadow file */
		return 1;

	if (stored_tcb_user)
		free(stored_tcb_user);

	stored_tcb_user = strdup(name);
	if (!stored_tcb_user ||
	    asprintf(&buf, TCB_FMT, name) < 0)
		NOMEM;

	retval = spw_setdbname(buf);	/* should be 1 */

	free(buf);

	return retval;
}

static int unlink_suffs(const char *user)
{
	static char *suffs[] = { "+", "-", LOCK_SUFFIX };
	char *tmp;
	int i;

	for (i = 0; i < 3; i++) {
		if (asprintf(&tmp, TCB_FMT "%s", user, suffs[i]) < 0)
			NOMEM;
		if (unlink(tmp) && errno != ENOENT) {
			fprintf(stderr, "unlink: %s: %s\n", tmp,
				strerror(errno));
			free(tmp);
			return 0;
		}
		free(tmp);
	}

	return 1;
}

/*
 * tcb_path_rel() must return relative (against TCB_DIR) directory, whose
 * last component is user's tcb directory.
 */
#define HASH_BY 1000
static char *tcb_path_rel(const char *name, uid_t uid)
{
	char *ret;

	if (!getdef_bool("TCB_SYMLINKS") || uid < HASH_BY) {
		if (asprintf(&ret, "%s", name) < 0)
			NOMEM;
	} else if (uid < HASH_BY * HASH_BY) {
		if (asprintf(&ret, ":%dK/%s", uid / HASH_BY, name) < 0)
			NOMEM;
	} else {
		if (asprintf(&ret, ":%dM/:%dK/%s", uid / (HASH_BY * HASH_BY),
			     (uid % (HASH_BY * HASH_BY)) / HASH_BY, name) < 0)
			NOMEM;
	}
	return ret;
}

static char *tcb_path_rel_existing(const char *name)
{
	char *path, *rval;
	struct stat st;
	char link[8192];
	int ret;

	if (asprintf(&path, TCB_DIR "/%s", name) < 0)
		NOMEM;
	if (lstat(path, &st)) {
		fprintf(stderr, "Cannot stat %s: %s\n", path,
			strerror(errno));
		free(path);
		return NULL;
	}
	if (S_ISDIR(st.st_mode)) {
		free(path);
		rval = strdup(name);
		if (!rval)
			NOMEM;
		return rval;
	}
	if (!S_ISLNK(st.st_mode)) {
		fprintf(stderr,
			"%s is neither a directory, nor a symlink.\n",
			path);
		free(path);
		return NULL;
	}
	ret = readlink(path, link, sizeof(link) - 1);
	free(path);
	if (ret == -1) {
		perror("readlink");
		return NULL;
	}
	link[ret] = 0;
	if (ret >= sizeof(link) - 1) {
		fprintf(stderr, "Suspiciously long symlink: %s\n", link);
		return NULL;
	}
	rval = strdup(link);
	if (!rval)
		NOMEM;
	return rval;
}

static char *tcb_path(const char *name, uid_t uid)
{
	char *ret, *rel;

	if (!(rel = tcb_path_rel(name, uid)))
		return 0;
	if (asprintf(&ret, TCB_DIR "/%s", rel) < 0)
		ret = NULL;
	free(rel);
	if (!ret)
		NOMEM;
	return ret;
}

static char *tcb_path_existing(const char *name)
{
	char *ret, *rel;

	if (!(rel = tcb_path_rel_existing(name)))
		return 0;
	if (asprintf(&ret, TCB_DIR "/%s", rel) < 0)
		ret = NULL;
	free(rel);
	if (!ret)
		NOMEM;
	return ret;
}

static int mkdir_leading(const char *name, uid_t uid)
{
	char *ind, *dir, *ptr, *path = tcb_path_rel(name, uid);
	struct stat st;

	if (!path)
		return 0;
	ptr = path;
	if (stat(TCB_DIR, &st)) {
		perror("stat");
		goto out_free_path;
	}
	while ((ind = strchr(ptr, '/'))) {
		*ind = 0;
		if (asprintf(&dir, TCB_DIR "/%s", path) < 0)
			NOMEM;
		if (mkdir(dir, 0700) && errno != EEXIST) {
			perror("mkdir");
			goto out_free_dir;
		}
		if (chown(dir, 0, st.st_gid)) {
			perror("chown");
			goto out_free_dir;
		}
		if (chmod(dir, 0711)) {
			perror("chmod");
			goto out_free_dir;
		}
		free(dir);
		*ind = '/';
		ptr = ind + 1;
	}
	free(path);
	return 1;
out_free_dir:
	free(dir);
out_free_path:
	free(path);
	return 0;
}

/* path should be a relative existing tcb directory */
static int rmdir_leading(char *path)
{
	char *ind, *dir;
	int ret = 1;

	while ((ind = strrchr(path, '/'))) {
		*ind = 0;
		if (asprintf(&dir, TCB_DIR "/%s", path) < 0)
			NOMEM;
		if (rmdir(dir)) {
			if (errno != ENOTEMPTY) {
				perror("rmdir");
				ret = 0;
			}
			free(dir);
			break;
		}
		free(dir);
	}
	return ret;
}

/* tcb directory must be empty before tcb_rmdir() is called */
int tcb_rmdir(const char *name)
{
	int ret = 1;
	char *path = tcb_path_existing(name);
	char *rel = tcb_path_rel_existing(name);

	if (!path || !rel || rmdir(path))
		return 0;
	if (!rmdir_leading(rel))
		return 0;
	free(path);
	free(rel);
	if (asprintf(&path, TCB_DIR "/%s", name) < 0)
		NOMEM;
	if (unlink(path) && errno != ENOENT)
		ret = 0;
	free(path);
	return ret;
}

static int move_dir(const char *user_newname, uid_t user_newid)
{
	char *olddir = NULL, *newdir = NULL;
	char *real_old_dir = NULL, *real_new_dir = NULL;
	char *real_old_dir_rel = NULL, *real_new_dir_rel = NULL;
	uid_t old_uid, the_newid;
	struct stat oldmode;
	int ret = 0;

	if (asprintf(&olddir, TCB_DIR "/%s", stored_tcb_user) < 0)
		goto out_free_nomem;
	if (stat(olddir, &oldmode)) {
		perror("stat");
		goto out_free;
	}
	old_uid = oldmode.st_uid;
	if (user_newid == -1)
		the_newid = old_uid;
	else
		the_newid = user_newid;
	if (!(real_old_dir = tcb_path_existing(stored_tcb_user)) ||
	    !(real_new_dir = tcb_path(user_newname, the_newid)))
		goto out_free;
	if (!strcmp(real_old_dir, real_new_dir)) {
		ret = 1;
		goto out_free;
	}
	if (!(real_old_dir_rel = tcb_path_rel_existing(stored_tcb_user)) ||
	    !mkdir_leading(user_newname, the_newid))
		goto out_free;
	if (rename(real_old_dir, real_new_dir)) {
		perror("rename");
		goto out_free;
	}
	if (!rmdir_leading(real_old_dir_rel))
		goto out_free;
	if (unlink(olddir) && errno != ENOENT) {
		perror("unlink");
		goto out_free;
	}
	if (asprintf(&newdir, TCB_DIR "/%s", user_newname) < 0)
		goto out_free_nomem;
	if (!(real_new_dir_rel = tcb_path_rel(user_newname, the_newid)))
		goto out_free;
	if (strcmp(real_new_dir, newdir) &&
	    symlink(real_new_dir_rel, newdir)) {
		perror("symlink");
		goto out_free;
	}
	ret = 1;
	goto out_free;
out_free_nomem:
	fprintf(stderr, "Out of memory\n");
out_free:
	free(olddir);
	free(newdir);
	free(real_old_dir);
	free(real_new_dir);
	free(real_old_dir_rel);
	free(real_new_dir_rel);
	return ret;
}

int tcb_move(const char *user_newname, uid_t user_newid)
{
	struct stat dirmode, filemode;
	char *tcbdir, *shadow;
	int ret = 0;

	if (!getdef_bool("USE_TCB"))
		return 1;
	if (!user_newname)
		user_newname = stored_tcb_user;
	if (!move_dir(user_newname, user_newid))
		return 0;
	/* Directory moved, adjust ownership */
	if (user_newid == -1)
		return 1;
	if (asprintf(&tcbdir, TCB_DIR "/%s", user_newname) < 0 ||
	    asprintf(&shadow, TCB_DIR "/%s/shadow", user_newname) < 0)
		NOMEM;
	if (stat(tcbdir, &dirmode)) {
		perror("stat");
		goto out_free;
	}
	if (chown(tcbdir, 0, 0)) {
		perror("chown");
		goto out_free;
	}
	if (chmod(tcbdir, 0700)) {
		perror("chmod");
		goto out_free;
	}
	if (lstat(shadow, &filemode)) {
		if (errno != ENOENT) {
			perror("lstat");
			goto out_free;
		}
		fprintf(stderr,
			"Warning, user %s has no shadow file.\n",
			user_newname);
	} else {
		if (!S_ISREG(filemode.st_mode) ||
		    filemode.st_nlink != 1) {
			fprintf(stderr,
				"Emergency: %s'shadow is not a regular file"
				" with st_nlink=1.\n"
				"The account is left locked.\n",
				user_newname);
			goto out_free;
		}
		if (chown(shadow, user_newid, filemode.st_gid)) {
			perror("chown");
			goto out_free;
		}
		if (chmod(shadow, filemode.st_mode & 07777)) {
			perror("chmod");
			goto out_free;
		}
	}
	if (!unlink_suffs(user_newname))
		goto out_free;
	if (chown(tcbdir, user_newid, dirmode.st_gid)) {
		perror("chown");
		goto out_free;
	}
	if (chmod(tcbdir, dirmode.st_mode & 07777)) {
		perror("chmod");
		goto out_free;
	}
	ret = 1;
out_free:
	free(tcbdir);
	free(shadow);
	return ret;
}

int tcb_create(const char *name, uid_t uid)
{
	char *dir, *shadow;
	struct stat st;
	gid_t shadowgid, authgid;
	struct group *gr;
	int fd, ret = 0;

	if (!getdef_bool("USE_TCB"))
		return 1;
	if (stat(TCB_DIR, &st)) {
		perror("stat");
		return 0;
	}
	shadowgid = st.st_gid;
	if (getdef_bool("TCB_AUTH_GROUP") &&
	    (gr = getgrnam("auth")))
		authgid = gr->gr_gid;
	else
		authgid = shadowgid;
	if (asprintf(&dir, TCB_DIR "/%s", name) < 0 ||
	    asprintf(&shadow, TCB_FMT, name) < 0)
		NOMEM;
	if (mkdir(dir, 0700)) {
		fprintf(stderr, "mkdir: %s: %s\n", dir, strerror(errno));
		goto out_free;
		return 0;
	}
	fd = open(shadow, O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (fd < 0) {
		perror("open");
		goto out_free;
	}
	close(fd);
	if (chown(shadow, 0, authgid)) {
		perror("chown");
		goto out_free;
	}
	if (chmod(shadow, authgid == shadowgid ? 0600 : 0640)) {
		perror("chmod");
		goto out_free;
	}
	if (chown(dir, 0, authgid)) {
		perror("chown");
		goto out_free;
	}
	if (chmod(dir, authgid == shadowgid ? 02700 : 02710)) {
		perror("chmod");
		goto out_free;
	}
	if (!tcb_user(name) || !tcb_move(NULL, uid))
		goto out_free;
	ret = 1;
out_free:
	free(dir);
	free(shadow);
	return ret;
}
