//*********************************************************************************************************************************************************************
//
// Ranjith Kumar Bodla
// Advanced Operating Systems
// Programming Project #1_2: Sending messages between processes with rank number in the order of input phonenumber
// Due date : 25 june 2014 Wed 6:00 p.m (before class)
// Instructor: Dr. Ajay K. Katangur
//
//*********************************************************************************************************************************************************************


		/************ Program to send messages to process with rank in the order of a phone number starting with the first digit*************/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>

int main(int argc, char *argv[])
{
	//Initializing variables
	int rank,i;
	char message[50],result[100];

	int Countdigits_inputnumber = strlen(argv[1]);	// storing command line argument string/phonenumber length		
	//Declaring and initializing MPI variables and commands	
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	unsigned long long int phonenumber_asmessage = atoll(argv[1]); //Using ULL to store 10 digit number rather than int

	for(i=0;i<Countdigits_inputnumber-1;i++)	//for loop running for the length of input number times
	{
	        if((argv[1][i] - '0') != (argv[1][i+1] - '0'))	// checking if two consecutive digits of input number is not same and ignores send,recv if same
        	{
                	if(rank == argv[1][i] - '0')	//starting with process with rank same as first digit of the input and then the next number for each loop
	                {
				// sprintf used to store the result message to be sent into the variable message
        	                sprintf(message,"hello - %d%d with phonenumber %llu",argv[1][i] - '0',argv[1][i+1] - '0',phonenumber_asmessage);
                	        MPI_Send(message,50,MPI_CHAR,argv[1][i+1] - '0',55,MPI_COMM_WORLD);
	                }
        	        if(rank == argv[1][i+1] - '0')	// receiveing the message by the next process after sent by process in the abouve statement
                	{
                        	MPI_Recv(result,500,MPI_CHAR,argv[1][i] - '0',55,MPI_COMM_WORLD,&status);
	                        printf("process with rank %d received %s from process with rank %d \n",argv[1][i+1] - '0',result,argv[1][i] - '0');
        	        }
	        }
	}
	MPI_Finalize();
}

