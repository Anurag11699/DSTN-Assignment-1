#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

L1_cache* initialize_L1_cache()
{
    //L1 cache has 2^6 indexes
    L1_cache *L1_cache_object = (L1_cache *)malloc(sizeof(L1_cache));
    L1_cache_object->L1_cache_entries = (L1_cache_entry *)malloc(sizeof(L1_cache_entry)*64);

    int i,j,k;
    for(i=0;i<64;i++)
    {
        L1_cache_object->L1_cache_entries[i].valid_set_0=0;
        L1_cache_object->L1_cache_entries[i].valid_set_1=0;
        L1_cache_object->L1_cache_entries[i].valid_set_2=0;
        L1_cache_object->L1_cache_entries[i].valid_set_3=0;

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
    L2_cache_object->L2_cache_entries = (L2_cache_entry *)malloc(sizeof(L2_cache_entry)*64);

    for(int i=0;i<64;i++)
    {
        L2_cache_object->L2_cache_entries[i].valid_set_0=0;
        L2_cache_object->L2_cache_entries[i].valid_set_1=0;
        L2_cache_object->L2_cache_entries[i].valid_set_2=0;
        L2_cache_object->L2_cache_entries[i].valid_set_3=0;
        L2_cache_object->L2_cache_entries[i].valid_set_4=0;
        L2_cache_object->L2_cache_entries[i].valid_set_5=0;
        L2_cache_object->L2_cache_entries[i].valid_set_6=0;
        L2_cache_object->L2_cache_entries[i].valid_set_7=0;
        L2_cache_object->L2_cache_entries[i].valid_set_8=0;
        L2_cache_object->L2_cache_entries[i].valid_set_9=0;
        L2_cache_object->L2_cache_entries[i].valid_set_10=0;
        L2_cache_object->L2_cache_entries[i].valid_set_11=0;
        L2_cache_object->L2_cache_entries[i].valid_set_12=0;
        L2_cache_object->L2_cache_entries[i].valid_set_13=0;
        L2_cache_object->L2_cache_entries[i].valid_set_14=0;
        L2_cache_object->L2_cache_entries[i].valid_set_15=0;
    }

    return L2_cache_object;
}

int L1_search(L1_cache* L1_cache_object,int index, int tag, int offset)
{
    int i,j,k;
    if(L1_cache_object->L1_cache_entries[index].valid_set_0==1 && L1_cache_object->L1_cache_entries[index].tag_set_0==tag)
    {
        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[0][i]=1;
        }

        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][0]=0;
        }

        return 1;
    }
    else if(L1_cache_object->L1_cache_entries[index].valid_set_1==1 && L1_cache_object->L1_cache_entries[index].tag_set_1==tag)
    {
        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[1][i]=1;
        }

        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][1]=0;
        }

        return 1;
    }
    else if(L1_cache_object->L1_cache_entries[index].valid_set_2==1 && L1_cache_object->L1_cache_entries[index].tag_set_2==tag)
    {
        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[2][i]=1;
        }

        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][2]=0;
        }

        return 1;
    }
    else if(L1_cache_object->L1_cache_entries[index].valid_set_3==1 && L1_cache_object->L1_cache_entries[index].tag_set_3==tag)
    {
        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[3][i]=1;
        }

        for(i=0;i<4;i++)
        {
            L1_cache_object->L1_cache_entries[index].LRU_square_matrix[i][3]=0;
        }

        return 1;
    }

    return -1;
    
}