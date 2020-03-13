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
            L1_cache_object->L1_cache_entries[i].way[j].valid_set=0;
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
            L2_cache_object->L2_cache_entries[i].way[j].valid_set_0=0;
            L2_cache_object->L2_cache_entries[i].way[j].modified_set_0=0;
        }       
    }

    return L2_cache_object;
}

