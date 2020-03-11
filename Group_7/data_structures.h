//frame of size 1KB
typedef struct frame
{
    char byte_array[1024];
}frame;

typedef struct frame_table_entry
{
    unsigned int valid:1;
    unsigned int pid;
    unsigned int page_number;

}frame_table_entry;


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


typedef struct main_memory
{
    frame_table_entry* frame_table;

    free_frame_list_dummy_head *ffl_dummy_head;
    free_frame *ffl_tail;

    used_frame_list_dummy_head *ufl_dummy_head;
    used_frame *recently_used_frame;

    frame *frame_array;
}main_memory;

typedef struct tlb_entry
{
    int page_number;
    unsigned int frame_number:15;
    unsigned int valid:1;
}tlb_entry;

typedef struct tlb
{
    tlb_entry *tlb_entries;
}tlb;

typedef struct _4_way_set_associative_cache_entry
{
    unsigned int valid_set_0:1;
    unsigned int tag_set_0;
    char data_set_0[32];

    unsigned int valid_set_1:1;
    unsigned int tag_set_1;
    char data_set_1[32];

    unsigned int valid_set_2:1;
    unsigned int tag_set_2;
    char data_set_2[32];

    unsigned int valid_set_3:1;
    unsigned int tag_set_3;
    char data_set_3[32];
}_4_way_set_associative_cache_entry;

typedef struct _4_way_set_associative_cache
{
    _4_way_set_associative_cache_entry *_4_way_set_associative_cache_entries;
}_4_way_set_associative_cache;


typedef struct page_table_entry
{
    int page_base_address;
    unsigned int global_page:1;
    unsigned int modified:1;
    unsigned int referenced:1;
    unsigned int cache_disabled:1;
    unsigned int write:1;
    
}page_table_entry;
