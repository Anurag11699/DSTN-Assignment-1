#ifndef TLB_DATA_STRUCTURES_H
#define TLB_DATA_STRUCTURES_H


/*
    ADT for a TLB entry

    int logical_page_number:22; -> used to store the logical page number of process

    unsigned int physical_frame_number:15; -> used to corresponding physical frame number

    unsigned int valid:1; -> used to tell if the tlb entry is valid or no

    unsigned int LRU_counter:8; -> Using LRU reference bit as replacement policy
*/
typedef struct tlb_entry
{
    unsigned int logical_page_number;
    unsigned int physical_frame_number;
    unsigned int valid:1;
    unsigned int LRU_counter:8;
}tlb_entry;



/*
    ADT for a TLB 

    Replacement Policy: LRU with 8 reference bits

    int number_of_entries; -> number of entries in TLB

    tlb_entry *tlb_entries; ->used for dynamically allocation the TLB based on number of entries
*/
typedef struct tlb
{
    int number_of_entries;
    tlb_entry *tlb_entries;
}tlb;

#endif /*TLB_DATA_STRUCTURES_H*/