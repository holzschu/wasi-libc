#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "syscall.h"

char *getcwd(char *buf, size_t size)
{
	char tmp[buf ? 1 : PATH_MAX];
	if (!buf) {
		buf = tmp;
		size = sizeof tmp;
	} else if (!size) {
		errno = EINVAL;
		return 0;
	}

	// iOS version: ask iOS for the current position.
	size_t bufused;
	__wasi_errno_t error = __wasi_ashell_getcwd(buf, size, &bufused);
	if (error != 0) {
		errno = error;
		return 0;
	}

	buf[bufused] = 0; 
   	   
	/* 
	long ret = syscall(SYS_getcwd, buf, size);
	if (ret < 0)
		return 0;
	if (ret == 0 || buf[0] != '/') {
		errno = ENOENT;
		return 0;
	} */
	return buf == tmp ? strdup(buf) : buf;
}
