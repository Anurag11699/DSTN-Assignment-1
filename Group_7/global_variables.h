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


//access time vairables

#endif /*GLOBAL_VARIABLES_H*/