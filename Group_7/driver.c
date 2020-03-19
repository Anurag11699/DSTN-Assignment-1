#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"


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
   
   // while(1)
   // {
   //    fscanf(kernel_object->pcb_array[0].fd,"%x",&process_request);
   //    execute_process_request(kernel_object,L1_tlb,L2_tlb,L1_instruction_cache_4KB,L1_data_cache_4KB,L2_cache_32KB,L2_cache_write_buffer_8,main_memory_32MB,0,process_request,1);
   // }
   
      

   fclose(output_fd);

}

