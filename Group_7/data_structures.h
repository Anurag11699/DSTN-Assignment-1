
typedef struct frame_table_entry
{
    int valid_bit;
    int pid;
    int page_number;

}frame_table_entry;


struct free_frame_list
{
	int frame_number;
	struct free_frame_list *next;
}free_frame_list;

typedef struct free_frame_list free_frame;

typedef struct free_frame_list_dummy_head
{
	int number_free_frames;
	free_frame *next;
}free_frame_list_dummy_head;


struct used_frame_list
{
	int frame_number;
    int reference_bit;
	struct used_frame_list *next;
    //struct used_frame_list *prev;
}free_frame_list;

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
}main_memory;
