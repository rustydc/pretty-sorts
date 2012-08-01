#include <stdio.h>
#include <math.h>

int parent(int i) { return floor((i-1)/2); }
int left(int i) { return 2 * i + 1; }
int right(int i) { return 2 * i + 2; }

int compare(unsigned short data[], int first, int second) {
	return data[second] - data[first];
}

// Swap the elements at indexes 'a' and 'b'
void swap(unsigned short data[], int a, int b) {
	int tmp;
	if (a == b) return;
	tmp = data[a];
	data[a] = data[b];
	data[b] = tmp;
}

void sift_down(unsigned short data[], int node, int high) {
	while ( left(node) <= high ) {
		int dest = node;
		if ( compare(data, left(node), dest) < 0 ) {
			dest = left(node);
		}
		if ( right(node) <= high && compare(data, right(node), dest) < 0 ) {
			dest = right(node);
		}
		if (dest != node) {
			swap(data, node, dest);
			node = dest;
		} else {
			return;
		}
	}
}

void repair_heap(unsigned short data[], int low, int high) {
	// Adjust it to be zero indexed.
	data += low;
	high -= low;
	low = 0;

	// Sift down from 1 to the last parent.
	for (int i = (high - 1) / 2; i >= 0; i--) {
		sift_down(data, i, high);
	}
}

void heap_sort(unsigned short data[], int low, int high) {
	repair_heap(data, low, high);

	
	for (int i = high; i > 0; i--) {
		swap(data, i, 0);
		sift_down(data, 0, i-1);
	}
}

int main(int argc, char **argv) {
	unsigned short data[] = {6,5,3,1,8,7,2,4};

	heap_sort(data, 0, 7);
	

	for (int i; i != 8; i++) {
		printf("%d ", data[i]);
	}
	printf("\n");
}
