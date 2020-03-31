#ifndef CACHE_DATA_STRUCTURES_H
#define CACHE_DATA_STRUCTURES_H

/*
    ADT for a single way of the L1 cache

    L1 cache specifications: 4KB, 4 way set associative, 32 byte block size

    Hence:
    Block Offset Size: 5 bits
    Index Size: 5 bits
    Tag Size: (25 - (5+5))  = 15 bits
*/
typedef struct L1_cache_way_entry
{
    unsigned int valid:1;
    unsigned int tag:15;
    //char data[32];

}L1_cache_way_entry;



/*
    ADT for a single index of L1 cache, which is 4 way set associative

    L1 cache specifications: 4KB, 4 way set associative, 32 byte block size

    Hence:
    Block Offset Size: 5 bits
    Index Size: 5 bits
    Tag Size: (25 - (5+5))  = 15 bits

    Replacement Policy: LRU Square Martix
*/
typedef struct L1_cache_index_entry
{   
    // an array of 4 ways as L1 cache is 4 way set associative
    L1_cache_way_entry way[4];

    //an LRU square matrix for replacement
    int LRU_square_matrix[4][4];

}L1_cache_index_entry;


/*
    ADT for L1 cache

    L1 cache specifications: 4KB, 4 way way set associative, 32 byte block size

    Hence:
    Block Offset Size: 5 bits
    Index Size: 5 bits
    Tag Size: (25 - (5+5))  = 15 bits

    Replacement Policy: LRU Square Martix
*/
typedef struct L1_cache
{
    //used to dynamically initiazlize an array of 32 index entries
    L1_cache_index_entry *L1_cache_entries;
}L1_cache;



/*
    ADT for a single way of the L2 cache

    L2 cache specifications: 32KB, 16 way set associative, 32 byte block size

    Hence:
    Block Offset Size: 5 bits
    Index Size: 6 bits
    Tag Size: (25 - (6+5))  = 14 bits

    Replacement Policy: 8 bit LFU counter
*/
typedef struct L2_cache_way_entry
{
    unsigned int valid:1;
    unsigned int tag:14;
    //char data[32];

    //LFU counter used for replacment
    unsigned int LFU_counter:8;

}L2_cache_way_entry;


/*
    ADT for a single index of L2 cache, which is 16 way set associative

    L2 cache specifications: 32KB, 16 way set associative, 32 byte block size

    
    Hence:
    Block Offset Size: 5 bits
    Index Size: 6 bits
    Tag Size: (25 - (6+5))  = 14 bits

    Replacement Policy: 8 bit  LFU counter
*/
typedef struct L2_cache_index_entry
{
    // an array of 16 ways as L1 cache is 16 way set associative
    L2_cache_way_entry way[16];

}L2_cache_index_entry;


/*
    ADT for a L2

    L2 cache specifications: 32KB, 16 way set associative, 32 byte block size

    
    Hence:
    Block Offset Size: 5 bits
    Index Size: 6 bits
    Tag Size: (25 - (6+5))  = 14 bits

    Replacement Policy: 8 bit  LFU counter
*/
typedef struct L2_cache
{
    //used to dynamically initiazlize an array of 64 index entries
    L2_cache_index_entry *L2_cache_entries;
}L2_cache;


/*
    ADT for a single write buffer block entry

    L2 cache write buffer specifications: 8 entries

    
    Hence:
    Index Size: 6 bits
    Tag Size: (25 - (6+5))  = 14 bits

*/
typedef struct L2_cache_write_buffer_entry
{
    unsigned int index:6;
    unsigned int tag:14;
    unsigned int valid:1;
    unsigned int corresponding_frame_number:15;
    //char data[32];
}L2_cache_write_buffer_entry;


/*
    ADT for a single write buffer block entry

    L2 cache write buffer specifications: 8 entries

    
    Hence:
    Index Size: 6 bits
    Tag Size: (25 - (6+5))  = 14 bits

*/
typedef struct L2_cache_write_buffer
{
    int number_of_entries;

    //used to dynamically initiazlize an array of write buffer blocks
    L2_cache_write_buffer_entry *L2_cache_write_buffer_entries;
}L2_cache_write_buffer;


#endif /*CACHE_DATA_STRUCTURES_H*/