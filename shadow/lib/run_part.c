#include <config.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <lib/prototypes.h>

#include "run_part.h"
#include "shadowlog_internal.h"
#include "string/sprintf/aprintf.h"

#define RUN_PARTS "/bin/run-parts"

static int run_part(char *script_path, const char *directory, const char *name, const char *action)
{
	pid_t pid;
	int wait_status;
	pid_t pid_status;

	pid=fork();
	if (pid==-1) {
		fprintf(shadow_logfd, "fork: %s\n", strerror(errno));
		return 1;
	}
	if (pid==0) {
		setenv("ACTION",action,1);
		setenv("SUBJECT",name,1);
		execl(script_path, script_path, directory, (char *)NULL);
		fprintf(shadow_logfd, "execl: %s\n", strerror(errno));
		_exit(1);
	}

	pid_status = wait(&wait_status);
	if (pid_status == pid) {
		return (wait_status);
	}

	fprintf(shadow_logfd, "waitpid: %s\n", strerror(errno));
	return (1);
}

int run_parts(const char *directory, const char *name, const char *action)
{
	struct dirent **namelist;
	int scanlist;
	int n;
	int execute_result = 0;
	struct stat sb;

	/* If run-parts utility exists then use it */
	if (access (RUN_PARTS, X_OK) == 0) {
		if ((stat (directory, &sb) < 0) || !S_ISDIR (sb.st_mode)) {
			fprintf (stderr, "Warning: directory %s does not exist.\n", directory);
			return 0;
		}
		return run_part (RUN_PARTS, directory, name, action);
	}

	scanlist = scandir(directory, &namelist, NULL, alphasort);
	if (scanlist<=0) {
		return (0);
	}

	for (n=0; n<scanlist; n++) {
		char         *s;

		s = aprintf("%s/%s", directory, namelist[n]->d_name);
		if (s == NULL) {
			fprintf(shadow_logfd, "aprintf: %s\n", strerror(errno));
			for (; n<scanlist; n++) {
				free(namelist[n]);
			}
			free(namelist);
			return (1);
		}

		execute_result = 0;
		if (stat(s, &sb) == -1) {
			fprintf(shadow_logfd, "stat: %s\n", strerror(errno));
			free(s);
			for (; n<scanlist; n++) {
				free(namelist[n]);
			}
			free(namelist);
			return (1);
		}

		if (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode)) {
			execute_result = run_part(s, NULL, name, action);
		}

		free(s);

		if (execute_result!=0) {
			fprintf(shadow_logfd,
				"%s: did not exit cleanly.\n",
			    namelist[n]->d_name);
			for (; n<scanlist; n++) {
				free(namelist[n]);
			}
			break;
		}

		free(namelist[n]);
	}
	free(namelist);

	return (execute_result);
}

