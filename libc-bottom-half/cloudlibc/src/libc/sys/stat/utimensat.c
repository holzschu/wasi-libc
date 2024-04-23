// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#include <wasi/api.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "stat_impl.h"

int __wasilibc_nocwd_utimensat(int fd, const char *path, const struct timespec times[2],
                               int flag) {
  // Convert timestamps and extract NOW/OMIT flags.
  __wasi_timestamp_t st_atim;
  __wasi_timestamp_t st_mtim;
  __wasi_fstflags_t flags;
  if (!utimens_get_timestamps(times, &st_atim, &st_mtim, &flags)) {
    errno = EINVAL;
    return -1;
  }

  // Create lookup properties.
  __wasi_lookupflags_t lookup_flags = 0;
  if ((flag & AT_SYMLINK_NOFOLLOW) == 0)
    lookup_flags |= __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW;

  // a-Shell: we had issues with 64 bits values being clamped to 32 bits. 
  // Need to check if this is still needed.
  // st_atim = time stamp, in nanoseconds.
  int st_atim_s = st_atim / 1000000000;
  int st_atim_ns = st_atim - (st_atim_s * 1000000000);
  int st_mtim_s = st_mtim / 1000000000;
  int st_mtim_ns = st_mtim - (st_mtim_s * 1000000000);
  // Perform system call.
  __wasi_errno_t error =
      // iOS/a-Shell version: parameters are clamped to 32 bits even though everyone says 64 bits. 
      // So we send seconds and nanoseconds separately.
      // This will still cause a bug in 2038, unless JS+Wasm has moved to 64 bits by then.
      // __wasi_path_filestat_set_times(fd, lookup_flags, path, st_atim, st_mtim, flags);
      // TODO: check if modifications (above) fix issue
      __wasi_path_filestat_set_times(fd, flag, path, st_atim_s, st_atim_ns, st_mtim_s, st_mtim_ns, flags);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}

// a-Shell: let's also have utimes:
int utimes(const char *pathname, const struct timeval times[2])
{
	struct timespec ts[2];
	if (times) {
		int i;
		for (i=0; i<2; i++) {
			if (times[i].tv_usec >= 1000000ULL) {
				errno = EINVAL; 
				return -1; 
			}
			ts[i].tv_sec = times[i].tv_sec;
			ts[i].tv_nsec = times[i].tv_usec * 1000;
		}
	}
	return __wasilibc_nocwd_utimensat(AT_FDCWD, pathname, times ? ts : 0, 0);
}

