#include <stdlib.h>

void genMatrix(int m, int n, double *A) {
	int i;
	int num = m * n;

	for(i=0; i<num; i++) {
	   A[i] = drand48();
	}
}

void genVector(int n, double *b) {
	int i;

	for(i=0; i<n; i++) {
	   b[i] = drand48();
	}
}
