#include <stdio.h>
#include <mpi.h>
#include "genmatvec.c"
#include "matvecres.c"

double dot_product(double *v1, double *v2, int n);
void distribute_data(int m, int n, int p, MPI_Request request, double *A, double *b);
void collect_data(int m, int p, double *y, MPI_Status status);

int main (int argc, char* argvp[]) {	
	int my_rank;
	int p;
	int tag;
	MPI_Request request;
	MPI_Status status;
	
	int i;
	int m = atoi(argvp[1]);
	int n = atoi(argvp[2]);
	double *buffer_rv1 = malloc(n*sizeof(double));
	double *buffer_rv2 = malloc(n*sizeof(double));

	MPI_Init(&argc, &argvp);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int num_rows_computed = (m/p + ((my_rank < m%p) ?1:0));	
	if (my_rank == 0) {
		double *A = malloc(m*n*sizeof(double));
		double *b = malloc(n*sizeof(double));
		genMatrix(m, n, A);
		genVector(n, b); 
	    /*printf("m:\n");         
	    for(i=0; i<m*n; i++){
	        printf("%lf\n", A[i]);
	   	}
	    printf("n:\n");
	    for(i=0; i<n; i++){
        	printf("%lf\n", b[i]);
	    }*/

        distribute_data(m, n, p, request, A, b);	   

        for(i=0; i<num_rows_computed; i++) {
        	tag = i*p + my_rank;
        	MPI_Recv(buffer_rv1, n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
	   		double result = dot_product(buffer_rv1, b, n);
	    	MPI_Isend(&result, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &request);
	    }

	    double *y = malloc(m*sizeof(double));
	    collect_data(m, p, y, status);
	    /*printf("y\n");
	    for(i=0; i<m; i++){
	    	printf("%lf\n", y[i]);
	    }*/
	    getResult(m, n, A, b, y);
	    
	    free(A);
	    free(b);
	    free(y);
	}
	else {
		MPI_Bcast(buffer_rv2, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		for(i=0; i<num_rows_computed; i++){
			tag = i*p + my_rank;
			MPI_Recv(buffer_rv1, n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
			double result = dot_product(buffer_rv1, buffer_rv2, n);
			MPI_Isend(&result, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &request);
		}
	}

	MPI_Finalize();
	
	free(buffer_rv1);
	free(buffer_rv2);	
	
	return 0;
}

double dot_product(double *v1, double *v2, int n){
	int i;
	double sum = 0;
	
	for(i=0; i<n; i++){
	   sum += v1[i] * v2[i];
	}

	return sum;
}

void distribute_data(int m, int n, int p, MPI_Request request, double *A, double *b){
	int i;
	int dest;
	int tag;

	for(i=0; i<m; i++) {
		dest = i % p;
        tag = i;
        MPI_Isend(A + i*n, n, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD, &request);
    }

	MPI_Bcast(b, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void collect_data(int m, int p, double *y, MPI_Status status){
	int i;
	int src;
	int tag;
	double result;
	
	for(i=0; i<m; i++){
		src = i % p;
	    tag = i;
        MPI_Recv(&result, 1, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &status);
        y[i] = result;
    }
}
