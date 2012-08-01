#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

#include "sort.h"

struct stack_frame stack[MAX_STACK];
unsigned int stack_depth = 0;

// Generate a random number between 'low' and 'high', 
int rand_from(int low, int high) {
	return low + rand() % (high - low);
}

// Swap the elements at indexes 'a' and 'b'
void swap(int a, int b) {
	int tmp;
	if (a == b) return;
	tmp = data[a];
	data[a] = data[b];
	data[b] = tmp;
	sample(1.0f);
}

// Shuffle the whole array
void fisher_yates_shuffle(int low, int high) {
	push(FYSHUFFLE, low, high);

	for (int i = low; i < high; i++) {
		swap(rand_from(i, high), i);
	}

	pop();
}

// Partition the range from 'low' to 'high', and quick-sort each.
void quick_sort(int low, int high) {
	push(QUICKSORT, low, high);

	int pivot;
	if (low < high) {
		pivot = partition(low, high);
		quick_sort(low, pivot - 1);
		quick_sort(pivot + 1, high);
	}

	pop();
}

// Put each element either above or below the first element.
int partition(int low, int high) {
	int val, store, i;
	val = data[low];
	swap(low, high);
	store = low;
	for (i = low; i < high; i++) {
		if (data[i] < val) {
			swap(i, store++);
		}
	}
	swap(high, store);
	return store;
}

// Just fill the whole array with values.
void fill() {
	for (int i = 0; i != N; i++) {
		data[i] = i;
	}
}

// Move a value through a sorted list into its place.
// (sample three times to simulate some expense.)
void insert(int src, int low) {
	int i, val = data[src];
	for (i = src - 1; i >= low && data[i] > val; i--) {
		data[i + 1] = data[i];
		data[i] = val;
	}
	data[i + 1] = val;

	sample(src - low);
}

void insertion_sort(int low, int high) {
	for(int i = low + 1; i <= high; i++) {
		insert(i, low);
	}
}

int select_min(int low, int high) {
	int min = low;
	for (int i = low + 1; i <= high; i++) {
		if (data[i] < data[min]) {
			min = i;
		}
	}
	return min;
}

void selection_sort(int low, int high) {
	for(int i = low; i <= high; i++) {
		swap(select_min(i, high), i);
	}
}

int is_sorted(int low, int high) {
	for (int i = low; i != high; i++) {
		if (compare(i, i+1) < 0) {
			return 0;
		}
	}
	return 1;
}

void bogo_sort(int low, int high) {
	while (!is_sorted(low, high)) {
		fisher_yates_shuffle(low, high);
	}
}

int compare(int first, int second) {
	return data[second] - data[first];
}

void push(int id, int low, int high) {
	if (stack_depth < MAX_STACK) {
		stack[stack_depth].id   = id;
		stack[stack_depth].low  = low;
		stack[stack_depth].high = high;
	}
	stack_depth++;
}

void pop() {
	stack_depth--;
}

void run_sorts() {
	memset(data, 0, N);
	fill();

	while(1) {
		fisher_yates_shuffle(0, N-1);
		quick_sort(0, N-1);
		/*
		quick_sort(0, N-1);
		fisher_yates_shuffle(0, N/2-1);
		quick_sort(N/2 + 1, N-1);
		fisher_yates_shuffle(2*N/4, 3*N/4);
		quick_sort(0, N-1);*/
	}
}
