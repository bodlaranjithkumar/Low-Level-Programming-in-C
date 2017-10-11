//*********************************************************************************************************************************************************************
//
// Ranjith Kumar Bodla
// Advanced Operating Systems
// Programming Project #1_4: Tokenring
// Due date : 25 june 2014 Wed 6:00 p.m (before class)
// Instructor: Dr. Ajay K. Katangur
//
//*********************************************************************************************************************************************************************


			/*********** Program to implement token ring in clockwise and counterclock wise direction********/

#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
	//Initializing variables
	int rank, nprocs,flag,token_receive;
	int i , token_clockwise = 0;
	//Declaring and Initializing MPI based commands
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;MPI_Status status_counterclockwise;
	MPI_Request request;
	
	int rounds = atoi(argv[1]);	// taking the command line arguments into rounds variable for number of iterations
	int token_counterclockwise = nprocs * rounds;	// calculating and assigning the value of token in counteclockwise
	int left_clockwise, right_clockwise,left_counterclockwise,right_counterclockwise;

        right_clockwise = (rank + 1) % nprocs;	//	calculating the right element for each process
        left_clockwise = (nprocs + rank - 1) % nprocs;// calculating the left element for each process
        right_counterclockwise = right_clockwise;	// same as above but using different variable for better understanding
        left_counterclockwise = left_clockwise;
    	
	if (rank == 0)	//starting the initial send to be done by process with rank 0
	{
		MPI_Isend(&token_clockwise, 1, MPI_INT, right_clockwise, 10, MPI_COMM_WORLD, &request);
		MPI_Isend(&token_counterclockwise, 1, MPI_INT, left_counterclockwise, 11, MPI_COMM_WORLD, &request);
	}

	for (i= 0; i < rounds*2; i++)	
	{

		MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);	//Using probe to check on the incoming message 
		
		MPI_Irecv(&token_receive, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &request);
		MPI_Wait(&request,&status);	//using wait for the routine no to return untill receive is completed

		if(status.MPI_SOURCE == left_clockwise)	//checking if receiving source is same as inteded in clockwise direction
		{
			printf("ClockWise - Rank %3d: received token %2d from rank %3d.\n", rank, token_receive, left_clockwise);
			token_receive= token_receive + 1;	// Increment token after receiving and send it to the next process on its right
			if ((rank != 0) || (i < 2*(rounds - 1)))
				MPI_Isend(&token_receive, 1, MPI_INT, right_clockwise, 10, MPI_COMM_WORLD, &request);
		}
		
		if(status.MPI_SOURCE == right_counterclockwise)	//check if receiving source is same as intended in counterclockwise direction
		{
			printf("CounterClockWise - Rank %3d: received token %2d from rank %3d.\n", rank, token_receive, right_counterclockwise);
			token_receive = token_receive - 1; // decrement token after receiving and the send it to the next process on its left
			if ((rank != 0) || (i < 2*(rounds - 1)))
				MPI_Isend(&token_receive, 1, MPI_INT, left_counterclockwise, 11, MPI_COMM_WORLD, &request);
		}
	}
	MPI_Finalize();
}
