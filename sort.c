#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

#include "sort.h"

extern void sample(float cost);

// Return 0 if they're out of order.
int compare(buf *buf, int a, int b) {
	//sample(1.0);
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
	return low + rand() % (high - low);
}

// Shuffle the whole array
void fisher_yates_shuffle(buf *b, int low, int high) {
	for (int i = low; i < high; i++) {
		swap(b, rand_from(i, high), i);
	}
}

// Put each element either above or below the first element.
int partition(buf *buf, int low, int high) {
	int val, store, i;
	val = buf->data[low];
	swap(buf, low, high);
	store = low;
	for (i = low; i < high; i++) {
		if (buf->data[i] < val) {
			swap(buf, i, store++);
		}
	}
	swap(buf, high, store);
	return store;
}

// Partition the range from 'low' to 'high', and quick-sort each.
void quick_sort(buf *buf, int low, int high) {
	int pivot;
	if (low < high) {
		pivot = partition(buf, low, high);
		quick_sort(buf, low, pivot - 1);
		quick_sort(buf, pivot + 1, high);
	}
}

// Just fill the whole array with values.
void fill(buf *buf) {
	for (int i = 0; i != buf->length; i++) {
		buf->data[i] = i;
	}
}

// Move a value through a sorted list into its place.
void insert(buf *buf, int a, int b) {
	int i, val = buf->data[b];
	for (i = b - 1; i >= a && buf->data[i] > val; i--) {
		buf->data[i + 1] = buf->data[i];
		buf->data[i] = val;
	}
	buf->data[i + 1] = val;
	sample(1.0);
}

void insertion_sort(buf *buf, int low, int high) {
	for(int i = low + 1; i <= high; i++) {
		insert(buf, low, i);
	}
}

int select_min(buf *buf, int low, int high) {
	int min = low;
	for (int i = low + 1; i <= high; i++) {
		if (!compare(buf, i, min)) {
			min = i;
		}
	}
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

int parent(int i) { return floor((i-1)/2); }
int left(int i) { return 2 * i + 1; }
int right(int i) { return 2 * i + 2; }

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

/*
int main (int argc, char **argv) {
	run_sorts();
	return 0;
}*/

void run_sorts(buf *buf) {
	fill(buf);

	while(1) {
		printf("Shuffle.\n");
		fisher_yates_shuffle(buf, 0, buf->length-1);
		printf("Insertion Sort.\n");
		insertion_sort(buf, 0, buf->length-1);
		printf("Shuffle.\n");
		fisher_yates_shuffle(buf, 0, buf->length-1);
		printf("Quicksort.\n");
		quick_sort(buf, 0, buf->length-1);
		printf("Shuffle.\n");
		fisher_yates_shuffle(buf, 0, buf->length-1);
		printf("Heap Sort.\n");
		heap_sort(buf, 0, buf->length-1);
		printf("Shuffle.\n");
		fisher_yates_shuffle(buf, 0, buf->length-1);
		printf("Selection Sort.\n");
		selection_sort(buf, 0, buf->length-1);
	}
}
