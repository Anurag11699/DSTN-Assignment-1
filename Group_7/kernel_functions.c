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

void execute_process_request(kernel* kernel_object, tlb* L1_tlb, tlb* L2_tlb, L1_cache* L1_instruction_cache_4KB, L1_cache* L1_data_cache_4KB ,L2_cache* L2_cache_32KB, main_memory* main_memory_32MB ,int virtual_address, int read)
{
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

        //as cache is lookaside, we need to search both the caches in parallel.
    }
    else
    {
        //initiatie page walk to get the physical frame number
    }
    

}