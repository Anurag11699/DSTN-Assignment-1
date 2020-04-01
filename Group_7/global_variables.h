#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

//global variables
FILE* output_fd;
long double total_time_taken;

//used to keep track of when to shift their respective counters
int number_of_L1_tlb_searches;
int number_of_L2_tlb_searches;

//lower and upper bound for number of pages a process can hold (as Global Replacement)
int number_of_frames_per_process_lower_bound;
int number_of_frames_per_process_upper_bound;
int number_of_frames_per_process_average;
int total_number_of_frames;

//variables to get hit rates
long double number_of_tlb_hits;
long double total_tlb_accesses;
long double tlb_hit_rate;

long double number_of_L1_cache_hits;
long double total_L1_cache_accesses;
long double L1_cache_hit_rate;

long double number_of_L2_cache_hits;
long double total_L2_cache_accesses;
long double L2_cache_hit_rate;

long double number_of_cache_misses;
long double total_cache_accesses;
long double cache_hit_rate;

long double number_of_page_accesses;
long double number_of_page_misses;
long double page_fault_rate;


//access time vairables
unsigned long int L1_tlb_search_time;
unsigned long int L2_tlb_search_time;
unsigned long int L1_tlb_miss_OS_overhead;
unsigned long int L2_tlb_miss_OS_overhead;
unsigned long int processor_to_from_L1_tlb_transfer_time;
unsigned long int processor_to_from_L2_tlb_transfer_time;
unsigned long int L1_tlb_to_from_L2_tlb_transfer_time;
unsigned long int L1_cache_indexing_time;
unsigned long int L1_cache_tag_comparison_time; 
unsigned long int L2_cache_search_time;
unsigned long int processor_to_from_L1_cache_transfer_time;
unsigned long int processor_to_from_L2_cache_transfer_time;
unsigned long int L1_cache_to_from_L2_cache_transfer_time;
unsigned long int L1_cache_to_from_main_memory_transfer_time;
unsigned long int L2_cache_to_from_main_memory_transfer_time;
unsigned long int L2_cache_write_buffer_search_time;
unsigned long int L2_cache_to_L2_cache_write_buffer_transfer_time;
unsigned long int L2_cache_write_buffer_to_main_memory_transfer_time;
unsigned long int update_bit_in_main_memory_time;
unsigned long int pcb_data_processor_to_from_main_memory_transfer_time;
unsigned long int page_table_entry_processor_to_from_main_memory_transfer_time;
unsigned long int update_frame_table_entry_time;
unsigned long int average_disk_to_from_swap_space_transfer_time_entire_process;
unsigned long int main_memory_to_swap_space_transer_time;
unsigned long int swap_space_to_main_memory_transfer_time;
unsigned long int page_fault_overhead_time;
unsigned long int restart_overhead_time;
unsigned long int context_switch_time;

#endif /*GLOBAL_VARIABLES_H*/