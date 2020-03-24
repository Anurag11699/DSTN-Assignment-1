#ifndef CONFIGURATION_FILE_H
#define CONFIGURATION_FILE_H

//define times needed for all the different memory accesses in ns

//all transfer times are for a single block 

#define L1_tlb_search_time 1
#define L2_tlb_search_time 1
#define L1_tlb_miss_OS_overhead 1
#define L2_tlb_miss_OS_overhead 1
#define processor_to_from_L1_tlb_transfer_time 1
#define processor_to_from_L2_tlb_transfer_time 1
#define L1_tlb_to_from_L2_tlb_transfer_time 1
#define L1_cache_search_time 1 
#define L2_cache_search_time 1
#define processor_to_from_L1_cache_transfer_time 1
#define processor_to_from_L2_cache_transfer_time 1 
#define L1_cache_to_from_L2_cache_transfer_time 1
#define L1_cache_to_from_main_memory_transfer_time 1
#define L2_cache_to_from_main_memory_transfer_time 1
#define L2_cache_write_buffer_search_time 1
#define L2_cache_to_L2_cache_write_buffer_transfer_time 1
#define L2_cache_write_buffer_to_main_memory_transfer_time 1
#define main_memory_access_time 1
#define main_memory_to_disk_transfer_time 1
#define main_memory_from_disk_transfer_time 1
#define restart_overhead_time 1
#define context_switch_time 1

#endif /*CONFIGURATION_FILE_H*/