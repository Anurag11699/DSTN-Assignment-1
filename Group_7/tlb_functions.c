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
    }
}

int tlb_search(tlb* tlb_object, int number_of_entries, int logical_page_number)
{
    for(int i=0;i<number_of_entries;i++)
    {
        if(tlb_object->tlb_entries[i].valid==1 && tlb_object->tlb_entries[i].logical_page_number==logical_page_number)
        {
            return tlb_object->tlb_entries[i].physical_frame_number;
        }
    }

    return -1;
}