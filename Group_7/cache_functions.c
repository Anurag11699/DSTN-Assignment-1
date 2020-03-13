#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

L1_cache* initialize_L1_cache()
{
    //L1 cache has 2^6 indexes
    L1_cache *L1_cache_object = (L1_cache *)malloc(sizeof(L1_cache));
    L1_cache_object->L1_cache_entries = (L1_cache_index_entry *)malloc(sizeof(L1_cache_index_entry)*64);

    int i,j,k;
    for(i=0;i<64;i++)
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

int L1_search(L1_cache* L1_cache_object, int index, int tag, int offset)
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

int L2_search(L2_cache* L2_cache_object, int index, int tag, int offset)
{
    int i,j,k;

    for(i=0;i<16;i++)
    {
        if(L2_cache_object->L2_cache_entries[index].way[i].valid==1 && L2_cache_object->L2_cache_entries[index].way[i].tag==tag)
        {
            L2_cache_object->L2_cache_entries[index].way[i].LFU_counter++;
        }

        //if any one entry reaches counter limit, right shift all counter values by 1
        if(L2_cache_object->L2_cache_entries[index].way[i].LFU_counter==255)
        {
            for(j=0;j<16;j++)
            {
                L2_cache_object->L2_cache_entries[index].way[j].LFU_counter=(L2_cache_object->L2_cache_entries[index].way[j].LFU_counter>>1);
            }
        }
    }
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
