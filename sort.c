#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

#include "sort.h"

extern void sample(float cost);

// Return 0 if they're out of order.
int compare(buf *buf, int a, int b) {
	return buf->data[b] - buf->data[a] >= 0;
}

void swap(buf *buf, int a, int b) {
	int tmp;
	if (a == b) return;
	tmp = buf->data[a];
	buf->data[a] = buf->data[b];
	buf->data[b] = tmp;
	sample(1.0);
}

// Generate a random number between 'low' and 'high', 
int rand_from(int low, int high) {
	return low + rand() % (1 + high - low);
}

// Shuffle the whole array
void fisher_yates_shuffle(buf *b, int low, int high) {
	for (int i = low; i < high; i++) {
		swap(b, rand_from(i, high), i);
	}
}

void reverse(buf *b, int low, int high) {
	for (int i = 0; i < ((high - low) / 2); i++) {
		swap(b, low+i, high-i);
	}
}

// Put each element either above or below the first element.
int partition(buf *buf, int low, int high) {
	int store, i;
	swap(buf, low, high);
	store = low;
	for (i = low; i < high; i++) {
		if (!compare(buf, high, i)) {
			swap(buf, i, store++);
		}
	}
	swap(buf, high, store);
	return store;
}

// Partition the range from 'low' to 'high', and quick-sort each.
void quicksort(buf *buf, int low, int high) {
	int pivot;
	if (low < high) {
		pivot = partition(buf, low, high);
		quicksort(buf, low, pivot - 1);
		quicksort(buf, pivot + 1, high);
	}
}

// Just fill the whole array with values.
void fill(buf *buf) {
	for (int i = 0; i != buf->length; i++) {
		buf->data[i] = i;
	}
}

// Move a value through a sorted list into its place.
// TODO: obsolete this.
void insert(buf *buf, int a, int b) {
	int i, val = buf->data[b];
	for (i = b - 1; i >= a && buf->data[i] > val; i--) {
		buf->data[i + 1] = buf->data[i];
	}
	buf->data[i + 1] = val;

	for (i = 0; i != (b-a)/10; i++) { sample(1.0); }
}

void insert2(buf *buf, int a, int b) {
	int i, val = buf->data[b];
	for (i = b - 1; i >= a; i--) {
		buf->data[i + 1] = buf->data[i];
	}
	buf->data[i + 1] = val;

	for (i = 0; i != (b-a)/10; i++) { sample(1.0); }
}

void insertion_sort(buf *buf, int low, int high) {
	for(int i = low + 1; i <= high; i++) {
		insert(buf, low, i);
	}
}

int select_min(buf *buf, int low, int high) {
	int min = low;
	for (int i = low + 1; i <= high; i++) {
		if (!compare(buf, min, i)) {
			min = i;
		}
	}
	sample(1.0);
	sample(1.0);
	return min;
}

void selection_sort(buf *buf, int low, int high) {
	for(int i = low; i <= high; i++) {
		swap(buf, select_min(buf, i, high), i);
	}
}

int is_sorted(buf *buf, int low, int high) {
	for (int i = low; i != high; i++) {
		if (!compare(buf, i, i+1)) {
			return 0;
		}
	}
	return 1;
}

void bogo_sort(buf *buf, int low, int high) {
	while (!is_sorted(buf, low, high)) {
		fisher_yates_shuffle(buf, low, high);
	}
}

int parent(int i) { return (i-1) / 2; }
int left(  int i) { return 2*i + 1;   }
int right( int i) { return 2*i + 2;   }

void sift_down(buf *buf, int low, int high, int node) {
	while ( left(node) <= high ) {
		int dest = node;
		if (compare(buf, dest, left(node))) {
			dest = left(node);
		}
		if (right(node) <= high && compare(buf, dest, right(node))) {
			dest = right(node);
		}
		if (dest != node) {
			swap(buf, node, dest);
			node = dest;
		} else {
			return;
		}
	}
}

void repair_heap(buf *buf, int low, int high) {
	// Sift down from 1 to the last parent.
	for (int i = (high - 1) / 2; i >= low; i--) {
		sift_down(buf, low, high, i);
	}
}

void heap_sort(buf *buf, int low, int high) {
	repair_heap(buf, low, high);
	
	for (int i = high; i > low; i--) {
		swap(buf, i, low);
		sift_down(buf, low, i-1, low);
	}
}

void merge_sort(buf *buf, int low, int high) {
	int mid = low + (high - low) / 2 + 1;

	if (high <= low) { return; }

	merge_sort(buf, low, mid-1);
	merge_sort(buf, mid, high);

	while (low <= mid && mid <= high) {
		if (compare(buf, low, mid)) {
			low++;
		} else {
			insert2(buf, low, mid);
			mid++;
		}
	}
}

void run_sorts(buf *buf) {
	fill(buf);

	while(1) {
		int r = rand_from(0, 4);

		fisher_yates_shuffle(buf, 0, buf->length-1);

		switch (r) {
		case 0:
			printf("Insertion Sort.\n");
			insertion_sort(buf, 0, buf->length-1);
			break;
		case 1:
			printf("Quick Sort.\n");
			quicksort(buf, 0, buf->length-1);
			break;
		case 2:
			printf("Heap Sort.\n");
			heap_sort(buf, 0, buf->length-1);
			break;
		case 3:
			printf("Selection Sort.\n");
			selection_sort(buf, 0, buf->length-1);
			break;
		case 4:
			printf("Merge Sort.\n");
			merge_sort(buf, 0, buf->length-1);
			break;
		}
	}
}
