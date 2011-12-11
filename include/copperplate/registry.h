/*
 * Copyright (C) 2008 Philippe Gerum <rpm@xenomai.org>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifndef _COPPERPLATE_REGISTRY_H
#define _COPPERPLATE_REGISTRY_H

#include <sys/types.h>
#include <pthread.h>
#include <copperplate/list.h>
#include <copperplate/hash.h>
#include <copperplate/init.h>

struct fsobj;

#ifdef CONFIG_XENO_REGISTRY

struct registry_operations {
	size_t (*read)(struct fsobj *fsobj,
		       char *buf, size_t size, off_t offset);
	size_t (*write)(struct fsobj *fsobj,
			const char *buf, size_t size, off_t offset);
};

struct regfs_dir;

struct fsobj {
	pthread_mutex_t lock;
	char *path;
	const char *basename;
	int mode;
	struct regfs_dir *dir;
	struct timespec ctime;
	struct timespec mtime;
	const struct registry_operations *ops;
	struct pvholder link;
	struct pvhashobj hobj;
};

#ifdef __cplusplus
extern "C" {
#endif

int registry_add_dir(const char *fmt, ...);

void registry_init_file(struct fsobj *fsobj,
			const struct registry_operations *ops);

int registry_add_file(struct fsobj *fsobj,
		      int mode,
		      const char *fmt, ...);

void registry_destroy_file(struct fsobj *fsobj);

void registry_touch_file(struct fsobj *fsobj);

int registry_pkg_init(char *arg0);

void registry_pkg_destroy(void);

#ifdef __cplusplus
}
#endif

#else /* !CONFIG_XENO_REGISTRY */

struct fsobj {
};

struct registry_operations {
};

static inline
int registry_add_dir(const char *fmt, ...)
{
	return 0;
}

static inline
int registry_init_file(struct fsobj *fsobj,
		       const struct registry_operations *ops)
{
	return 0;
}

static inline
int registry_add_file(struct fsobj *fsobj,
		      int mode,
		      const char *fmt, ...)
{
	return 0;
}

static inline
void registry_destroy_file(struct fsobj *fsobj)
{
}

static inline
void registry_touch_file(struct fsobj *fsobj)
{
}

static inline
int registry_pkg_init(char *arg0)
{
	return 0;
}

static inline
void registry_pkg_destroy(void)
{
}

#endif /* !CONFIG_XENO_REGISTRY */

#endif /* _COPPERPLATE_REGISTRY_H */
