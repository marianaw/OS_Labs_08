#ifdef __KERNEL__
  #include <linux/slab.h>
  #include <linux/string.h> /* memset */
#define MALLOC(s, u)	kmalloc((s)*(u), GFP_KERNEL)

#define FREE(p)		kfree(p)
#define ASSERT(p)	BUG_ON(!(p))
#else /* ! __KERNEL__ */
  #include <stdlib.h>
  #include <string.h> /* memset */
  
  #include <assert.h>
#define MALLOC(s, u)	malloc((s)*(u))
#define FREE(p)		free(p)
#define ASSERT(p)	assert(p)
#endif

/* Representation invariant */
#define inv_rep(b)	(b->ring && b->rpos<=b->size && b->wpos<=b->size)

#include "rbuffer.h"

struct _rbuffer_t {
	volatile unsigned int rpos;
	volatile unsigned int wpos;
	unsigned int size;
	byte *ring;
};

rbuffer_t rbuffer_create(const unsigned int size)
{
	rbuffer_t buf = NULL;

	buf = (rbuffer_t) MALLOC(1, sizeof(struct _rbuffer_t));
	if (buf) {
		buf->rpos = 0;
		buf->wpos = 1;
		buf->size = size + 1;
		buf->ring = (byte *) MALLOC(1, buf->size);
		if (buf->ring) {
			memset((void *) buf->ring, 0, buf->size);
		} else {
			FREE((void*) buf);
			buf = NULL;
		}
	}

	ASSERT(!buf || inv_rep(buf));
	return buf;
}

void rbuffer_destroy(rbuffer_t buf)
{
	ASSERT(buf!=NULL);
	ASSERT(inv_rep(buf));

	FREE((void *) buf->ring);
	buf->ring = NULL;
	FREE((void *) buf);
	buf = NULL;
}

bool rbuffer_is_empty(rbuffer_t buf)
{
	bool is_empty = false;

	ASSERT(buf!=NULL);
	ASSERT(inv_rep(buf));

	if ((buf->rpos + 1 == (buf->wpos)) ||
	    ((buf->wpos == 0) && (buf->rpos + 1 == buf->size))){
		is_empty = true;
	}
	return (is_empty);
}

bool rbuffer_is_full(rbuffer_t buf)
{
	ASSERT(buf!=NULL);
	ASSERT(inv_rep(buf));
	return (buf->rpos == buf->wpos);
}

bool rbuffer_insert(rbuffer_t buf, const byte data)
{
	bool err = true;

	ASSERT(buf!=NULL);
	ASSERT(inv_rep(buf));

	if (! rbuffer_is_full(buf)) {
		buf->ring[buf->wpos] = data;
		buf->wpos++;
		buf->wpos %= buf->size;
		err = false;
	}

	return err;
}

bool rbuffer_remove(rbuffer_t buf, byte *data)
{
	bool err = true;

	ASSERT(buf!=NULL);
	ASSERT(inv_rep(buf));

	if (! rbuffer_is_empty(buf)) {
		buf->rpos++;
		buf->rpos %= buf->size;
		*data = buf->ring[buf->rpos];
		err = false;
	}

	return err;
}
