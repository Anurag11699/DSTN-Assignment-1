typedef struct L1_cache_entry
{
    unsigned int valid_set_0:1;
    unsigned int tag_set_0:14;
    char data_set_0[32];

    unsigned int valid_set_1:1;
    unsigned int tag_set_1:14;
    char data_set_1[32];

    unsigned int valid_set_2:1;
    unsigned int tag_set_2:14;
    char data_set_2[32];

    unsigned int valid_set_3:1;
    unsigned int tag_set_3:14;
    char data_set_3[32];

    int LRU_square_matrix[4][4];
}L1_cache_entry;

typedef struct L1_cache
{
    L1_cache_entry *L1_cache_entries;
}L1_cache;

typedef struct L2_cache_entry
{
    unsigned int valid_set_0:1;
    unsigned int tag_set_0:14;
    char data_set_0[32];
    unsigned int LFU_counter_0:16;
    unsigned int modified_set_0:1;

    unsigned int valid_set_1:1;
    unsigned int tag_set_1:14;
    char data_set_1[32];
    unsigned int LFU_counter_1:16;
    unsigned int modified_set_1:1;

    unsigned int valid_set_2:1;
    unsigned int tag_set_2:14;
    char data_set_2[32];
    unsigned int LFU_counter_2:16;
    unsigned int modified_set_2:1;

    unsigned int valid_set_3:1;
    unsigned int tag_set_3:14;
    char data_set_3[32];
    unsigned int LFU_counter_3:16;
    unsigned int modified_set_3:1;

    unsigned int valid_set_4:1;
    unsigned int tag_set_4:14;
    char data_set_4[32];
    unsigned int LFU_counter_4:16;
    unsigned int modified_set_4:1;

    unsigned int valid_set_5:1;
    unsigned int tag_set_5:14;
    char data_set_5[32];
    unsigned int LFU_counter_5:16;
    unsigned int modified_set_5:1;

    unsigned int valid_set_6:1;
    unsigned int tag_set_6:14;
    char data_set_6[32];
    unsigned int LFU_counter_6:16;
    unsigned int modified_set_6:1;

    unsigned int valid_set_7:1;
    unsigned int tag_set_7:14;
    char data_set_7[32];
    unsigned int LFU_counter_7:16;
    unsigned int modified_set_7:1;

    unsigned int valid_set_8:1;
    unsigned int tag_set_8:14;
    char data_set_8[32];
    unsigned int LFU_counter_8:16;
    unsigned int modified_set_8:1;

    unsigned int valid_set_9:1;
    unsigned int tag_set_9:14;
    char data_set_9[32];
    unsigned int LFU_counter_9:16;
    unsigned int modified_set_9:1;

    unsigned int valid_set_10:1;
    unsigned int tag_set_10:14;
    char data_set_10[32];
    unsigned int LFU_counter_10:16;
    unsigned int modified_set_10:1;

    unsigned int valid_set_11:1;
    unsigned int tag_set_11:14;
    char data_set_11[32];
    unsigned int LFU_counter_11:16;
    unsigned int modified_set_11:1;

    unsigned int valid_set_12:1;
    unsigned int tag_set_12:14;
    char data_set_12[32];
    unsigned int LFU_counter_12:16;
    unsigned int modified_set_12:1;

    unsigned int valid_set_13:1;
    unsigned int tag_set_13:14;
    char data_set_13[32];
    unsigned int LFU_counter_13:16;
    unsigned int modified_set_13:1;

    unsigned int valid_set_14:1;
    unsigned int tag_set_14:14;
    char data_set_14[32];
    unsigned int LFU_counter_14:16;
    unsigned int modified_set_14:1;

    unsigned int valid_set_15:1;
    unsigned int tag_set_15:14;
    char data_set_15[32];
    unsigned int LFU_counter_15:16;
    unsigned int modified_set_15:1;

}L2_cache_entry;

typedef struct L2_cache
{
    L2_cache_entry *L2_cache_entries;
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


