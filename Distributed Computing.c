//*********************************************************************************************************************************************************************
//
// Ranjith Kumar Bodla
// Advanced Operating Systems
// Programming Project #1_3: Calculating sum of first 1024 numbers by n processes
// Due date : 25 june 2014 Wed 6:00 p.m (before class)
// Instructor: Dr. Ajay K. Katangur
//
//*********************************************************************************************************************************************************************

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

//*********************************************************************************************************************************************************************
// calculatesum function
// funnction calculatesum is used to calculate sum of the first 1024 numbers with each process 
// finding the sum for its range of numbers
//
// Return Value
// -------------
// int			returns the sum calculated by the process
//
// Value Parameters
// -----------------
// rank		int	rank of the processing calling this function
// size		int	size/total number of processes executing this program
//
// Reference Paramater
// ----------------------
// N/A
//
// Local Varialbes
// ----------------------
// i		int	used for for loop iteration and holds the iteration number
// quotient	int	to calculate the quotient by dividing number 1024 by total number of process used as a criteria in calculating sum
// remainder 	int 	to calculate the remainder by dividing number 1024 by total number of process used as a criteria in calculating sum
//
//*********************************************************************************************************************************************************************

int calculatesum(int rank, int size)
{
	int i = 0, quotient, remainder, sum = 0;
	
	// finding the quotient and remainder by dividing 1024 count of numbers with number of processes
        quotient = 1024/size;
        remainder = 1024%size;

        if(rank !=size-1) // computing sum for processes other than the last process
        {
                // for loop running for respective range of values for process
                for(i=(rank*quotient)+1;i<=(quotient*(rank+1));i++)
                        sum = sum + i;
        }
        else if(rank == size-1) //computing sum for last process
        {
                // for loop running for its range of values for last process
                for(i=(rank*quotient)+1;i<=((quotient*size)+remainder);i++)
                        sum = sum + i;
        }

	return sum;
}


int main(int argc, char *argv[])
{
	//Initializing the variables
	int rank,size,sum=0,result;
	double start_time,stop_time,computation_time;
	//declaring and initializing MPI variables and commands
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	start_time = MPI_Wtime();	//Initializing the start time of processes

	sum = calculatesum(rank, size); //calling user-defined function calculate sum and store the return value in variable sum

	// sending the computed values by each process to process as root with rank 0 where final sum is calculated
	
	MPI_Reduce(&sum,&result,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);	//all proceesses sends their computed sum values to P 0 to compute final sum

	stop_time = MPI_Wtime();	// calling the stop time of the processes
	computation_time = stop_time - start_time;	// computing the execution time of the process

	if(rank==0)
	{
		// printing the final sum computed by the process 0
		printf("sum of first 1024 numbers calculated by %d processes is %d\n",size,result);
		printf("time taken to compute the sum is %fs\n",computation_time);
	}

	MPI_Finalize();	
}
