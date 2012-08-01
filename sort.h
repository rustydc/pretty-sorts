#ifndef __SORT_H__
#define __SORT_H__

#define N 512

int partition(int low, int high);
int compare(int first, int second);
extern void sample(float cost);
void run_sorts();

unsigned short data[N];

#define FYSHUFFLE  0
#define QUICKSORT  1
#define INSERTSORT 2
#define SELECTSORT 3
#define BOGOSORT   4

#define MAX_STACK 1000

extern unsigned int stack_depth;
struct stack_frame {
	unsigned int id;
	unsigned int low;
	unsigned int high;
};
extern struct stack_frame stack[];

void push(int id, int low, int high);
void pop();

#endif // __SORT_H__
