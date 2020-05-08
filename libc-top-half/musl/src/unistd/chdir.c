#include <unistd.h>
#include "syscall.h"
// iOS: 
#include <errno.h>
#include <string.h>

int chdir(const char *path)
{
	// iOS:
	__wasi_errno_t error = __wasi_ashell_chdir(path, strlen(path)); 

	if (error != 0) {
		errno = error;
		return -1;
	}

	return 0;

	// before iOS: 
	// return syscall(SYS_chdir, path);
}
