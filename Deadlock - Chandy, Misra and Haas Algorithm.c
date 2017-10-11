//*********************************************************************************************************************************************************************
//
// Ranjith Kumar Bodla
// Advanced Operating Systems
// Programming Project #3_2: Chandy, Misra and Haas algorithm
// Due date : 21 July 2014 Mon 6:00 p.m
// Instructor: Dr. Ajay K. Katangur
//
//*********************************************************************************************************************************************************************

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>

int main(int argc, char *argv[])
{

	int rank,size;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_rank(MPI_COMM_WORLD,&size);
	
	FILE* Inputfile;
	char buffer[10], delimiter[2]="-", message[10], recv_message[10];
	int InputValues[100], index=0, i=0;
	char* token;

	Inputfile = fopen(argv[1],"r");
	while(fgets(buffer,10,Inputfile))
	{ 
		if(strcmp(buffer,"\n") != 0)	//check if new line is going to reach
		{
			token = strtok(buffer,delimiter);
			while(token != NULL)
			{
				InputValues[index] = atoi(token);	//store all the contents of the file except delimiter in array at consecutive locations
				index++;	//increment the index pointer pointing to array location
				token = strtok(NULL,delimiter);
			}
		}
	}
	for(i=0;i<=index-3;i=i+2)	// this for loop is used to calculate the deadlock detecting process's index in the array
	{
		if(InputValues[i] == InputValues[index-1] )
			break;
	}
	
	if(rank == InputValues[i])
	{
		printf("Process with rank %d is initiating deadlock detection\n\n",InputValues[i]);
	}
	int temp = i;	// storing i value in temp variable for further usage
	int j = 0,exists = 1, check = 1,x=1;	// flags and loop iterating variables
	int count = 1;
	for(;j<=index-3;j = j+2)	//calculating count for a variable which is last node in the system
	{
		if(InputValues[temp] == InputValues[j])
		{
			count ++ ;
		}
	}
	if(count == 1)	// if a node is startng the deadlock detection it finds there is no deadlock and finalize
	{
		if(rank == InputValues[temp])
		{ 
			printf("THE SYSTEM IS NOT DEADLOCKED\n");
			MPI_Finalize();
		}
		else
			MPI_Finalize();
	}	
	else
	{
     if(temp != 0)	// checking if the detecting process is not at 0th position in the array
     {
		if(check == 1)	//flag check
		{   
			for(j=0;j<=index-3;j=j+2)	//for loop used to check if the detecting process sending process is final node or not
		         {
				if(InputValues[temp+1] == InputValues[j])	// if not final node proceed sending to other nodes and change flag values
                	 	{   
					exists = 0;
					check = 0;
					break;	
				}
			}
			if(exists == 1 )	// if final node then only send to the final node and finalize both the processes and it finds there is no deadlock
			{
				 if(rank == InputValues[temp])
			          {
	        	               	sprintf(message,"[%d,%d,%d]",InputValues[temp],InputValues[temp],InputValues[temp+1]);
	        	                MPI_Send(message,10,MPI_CHAR,InputValues[temp+1],15,MPI_COMM_WORLD);
		                        printf("PROCESS %d SENDS %s TO PROCESS %d\n",InputValues[temp],message,InputValues[temp+1]);
	                       	   }
		                  if(rank == InputValues[temp+1])
		                  {
		                       	MPI_Recv(message,10,MPI_CHAR,InputValues[temp],15,MPI_COMM_WORLD,&status);
					printf("THE SYSTEM IS NOT DEADLOCKED.\n");
		                  }
			}	                 
		}
          	if(check == 0)	//flag check
		{
			for(i = temp; i<=index-3 ;i=i+2)	// loop starting form the detecting node index to final index of array
			{ 
				 for(j=0;j<=index-3;j=j+2)	// for loop used to check if the detecting process sending process is final node or not
                        	 {
					if(InputValues[i+1] == InputValues[j])
					{	
						exists = 1;	// if exists set flag to 1
						break;	// if found sending to other node then break
					}
				 }	
				if(InputValues[i] != InputValues[i+1] && exists == 1)	// check if senders and receivers value are not same and check flag status
				{ 
					if(rank == InputValues[i])	// sender's rank
					{
						sprintf(message,"[%d,%d,%d]",InputValues[index-1],InputValues[i],InputValues[i+1]);
						MPI_Send(message,10,MPI_CHAR,InputValues[i+1],15,MPI_COMM_WORLD);
						printf("PROCESS %d SENDS %s TO PROCESS %d\n",InputValues[i],message,InputValues[i+1]);
						 if(InputValues[i+1] == InputValues[temp])	// check if detecting process is the receiving process
						 { 
//printf("value at 1 %d and 5 %d \n",  message[1] - '0',message[strlen(message)-2] - '0');
                                                         if(message[1] - '0' == message[strlen(message)-2] - '0')	// check if positions 1 and strleng -2 to be same or not
                                                                  printf("THE SYSTEM IS DEADLOCKED\n");
                                                         else if(message[1] - '0' !=  message[strlen(message)-2] - '0')
                                                                  printf("tstTHE SYSTEM IS NOT DEADLOCKED \n");
                                                 }

					}
					if(rank == InputValues[i+1])
					{
						MPI_Recv(message,10,MPI_CHAR,InputValues[i],15,MPI_COMM_WORLD,&status);
					}
					memset(message,' ', sizeof(message));
					 /* if(rank == InputValues[temp] && i == index-3)
        		                        if(recv_message[1] - '0' == recv_message[5] - '0')
	                	                        printf("THE SYSTEM IS DEADLOCKED\n");
	                                	else if(recv_message[1] - '0' != recv_message[5] - '0')
        	                                	printf("THE SYSTEM IS NOT DEADLOCKED \n");*/

					if(i == index-3)	// if i is the last index of sending elements in the array then start the process again from index 0
					{
						x = 0;	//setting index 0
						break; 	// break the loop 
					}
				}
			}//end of for
			exists = 0;
			if(x==0)	
			{
				for(i=0;i<=temp-2;i=i+2)	// loop starting form the index 0  to detecting node index
			        {
	 				 for(j=0;j<=index-3;j=j+2)	// for loop used to check if the detecting process sending process is final node or not
		                         {
			                        if(InputValues[i] == InputValues[j+1])
			                        {
	                		                exists = 1;	// if exists set flag to 1
			                        }
					}
					if(InputValues[i] != InputValues[i+1] && exists == 1 )	// check if senders and receivers value are not same and check flag status
			                {
 						if(rank == InputValues[i])	// sender's rank
                        			{
			                                sprintf(message,"[%d,%d,%d]",InputValues[index-1],InputValues[i],InputValues[i+1]);
			                                MPI_Send(message,10,MPI_CHAR,InputValues[i+1],15,MPI_COMM_WORLD);
       			                	        printf("PROCESS %d SENDS %s TO PROCESS %d\n",InputValues[i],message,InputValues[i+1]);
							if(InputValues[i+1] == InputValues[temp])	// check if detecting process is the receiving process
                                                         { 
                                                               if(message[1] - '0' ==  message[strlen(message)-2] - '0')	// check if positions 1 and strleng -2 to be same or not
                                                                        printf("THE SYSTEM IS DEADLOCKED\n");
                                                               else if(message[1] - '0' !=  message[strlen(message)-2] - '0')
                                                                        printf("testTHE SYSTEM IS NOT DEADLOCKED \n");
                                                         }

                			        }
			                        if(rank == InputValues[i+1])	// receiver's rank
			                        {        
							MPI_Recv(recv_message,10,MPI_CHAR,InputValues[i],15,MPI_COMM_WORLD,&status);
							/* if(rank == InputValues[temp])
                			                 { printf("test");
							       if(recv_message[1] - '0' == recv_message[5] - '0')
		                        	                        printf("THE SYSTEM IS DEADLOCKED\n");
			                                       else if(recv_message[1] - '0' != recv_message[5] - '0')
			                                                printf("THE SYSTEM IS NOT DEADLOCKED \n");
							 }*/

			                        }
			                        memset(message,' ', sizeof(message));
			         	}
				}    //end of for  
  			} //end of if
		}//end of elseif(check == 0)
	} // end of else - count == 1
	else if(temp == 0)	// checking if the detecting process is at 0th position in the array
	{
		for(;i<=index-3;i=i+2)	// loop starting form the index 0 to last node index
	        {
        	        if(InputValues[i] != InputValues[i+1])	// check if senders and receivers value are not same
	                {
	                         if(rank == InputValues[i])	// sender's rank
	                        {
	                                sprintf(message,"[%d,%d,%d]",InputValues[index-1],InputValues[i],InputValues[i+1]);
	                                MPI_Send(message,10,MPI_CHAR,InputValues[i+1],15,MPI_COMM_WORLD);
	                                printf("PROCESS %d SENDS %s TO PROCESS %d\n",InputValues[i],message,InputValues[i+1]);
			                }
	                        if(rank == InputValues[i+1])	// receiver's rank
	                        {
	                                MPI_Recv(recv_message,10,MPI_CHAR,InputValues[i],15,MPI_COMM_WORLD,&status);
						}
	                        memset(message,' ', sizeof(message));
		         }
			if(rank == InputValues[temp] && i == index-3)	// if last index reached and rank == detecting process
			{ 
// printf("value at 1 %d and 5 %d \n", recv_message[1] - '0',recv_message[strlen(recv_message)-2] - '0');
				if(recv_message[0] == '[' && recv_message[1] - '0' ==  recv_message[strlen(recv_message)-2] - '0' )	// check if positions 1 and strleng -2 to be same or not
					printf("THE SYSTEM IS DEADLOCKED\n");
				else
					printf("THE SYSTEM IS NOT DEADLOCKED \n");
			}
	        }
	} //end of else(temp == 0)

	fclose(Inputfile);	// close opened input file
	MPI_Finalize();		// finalize all the processes
}
}

