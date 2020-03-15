#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

kernel* initialize_kernel(int max_number_of_processes)
{
    kernel* kernel_object = (kernel *)malloc(sizeof(kernel));

    //initialize pcb array
    kernel_object->pcb_array=(pcb *)malloc(max_number_of_processes*sizeof(pcb));
    return kernel_object;
}

//return 0 if process is requesting an instruction, returns 1 if process is requesting for data
int get_request_type(int virtual_address)
{
    //the virtual address of instructions begins with 1 
    if(virtual_address&0x10000000)
    {
        return 0;
    }

    return 1;
}

void execute_process_request(kernel* kernel_object, tlb* L1_tlb, tlb* L2_tlb, L1_cache* L1_instruction_cache_4KB, L1_cache* L1_data_cache_4KB ,L2_cache* L2_cache_32KB, L2_cache_write_buffer* L2_cache_write_buffer_8, main_memory* main_memory_32MB ,int virtual_address, int read_write)
{
    int request_type=get_request_type(virtual_address);
    //we can get index and offest for L1 and L2 cache from the virtual address and use it for virtually tagged, physically offset. this is because (index + offset = page size)

    //last 5 bits of the virtual address as cache block size is 32bits
    int cache_block_offset_size=5;
    int cache_block_offset = (virtual_address & get_cache_block_offset);

    int L1_cache_index_size=5;
    int L2_cache_index_size=5;
    int L1_cache_index = ((virtual_address>>cache_block_offset_size)&get_L1_cache_block_index);
    int L2_cache_index = ((virtual_address>>cache_block_offset_size)&get_L2_cache_block_index);
    
    //we can get cache_tags only after getting physical address
    int L1_cache_tag;
    int L2_cache_tag;

    int logical_page_number = (virtual_address>>10); //as page size is 10bits, hence 10bit offset
    int physical_frame_number = complete_tlb_search(L1_tlb,12,L2_tlb,24,logical_page_number);

    //tlb hit, get direct physical address
    int physical_address;
    if(physical_frame_number!=-1)
    {
        physical_address=(physical_frame_number<<10)+(virtual_address&get_frame_offset);

        //use this physical addresss to search in cache;
        L1_cache_tag=(physical_address<<(L1_cache_index_size+cache_block_offset_size));
        L2_cache_tag=(physical_address<<(L2_cache_index_size+cache_block_offset_size));

        int L1_cache_hit;
        int L2_cache_hit;
        //as cache is lookaside, we need to search both the caches in parallel.
        if(request_type==0)
        {
            L1_cache_hit = L1_search(L1_instruction_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,read_write); //as it is an instruction will be read only
        }
        else
        {
            L1_cache_hit = L1_search(L1_data_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,read_write); 
        }

        L2_cache_hit = L2_search(L1_data_cache_4KB ,L1_cache_index,L1_cache_tag,cache_block_offset,read_write); 

        //if L1 cache or L2 cache was hit all good, otherwise need to request transfer of that block from main memory
        
        
    }    
    else
    {
        //initiatie page walk to get the physical frame number
        //recevie a physical frame number upon page walk and service page faults in that process
        int physical_frame_number_received_from_page_walk;
        physical_address=(physical_frame_number_received_from_page_walk<<10)+(virtual_address&get_frame_offset);

        //insert this new mapping of logical page number to physical frame number into the L2 and L1 TLB and restart the instruction

        insert_new_tlb_entry(L2_tlb,24,logical_page_number,physical_frame_number_received_from_page_walk);
        insert_new_tlb_entry(L1_tlb,12,logical_page_number,physical_frame_number_received_from_page_walk);

        //restart this request
        execute_process_request(kernel_object,L1_tlb,L2_tlb,L1_instruction_cache_4KB,L1_data_cache_4KB,L2_cache_32KB,L2_cache_write_buffer_8,main_memory_32MB,virtual_address,read_write);

    }

}