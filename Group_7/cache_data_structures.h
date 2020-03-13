typedef struct L1_cache_way_entry
{
    unsigned int valid:1;
    unsigned int tag:14;
    char data[32];

}L1_cache_way_entry;

typedef struct L1_cache_index_entry
{
    L1_cache_way_entry way[4];
    int LRU_square_matrix[4][4];

}L1_cache_index_entry;

typedef struct L1_cache
{
    L1_cache_index_entry *L1_cache_entries;
}L1_cache;

typedef struct L2_cache_way_entry
{
    unsigned int valid:1;
    unsigned int tag:14;
    char data[32];
    unsigned int LFU_counter:8;

}L2_cache_way_entry;

typedef struct L2_cache_index_entry
{
    L2_cache_way_entry way[16];

}L2_cache_index_entry;

typedef struct L2_cache
{
    L2_cache_index_entry *L2_cache_entries;
}L2_cache;

typedef struct L2_cache_write_buffer_entry
{
    unsigned int index:6;
    unsigned int tag:14;
    char data[32];
}L2_cache_write_buffer_entry;

typedef struct L2_cache_write_buffer
{
    L2_cache_write_buffer_entry *L2_cache_write_buffer_entries;
}L2_cache_write_buffer;


