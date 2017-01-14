#include <stdio.h> /* for NULL */
#include <assert.h>
#include <stdlib.h> /* for abs */
#include "rbuffer.h"

#define N 128

int main(void) {
	unsigned int n=0, i=0, j=0, o=0, d=0;
	rbuffer_t fifo = NULL;

	for(n=1; n<N; n++) {
		printf("Fifo size: %d\n", n);
		fifo = rbuffer_create(n);
		assert(rbuffer_is_empty(fifo));
		assert(!rbuffer_is_full(fifo));

		printf("Test 1: put one, take one, for 2 times the size.");
		assert(rbuffer_is_empty(fifo));
		for (i=0; i<2*n; i++) {
			assert(rbuffer_is_empty(fifo));
			assert(!rbuffer_is_full(fifo));

			assert(!rbuffer_insert(fifo, (byte)i));

			assert(!rbuffer_is_empty(fifo));
			assert(n>1 || rbuffer_is_full(fifo)); /* if holds one => is_full */

			assert(!rbuffer_remove(fifo, (byte *)&d));

			assert(!rbuffer_is_full(fifo));
			assert(rbuffer_is_empty(fifo));
			assert(d==i);
		}
		printf(" PASS\n");

		printf("Test 2: fill up, then empty.");
		assert(rbuffer_is_empty(fifo));
		for (i=0; i<n; i++) {
			assert(!rbuffer_is_full(fifo));
			assert(!rbuffer_insert(fifo, (byte)i));
			assert(!rbuffer_is_empty(fifo));
		}
		assert(rbuffer_is_full(fifo));
		for (i=0; i<n; i++) {
			assert(!rbuffer_is_empty(fifo));
			assert(!rbuffer_remove(fifo, (byte *)&d));
			assert(!rbuffer_is_full(fifo));
			assert(d==i);
		}
		assert(rbuffer_is_empty(fifo));
		printf(" PASS\n");

		printf("Test 3: fill to size-offset, then empty, and all this size times.");
		for (o=1; o<n; o++) {
			for (j=0; j<n; j++) {
				assert(rbuffer_is_empty(fifo));
				assert(!rbuffer_is_full(fifo));
				for (i=0; i<(unsigned int)abs(n-o); i++) {
					assert(!rbuffer_is_full(fifo));
					assert(!rbuffer_insert(fifo, (byte)i));
					assert(!rbuffer_is_empty(fifo));
				}
				assert(!rbuffer_is_full(fifo));
				for (i=0; i<(unsigned int)abs(n-o); i++) {
					assert(!rbuffer_is_empty(fifo));
					assert(!rbuffer_remove(fifo, (byte *)&d));
					assert(!rbuffer_is_full(fifo));
					assert(d==i);
				}
				assert(rbuffer_is_empty(fifo));
				assert(!rbuffer_is_full(fifo));
			}
		}
		printf(" PASS\n");

		rbuffer_destroy(fifo);
	}
	return 0;
}

