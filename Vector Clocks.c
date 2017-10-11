//*********************************************************************************************************************************************************************
//
// Ranjith Kumar Bodla
// Advanced Operating Systems
// Programming Project #2_2: Implementing Vector Clocks
// Due date : 07 July 2014 Wed 6:00 p.m (before class)
// Instructor: Dr. Ajay K. Katangur
//
//*********************************************************************************************************************************************************************

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>


/*Defining structure with multiple datatypes used in storing the contents of message*/
struct Message
{
        char Event_Type[5];		// holds Exec, send in this program
        char msg[30];			// message to be transferred when send event is triggered
        int  sender;			// process sending the message
        int  receiver;			// process receiving the message
}message;

//*********************************************************************************************************************************************************************
// printclockvalues function
// printclockvalues function used to print updated vector clock values at each event execution
//
// Return Value
// -------------
//	N/A
//
// Value Parameters
// -----------------
//	size		total number of processes
//	rank		rank of the process
//	logicalclock[]	array of logicalclock values
//
// Reference Paramater
// ----------------------
// N/A
//
// Local Varialbes
// ----------------------
// i	int	used for for loop iteration and holds the iteration number
//
//*********************************************************************************************************************************************************************

void printclockvalues(int size, int rank, int logicalclock[])
{
        printf("Vector clock at process %d is (",rank);
        int i;
        for(i=1;i<size-1;i++)
        {
                printf("%d ,",logicalclock[i]);
        }
        printf("%d",logicalclock[size-1]);
        printf(")\n");
}



int main(int argc,char** argv)
{
        int rank,size,lamport=0;
	//Declaring and initializing MPI Variables
	MPI_Request request;
        MPI_Status status;
	MPI_Status status_LC;
        MPI_Init(&argc,&argv );
        MPI_Comm_rank(MPI_COMM_WORLD,&rank );
        MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	//Initializing the array variables storing the lamport clocks and storing 0 initially
	int Logical_Clock[size],L_C[size];
        int j;
        for(j=1;j<size;j++)
        {
                Logical_Clock[j] = 0;
		L_C[j] = 0;
        }

	//Steps used to create structure with different datatypes
	MPI_Datatype Datatype_msg;
        MPI_Datatype type[4] = {MPI_CHAR,MPI_CHAR,MPI_INT,MPI_INT};
        int blocklength[4] = {5,30,1,1}; 
        MPI_Aint displacement[4] = {0,5,30+5,30+5+4}; 
        MPI_Type_create_struct(4,blocklength,displacement,type,&Datatype_msg); 
        MPI_Type_commit(&Datatype_msg); 

	// Functionality of the master process - P0
        if(rank == 0) 
        {
                FILE* Inputfile; 
                char** line_parts = malloc(30*sizeof(char*));
                char* event_type;
		char* Temp;
		char* temp_string;
                char line[30], msg[30]; 
                int length, sender, receiver, temp_number=0, count=0;
                
		Inputfile = fopen(argv[1],"r"); 
		//Iterating while loop till end of file
                if(Inputfile == NULL)
                      fprintf(stderr,"There is no file\n");

                line[count]=getc(Inputfile);

                while(line[count]!=EOF) 
		{
                        while(line[count]!='\n')		//iterating while loop till new line is encountered
                        {
                                count=count+1;
                                line[count] = getc(Inputfile);	//reading each character at a time and storing it in the array
                        }
                        line[count] ='\0';
                        Temp = strtok(line," "); 		//strtok used to get the number of processes being declared in first line
                        count =0;
                        length = 0;   
                        line_parts[count] = Temp;

                        while(Temp!=NULL)				// this starts for the second line
                        {
                                Temp = strtok(NULL," ");	// retrieving the rest of the string variables delimited by a space
                                count=count+1;
                                line_parts[count] = Temp;

                                if(count>=3)				// checking if the event type is "send"
                                {
                                        if(line_parts[count]!=NULL)
                                        {
                                                temp_string = &msg[length];
                                                strncpy(temp_string,line_parts[count],strlen(line_parts[count]));
                                                length = length + strlen(line_parts[count]); 
                                                msg[length] = ' ';
                                                length=length+1;
                                        }
                                }
                        }
                        msg[length] = '\0'; 
                        length = 0;
                        count++;
                        line_parts[count] = NULL;

                        if(temp_number ==0)		// condition used to print the number of processes encountered in the first line
                                printf("There are %d processes in the system\n",atoi(line_parts[0])); 
                        temp_number++;
                        event_type = line_parts[0];

                        if(strcmp("exec",event_type)== 0)  	//comparing if event_type is exec
                        {
                                sender = atoi(line_parts[1]);	//get to whom the message must be sent
                                memset(message.Event_Type,0,5); 	//clearing the contents of the event_type
                                strcpy(message.Event_Type,"exec");
                                MPI_Send(&message,1,Datatype_msg,sender,18,MPI_COMM_WORLD); 
				MPI_Recv(&message.msg,20,MPI_CHAR,sender,20,MPI_COMM_WORLD,&status);		//used as an acknowledgement
                        }
                        else if(strcmp("end",event_type)==0)	// comparing if event_type is end which states end of file
                        {
                                memset(message.Event_Type,0,5);
                                strcpy(message.Event_Type,"end");
			// for loop running for n-1 number of proceses (excluding master process). This loop is used to send message to n-1 number of prcess that
			// file has come to an end and send the final lamport logical clock values at each process to print it to output
				printf("\nFinal Vector Clock values are \n");
                                for(count = 1;count<size;count++) 
                                 {
                                           MPI_Send(&message,1,Datatype_msg,count,18,MPI_COMM_WORLD);
                                           MPI_Recv(&Logical_Clock,size,MPI_INT,count,10,MPI_COMM_WORLD,&status);
                                 	   printclockvalues(size,status.MPI_SOURCE,Logical_Clock);
				}
				MPI_Finalize();
                        }
                      else if(strcmp("send",event_type)==0)  	// comparing if event_type is send
                      {
                               memset(message.Event_Type,0,5); 	//clearing event_type variable contents
                               memset(message.msg,0,30);		//clearing msg variable contents
                               strcpy(message.Event_Type,"send");
                               strcpy(message.msg,msg);
                               sender = atoi(line_parts[1]);	// senders rank from whom message must be sent
                               receiver = atoi(line_parts[2]); 	// receivers rank
                               message.sender = sender;
                               message.receiver = receiver;
                               MPI_Send(&message,1,Datatype_msg,sender,18,MPI_COMM_WORLD);
			       MPI_Recv(&msg,20,MPI_CHAR,receiver,20,MPI_COMM_WORLD,&status);	//used as an acknowledgement
                      }

                      count =0;
                      line[count]=getc(Inputfile); 
            }
            free(line_parts); 	// used to empty the dynamically created contents
        }
		//Code executed by all other processes except p0
        if(rank!=0)
        {
            while(1)		//to be able to run multiple times
           {
                MPI_Recv(&message,1,Datatype_msg,MPI_ANY_SOURCE,18,MPI_COMM_WORLD,&status);

                if(strcmp(message.Event_Type,"exec")==0) 
                {
			Logical_Clock[rank] = Logical_Clock[rank] + 1;	// incrementing logical clock array value at index equal to its rank by 1 on exec event
                        printf("Execution event in process %d\n",rank);
			printclockvalues(size,rank,Logical_Clock);		// calling user defined function to print vector clock values
			MPI_Send(&message.msg,20,MPI_CHAR,0,20,MPI_COMM_WORLD);		//acknowledgement to p0

                }

                if(strcmp(message.Event_Type,"end")==0) 
                {
                        MPI_Send(&Logical_Clock,size,MPI_INT,0,10,MPI_COMM_WORLD);
			MPI_Finalize();
                        break;
                }
                if(strcmp(message.Event_Type,"send")==0) 
                {
                        strcpy(message.Event_Type,"recv"); 
                        Logical_Clock[rank] = Logical_Clock[rank] + 1;	// incrementing logical clock array value at index equal to its rank by 1 before send event initialized
                        message.msg[0]= ' ';	// removing " symbol from input message at start
                        message.msg[strlen(message.msg)-2]  = '\0';	// removing " symbol from input message at end
                        printf("Message sent from process %d to process %d:%s\n",rank,message.receiver,message.msg);
			printclockvalues(size,rank,Logical_Clock);              // calling user defined function to print vector clock values
                        MPI_Send(&message,1,Datatype_msg,message.receiver,18,MPI_COMM_WORLD);	//sending message to receiver
			MPI_Send(&Logical_Clock,size,MPI_INT,message.receiver,10,MPI_COMM_WORLD);	//sending senders logical clock as a message to receiver
                        strcpy(message.Event_Type,"     ");
                }
                if(strcmp(message.Event_Type,"recv")==0) 
                {
			MPI_Recv(&L_C,size,MPI_INT,MPI_ANY_SOURCE,10,MPI_COMM_WORLD,&status_LC);	//receiving senders logical clock as a message
                        if(L_C[status_LC.MPI_SOURCE]>=Logical_Clock[rank])	//checking senders and receivers logical clock value and performing relevant operation
                                Logical_Clock[status_LC.MPI_SOURCE] = L_C[status_LC.MPI_SOURCE]; 
			L_C[rank] = Logical_Clock[rank] + 1;
                        printf("Message received from process %d by process %d:%s\n",message.sender,rank,message.msg);
			printclockvalues(size,rank,L_C);		// calling user defined function to print vector clock values
			int z;
			for(z=1;z<size;z++)
                        {
                                Logical_Clock[z]=L_C[z];
                        }
	
                        MPI_Send(&message.msg,20,MPI_CHAR,0,20,MPI_COMM_WORLD);	//acknowledgement to p0
                }	
        }
   }
   return 0;
}

             

