#ifndef __SIMPLE_H
#define __SIMPLE_H

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <utime.h>
#include <time.h>
#define FUSE_USE_VERSION 26 /* Se necesita por lo menos 2.6 */
#include <fuse.h> /* para fuse_fill_dir_t, off_t */

typedef struct simple_s simple;

/* Constructores y Destructores */
simple *simple_create(void);
void simple_destroy(simple *self);

/* Operaciones Básicas */
int simple_getattr(simple *self, const char *path, struct stat *stbuf);
int simple_mknod(simple *self, const char *path, mode_t mode);
int simple_unlink(simple *self, const char *path);
int simple_rename(simple *self, const char *from, const char *to);
int simple_chmod(simple *self, const char *path, mode_t mode);
int simple_chown(simple *self, const char *path, uid_t uid, gid_t gid);
int simple_read(simple *self, const char *path,
	      char *buffer, size_t size, off_t offset);
int simple_write(simple *self, const char *path,
	       const char *buffer, size_t size, off_t offset);
int simple_utimens(simple *self, const char *path, const struct timespec tv[2]);

/* Operaciones sobre Directorios */
int simple_mkdir(simple *self, const char *path, mode_t mode);
int simple_rmdir(simple *self, const char *path);
int simple_readdir(simple *self, const char *path, void *buf,
		     fuse_fill_dir_t filler);

#endif /* __SIMPLE_H */
