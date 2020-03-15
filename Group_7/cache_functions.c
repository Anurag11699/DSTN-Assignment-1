#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

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
        }       
    }

    return L2_cache_object;
}

L2_cache_write_buffer* initialize_L2_cache_write_buffer()
{
    //write buffer has only 8 buffers
    L2_cache_write_buffer *L2_cache_write_buffer_object = (L2_cache_write_buffer *)malloc(sizeof(L2_cache_write_buffer));
    L2_cache_write_buffer_object->L2_cache_write_buffer_entries = (L2_cache_write_buffer_entry *)malloc(sizeof(L2_cache_write_buffer_entry)*8);

    return L2_cache_write_buffer_object;
}

int L1_search(L1_cache* L1_cache_object, int index, int tag, int offset, int read_write)
{
    int i,j,k;

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

            return 1;
        }
    }

    return -1;
}

int L2_search(L2_cache* L2_cache_object, int index, int tag, int offset, int read_write)
{
    int i,j,k;
    int flag=0;

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
            return 1;
        }
    }

    return -1;
}

void replace_L2_cache_entry(L2_cache* L2_cache_object, int index, int tag, int offset)
{
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

void replace_L1_cache_entry(L1_cache* L1_cache_object, L2_cache* L2_cache_object, int index, int tag, int offset)
{
    int way_to_replace=-1;
    int i,j;

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

    //as this is exclusive cache, we must send this entry into L2 cache

    replace_L2_cache_entry(L2_cache_object, index, L1_cache_object->L1_cache_entries[index].way[way_to_replace].tag,offset);


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