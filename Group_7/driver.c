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
   int max_number_of_processes=5;
   kernel* kernel_object = initialize_kernel(max_number_of_processes);
   main_memory* main_memory_32MB = initialize_main_memory(32,1); 
   tlb* L1_tlb = initialize_tlb(12);
   tlb *L2_tlb = initialize_tlb(24);
   L1_cache* L1_instruction_cache_4KB = initialize_L1_cache();
   L1_cache* L1_data_cache_4KB = initialize_L1_cache();
   L2_cache* L2_cache_32KB = initialize_L2_cache();
   L2_cache_write_buffer* L2_cache_write_buffer_8 = initialize_L2_cache_write_buffer(8);

   long long int number_of_requests_processed=0;
   unsigned int process_request;
   int executing_pid=0;
   int process_switch_instruction_count=200;

   
   output_fd=fopen("OUTPUT.txt","w");


   //loading all proccesses
   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,0,"APSI.txt");
   fseek(kernel_object->pcb_array[0].fd,0,SEEK_SET);

   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,1,"CC1.txt");
   fseek(kernel_object->pcb_array[1].fd,0,SEEK_SET);


   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,2,"LI.txt");
   fseek(kernel_object->pcb_array[2].fd,0,SEEK_SET);

   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,3,"M88KSIM.txt");
   fseek(kernel_object->pcb_array[3].fd,0,SEEK_SET);

   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,4,"VORTEX.txt");
   fseek(kernel_object->pcb_array[4].fd,0,SEEK_SET);

   int is_eof;
   int read_write;
   
   while(1)
   {
      if(number_of_requests_processed%process_switch_instruction_count==0)
      {
         executing_pid=(executing_pid+1)%(max_number_of_processes);
         context_switch(kernel_object,L1_tlb,L2_tlb,executing_pid);
      }

      is_eof=fscanf(kernel_object->pcb_array[executing_pid].fd,"%x",&process_request);

      if(is_eof==EOF)
      {
         terminate_process(kernel_object,main_memory_32MB,executing_pid);
         return 0;
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
      
      execute_process_request(kernel_object,L1_tlb,L2_tlb,L1_instruction_cache_4KB,L1_data_cache_4KB,L2_cache_32KB,L2_cache_write_buffer_8,main_memory_32MB,executing_pid,process_request,read_write);

      number_of_requests_processed++;
   }
   
      

   fclose(output_fd);

}

