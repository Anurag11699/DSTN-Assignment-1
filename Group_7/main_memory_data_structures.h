#ifndef MAIN_MEMORY_DATA_STRUCTURES_H
#define MAIN_MEMORY_DATA_STRUCTURES_H

struct page_table_entry;
typedef struct page_table_entry page_table_entry;

struct page_table;
typedef struct page_table page_table;


/*
    ADT for Page Table Entry

    page_table* pointer_to_page_table; -> pointer to next level page table
    int frame_base_address:15; -> the frame of the next level page table or the actual frame needed by process. This field is 15 bits in length for a 32MB with 1KB frame size memory. 
    int initialized_before:1;-> has the page table which it points to been initialized before
    unsigned int valid:1;-> is the entry valid
    
*/
struct page_table_entry
{
    //assume each page table entry is 4 bytes
    
    page_table* pointer_to_page_table;
    unsigned int frame_base_address:15; //frame which is occupied by the page table pointed to by this entry
    unsigned int initialized_before:1;
    unsigned int valid:1;
};


/*
    ADT for Page Table

    page_table_entry* page_table_entries; -> pointer to array of page table entries. We assume that each page table entry is 4 bytes and thus each page table has 2^8 entries
*/
struct page_table
{

    //each page table will have 2^8 page table entries
    page_table_entry* page_table_entries;
};


/*
    ADT for Frame Table Entry

    int page_number:22; -> Logical page number of the process that is stored in this frame. This is 22 bits in size for our 32 bit VA address.

    int pid; -> pid of the process that is using this frame. If nobody is using this frame then pid is -1

    unsigned int modified:1; -> if this bit is 1, the frame has been modified and hence must be swapped out on replacement
*/
typedef struct frame_table_entry
{
    //unsigned int valid:1;
    unsigned int page_number:22;
    int pid;
    unsigned int modified:1;
    //page_table* pointer_to_stored_page_table; //if the frame is a page table, then this is the pointer to that page table

}frame_table_entry;

/*
    ADT for Free Frame List

    Dummy head stores the number of free frames in the free frame list

    Each node in the free frame linked list stores the frame number of the free frame
*/
struct free_frame_list
{
	unsigned int frame_number:15;
	struct free_frame_list *next;
};

typedef struct free_frame_list free_frame;

typedef struct free_frame_list_dummy_head
{
	int number_free_frames;
	free_frame *next;
}free_frame_list_dummy_head;



/*
    ADT for Used Frame List

    Replacement Policy: Second Chance FIFO

    Dummy Header stores number of used frame in the used frame list

    Each node in the used frame linked list stores:

    unsigned int frame_number:15; -> frame number of the used frame

    unsigned int reference_bit:1; -> has the frame been referenced since its allocation. This is used to implement the replacement policy
*/
struct used_frame_list
{
	unsigned int frame_number:15;
    unsigned int reference_bit:1;
	struct used_frame_list *next;
};

typedef struct used_frame_list used_frame;

typedef struct used_frame_list_dummy_head
{
	int number_used_frames;
	used_frame *next;
}used_frame_list_dummy_head;



/*
    ADT for Main Memory

    unsigned int number_of_frames; -> total number of frames that the main memory has

    frame_table_entry* frame_table; ->Frame Table of the OS. Array of frame table entries

    free_frame_list_dummy_head *ffl_dummy_head; -> free frame list dummy header

    free_frame *ffl_tail; -> tail of the free frame list

    used_frame_list_dummy_head *ufl_dummy_head; ->used frame list dummy header

    used_frame *recently_used_frame; -> pointer to the most recently used frame in the used frame list. The frame next to it will be the frame brought in first as it is a fifo queue.
    
*/
typedef struct main_memory
{
    unsigned int number_of_frames;
    frame_table_entry* frame_table;

    free_frame_list_dummy_head *ffl_dummy_head;
    free_frame *ffl_tail;

    used_frame_list_dummy_head *ufl_dummy_head;
    used_frame *recently_used_frame;
  
}main_memory;


#endif /*MAIN_MEMORY_DATA_STRUCTURES_H*/