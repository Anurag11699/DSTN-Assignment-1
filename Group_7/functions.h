#include "main_memory_data_structures.h"
#include "cache_data_structures.h"
#include "tlb_data_structures.h"
#include "kernel_data_structures.h"
#define set_MSB_bit_8 0x80
#define get_frame_offset 0x3ff
#define get_cache_block_offset 0x1f
#define get_L1_cache_block_index 0x3f
#define get_L2_cache_block_index 0x3f


//kernel functions
extern kernel* initialize_kernel(int);
extern void execute_process_request(kernel*, tlb*, tlb*, L1_cache*, L1_cache*, L2_cache*, L2_cache_write_buffer* ,main_memory*,int, int);
extern int get_request_type(int);
extern int load_new_process(kernel*,main_memory*,int, int, char*); 

//main memory functions
extern main_memory* initialize_main_memory(int, int);
extern void add_free_frame(main_memory*, int);
extern int remove_free_frame(free_frame_list_dummy_head *);
extern void add_used_frame(used_frame_list_dummy_head *, used_frame **, int);
extern int remove_used_frame(used_frame_list_dummy_head *, used_frame **);

//cache functions
extern L1_cache* initialize_L1_cache();
extern L2_cache* initialize_L2_cache();
extern L2_cache_write_buffer* initialize_L2_cache_write_buffer();
extern int L1_search(L1_cache*,int,int,int,int);
extern int L2_search(L2_cache*,L2_cache_write_buffer*,int,int,int,int);
extern void replace_L2_cache_entry(L2_cache *,int,int,int);
extern void replace_L1_cache_entry(L1_cache*, L2_cache*, int, int, int);


//tlb functions
extern tlb* initialize_tlb(int);
extern void tlb_flush(tlb*, int);
extern int tlb_search(tlb*, int, int);
extern void L2_to_L1_tlb_transfer(tlb*, int, tlb*, int, int);
extern int complete_tlb_search(tlb*, int, tlb*,int, int);
extern void insert_new_tlb_entry(tlb*, int, int, int);
