#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "stdio_impl.h"

// a-Shell:
#include <string.h>
#include <unistd.h>

#define MAXTRIES 100

FILE *tmpfile(void)
{
#if 0
	char s[] = "/tmp/tmpfile_XXXXXX";
	int fd;
	FILE *f;
	int try;
	for (try=0; try<MAXTRIES; try++) {
		__randname(s+13);
		fd = sys_open(s, O_RDWR|O_CREAT|O_EXCL, 0600);
		if (fd >= 0) {
#ifdef SYS_unlink
			__syscall(SYS_unlink, s);
#else
			__syscall(SYS_unlinkat, AT_FDCWD, s, 0);
#endif
			f = __fdopen(fd, "w+");
			if (!f) __syscall(SYS_close, fd);
			return f;
		}
	}
	return 0;
#else
	// a-Shell version, using local file system access:
	static char *tmpfilename = NULL;
	static FILE *safe_f = NULL;
	
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
			tmpdir = "~/tmp";

		len = strlen(tmpdir) + 1 + strlen(template) + 1;
	}

	if ((tmpfilename = (char *) malloc(len)) == NULL)
		return NULL;
	else
		sprintf(tmpfilename, "%s/%s", tmpdir, template);

	if ((fd = mkstemp (tmpfilename)) < 0)
		return NULL;

	unlink (tmpfilename);
	free(tmpfilename);
	tmpfilename = NULL;

	if ((safe_f = fdopen (fd, "w+b")) == NULL) {
		close (fd);
		return NULL;
	}
	return safe_f;
#endif
}

weak_alias(tmpfile, tmpfile64);
