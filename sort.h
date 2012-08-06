#ifndef __SORT_H__
#define __SORT_H__

#define N 1500

#include <inttypes.h>

typedef struct buf_s {
	uint16_t *data;
	int length;
} buf;

int  compare  (buf *buf, int a, int b);
void swap     (buf *buf, int a, int b);
void insert   (buf *buf, int a, int b);

int rand_from(int low, int high);
extern void sample(float cost);
void run_sorts();

#endif // __SORT_H__
