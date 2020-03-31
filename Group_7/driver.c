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
   output_fd=fopen(outputfile,"w"); 

   FILE* input_fd = fopen(inputfile,"r");
   int max_number_of_processes;
   fscanf(input_fd,"%d",&max_number_of_processes);
   int j;


   //initialize total time taken as 0
   total_time_taken=0;

   number_of_tlb_hits=0;
   total_tlb_accesses=0;
   number_of_L1_cache_hits=0;
   total_L1_cache_accesses=0;
   number_of_L2_cache_hits=0;
   total_L2_cache_accesses=0;

   number_of_L1_tlb_searches=0;
   number_of_L2_tlb_searches=0;
   
   
   kernel* kernel_object = initialize_kernel(max_number_of_processes);
   main_memory* main_memory_32MB = initialize_main_memory(); 
   
   tlb* L1_tlb = initialize_tlb(12);
   tlb *L2_tlb = initialize_tlb(24);
   L1_cache* L1_instruction_cache_4KB = initialize_L1_cache();
   L1_cache* L1_data_cache_4KB = initialize_L1_cache();
   L2_cache* L2_cache_32KB = initialize_L2_cache();
   L2_cache_write_buffer* L2_cache_write_buffer_8 = initialize_L2_cache_write_buffer(8);

   //each process should ideally have these many frames
   number_of_frames_per_process_average = total_number_of_frames/max_number_of_processes;

   number_of_frames_per_process_lower_bound = number_of_frames_per_process_average/2;
   number_of_frames_per_process_upper_bound = number_of_frames_per_process_average+number_of_frames_per_process_lower_bound;

   long int number_of_requests_processed=0;
   unsigned int process_request;
   int executing_pid_index=1;
   int process_switch_instruction_count=200;
   int number_of_processes_ready=0;
   int pid_array[max_number_of_processes];

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

   char filename[100]={0};
   char x;


   //load all the new processes
   for(j=0;j<max_number_of_processes;j++)
   {  
      fscanf(input_fd,"%c",&x);
      fscanf(input_fd,"%[^\n]",filename);
      printf("Data from the file: %s\n", filename);

      load_new_process(kernel_object, main_memory_32MB,max_number_of_processes,j,filename);
      fseek(kernel_object->pcb_array[j].fd,j,SEEK_SET);
      pid_array[j]=j;
      number_of_processes_ready++;

   }
   fclose(input_fd);
 

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
         //sleep(1);
         for(j=executing_pid_index;j<number_of_processes_ready-1;j++)
         {
            pid_array[j]=pid_array[j+1];
         }
         number_of_processes_ready--;
         executing_pid_index=0;
         
         
         if(number_of_processes_ready==0)
         {
            fprintf(stderr,"Simulation Over\n");
            
            break;
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
   
   //printing simulation results

   fprintf(output_fd,"\n\nTotal Time Taken: %Lf ns\nNumber of Requests Processed: %ld\nEMAT: %Lf ns\n",total_time_taken,number_of_requests_processed,total_time_taken/number_of_requests_processed);

   fprintf(stderr,"\n\nTotal Time Taken: %Lf\nNumber of Requests Processed: %ld\nEMAT: %Lf ns\n",total_time_taken,number_of_requests_processed,total_time_taken/number_of_requests_processed);

   tlb_hit_rate=number_of_tlb_hits/total_tlb_accesses;
   fprintf(output_fd,"TLB Hit Rate: %Lf\n",tlb_hit_rate);
   fprintf(stderr,"TLB Hit Rate: %Lf\n",tlb_hit_rate);

   L1_cache_hit_rate=number_of_L1_cache_hits/total_L1_cache_accesses;
   fprintf(output_fd,"L1 Cache Hit Rate: %Lf\n",L1_cache_hit_rate);
   fprintf(stderr,"L1 Cache Hit Rate: %Lf\n",L1_cache_hit_rate);

   L2_cache_hit_rate=number_of_L2_cache_hits/total_L2_cache_accesses;
   fprintf(output_fd,"L2 Cache Hit Rate: %Lf\n",L2_cache_hit_rate);
   fprintf(stderr,"L2 Cache Hit Rate: %Lf\n",L2_cache_hit_rate);
   
   fclose(output_fd);
   

}

