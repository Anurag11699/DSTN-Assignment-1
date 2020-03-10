
typedef struct frame_table_entry
{
    int present_bit;
    int pid;
    int page_number;

}frame_table_entry;

typedef struct main_memory
{
    frame_table_entry* frame_table;
}main_memory;
