/////////////////////////////////////////////////////////////
// Author:        Craig Swearingen
// Original:      23 October 2016
// Last modified: 29 October 2016
//
// Purpose: Implement a parallel geometric mean algorithm
//
// Compile:  mpicc Geo-Mean.c -lm -o Geo-Mean
//
// Run:
//      mpiexec -n  <p>  ./Geo-Mean  <v>
//        -p : the number of processes
//        -v : number of elements in the vector
//
/////////////////////////////////////////////////////////////

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

void GetArgs(int, char**, int*, int );

void Error();

double Seconds();

int main(int argc, char* argv[])
{
	// Set up variables
	int my_rank, comm_sz, vector_size;
	double vector = 1;
	double sum = 0;
	
	// Initialize MPI
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    double start_time = Seconds();

    // Read arguments from the command line and exit if arguments are improper
   	//if(my_rank == 0)
    	GetArgs(argc, argv, &vector_size, my_rank);

    // Get the number of vector elements each process will calculate
	int factor = vector_size / comm_sz;
    
    // Find the start and end index of this process vector;
    int local_start = factor * my_rank;
    int local_end = factor * my_rank + factor -1;

    // If it is the last process, check if it was divided evenly, if not add what is left to the last process
    if(my_rank + 1 == comm_sz)
    	local_end += vector_size % comm_sz;
    
    // Initalize random function
    srand((unsigned)time(NULL));

    // Each process caluclates its chunk of the vector
    for(int i = local_start; i <=local_end; i++){
    	int temp =  rand();
    	printf("vector[%d] = %d\n",i, temp);
    	vector *= temp;
    }

    // Multiply all the chunks calculated by each process
    MPI_Reduce(&vector, &sum, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD );
	
	// Print the sum, vector size, and geometric mean
    if(my_rank == 0)
    {
    	double finish_time = Seconds();
    	printf("Sum: %f\nVector Size: %d\n", sum, vector_size);
    	printf("Geometric mean: %f\n", pow(sum,(1/(double) vector_size)));
    	printf("Processing time: %f\n", finish_time - start_time);
    }

    

	MPI_Finalize();

	return 0;
}

// Precondition: The arguments have been processed and are invalid
// Postcondition: Error message is printed showing the correct usage
void Error()
{
	fprintf(stderr, "usage: mpiexec -n <p> ./Geo-Mean <size of vector>[1-8]\n");
	fflush(stderr);
}

// Precondition: Arguments have been initializeed
// Postcondition: If arguments are valid then each process knows the vector size
//				  If the arguments are invalid then the program exits and error message is printed
void GetArgs(int a, char** b, int* c, int d)
{
	if(d == 0)
	{
		if(a != 2)
		{
			Error();
			exit(-1);
		}else if(atoi(&b[1][0]) > 8 || atoi(&b[1][0]) < 1){
			Error();
			exit(-1);
		}else
		{
			*c = atoi(&b[1][0]);
		}



	}
	MPI_Bcast(c,1,MPI_INT,0,MPI_COMM_WORLD);
}

double Seconds()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double) tp.tv_sec + (double) tp.tv_usec*1.e-6);
}