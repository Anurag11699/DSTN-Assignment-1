#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<assert.h>
#include "functions.h"

/*
PreConditions
Inputs: {maximum size of the process ready queue}
max_number_of_prcocesses>=0

Purpose of the Function: Initialze Kernel Data Structure and its components

PostConditions
Output: {pointer to intialized kernel object}
*/
kernel* initialize_kernel(int max_number_of_processes)
{
    //check PreConditions
    assert(max_number_of_processes>=0 && "Number of Processes must be >= 0d");

    kernel* kernel_object = (kernel *)malloc(sizeof(kernel));

    //initialize pcb array
    kernel_object->pcb_array=(pcb *)malloc(max_number_of_processes*sizeof(pcb));
    kernel_object->number_of_processes=0;
    kernel_object->max_number_of_processes=max_number_of_processes;
    //kernel_object->CR3_reg=-1;
    //kernel_object->current_instruction=-1;
    kernel_object->currently_running_process_pid=-1;

    int i;
    for(i=0;i<max_number_of_processes;i++)
    {
        kernel_object->pcb_array[i].state=0;
        kernel_object->pcb_array[i].fd=NULL;
        kernel_object->pcb_array[i].outer_page_base_address=0;
        kernel_object->pcb_array[i].outer_page_base_address_initialized_before=0;
        kernel_object->pcb_array[i].outer_page_table=NULL;
        kernel_object->pcb_array[i].number_of_frames_used=0;
    }
    return kernel_object;
}


/*
PreConditions
Inputs: {pointer to kernel object, pointer to main memory object, pid of process, filename containing processes memory access requests}

kernel_object!=NULL
main_memory_object!=NULL
filename!=NULL
0<=pid<maximum number of processes


Purpose of the Function: This function is used to load a new process into the system. The pcb array entry corresponding to this processes pid are filled. Also the first two pages of the process are pre paged using the page_walk function

PostConditions
Updated kernel and main memory objects
Output
Return Value:
1, if process was successfully loaded
-1, otherwise
*/
int load_new_process(kernel* kernel_object,main_memory* main_memory_object, int pid, char* filename)
{

    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");
    assert(filename!=NULL && "Filename cannot be NULL");

    assert(pid<kernel_object->max_number_of_processes && "pid exceeded bounds");
    assert(pid>=0 && "pid less than 0 not allowed");

    fprintf(output_fd,"Loading New Process with PID: %d\n\n",pid);
    fflush(output_fd);

    
    //process limit reached, this process cannot be loaded yet 
    if(kernel_object->number_of_processes>=kernel_object->max_number_of_processes)
    {
        return -1;
    }

    // if state==0, that means that entry in pcb array is free or the process before has been terminated
   
    //indexing pcb array using pid
    if(kernel_object->pcb_array[pid].state==TASK_TERMINATED)
    {
        kernel_object->pcb_array[pid].state=TASK_READY; //make the state ready to run
        kernel_object->pcb_array[pid].pid=pid;
        kernel_object->pcb_array[pid].fd=fopen(filename,"r");
        kernel_object->number_of_processes++;
        //break;
    }
    

    int pcb_array_entry=pid;
    
    unsigned long request_1;
    unsigned long request_2;

    fscanf(kernel_object->pcb_array[pcb_array_entry].fd,"%lx",&request_1);
    fscanf(kernel_object->pcb_array[pcb_array_entry].fd,"%lx",&request_2);


    //add time to transfer the process pages from disk to Virtual Memory
    total_time_taken = total_time_taken + average_disk_to_from_swap_space_transfer_time_entire_process;


    //prepage 2 pages for the first 2 logical addresses the process requests
    //do page table walk for request1 and request2 to load these pages

    fprintf(output_fd,"PID: %d | Request: %lx (hexadecimal) or %ld (decimal)\n\n",pid,request_1,request_1);
    fflush(output_fd);

    //fprintf(stderr,"CHECK IN LOAD_NEW_PROCESS\n");
    //int frame_number_1;
    page_table_walk(kernel_object,main_memory_object,pid,request_1);

    fprintf(output_fd,"PID: %d | Request: %lx (hexadecimal) or %ld (decimal)\n\n",pid,request_2,request_2);
    fflush(output_fd);
    //int frame_number_2;
    page_table_walk(kernel_object,main_memory_object,pid,request_2);

    
   return 1;
}


/*
PreConditions
Inputs: {pointer to kernel object, pointer to main memory object, pid of process that has finished execution}
kernel_object!=NULL
main_memory_object!=NULL
0<=pid<maximum number of processes

Purpose of the Function: This function is used terminate a process that has finished execution. All the pages that this process owns are transfered to the free frame list. The state of this process is set to 0 (terminated) and the file descriptor for the input file for this process is closed.

PostConditions
Updated kernel and main memory objects
*/
void terminate_process(kernel* kernel_object, main_memory* main_memory_object, int pid)
{
    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(pid<kernel_object->max_number_of_processes && "pid exceeded bounds");
    assert(pid>=0 && "pid less than 0 not allowed");

    // fprintf(output_fd,"Terminating Process: %d | Has %d frames\n",pid, kernel_object->pcb_array[pid].number_of_frames_used);
    // fflush(output_fd);

    //print_ufl(main_memory_object);

    //transfer all the frames of this process from used frame list to free frame list.
    transfer_to_free_frame_list(kernel_object,main_memory_object,pid,kernel_object->pcb_array[pid].number_of_frames_used);

    // fprintf(output_fd,"After Terminating Process: %d | Has %d frames\n",pid, kernel_object->pcb_array[pid].number_of_frames_used);
    // fflush(output_fd);

    //print_ufl(main_memory_object);

    //add time to transfer the process pages from Virtual Memory to Disk
    total_time_taken = total_time_taken + average_disk_to_from_swap_space_transfer_time_entire_process;

    //close the file descriptor of the process
    fclose(kernel_object->pcb_array[pid].fd);

    //set state of the process to terminated = 0
    kernel_object->pcb_array[pid].state=TASK_TERMINATED;

    kernel_object->number_of_processes--;
    
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
    //check PreConditions
    assert(virtual_address >= 0x0 && "Virtual Address must be >= 0x0");
    assert(virtual_address <= 0xffffffff && "Virtual Address must be <= 0xffffffff");


    //the virtual address of instructions begins with 1 
    if(virtual_address>>28 == 0x1)
    {
        return INSTRUCTION_REQUEST;
    }

    return DATA_REQUEST;
}

/*
PreConditions
Inputs: {pointer to kernel object, pointer to L1 tlb object, pointer to L2 tlb object, pid of process that is switched in}
kernel_object!=NULL
L1_tlb!=NULL
L2_tlb!=NULL
0<=newpid<maximum number of processes

Purpose of the Function: This function is used to change the execting process. The state of the process switched out is changed to ready and the state of the process switched is changed to executing. Also, both the L1 and L2 tlbs are flushed upon context switch.

PostConditions
Updated kernel and tlb objects
*/
void context_switch(kernel* kernel_object, tlb* L1_tlb, tlb* L2_tlb, int newpid)
{

    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(L1_tlb!=NULL && "pointer to L1 tlb cannot be NULL");
    assert(L2_tlb!=NULL && "pointer to L2 tlb cannot be NULL");

    assert(newpid<kernel_object->max_number_of_processes && "newpid exceeded bounds");
    assert(newpid>=0 && "newpid less than 0 not allowed");

    //state of the process switched out now set to ready to execute
    kernel_object->pcb_array[kernel_object->currently_running_process_pid].state=TASK_READY;

    kernel_object->currently_running_process_pid=newpid;

    

    //state of the process switched in now set to executing
    kernel_object->pcb_array[newpid].state=TASK_EXECUTING;

    tlb_flush(L1_tlb);
    tlb_flush(L2_tlb);

    //add context switch time
    total_time_taken = total_time_taken + context_switch_time;
}


/*
PreConditions
Inputs: {pointer to kernel object, pointer to L1 tlb , pointer to L2 tlb ,pointer to L1 instruction cache object, pointer to L1 data cache, pointer to L2 cache,pointer to L2 cache write buffer, pointer to main memory ,pid of process whose request it is}

kernel_object!=NULL
L1_tlb!=NULL
L2_tlb!=NULL
L1_instruction_cache_4KB!=NULL
L1_data_cache_4KB!=NULL
L2_cache_32KB!=NULL
L2_cache_write_buffer_8!=NULL
main_memory_32MB!=NULL
0<=virtual_address <= 0xffffffff
0<=pid<maximum number of processes

Purpose of the Function: This is the function used to execute the process request. First both tlb's are checked, if the entry for logical page is not found in tlb, page walk for the logical page is initiated and the instruction is restarted. Upon tlb hit, we get the physical address and use it to access the cache for the required data needed.

PostConditions
Updated all memory levels accordingly 
*/
void execute_process_request(kernel* kernel_object, tlb* L1_tlb, tlb* L2_tlb, L1_cache* L1_instruction_cache_4KB, L1_cache* L1_data_cache_4KB ,L2_cache* L2_cache_32KB, L2_cache_write_buffer* L2_cache_write_buffer_8, main_memory* main_memory_32MB ,int pid, unsigned long int virtual_address, int write)
{
    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(L1_tlb!=NULL && "pointer to L1 tlb cannot be NULL");
    assert(L2_tlb!=NULL && "pointer to L2 tlb cannot be NULL");
    assert(L1_instruction_cache_4KB!=NULL && "pointer to L1 instruction cache cannot be NULL");
    assert(L1_data_cache_4KB!=NULL && "pointer to L1 data cache cannot be NULL");
    assert(L2_cache_32KB!=NULL && "pointer to L2 cache cannot be NULL");
    assert(L2_cache_write_buffer_8!=NULL && "pointer to L2 cache write buffer cannot be NULL");
    assert(main_memory_32MB!=NULL && "pointer to main memory cannot be NULL");

    assert(virtual_address >= 0x0 && "Virtual Address must be >= 0x0");
    assert(virtual_address <= 0xffffffff && "Virtual Address must be <= 0xffffffff");

    assert(pid<kernel_object->max_number_of_processes && "pid exceeded bounds");
    assert(pid>=0 && "pid less than 0 not allowed");


    fprintf(output_fd,"\n\nExecuting Process Request for PID: %d | Logical Address: %lx (hexadecimal) or %ld (decimal)\n",pid,virtual_address,virtual_address);
    fflush(output_fd);

    //request type = 0 if instruction, request type = 1 if data
    int request_type=get_request_type(virtual_address);
    // fprintf(output_fd,"Request Type %d\n",request_type);
    // fflush(output_fd);

    //we can get index and offest for L1 and L2 cache from the virtual address and use it for virtually tagged, physically offset. this is because (index + offset = page size)

    //last 5 bits of the virtual address as cache block size is 32bits

    //we get cache offset and index directly from the virtual address as offset+index = page size {virtually tagged, physically offset}
   
    int cache_block_offset = get_cache_block_offset(virtual_address);

    
    int L1_cache_index = get_L1_cache_block_index(virtual_address,cache_block_offset_size);
    int L2_cache_index = get_L2_cache_block_index(virtual_address,cache_block_offset_size);
    
    //we can get cache_tags only after getting physical address
    int L1_cache_tag;
    int L2_cache_tag;

    int logical_page_number = get_logical_page_number(virtual_address,frame_size); //as page size is 10bits, hence 10bit offset

    //TLB SEARCH
    int physical_frame_number=-1;
    int time_taken_for_tlb_lookup = complete_tlb_search(L1_tlb,L2_tlb,logical_page_number,&physical_frame_number);

    total_tlb_accesses++;

    //tlb hit, get direct physical frame number
    unsigned int physical_address;
    fprintf(output_fd,"Logical Page Number: %x (hexdecimal) or %d (decimal) \n",logical_page_number,logical_page_number);
    fflush(output_fd);
    // fprintf(output_fd,"Printing L1 tlb\n");
    //fflush(output_fd);
    // print_tlb(L1_tlb);
    // fprintf(output_fd,"Printing L2 tlb\n");
    //fflush(output_fd);
    // print_tlb(L2_tlb);

    if(physical_frame_number!=-1)
    {
        fprintf(output_fd,"TLB HIT\n");
        fflush(output_fd);
        number_of_tlb_hits++;

        physical_address= get_physical_address(physical_frame_number,virtual_address);

        fprintf(output_fd,"Physical Frame Number: %d (decimal) | Frame Offset: %ld (decimal) | Physical Address: %d (decimal)\n",physical_frame_number,(virtual_address&get_frame_offset),physical_address);
        fflush(output_fd);

        //search in cache
        total_cache_accesses++;

        //use this physical addresss to search in cache

        //get the L1 and L2 cache tags from the physical address
        L1_cache_tag= get_L1_cache_tag(physical_address,L1_cache_index_size,cache_block_offset_size);
        L2_cache_tag= get_L2_cache_tag(physical_address,L2_cache_index_size,cache_block_offset_size);

        //fprintf(output_fd,"L1 Cache Index: %d | L1 cache Tag: %d\n",L1_cache_index,L1_cache_tag);

        int L1_cache_hit=-1;
        int L2_cache_hit=-1;

        //as cache is lookaside, we need to search both the caches in parallel.
        //we need to check request type. if process is requesting for instruction, search in 
        if(request_type==INSTRUCTION_REQUEST)
        {
            L1_cache_hit = L1_search(main_memory_32MB,L1_instruction_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,physical_frame_number,write); //as it is an instruction will be read only, write field will be 0
        }
        else if(request_type==DATA_REQUEST)
        {
            L1_cache_hit = L1_search(main_memory_32MB,L1_data_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,physical_frame_number,write); 
        }
        total_L1_cache_accesses++;

       
        if(L1_cache_hit==-1)
        {
            total_L2_cache_accesses++;
            //searching L2 cache in parallel
            L2_cache_hit = L2_search(main_memory_32MB,L2_cache_32KB,L2_cache_write_buffer_8,L2_cache_index,L2_cache_tag,cache_block_offset,physical_frame_number,write); 
        }
            
        
       

        //if L1 cache or L2 cache was hit all good, otherwise need to request transfer of that block from main memory

        //fprintf(output_fd,"Print L1 instruction cache\n");    
        //fflush(output_fd);
        //print_L1_cache(L1_instruction_cache_4KB);

        // as was lookaside, we only add the times taken by one of them, not both   
        if(L1_cache_hit!=-1)
        {
            //cancel the request to L2 cache, and add time taken by L1 cache to do the required as returned by the L1 cache search function

            total_time_taken = total_time_taken + L1_cache_hit;

            //add time taken for tlb and cache index search. add only max of them as they were done in parallel (virtually tagged, physically indexed)

            total_time_taken = total_time_taken + max(time_taken_for_tlb_lookup, L1_cache_indexing_time);

            number_of_L1_cache_hits++;

            fprintf(output_fd,"L1 Cache HIT\n");
            fflush(output_fd);
            return; 
        }
        else if(L2_cache_hit!=-1)
        {
            //add the time taken by L2 cache to process request 

            total_time_taken = total_time_taken + L2_cache_hit + time_taken_for_tlb_lookup;
            
            number_of_L2_cache_hits++;

            // fprintf(output_fd,"L2 Cache Index: %d | L2 cache Tag: %d\n",L2_cache_index,L2_cache_tag);
            // fflush(output_fd);

            // fprintf(output_fd,"L1 Cache Index: %d | L1 cache Tag: %d\n",L1_cache_index,L1_cache_tag);
            // fflush(output_fd);

            // if(request_type==0)
            // {
            //     print_L1_cache(L1_instruction_cache_4KB);
            // }
            // else
            // {
            //     print_L1_cache(L1_data_cache_4KB);
            // }

            // print_L2_cache(L2_cache_32KB);
            


            //need to enter this entry into the L1 cache for performance improvement and remove from L2 cache as caches are exclusive

            //removing entry from L2 cache
            remove_L2_cache_block(L2_cache_32KB,L2_cache_index,L2_cache_tag,cache_block_offset);

            //add this entry to L1 cache
            if(request_type==INSTRUCTION_REQUEST)
            {
                replace_L1_cache_entry(L1_instruction_cache_4KB,L2_cache_32KB,L1_cache_index,L1_cache_tag,cache_block_offset);
            }
            else if(request_type==DATA_REQUEST)
            {
                replace_L1_cache_entry(L1_data_cache_4KB,L2_cache_32KB,L1_cache_index,L1_cache_tag,cache_block_offset);
            }

            //add time taken to transfer this entry to L1 cache to be replaced
            total_time_taken = total_time_taken + L1_cache_to_from_L2_cache_transfer_time;

            // if(request_type==0)
            // {
            //     print_L1_cache(L1_instruction_cache_4KB);
            // }
            // else
            // {
            //     print_L1_cache(L1_data_cache_4KB);
            // }

            //print_L2_cache(L2_cache_32KB);

            fprintf(output_fd,"L2 Cache HIT\n");
            fflush(output_fd);
            return;
        }

        // add time to search in the caches. take max of the two times as we search lookaside

        //time taken to search in L1 cache is max(L1 indexing time,Tlb lookup time)+L1 tag comparison time
        //time taken to search in L2 cache is L2 cache search time
        total_time_taken = total_time_taken +max((max(time_taken_for_tlb_lookup,L1_cache_indexing_time)+L1_cache_tag_comparison_time), L2_cache_search_time);

        //entry was not found in any cache. we must get the block from main memory and insert it into L1 cache
        number_of_cache_misses++;

        fprintf(output_fd,"CACHE MISS\n");
        fflush(output_fd);

        fprintf(output_fd,"Getting data from MM\n");
        fflush(output_fd);
        //now, insert this new entry into L1 cache. add time taken to transfer this entry from main memory to L1 cache
        total_time_taken = total_time_taken + L1_cache_to_from_main_memory_transfer_time;

        //fprintf(output_fd,"L1 Cache Index: %d | L1 cache Tag: %d\n",L1_cache_index,L1_cache_tag);
        //fflush(output_fd);
        if(request_type==INSTRUCTION_REQUEST)
        {
            
            replace_L1_cache_entry(L1_instruction_cache_4KB,L2_cache_32KB,L1_cache_index,L1_cache_tag,cache_block_offset);

        }
        else if(request_type==DATA_REQUEST)
        {
            replace_L1_cache_entry(L1_data_cache_4KB,L2_cache_32KB,L1_cache_index,L1_cache_tag,cache_block_offset);
        }

        //fprintf(output_fd,"Print L1 instruction cache after Replacing\n");   
        //fflush(output_fd) 
        //print_L1_cache(L1_instruction_cache_4KB);


        if(request_type==INSTRUCTION_REQUEST)
        {
            L1_cache_hit = L1_search(main_memory_32MB,L1_instruction_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,physical_frame_number,write); //as it is an instruction will be read only, write field will be 0
        }
        else if(request_type==DATA_REQUEST)
        {
            L1_cache_hit = L1_search(main_memory_32MB,L1_data_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,physical_frame_number,write); 
        }

        total_L1_cache_accesses++;

        if(L1_cache_hit!=-1)
        {
            fprintf(output_fd,"L1 Cache HIT\n");
            //fflush(output_fd);

            total_time_taken = total_time_taken + L1_cache_indexing_time + L1_cache_tag_comparison_time;

            total_time_taken = total_time_taken + L1_cache_hit;

            number_of_L1_cache_hits++;
        }
        
        
        
    }    
    else
    {
        fprintf(output_fd,"TLB MISS\n");
        fflush(output_fd);

        //add time taken for tlb search
        total_time_taken = total_time_taken + time_taken_for_tlb_lookup;

        //initiatie page walk to get the physical frame number
        //recevie a physical frame number upon page walk and service page faults in that process
        int physical_frame_number_received_from_page_walk = page_table_walk(kernel_object,main_memory_32MB,pid,virtual_address);
        physical_address=get_physical_address(physical_frame_number,virtual_address);



        //insert this new mapping of logical page number to physical frame number into the L2 and L1 TLB and restart the instruction
        
        insert_new_tlb_entry(L2_tlb,logical_page_number,physical_frame_number_received_from_page_walk);
        //add time taken to insert into L2 tlb
        total_time_taken = total_time_taken + processor_to_from_L2_tlb_transfer_time;

        insert_new_tlb_entry(L1_tlb,logical_page_number,physical_frame_number_received_from_page_walk);
        //add time taken to insert into L1 tlb
        total_time_taken = total_time_taken + processor_to_from_L1_tlb_transfer_time;

        //restart this request
        fprintf(output_fd,"Restart Instruction after doing page walk and updating TLB\n");
        fflush(output_fd);

        execute_process_request(kernel_object,L1_tlb,L2_tlb,L1_instruction_cache_4KB,L1_data_cache_4KB,L2_cache_32KB,L2_cache_write_buffer_8,main_memory_32MB,pid,virtual_address,write);

    }


}