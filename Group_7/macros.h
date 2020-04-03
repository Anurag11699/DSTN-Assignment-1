#ifndef MACROS_H
#define MACROS_H

#define INSTRUCTION_REQUEST 0

#define DATA_REQUEST 1

#define READ_REQUEST 0

#define WRITE_REQUEST 1

#define TASK_TERMINATED 0

#define TASK_READY 1

#define TASK_EXECUTING 2

#define cache_block_offset_size 5 //in number of bits (as 32 byte block)

#define L1_cache_index_size 5 //in number of bits (as 32 index entries in L1 cache)

#define L2_cache_index_size 6 //in number of bits (as 64 index entries in L2 cache)

#define frame_size 10 //size is log based 2 of frame size (2^10 = 1K is frame size)

#define main_memory_size 18 //size is log based 2 of frame size (2^25 = 32MB is main memory size)

#define max(x,y) ((x>y)?x:y)

#define set_MSB_bit_8(LRU_counter) ((LRU_counter)|0x80)

#define get_frame_offset 0x3ff

#define get_physical_address(physical_frame_number,virtual_address) (physical_frame_number<<10)+(virtual_address&get_frame_offset)

#define get_cache_block_offset(virtual_address) (virtual_address&0x1f)

#define get_L1_cache_block_index(virtual_address,L1_cache_block_offset_size) ((virtual_address>>L1_cache_block_offset_size)&0x1f)

#define get_L2_cache_block_index(virtual_address,L2_cache_block_offset_size) ((virtual_address>>L2_cache_block_offset_size)&0x3f)

#define get_L1_cache_tag(physical_address,L1_cache_index_size,L1_cache_block_size) (physical_address>>(L1_cache_index_size+L1_cache_block_size))

#define get_L2_cache_tag(physical_address,L2_cache_index_size,L2_cache_block_size) (physical_address>>(L2_cache_index_size+L2_cache_block_size))

#define get_physical_address_from_L1_cache(tag,index,offset) ((tag<<10)+(index<<5)+offset)


#define get_outer_page_table_offset(x) (x>>26)

#define get_middle_page_table_offset(x) ((x>>18)&0xff)

#define get_inner_page_table_offset(x) ((x>>10)&0xff)

#define get_logical_page_number(virtual_address,page_size) (virtual_address>>page_size)

#define right_shift_L1_tlb_counter 64

#define right_shift_L2_tlb_counter 64

#define right_shift_L2_cache_counter 64

#define output_file "OUTPUT.txt"

#define input_file "input.txt"

#define time_parameter_file "time_values.txt"

#define number_of_time_parameters 28

#endif /*MACROS_H*/