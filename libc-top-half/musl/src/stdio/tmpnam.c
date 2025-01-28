#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "syscall.h"
#if 0
#include "kstat.h"
#endif

#define MAXTRIES 100

char *tmpnam(char *buf)
{
#if 0
	static char internal[L_tmpnam];
	char s[] = "/tmp/tmpnam_XXXXXX";
	int try;
	int r;
	for (try=0; try<MAXTRIES; try++) {
		__randname(s+12);
#ifdef SYS_lstat
		r = __syscall(SYS_lstat, s, &(struct kstat){0});
#else
		r = __syscall(SYS_fstatat, AT_FDCWD, s,
			&(struct kstat){0}, AT_SYMLINK_NOFOLLOW);
#endif
		if (r == -ENOENT) return strcpy(buf ? buf : internal, s);
	}
	return 0;
#else 
	// a-Shell version, using local file system access
	static char *tmpfilename = NULL;
	static int first = 1;
	static const char template[] = "snprintfXXXXXX";
	int fd;
	static char *tmpdir = NULL;
	static int len = 0;

	if (first) {
		first = 0;
		/*
		 * First try Unix standard env var, then Windows var,
		 * then fall back to /tmp.
		 */
		if ((tmpdir = getenv("TMPDIR")) != NULL && *tmpdir != '\0')
			;	/* got it */
		else if ((tmpdir = getenv("TEMP")) != NULL && *tmpdir != '\0')
			;	/* got it */
		else
			tmpdir = "/tmp";

		len = strlen(tmpdir) + 1 + strlen(template) + 1;
	}

	if ((tmpfilename = (char *) malloc(len)) == NULL)
		return NULL;
	else
		sprintf(tmpfilename, "%s/%s", tmpdir, template);

	size_t l = strlen(tmpfilename);

	struct stat stat_buf;
	int retries = 100;
	do {
		__randname(tmpfilename+l-len-6);
		if (stat(tmpfilename, &stat_buf) != 0)
			if (errno == ENOENT) {
				if (buf != NULL) {
					strcpy(buf, tmpfilename);
				}
				return strdup(tmpfilename);
			}
	} while (--retries);
	
	return NULL;

#endif
}
