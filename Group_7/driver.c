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
   L2_cache_write_buffer* L2_cache_write_buffer_8 = initialize_L2_cache_write_buffer();

   output_fd=fopen("OUTPUT.txt","w");

   load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,0,"APSI.txt");
   fseek(kernel_object->pcb_array[0].fd,0,SEEK_SET);
   unsigned int process_request;

   for (int i = 0; i < 2000; i++)
   {
      fscanf(kernel_object->pcb_array[0].fd,"%x",&process_request);
      execute_process_request(kernel_object,L1_tlb,L2_tlb,L1_instruction_cache_4KB,L1_data_cache_4KB,L2_cache_32KB,L2_cache_write_buffer_8,main_memory_32MB,0,process_request,0);
   }
   
      

   fclose(output_fd);

}

