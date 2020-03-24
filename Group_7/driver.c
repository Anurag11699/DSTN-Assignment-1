#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

// Random Function to that returns 0 or 1 with 
// equal probability 
int rand50() 
{ 
	// rand() function will generate odd or even 
	// number with equal probability. If rand() 
	// generates odd number, the function will 
	// return 1 else it will return 0. 
	return rand() & 1; 
} 

// Random Function to that returns 1 with 75% 
// probability and 0 with 25% probability using 
// Bitwise OR 
int rand75() 
{ 
	return rand50() | rand50(); 
} 


int main()
{
   output_fd=fopen("OUTPUT.txt","w"); 
   int max_number_of_processes=5;
   
   //initialize total time taken as 0
   total_time_taken=0;
   int j;
   kernel* kernel_object = initialize_kernel(max_number_of_processes);
   main_memory* main_memory_32MB = initialize_main_memory(32,1); 
   
   tlb* L1_tlb = initialize_tlb(12);
   tlb *L2_tlb = initialize_tlb(24);
   L1_cache* L1_instruction_cache_4KB = initialize_L1_cache();
   L1_cache* L1_data_cache_4KB = initialize_L1_cache();
   L2_cache* L2_cache_32KB = initialize_L2_cache();
   L2_cache_write_buffer* L2_cache_write_buffer_8 = initialize_L2_cache_write_buffer(8);

   long int number_of_requests_processed=0;
   unsigned int process_request;
   int executing_pid_index=-1;
   int process_switch_instruction_count=200;
   int number_of_processes_ready=0;
   int pid_array[5]={0};

   // while(1)
   // {
   //    fprintf(stderr,"%d\n",get_frame(kernel_object,main_memory_32MB));
   // }

   //return 0;

//    print_tlb(L1_tlb);
//    print_tlb(L2_tlb);

//    insert_new_tlb_entry(L2_tlb,5,6);
//    L2_to_L1_tlb_transfer(L1_tlb,L2_tlb,5);

//    print_tlb(L1_tlb);
//    print_tlb(L2_tlb);
   
//   return 0;


   //loading all proccesses
   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,0,"APSI.txt");
   fseek(kernel_object->pcb_array[0].fd,0,SEEK_SET);
   pid_array[0]=0;
   number_of_processes_ready++;

   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,1,"CC1.txt");
   fseek(kernel_object->pcb_array[1].fd,0,SEEK_SET);
   pid_array[1]=1;
   number_of_processes_ready++;


   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,2,"LI.txt");
   fseek(kernel_object->pcb_array[2].fd,0,SEEK_SET);
   pid_array[2]=2;
   number_of_processes_ready++;

   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,3,"M88KSIM.txt");
   fseek(kernel_object->pcb_array[3].fd,0,SEEK_SET);
   pid_array[3]=3;
   number_of_processes_ready++;

   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,4,"VORTEX.txt");
   fseek(kernel_object->pcb_array[4].fd,0,SEEK_SET);
   pid_array[4]=4;
   number_of_processes_ready++;

   int is_eof;
   int read_write;
   
   int oldpid;
   int newpid;

   while(1)
   {
      if(number_of_requests_processed%process_switch_instruction_count==0)
      {
         oldpid=pid_array[executing_pid_index];
         executing_pid_index=(executing_pid_index+1)%(number_of_processes_ready);
         newpid=pid_array[executing_pid_index];
         context_switch(kernel_object,L1_tlb,L2_tlb,oldpid,newpid);
      }

      is_eof=fscanf(kernel_object->pcb_array[pid_array[executing_pid_index]].fd,"%x",&process_request);

      if(is_eof==EOF)
      {
         oldpid=pid_array[executing_pid_index];
         //terminate the process who has no more requests to process
         terminate_process(kernel_object,main_memory_32MB,pid_array[executing_pid_index]);

         //remove element of array pointed to by executing_pid_index
         fprintf(stderr,"Process %d Over\n",pid_array[executing_pid_index]);
         sleep(1);
         for(j=executing_pid_index;j<number_of_processes_ready-1;j++)
         {
            pid_array[j]=pid_array[j+1];
         }
         number_of_processes_ready--;
         executing_pid_index=0;
         
         
         if(number_of_processes_ready==0)
         {
            fprintf(stderr,"Simulation Over\n");
            fclose(output_fd);
            return 0;
         }

         newpid=pid_array[executing_pid_index];
         context_switch(kernel_object,L1_tlb,L2_tlb,oldpid,newpid);
         continue;
        
      }

      //if the process is requesting for an instruction, it can be Read Only
      if(get_request_type(process_request)==0)
      {
         read_write=0;
      }
      else
      {
         read_write=abs(1-rand75()); //this will produce 75% data requests as read and 25% data requests as write
      }
      
      fprintf(stderr,"Executing PID: %d\n",pid_array[executing_pid_index]);

      execute_process_request(kernel_object,L1_tlb,L2_tlb,L1_instruction_cache_4KB,L1_data_cache_4KB,L2_cache_32KB,L2_cache_write_buffer_8,main_memory_32MB,pid_array[executing_pid_index],process_request,read_write);

      number_of_requests_processed++;
   }
   
      

   fclose(output_fd);

}

