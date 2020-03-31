#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//data structures
#include "main_memory_data_structures.h"
#include "cache_data_structures.h"
#include "tlb_data_structures.h"
#include "kernel_data_structures.h"
#include "configuration_file.h"
#include "macros.h"
#include "global_variables.h"



//kernel functions
extern kernel* initialize_kernel(int);
extern void execute_process_request(kernel*, tlb*, tlb*, L1_cache*, L1_cache*, L2_cache*, L2_cache_write_buffer* ,main_memory*,int,unsigned int,int);
extern int get_request_type(int);
extern int load_new_process(kernel*,main_memory*,int, int, char*); 
extern void terminate_process(kernel*, main_memory*, int);
extern void context_switch(kernel*, tlb*,tlb*,int,int);


//main memory functions
extern main_memory* initialize_main_memory();
extern void add_free_frame(main_memory*, int);
extern int remove_free_frame(main_memory *);
extern void add_used_frame(main_memory*, int);
extern int remove_used_frame(kernel*,main_memory*);
extern void transfer_to_free_frame_list(main_memory*, int);
extern void set_reference_bit(main_memory*,int);
extern int get_frame(kernel*,main_memory*,int,int,int);
extern void update_frame_table_entry(main_memory*,int,int,int);
extern int get_pid_of_frame(main_memory*,int);
extern int get_page_number_of_frame(main_memory*, int);
//page_table* get_page_table_pointer_of_frame(main_memory*,int);
extern int check_frame_ownership(main_memory*,int,int);
extern page_table* initialize_page_table(int);
extern int page_table_walk(kernel* ,main_memory*,int,int);
extern void invalidate_page_table_entry(kernel*, main_memory*, int, int);
extern void mark_frame_modified(main_memory*, int);
extern int power(int, unsigned int); 

//main memory printing functions
extern void print_page_table(page_table*);
extern void print_frame_table(main_memory*);
extern void print_ffl(main_memory*);
extern void print_ufl(main_memory*); 


//cache functions
extern L1_cache* initialize_L1_cache();
extern L2_cache* initialize_L2_cache();
extern L2_cache_write_buffer* initialize_L2_cache_write_buffer(int);
extern int L1_search(main_memory*,L1_cache*,int,int,int,int,int);
extern int L2_search(main_memory*,L2_cache*,L2_cache_write_buffer*,int,int,int,int,int);
extern void replace_L2_cache_entry(L2_cache *,int,int,int);
extern void replace_L1_cache_entry(L1_cache*, L2_cache*, int, int, int);

//cache printing functions
extern void print_L1_cache(L1_cache*);
extern void print_L2_cache(L2_cache*);
extern void print_L2_buffer_cache(L2_cache_write_buffer*);


//tlb functions
extern tlb* initialize_tlb(int);
extern void tlb_flush(tlb*);
extern int tlb_search(tlb*, int);
extern void L2_to_L1_tlb_transfer(tlb*, tlb*, int);
extern int complete_tlb_search(tlb*, tlb*, int, int*);
extern void insert_new_tlb_entry(tlb*, int, int);

//tlb printing functions
extern void print_tlb(tlb*);

#endif /*FUNCTIONS_H*/