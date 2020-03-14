#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"


tlb* initialize_tlb(int number_of_entries)
{
    tlb *tlb_object = (tlb *)malloc(sizeof(tlb));
    tlb_object->tlb_entries=(tlb_entry *)malloc(sizeof(tlb_entry)*number_of_entries);

    tlb_flush(tlb_object,number_of_entries);

    return tlb_object;
}

void tlb_flush(tlb* tlb_object, int number_of_entries)
{
    for(int i=0;i<number_of_entries;i++)
    {
        tlb_object->tlb_entries[i].valid=0;
        tlb_object->tlb_entries[i].LRU_counter=0;
    }
}

int tlb_search(tlb* tlb_object, int number_of_entries, int logical_page_number)
{
    //shift all the counters to the right by 1
    int i;
    for(i=0;i<number_of_entries;i++)
    {
        tlb_object->tlb_entries[i].LRU_counter<<1;
    }
    for(i=0;i<number_of_entries;i++)
    {
        if(tlb_object->tlb_entries[i].valid==1 && tlb_object->tlb_entries[i].logical_page_number==logical_page_number)
        {
            //set MSB to 1 on TLB hit
            tlb_object->tlb_entries[i].LRU_counter=(tlb_object->tlb_entries[i].LRU_counter|set_MSB_bit_8);
            return tlb_object->tlb_entries[i].physical_frame_number;
        }
    }

    return -1;
}

void L2_to_L1_tlb_transfer(tlb *L1_tlb, int number_of_entries_L1, tlb *L2_tlb, int number_of_entires_L2, int logical_page_number)
{
    tlb_entry entry_to_be_transfered;

    int i;
    for(i=0;i<number_of_entires_L2;i++)
    {
        if(L2_tlb->tlb_entries[i].valid==1 && L2_tlb->tlb_entries[i].logical_page_number==logical_page_number)
        {
            entry_to_be_transfered=L2_tlb->tlb_entries[i];
        }
    }

    int flag=1;
    for(i=0;i<number_of_entries_L1;i++)
    {
        if(L1_tlb->tlb_entries[i].valid==0)
        {
            L1_tlb->tlb_entries[i]=entry_to_be_transfered;
            flag=0;
            break;
        }
    }

    if(flag)
    {
        L1_tlb->tlb_entries[i]=entry_to_be_transfered;
    }
}

int complete_tlb_search(tlb *L1_tlb, int number_of_entries_L1, tlb *L2_tlb, int number_of_entires_L2, int logical_page_number)
{
    int physical_frame_number = tlb_search(L1_tlb,12,logical_page_number);

    if(physical_frame_number!=-1)
    {
        return physical_frame_number;
    }

    physical_frame_number = tlb_search(L2_tlb,24,logical_page_number);

    if(physical_frame_number!=-1)
    {
        L2_to_L1_tlb_transfer(L1_tlb,12,L2_tlb,24,logical_page_number);
        physical_frame_number=tlb_search(L1_tlb,12,logical_page_number);
        return physical_frame_number;
    }

    return -1;
}