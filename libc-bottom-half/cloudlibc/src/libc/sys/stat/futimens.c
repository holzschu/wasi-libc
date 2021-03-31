// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#include <wasi/api.h>
#include <errno.h>

#include "stat_impl.h"

int futimens(int fd, const struct timespec *times) {
  // Convert timestamps and extract NOW/OMIT flags.
  __wasi_timestamp_t st_atim;
  __wasi_timestamp_t st_mtim;
  __wasi_fstflags_t flags;

  if (!utimens_get_timestamps(times, &st_atim, &st_mtim, &flags)) {
    errno = EINVAL;
    return -1;
  }

  // Perform system call.
  // iOS/a-Shell version: parameters are clamped to 32 bits even though everyone says 64 bits. 
  // So we send seconds and nanoseconds separately.
  // This will still cause a bug in 2038, unless JS+Wasm has moved to 64 bits by then.
  // __wasi_errno_t error = __wasi_fd_filestat_set_times(fd, st_atim, st_mtim, flags);
  __wasi_errno_t error = __wasi_fd_filestat_set_times(fd, times[0].tv_sec, times[0].tv_nsec, times[1].tv_sec, times[1].tv_nsec, flags);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
