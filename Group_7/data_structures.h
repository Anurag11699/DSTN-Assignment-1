
typedef struct frame_table_entry
{
    int present_bit;
    int pid;
    int page_number;

}frame_table_entry;


typedef struct free_frame_list
{
	int frame_number;
	struct free_frame_list *next;
}free_frame_list;

typedef struct free_frame_list_dummy_head
{
	int number_free_frames;
	free_frame_list *next;
}free_frame_list_dummy_head;


typedef struct main_memory
{
    frame_table_entry* frame_table;
    free_frame_list_dummy_head *ffl_dummy_head;
}main_memory;
