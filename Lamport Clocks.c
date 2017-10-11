//*********************************************************************************************************************************************************************
//
// Ranjith Kumar Bodla
// Advanced Operating Systems
// Programming Project #2_1: Implementing Lamport Clocks
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
        char Event_Type[5];	// holds Exec, send in this program
        char msg[30];		// message to be transferred when send event is triggered
        int  sender;		// process sending the message
        int  receiver;		// process receiving the message
        int  Logical_Clock;	// variable holding the integer value of the lamport logical clock
}message;


int main(int argc,char** argv)
{
        int rank,size,lamport=0;
	//Declaring and initializing MPI Variables
	MPI_Request request;
        MPI_Status status;
        MPI_Init( &argc, &argv );
        MPI_Comm_rank( MPI_COMM_WORLD, &rank );
        MPI_Comm_size(MPI_COMM_WORLD , &size);
        //Steps used to create structure with different datatypes
	MPI_Datatype Datatype_msg;
        MPI_Datatype type[5] = {MPI_CHAR,MPI_CHAR,MPI_INT,MPI_INT,MPI_INT};
        int blocklength[5] = {5,30,1,1,1}; 
        MPI_Aint displacement[5] = {0,5,30+5,30+5+4,30+5+4+4}; 
        MPI_Type_create_struct(5,blocklength,displacement,type,&Datatype_msg); 
        MPI_Type_commit(&Datatype_msg); 	// Committing the new datatype named Datatype_msg
		
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

                if(Inputfile == NULL)
                      fprintf(stderr,"There is no file\n");

                line[count]=getc(Inputfile);
		//Iterating while loop till end of file
                while(line[count]!=EOF) 
		{
                        while(line[count]!='\n')	//iterating while loop till new line is encountered
                        {
                                count=count+1;
                                line[count] = getc(Inputfile);	//reading each character at a time and storing it in the array
                        }
                        line[count] ='\0';
                        Temp = strtok(line," "); 		//strtok used to get the number of processes being declared in first line
                        count =0;
                        length = 0;   
                        line_parts[count] = Temp;

                        while(Temp!=NULL)			// this starts for the second line
                        {
                                Temp = strtok(NULL," ");	// retrieving the rest of the string variables delimited by a space
                                count=count+1;
                                line_parts[count] = Temp;

                                if(count>=3)			// checking if the event type is "send"
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

                        if(strcmp("exec",event_type)== 0)  //comparing if event_type is exec
                        {
                                sender = atoi(line_parts[1]);	//get to whom the message must be sent
                                memset(message.Event_Type,0,5); 	//clearing the contents of the event_type
                                strcpy(message.Event_Type,"exec");	
                                MPI_Send(&message,1,Datatype_msg,sender,18,MPI_COMM_WORLD); 
				MPI_Recv(msg,20,MPI_CHAR,MPI_ANY_SOURCE,12,MPI_COMM_WORLD,&status);		//used as an acknowledgement
                        }
                        else if(strcmp("end",event_type)==0)	// comparing if event_type is end which states end of file
                        {
                                memset(message.Event_Type,0,5);	
                                strcpy(message.Event_Type,"end");
			// for loop running for n-1 number of proceses (excluding master process). This loop is used to send message to n-1 number of prcess that
			// file has come to an end and send the final lamport logical clock values at each process to print it to output
                                 for(count = 1;count<size;count++) 
                                 {
                                           MPI_Send(&message,1,Datatype_msg,count,18,MPI_COMM_WORLD);
                                           MPI_Recv(&lamport,1,MPI_INT,count,24,MPI_COMM_WORLD,&status);
                                          //ranks[count] = count;
                                           //clocks[count] = lamport;
					   printf("process with rank %d has final logical clock value - %d \n",count,lamport);
                                 }
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
                               MPI_Recv(msg,20,MPI_CHAR,receiver,12,MPI_COMM_WORLD,&status);	//used as an acknowledgement
                      }

                      count =0;
                      line[count]=getc(Inputfile); 
            }
            free(line_parts); 	// used to empty the dynamically created contents
        }
		//Code executed by all other processes except p0
        if(rank!=0)
        {
            while(1)	//to be able to run multiple times
            {

                MPI_Recv(&message,1,Datatype_msg,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status); 

                if(strcmp(message.Event_Type,"exec")==0) 
                {
                        lamport=lamport+1;	// increment lamport value by 1 on exec event
                        printf("Execution event in process %d\n",rank);
                        printf("Logical time at process %d is %d\n",rank,lamport);
			MPI_Send(message.msg,10,MPI_CHAR,0,12,MPI_COMM_WORLD);	//sending the message as only an acknowledgement as completion of event
                }

                if(strcmp(message.Event_Type,"end")==0) 
                {
                        MPI_Send(&lamport,1,MPI_INT,0,24,MPI_COMM_WORLD);
                        break;
                }
                if(strcmp(message.Event_Type,"send")==0) 
                {
                        strcpy(message.Event_Type,"recv"); 
                        lamport=lamport+1;	// increment lamport value by 1 on exec event
                        message.Logical_Clock = lamport;	//sending the senders lamport value to compare on receivers lamport value
                        message.msg[0]= ' ';	// removing " symbol from input message at start
                        message.msg[strlen(message.msg)-2]  = '\0';	// removing " symbol from input message at end
                        printf("Message sent from process %d to process %d:%s\n",rank,message.receiver,message.msg);
			printf("Logical time at process %d is %d\n",rank,lamport);
                        MPI_Send(&message,1,Datatype_msg,message.receiver,12,MPI_COMM_WORLD);	//sending message to receiver
                        strcpy(message.Event_Type,"     ");
                }
                if(strcmp(message.Event_Type,"recv")==0) 
                {
			// implementing the lamport functionality on send event
                        if(message.Logical_Clock>=lamport)
                                lamport = message.Logical_Clock+1; 
                        else
                                lamport=lamport+1;

                        printf("Message received from process %d by process %d:%s\n",message.sender,rank,message.msg);
                        printf("Logical time at process %d is %d\n",rank,lamport);
                        MPI_Send(message.msg,10,MPI_CHAR,0,12,MPI_COMM_WORLD);	//acknowledgement to p0
                }
        }
   }
   MPI_Finalize();
   return 0;
}

                

