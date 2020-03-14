typedef struct tlb_entry
{
    int logical_page_number:22;
    unsigned int physical_frame_number:15;
    unsigned int valid:1;
    unsigned int LRU_counter:8;
}tlb_entry;

typedef struct tlb
{
    tlb_entry *tlb_entries;
}tlb;

