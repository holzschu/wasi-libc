#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
// #include "syscall.h" // iOS: commented out since it breaks compilation.

int fchdir(int fd)
{
	// iOS / a-Shell:
	__wasi_errno_t error = __wasi_ashell_fchdir(fd); 

	if (error != 0) {
		errno = error;
		return -1;
	}
	return 0; 
	// before iOS: 
	// int ret = __syscall(SYS_fchdir, fd);
	// if (ret != -EBADF || __syscall(SYS_fcntl, fd, F_GETFD) < 0)
	// 	return __syscall_ret(ret);

	// char buf[15+3*sizeof(int)];
	// __procfdname(buf, fd);
	// return syscall(SYS_chdir, buf);
}
