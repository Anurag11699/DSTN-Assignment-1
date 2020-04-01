#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"
#include<assert.h>


/*
PreConditions
Inputs: None, we have hardcoded the cache as it is a given hardware structure. 

Cache Type: 4KB, 4 way set associative L1 cache. 
Replacement Policy: LRU Square Matrix
Follows write through

Purpose of the Function: Initialize L1 cache data structure

PostConditions
Output: {pointer to intialized L1 cache}
*/
L1_cache* initialize_L1_cache()
{
    //L1 cache has 2^5 indexes
    L1_cache *L1_cache_object = (L1_cache *)malloc(sizeof(L1_cache));
    L1_cache_object->L1_cache_entries = (L1_cache_index_entry *)malloc(sizeof(L1_cache_index_entry)*64);

    int i,j,k;
    for(i=0;i<32;i++)
    {
        for(j=0;j<4;j++)
        {
            L1_cache_object->L1_cache_entries[i].way[j].valid=0;
        }

        for(j=0;j<4;j++)
        {
            for(k=0;k<4;k++)
            {
                L1_cache_object->L1_cache_entries[i].LRU_square_matrix[j][k]=0;
            }
        }
    }

    return L1_cache_object;
}


/*
PreConditions
Inputs: None, we have hardcoded the cache as it is a given hardware structure. 

Cache Type: 32KB, 16 way set associative L2 cache. 
Replacement Policy: LFU with aging
Follows write buffer

Purpose of the Function: Initialize L2 cache data structure

PostConditions
Output: {pointer to intialized L2 cache}
*/
L2_cache* initialize_L2_cache()
{
    //L2 cache has 2^6 indexes
    L2_cache *L2_cache_object = (L2_cache *)malloc(sizeof(L2_cache));
    L2_cache_object->L2_cache_entries = (L2_cache_index_entry *)malloc(sizeof(L2_cache_index_entry)*64);

    int i,j;
    for(i=0;i<64;i++)
    {
        for(j=0;j<16;j++)
        {
            L2_cache_object->L2_cache_entries[i].way[j].valid=0;
            L2_cache_object->L2_cache_entries[i].way[j].LFU_counter=0;
        }       
    }

    return L2_cache_object;
}


/*
PreConditions
Inputs: None, we have hardcoded the cache as it is a given hardware structure. 

Number of entries in write buffer = 8.

Purpose of the Function: Initialize L2 cache write buffer

PostConditions
Output: {pointer to intialized L2 cache write buffer}
*/
L2_cache_write_buffer* initialize_L2_cache_write_buffer(int number_of_entries)
{
    //write buffer has only 8 buffers

    L2_cache_write_buffer *L2_cache_write_buffer_object = (L2_cache_write_buffer *)malloc(sizeof(L2_cache_write_buffer));
    L2_cache_write_buffer_object->number_of_entries=number_of_entries;
    L2_cache_write_buffer_object->L2_cache_write_buffer_entries = (L2_cache_write_buffer_entry *)malloc(sizeof(L2_cache_write_buffer_entry)*number_of_entries);

    int i;
    for(i=0;i<number_of_entries;i++)
    {
        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[i].valid=0;
    }

    return L2_cache_write_buffer_object;
}


/*
PreConditions
Inputs: {pointer to main memory object,pointer to L1 cache object, index of entry, tag of entry , block offset, frame number of entry where the block is present in, request is read or write}

0<=index<=32 
0<=tag<=0x7fff
0<=offset<=32
if cache is L1 instruction cache, write always 0.
if cache is L1 data cache, write is 1 if process requests a write. write is 0 if process requests a read. 

Purpose of the Function: 
Searches for the required block in the L1 cache.
If entry is present return 1 and update the LRU sqaure matrix row of that entry as all 1 and column as all 0. 
As policy is write through, we initiate immediate write to the main memory if the request is a write and mark the frame given by frame number as modified.
If entry is absent return -1; 

PostConditions
Output: 
If entry is present return time taken to process request
If entry is absent return -1;
*/
int L1_search(main_memory* main_memory_object,L1_cache* L1_cache_object, int index, int tag, int offset, int frame_number, int write)
{
    //check PreConditions

    assert(offset<32 && "L1 Cache Block Offset must be < 32d");
    assert(offset>=0 && "L1 Cache Block Offset must be >= 0d");

    assert(index<32 && "L1 Cache Block Index must be < 32d");
    assert(index>=0 && "L1 Cache Block Index must be >= 0d");

    assert(tag<0x7fff && "L1 Cache Block Tag must be < 0x7fff");
    assert(tag>=0 && "L1 Cache Block Tag must be >= 0d");
    
    
    int i,j;

    long int time_taken = 0;
    time_taken=time_taken+L1_cache_tag_comparison_time;

    for(i=0;i<4;i++)
    {
        if(L1_cache_object->L1_cache_entries[index].way[i].valid==1 && L1_cache_object->L1_cache_entries[index].way[i].tag==tag)
        {
            //make all entries of that row 1
            for(j=0;j<4;j++)
            {
                L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][j]=1;
            }

            //make all entries of that column 0
            for(j=0;j<4;j++)
            {
                L1_cache_object->L1_cache_entries[index].LRU_square_matrix[j][i]=0;
            }

            if(write==1)
            {   
                //as write through, we tranfer the entry immediately to memory
                time_taken = time_taken + L1_cache_to_from_main_memory_transfer_time;

                //mark frame as modified
                mark_frame_modified(main_memory_object,frame_number);

                //add time to mark the frame as modified -> done in that function itself 
                //time_taken=time_taken + update_bit_in_main_memory_time;
            }
            else
            {
                //it was a read request 

                time_taken = time_taken + processor_to_from_L1_cache_transfer_time;
            }
            

            return time_taken;
        }
    }

    return -1;
}


/*
PreConditions
Inputs: {pointer to main memory object, pointer to L2 cache object, pointer to L2 write buffer object, index of entry, tag of entry , block offset, frame number of the frame where this block is from, request is read or write}

0<=index<=64 
0<=tag<=0x3fff
0<=offset<=32

Purpose of the Function: 
Searches for the required block in the L2 cache.
If entry is present return 1. 
Right shift LRU counters of all ways by 1 if any of the counters reaches max value of 255 and set MSB of the way which was hit.
If it was a write request copy that block and its metadata to the write buffer, If write buffer was full, copy data from write buffer to main memory.
If entry is absent return -1; 

PostConditions
Output: 
If entry is present return return time taken to process request
If entry is absent return -1;
*/
int L2_search(main_memory* main_memory_object,L2_cache* L2_cache_object,L2_cache_write_buffer* L2_cache_write_buffer_object, int index, int tag, int offset, int frame_number, int write)
{

    //check PreConditions

    assert(offset<32 && "L2 Cache Block Offset must be < 32d");
    assert(offset>=0 && "L2 Cache Block Offset must be >= 0d");

    assert(index<64 && "L2 Cache Block Index must be < 64d");
    assert(index>=0 && "L2 Cache Block Index must be >= 0d");

    assert(tag<0x3fff && "L2 Cache Block Tag must be < 0x3fff");
    assert(tag>=0 && "L2 Cache Block Tag must be >= 0d");

    int i,j;
    int flag=0;

    long int time_taken = 0;
    time_taken=time_taken+L2_cache_search_time;


    for(i=0;i<16;i++)
    {
        if(L2_cache_object->L2_cache_entries[index].way[i].valid==1 && L2_cache_object->L2_cache_entries[index].way[i].tag==tag)
        {
            L2_cache_object->L2_cache_entries[index].way[i].LFU_counter++;
            flag=1;
        }

        //if any one entry reaches counter limit, right shift all counter values by 1
        if(L2_cache_object->L2_cache_entries[index].way[i].LFU_counter==255)
        {
            for(j=0;j<16;j++)
            {
                L2_cache_object->L2_cache_entries[index].way[j].LFU_counter=(L2_cache_object->L2_cache_entries[index].way[j].LFU_counter>>1);
            }
        }

       
        if(flag)
        {
            //also, if it was a write request, put this block into buffer cache
            if(write==1) //write==1 means the entry was write
            {
                //print_L2_buffer_cache(L2_cache_write_buffer_object);

                int L2_cache_write_buffer_num_entries = L2_cache_write_buffer_object->number_of_entries;
                total_time_taken=total_time_taken+L2_cache_write_buffer_search_time;

                //first search if the current block which was modified exists in buffer cache. If it does remove it and insert the newly updated one, else, copy this block to buffer cache
                for(j=0;j<L2_cache_write_buffer_num_entries;j++)
                {
                    if(L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].valid==1 && L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].index==index && L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].tag== L2_cache_object->L2_cache_entries[index].way[i].tag)
                    {
                        //just replace the existing block in buffer cache add time taken for it

                        time_taken = time_taken + L2_cache_to_L2_cache_write_buffer_transfer_time;
                        
                        return time_taken;
                    }
                }

                //check if any entry in buffer cache is invalid, if not, empty buffer cache and insert this into it.
                for(j=0;j<L2_cache_write_buffer_num_entries;j++)
                {
                    if(L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].valid==0)
                    {
                        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].valid=1;
                        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].index=index;
                        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].tag=L2_cache_object->L2_cache_entries[index].way[i].tag;
                        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].corresponding_frame_number=frame_number;

                        //just replace this block in buffer cache add time taken for it

                        time_taken = time_taken + L2_cache_to_L2_cache_write_buffer_transfer_time;
                        
                        return time_taken;

                    }
                }

                //if we reached this section means the buffer cache was full. Write all the entries from buffer cache to main memory and empty it. 

                for(j=0;j<L2_cache_write_buffer_num_entries;j++)
                {
                    //add time taken to write each buffer cache block to the main memory
                    time_taken = time_taken + L2_cache_write_buffer_to_main_memory_transfer_time;

                    //need to mark this frame as dirty
                    mark_frame_modified(main_memory_object,L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].corresponding_frame_number);

                    //add time to mark the frame as modified -> done in that function itself
                    //time_taken=time_taken + update_bit_in_main_memory_time;
                    
                    L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].valid=0;
                    
                }

                for(j=0;j<L2_cache_write_buffer_num_entries;j++)
                {
                    if(L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].valid==0)
                    {
                        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].valid=1;
                        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].index=index;
                        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].tag=L2_cache_object->L2_cache_entries[index].way[i].tag;
                        L2_cache_write_buffer_object->L2_cache_write_buffer_entries[j].corresponding_frame_number=frame_number;

                        //just replace this block in buffer cache add time taken for it

                        time_taken = time_taken + L2_cache_to_L2_cache_write_buffer_transfer_time;
                        
                        return time_taken;

                    }
                }


            }
            else
            {
                //it was a read request
                time_taken = time_taken + processor_to_from_L2_cache_transfer_time;
            }
            
            return time_taken;
        }
    }

    return -1;
}


/*
PreConditions
Inputs: {pointer to L2 cache object, pointer to L2 write buffer object, index of entry, tag of entry , block offset}

0<=index<=64 
0<=tag<=0x3fff
0<=offset<=32

Purpose of the Function: 
First checks if there are any invalid block in that index to place the new block in. If not,the function replaces the LFU block in that index of the cache. The block going to be replaced can be dropped as the main memory is inclusive

PostConditions
The LFU way in the given index is replaced and its counter is set to 0
*/
void replace_L2_cache_entry(L2_cache* L2_cache_object, int index, int tag, int offset)
{

    //check PreConditions

    assert(offset<32 && "L2 Cache Block Offset must be < 32d");
    assert(offset>=0 && "L2 Cache Block Offset must be >= 0d");

    assert(index<64 && "L2 Cache Block Index must be < 64d");
    assert(index>=0 && "L2 Cache Block Index must be >= 0d");

    assert(tag<0x3fff && "L2 Cache Block Tag must be < 0x3fff");
    assert(tag>=0 && "L2 Cache Block Tag must be >= 0d");


    //L2 cache entry is replaced by the one transfered from L1 cache, hence add the transfer time and search time in L2 cache to total time

    total_time_taken = total_time_taken + L1_cache_to_from_L2_cache_transfer_time + L2_cache_search_time;

    //print_L2_cache(L2_cache_object);

    int way_to_replace=-1;
    int i;
    for(i=0;i<16;i++)
    {
        if(L2_cache_object->L2_cache_entries[index].way[i].valid==0)
        {
            way_to_replace=i;
            break;
        }
    }

    if(way_to_replace!=-1)
    {
        L2_cache_object->L2_cache_entries[index].way[i].valid=1;
        L2_cache_object->L2_cache_entries[index].way[i].tag=tag;
        L2_cache_object->L2_cache_entries[index].way[i].LFU_counter=0;
        return;
    }

    int min_counter=__INT16_MAX__;

    for(i=0;i<16;i++)
    {
        if(L2_cache_object->L2_cache_entries[index].way[i].valid==1)
        {
            if(L2_cache_object->L2_cache_entries[index].way[i].LFU_counter<min_counter)
            {
                min_counter=L2_cache_object->L2_cache_entries[index].way[i].LFU_counter;
                way_to_replace=i;
            }
        }
    }

    L2_cache_object->L2_cache_entries[index].way[i].valid=1;
    L2_cache_object->L2_cache_entries[index].way[i].tag=tag;
    L2_cache_object->L2_cache_entries[index].way[i].LFU_counter=0;

}

/*
PreConditions
Inputs: {pointer to L2 cache object, index of entry, tag of entry , block offset}

0<=index<=64 
0<=tag<=0x3fff
0<=offset<=32

Purpose of the Function: 
It invalidates the given given entry of L2 cache. This function is used in conjuction with replace_L1_cache entry to remove a block from L2 cache and transfer it to L1 cache.

PostConditions
The given entry is invalidated 
*/
void remove_L2_cache_block(L2_cache* L2_cache_object, int index, int tag, int offset)
{   
    
    //check PreConditions

    assert(offset<32 && "L2 Cache Block Offset must be < 32d");
    assert(offset>=0 && "L2 Cache Block Offset must be >= 0d");

    assert(index<64 && "L2 Cache Block Index must be < 64d");
    assert(index>=0 && "L2 Cache Block Index must be >= 0d");

    assert(tag<0x3fff && "L2 Cache Block Tag must be < 0x3fff");
    assert(tag>=0 && "L2 Cache Block Tag must be >= 0d");


    //add time taken to search for the entry to remove it
    total_time_taken = total_time_taken + L2_cache_search_time;

    for(int i=0;i<16;i++)
    {
        if(L2_cache_object->L2_cache_entries[index].way[i].valid==1 && L2_cache_object->L2_cache_entries[index].way[i].tag==tag)
        {
            L2_cache_object->L2_cache_entries[index].way[i].valid=0;
            return;
        }
    }

}


/*
PreConditions
Inputs: {pointer to L1 cache object, pointer to L2 cache object, index of entry, tag of entry , block offset}

0<=index<=32 
0<=tag<=0x7fff
0<=offset<=32

Purpose of the Function: 
First checks if there are any invalid block in that index to place the new block in. If not,the function replaces the LRU block in that index of the cache. The block going to be replaced in L1 cannot be simply dropped as the cache is exclusive. Hence, we must transfer the going to be replaced block to the L2 cache by calling the replace_L2_cache_entry function with appropriate index and tag of L2 cache.

PostConditions
The LRU way in the given index is replaced.
*/
void replace_L1_cache_entry(L1_cache* L1_cache_object, L2_cache* L2_cache_object, int index, int tag, int offset)
{
    //check PreConditions

    assert(offset<32 && "L1 Cache Block Offset must be < 32d");
    assert(offset>=0 && "L1 Cache Block Offset must be >= 0d");

    assert(index<32 && "L1 Cache Block Index must be < 32d");
    assert(index>=0 && "L1 Cache Block Index must be >= 0d");

    assert(tag<0x7fff && "L1 Cache Block Tag must be < 0x7fff");
    assert(tag>=0 && "L1 Cache Block Tag must be >= 0d");


    //add the time taken to search which tag to put it in
    total_time_taken = total_time_taken  + (L1_cache_indexing_time+L1_cache_tag_comparison_time);

    int way_to_replace=-1;
    int i;

    //prefer placement compared to replacement
    for(i=0;i<4;i++)
    {
        if(L1_cache_object->L1_cache_entries[index].way[i].valid==0)
        {
            way_to_replace=i;
            break;
        }
    }

    if(way_to_replace!=-1)
    {
        L1_cache_object->L1_cache_entries[index].way[way_to_replace].valid=1;
        L1_cache_object->L1_cache_entries[index].way[way_to_replace].tag=tag;

        //fprintf(output_fd,"CHECK 1: %d | 2: %d\n",tag,L1_cache_object->L1_cache_entries[index].way[way_to_replace].tag);
        //fflush(output_fd);
        //update the LRU square matrix
        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[way_to_replace][i]=1;
        }
        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][way_to_replace]=0;
        }

        return;
    }

    //no invalid entry, need to replace. Check the row entry with all zeros in LRU square matrix. There will be exactly one such entry

    way_to_replace=-1;

    for(i=0;i<4;i++)
    {
        if(L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][0]==0 && L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][1]==0 && L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][2]==0 && L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][3]==0)
        {
            way_to_replace=i;
            break;
        }
    }

    //as this is exclusive cache, we must send this entry into L2 cache add the time taken to transfer this entry to L2 cache


    int physical_address = get_physical_address_from_L1_cache(L1_cache_object->L1_cache_entries[index].way[way_to_replace].tag,index,offset);

    int L2_cache_needed_index_to_replace = get_L2_cache_block_index(physical_address,cache_block_offset_size);
    int L2_cache_needed_tag_to_replace = get_L2_cache_tag(physical_address,L2_cache_index_size,cache_block_offset_size);

    fprintf(output_fd,"Replacing in L2 cache, Index: %d | Tag: %d\n ",L2_cache_needed_index_to_replace,L2_cache_needed_tag_to_replace);
    fflush(output_fd);

    replace_L2_cache_entry(L2_cache_object, L2_cache_needed_index_to_replace,  L2_cache_needed_tag_to_replace,offset);


    L1_cache_object->L1_cache_entries[index].way[i].valid=1;
    L1_cache_object->L1_cache_entries[index].way[i].tag=tag;

    //update the LRU square matrix 
    for(i=0;i<4;i++)
    {
        L1_cache_object->L1_cache_entries[index].LRU_square_matrix[way_to_replace][i]=1;
    }
    for(i=0;i<4;i++)
    {
        L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][way_to_replace]=0;
    }

}

/*auxillary function used to print L1 cache useful for debugging*/
void print_L1_cache(L1_cache* L1_cache_object)
{
    fprintf(output_fd,"Printing L1 Cache\n");
    fflush(output_fd);
    int i,j,k;

    for(i=0;i<32;i++)
    {
        fprintf(output_fd,"INDEX: %d\t",i);
        fflush(output_fd);
        for(j=0;j<4;j++)
        {
            fprintf(output_fd,"Way: %d | Tag: %d | Valid: %d\t||",j,L1_cache_object->L1_cache_entries[i].way[j].tag,L1_cache_object->L1_cache_entries[i].way[j].valid);
            fflush(output_fd);
        }

        fprintf(output_fd,"\nLRU Sqaure Matrix of Index %d\n",i);
        fflush(output_fd);
        for(j=0;j<4;j++)
        {
            for(k=0;k<4;k++)
            {
                fprintf(output_fd,"%d ",L1_cache_object->L1_cache_entries[i].LRU_square_matrix[j][k]);
                fflush(output_fd);
            }
            fprintf(output_fd,"\n");
            fflush(output_fd);
        }
    }
    fprintf(output_fd,"\n");
    fflush(output_fd);
}


/*auxillary function used to print L2 cache useful for debugging*/
void print_L2_cache(L2_cache* L2_cache_object)
{
    fprintf(output_fd,"Printing L2 Cache\n");
    fflush(output_fd);
    int i,j;

    for(i=0;i<64;i++)
    {
        fprintf(output_fd,"INDEX: %d\t",i);
        fflush(output_fd);
        for(j=0;j<16;j++)
        {
            fprintf(output_fd,"Way: %d | Tag: %d | Valid: %d | LFU Counter: %d || ",j,L2_cache_object->L2_cache_entries[i].way[j].tag,L2_cache_object->L2_cache_entries[i].way[j].valid,L2_cache_object->L2_cache_entries[i].way[j].LFU_counter);
            fflush(output_fd);
        }

        fprintf(output_fd,"\n");
        fflush(output_fd);
    }
    
}

/*auxillary function used to print L2 cache write buffer, useful for debugging*/
void print_L2_buffer_cache(L2_cache_write_buffer* L2_cache_write_buffer_object)
{
    fprintf(output_fd,"Printing L2 Cache Write Buffer\n");
    fflush(output_fd);

    int number_of_entries = L2_cache_write_buffer_object->number_of_entries;

    for(int i=0;i<number_of_entries;i++)
    {
        fprintf(output_fd,"Entry #%d | Index: %d | Tag: %d | Valid: %d |\n",i,L2_cache_write_buffer_object->L2_cache_write_buffer_entries[i].index,L2_cache_write_buffer_object->L2_cache_write_buffer_entries[i].tag,L2_cache_write_buffer_object->L2_cache_write_buffer_entries[i].valid);
        fflush(output_fd);
    }
    fprintf(output_fd,"\n");
    fflush(output_fd);
}