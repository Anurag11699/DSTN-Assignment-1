#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

/*
PreConditions
Inputs: {maximum size of the process ready queue}

Purpose of the Function: Initialze Kernel Data Structure and its components

PostConditions
Output: {pointer to intialized kernel object}
*/
kernel* initialize_kernel(int max_number_of_processes)
{
    kernel* kernel_object = (kernel *)malloc(sizeof(kernel));

    //initialize pcb array
    kernel_object->pcb_array=(pcb *)malloc(max_number_of_processes*sizeof(pcb));
    kernel_object->number_of_processes=0;
    kernel_object->CR3_reg=-1;
    kernel_object->current_instruction=-1;
    kernel_object->currently_running_process_pid=-1;

    int i;
    for(i=0;i<max_number_of_processes;i++)
    {
        kernel_object->pcb_array[i].state=0;
        kernel_object->pcb_array[i].fd=NULL;
        kernel_object->pcb_array[i].outer_page_base_address=-1;
    }
    return kernel_object;
}


/*
PreConditions
Inputs: {pointer to kernel object, pointer to main memory object, maximum number of processes, pid of process, filename containing processes memory access requests}

Purpose of the Function: This function is used to load a new process into the system. The pcb array entry corresponding to this processes pid are filled. Also the first two pages of the process are pre paged using the page_walk function

PostConditions
Updated kernel and main memory objects
Output
Return Value:
1, if process was successfully loaded
-1, otherwise
*/
int load_new_process(kernel* kernel_object,main_memory* main_memory_object, int max_number_processes, int pid, char* filename)
{
    
    //process limit reached, this process cannot be loaded yet 
    if(kernel_object->number_of_processes>=max_number_processes)
    {
        return -1;
    }

    // if state==0, that means that entry in pcb array is free or the process before has been terminated
   
    //indexing pcb array using pid
    if(kernel_object->pcb_array[pid].state==0)
    {
        kernel_object->pcb_array[pid].state=1; //make the state ready to run
        kernel_object->pcb_array[pid].pid=pid;
        kernel_object->pcb_array[pid].fd=fopen(filename,"r");
        kernel_object->number_of_processes++;
        //break;
    }
    

    int pcb_array_entry=pid;
    
    int request_1;
    int request_2;

    fscanf(kernel_object->pcb_array[pcb_array_entry].fd,"%x",&request_1);
    fscanf(kernel_object->pcb_array[pcb_array_entry].fd,"%x",&request_2);

    //prepage 2 pages for the first 2 logical addresses the process requests
    //do page table walk for request1 and request2 to load these pages

    fprintf(output_fd,"PID: %d | Request: %x or %d\n\n",pid,request_1,request_1);

    //fprintf(stderr,"CHECK IN LOAD_NEW_PROCESS\n");
    //int frame_number_1;
    page_table_walk(kernel_object,main_memory_object,pid,request_1);

    fprintf(output_fd,"PID: %d | Request: %x or %d\n\n",pid,request_2,request_2);
    //int frame_number_2;
    page_table_walk(kernel_object,main_memory_object,pid,request_2);

    //should we put these into tlb?

   return 1;
}


/*
PreConditions
Inputs: {pointer to kernel object, pointer to main memory object, pid of process that has finished execution}

Purpose of the Function: This function is used terminate a process that has finished execution. All the pages that this process owns are transfered to the free frame list. The state of this process is set to 0 (terminated) and the file descriptor for the input file for this process is closed.

PostConditions
Updated kernel and main memory objects
*/
void terminate_process(kernel* kernel_object, main_memory* main_memory_object, int pid)
{
    int i;

    //release all the pages held by the process
    for(i=0;i<main_memory_object->number_of_frames;i++)
    {
        if(main_memory_object->frame_table[i].pid==pid)
        {
            transfer_to_free_frame_list(main_memory_object,i);
        }
    }

    //close the file descriptor of the process
    fclose(kernel_object->pcb_array[pid].fd);

    //set state of the process to terminated = 0
    kernel_object->pcb_array[pid].state=0;
    
}


/*
PreConditions
Inputs:{virtual address}
0<=virtual address<2^32

Purpose of the Function: This function classifies the type of request that the process asks for as instruction request or data request

PostConditions
Return Value:
0, if process requests for instruction
1, if process requests for data
*/
int get_request_type(int virtual_address)
{
    //the virtual address of instructions begins with 1 
    if(virtual_address>>28 == 0x1)
    {
        return 0;
    }

    return 1;
}

/*
PreConditions
Inputs: {pointer to kernel object, pointer to L1 tlb object, pointer to L2 tlb object, pid of process that has switched out, pid of process that is switched in}

Purpose of the Function: This function is used to change the execting process. The state of the process switched out is changed to ready and the state of the process switched is changed to executing. Also, both the L1 and L2 tlbs are flushed upon context switch.

PostConditions
Updated kernel and tlb objects
*/
void context_switch(kernel* kernel_object, tlb* L1_tlb, tlb* L2_tlb, int oldpid, int newpid)
{
    kernel_object->currently_running_process_pid=newpid;

    //state of the process switched out now set to ready to execute
    kernel_object->pcb_array[oldpid].state=1;

    //state of the process switched in now set to executing
    kernel_object->pcb_array[newpid].state=2;

    tlb_flush(L1_tlb);
    tlb_flush(L2_tlb);
}


/*
PreConditions
Inputs: {pointer to kernel object, pointer to L1 tlb , pointer to L2 tlb ,pointer to L1 instruction cache object, pointer to L1 data cache, pointer to L2 cache,pointer to L2 cache write buffer, pointer to main memory ,pid of process whose request it is}

Purpose of the Function: This is the function used to execute the process request. First both tlb's are checked, if the entry for logical page is not found in tlb, page walk for the logical page is initiated and the instruction is restarted. Upon tlb hit, we get the physical address and use it to access the cache for the required data needed.

PostConditions
Updated all memory levels accordingly 
*/
void execute_process_request(kernel* kernel_object, tlb* L1_tlb, tlb* L2_tlb, L1_cache* L1_instruction_cache_4KB, L1_cache* L1_data_cache_4KB ,L2_cache* L2_cache_32KB, L2_cache_write_buffer* L2_cache_write_buffer_8, main_memory* main_memory_32MB ,int pid,unsigned int virtual_address, int write)
{
    fprintf(output_fd,"\n\nExecuting Process Request for PID: %d | Logical Address: %x or %d\n",pid,virtual_address,virtual_address);

    //request type = 0 if read, request type = 1 if write
    int request_type=get_request_type(virtual_address);
    fprintf(output_fd,"Request Type %d\n",request_type);
    //we can get index and offest for L1 and L2 cache from the virtual address and use it for virtually tagged, physically offset. this is because (index + offset = page size)

    //last 5 bits of the virtual address as cache block size is 32bits
    int cache_block_offset_size=5;
    int cache_block_offset = get_cache_block_offset(virtual_address);

    int L1_cache_index_size=5;
    int L2_cache_index_size=6;
    int L1_cache_index = get_L1_cache_block_index(virtual_address,cache_block_offset_size);
    int L2_cache_index = get_L2_cache_block_index(virtual_address,cache_block_offset_size);
    
    //we can get cache_tags only after getting physical address
    int L1_cache_tag;
    int L2_cache_tag;

    int logical_page_number = get_logical_page_number(virtual_address); //as page size is 10bits, hence 10bit offset
    int physical_frame_number = complete_tlb_search(L1_tlb,L2_tlb,logical_page_number);

    //tlb hit, get direct physical frame number
    unsigned int physical_address;
    fprintf(output_fd,"Logical Page Number: %x or %d\n",logical_page_number,logical_page_number);
    // fprintf(output_fd,"Printing L1 tlb\n");
    // print_tlb(L1_tlb);
    // fprintf(output_fd,"Printing L2 tlb\n");
    // print_tlb(L2_tlb);

    if(physical_frame_number!=-1)
    {
        fprintf(output_fd,"TLB HIT\n");
        physical_address= get_physical_address(physical_frame_number,virtual_address);

        fprintf(output_fd,"Physical Frame Number: %d | Frame Offset: %d | Physical Address: %d\n",physical_frame_number,(virtual_address&get_frame_offset),physical_address);

        //use this physical addresss to search in cache

        //get the L1 and L2 cache tags from the physical address
        L1_cache_tag= get_L1_cache_tag(physical_address,L1_cache_index_size,cache_block_offset_size);
        L2_cache_tag= get_L2_cache_tag(physical_address,L2_cache_index_size,cache_block_offset_size);

        fprintf(output_fd,"L1 Cache Index: %d | L1 cache Tag: %d\n",L1_cache_index,L1_cache_tag);

        int L1_cache_hit;
        int L2_cache_hit;

        //as cache is lookaside, we need to search both the caches in parallel.
        if(request_type==0)
        {
            L1_cache_hit = L1_search(main_memory_32MB,L1_instruction_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,physical_frame_number,write); //as it is an instruction will be read only
        }
        else
        {
            L1_cache_hit = L1_search(main_memory_32MB,L1_data_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,physical_frame_number,write); 
        }

        L2_cache_hit = L2_search(main_memory_32MB,L2_cache_32KB,L2_cache_write_buffer_8,L2_cache_index,L2_cache_tag,cache_block_offset,physical_frame_number,write); 

        //if L1 cache or L2 cache was hit all good, otherwise need to request transfer of that block from main memory

        fprintf(output_fd,"Print L1 instruction cache\n");    
        //print_L1_cache(L1_instruction_cache_4KB);

        if(L1_cache_hit==1)
        {
            //cancel the request to L2 cache, and calculate the hit time
            fprintf(output_fd,"L1 Cache HIT\n");
            return; 
        }
        else if(L2_cache_hit==1)
        {
            //calculate the hit time
            fprintf(output_fd,"L2 Cache HIT\n");
            return;
        }

        //entry was not found in any cache. we must get the block from main memory and insert it into L1 cache

        
        //write code to check if frame is in main memory and to retrieve it and put into cache
        // int own_frame_needed = check_frame_ownership(main_memory_32MB,pid,physical_frame_number);
        // if(own_frame_needed!=1)
        // {

        // }

        
        //now, insert this new entry into L1 cache
        fprintf(output_fd,"L1 Cache Index: %d | L1 cache Tag: %d\n",L1_cache_index,L1_cache_tag);
        if(request_type==0)
        {
            
            replace_L1_cache_entry(L1_instruction_cache_4KB,L2_cache_32KB,L1_cache_index,L1_cache_tag,cache_block_offset);
        }
        else
        {
            replace_L1_cache_entry(L1_data_cache_4KB,L2_cache_32KB,L1_cache_index,L1_cache_tag,cache_block_offset);
        }

        fprintf(output_fd,"Print L1 instruction cache after Replacing\n");    
        //print_L1_cache(L1_instruction_cache_4KB);
        
        
        
    }    
    else
    {
        //initiatie page walk to get the physical frame number
        //recevie a physical frame number upon page walk and service page faults in that process
        int physical_frame_number_received_from_page_walk = page_table_walk(kernel_object,main_memory_32MB,pid,virtual_address);
        physical_address=get_physical_address(physical_frame_number,virtual_address);

        //insert this new mapping of logical page number to physical frame number into the L2 and L1 TLB and restart the instruction

        insert_new_tlb_entry(L2_tlb,logical_page_number,physical_frame_number_received_from_page_walk);
        insert_new_tlb_entry(L1_tlb,logical_page_number,physical_frame_number_received_from_page_walk);

        //restart this request
        execute_process_request(kernel_object,L1_tlb,L2_tlb,L1_instruction_cache_4KB,L1_data_cache_4KB,L2_cache_32KB,L2_cache_write_buffer_8,main_memory_32MB,pid,virtual_address,write);

    }


}