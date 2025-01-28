#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "syscall.h"
// a-Shell: not needed
#if 0
#include "kstat.h"
#endif

#define MAXTRIES 100

char *tempnam(const char *dir, const char *pfx)
{
	char s[PATH_MAX];
	size_t l, dl, pl;
	int try;
	int r;

#if 0
	if (!dir) dir = P_tmpdir;
#else
	static char *tmpdir = NULL;
	// a-Shell version:
	if (!dir) {
		if (tmpdir) dir = tmpdir; 
		else {
			if ((tmpdir = getenv("TMPDIR")) != NULL && *tmpdir != '\0')
				;	/* got it */
			else if ((tmpdir = getenv("TEMP")) != NULL && *tmpdir != '\0')
				;	/* got it */
			else tmpdir = "/tmp";
			dir = tmpdir;
		}
	}
#endif
	if (!pfx) pfx = "temp";

	dl = strlen(dir);
	pl = strlen(pfx);
	l = dl + 1 + pl + 1 + 6;

	if (l >= PATH_MAX) {
		errno = ENAMETOOLONG;
		return 0;
	}

	memcpy(s, dir, dl);
	s[dl] = '/';
	memcpy(s+dl+1, pfx, pl);
	s[dl+1+pl] = '_';
	s[l] = 0;

	for (try=0; try<MAXTRIES; try++) {
		__randname(s+l-6);
#if 0
#ifdef SYS_lstat
		r = __syscall(SYS_lstat, s, &(struct kstat){0});
#else
		r = __syscall(SYS_fstatat, AT_FDCWD, s,
			&(struct kstat){0}, AT_SYMLINK_NOFOLLOW);
#endif
		if (r == -ENOENT) return strdup(s);
#else 
		struct stat stat_buf;
		if (stat(s, &stat_buf) != 0)
			if (errno == ENOENT) {
				return strdup(s);
			}
#endif 
	}
	return 0;
}
