#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "functions.h"

void add_free_frame(main_memory* main_memory_object, int frame_number)
{
    //free_frame *temp_tail = *tail;

    main_memory_object->frame_table[frame_number].pid=-1;
    main_memory_object->frame_table[frame_number].page_number=-1;

    free_frame *new_frame = (free_frame *)malloc(sizeof(free_frame));
    new_frame->frame_number=frame_number;
    new_frame->next=NULL;
    main_memory_object->ffl_dummy_head->number_free_frames++;
    //head->number_free_frames++;
    if(main_memory_object->ffl_dummy_head->next==NULL)
    {
        main_memory_object->ffl_dummy_head->next=new_frame;
        main_memory_object->ffl_tail=new_frame;
        //*tail = temp_tail;
        return;
    }
    main_memory_object->ffl_tail->next=new_frame;
    main_memory_object->ffl_tail=new_frame;
    //temp_tail->next=new_frame;
    //temp_tail=new_frame;
    //*tail = temp_tail;
}

int remove_free_frame(main_memory* main_memory_object)
{

    if( main_memory_object->ffl_dummy_head->next==NULL)
    {
        return -1; //no free frame to return;
    }

    int frame_number =  main_memory_object->ffl_dummy_head->next->frame_number;
    
    free_frame * temp =  main_memory_object->ffl_dummy_head->next;
    main_memory_object->ffl_dummy_head->next= main_memory_object->ffl_dummy_head->next->next;
    free(temp);
    main_memory_object->ffl_dummy_head->number_free_frames--;

    return frame_number;
}

void add_used_frame(main_memory* main_memory_object, int frame_number)
{
    //used_frame *temp_recently_used_frame = *recently_used_frame;

    used_frame *new_frame = (used_frame *)malloc(sizeof(free_frame));
    new_frame->frame_number=frame_number;
    new_frame->reference_bit=0;
    new_frame->next=NULL;
    main_memory_object->ufl_dummy_head->number_used_frames++;
    //head->number_used_frames++;

    if(main_memory_object->ufl_dummy_head->next==NULL)
    {
        main_memory_object->ufl_dummy_head->next=new_frame;
        new_frame->next=new_frame;

        main_memory_object->recently_used_frame=new_frame;
        //*recently_used_frame=temp_recently_used_frame;
        return;
    }

    new_frame->next=main_memory_object->recently_used_frame->next;
    main_memory_object->recently_used_frame->next=new_frame;
    main_memory_object->recently_used_frame=new_frame;

    //*recently_used_frame=temp_recently_used_frame;

}

int remove_used_frame(main_memory* main_memory_object)
{
    if(main_memory_object->ufl_dummy_head->next==NULL)
    {
        return -1;
    }

    //used_frame *temp_recently_used_frame = *recently_used_frame;

    while(main_memory_object->recently_used_frame->next->reference_bit!=0)
    {
        main_memory_object->recently_used_frame->next->reference_bit=0;
        main_memory_object->recently_used_frame=main_memory_object->recently_used_frame->next;
    }

    int frame_number = main_memory_object->recently_used_frame->next->frame_number;

    used_frame *temp = main_memory_object->recently_used_frame->next;
    main_memory_object->recently_used_frame->next=main_memory_object->recently_used_frame->next->next;
    free(temp);

    main_memory_object->ufl_dummy_head->number_used_frames--;
    //*recently_used_frame=temp_recently_used_frame;
    return frame_number;

    //need to update the page tables for the process from which this frame was removed. Will be done later in another function elsewhere. 
}

void transfer_to_free_frame_list(main_memory* main_memory_object, int frame_number)
{
    used_frame *walker = main_memory_object->ufl_dummy_head->next;
    used_frame *start = walker;

    while(walker->next != start)
    {
        if(walker->next->frame_number==frame_number)
        {
            used_frame* temp = walker->next;
            walker->next = walker->next->next;
            free(temp);
            main_memory_object->ufl_dummy_head->number_used_frames--;

            add_free_frame(main_memory_object,frame_number);
        }
    }

}

void set_reference_bit(main_memory* main_memory_object,int frame_number)
{
    if(main_memory_object->ufl_dummy_head->next==NULL)
    {
        return;
    }

    used_frame* walker = main_memory_object->ufl_dummy_head->next;

    while(walker->frame_number!=frame_number)
    {
        walker=walker->next;
    }
    walker->reference_bit=1;
    return;
}

int get_frame(main_memory *main_memory_object)
{
    //prefer placement over replacement
    int frame_number = remove_free_frame(main_memory_object);

    if(frame_number!=-1)
    {
        return frame_number;
    }

    frame_number = remove_used_frame(main_memory_object);

    //need to update the page table of the process from which this was taken from and frame table of the OS

    int current_pid_of_frame = get_pid_of_frame(main_memory_object,frame_number);
    int current_logical_page_of_frame = get_page_number_of_frame(main_memory_object,frame_number);

    //invalidate the page table entry for the pid and logical page we just got


   
    //add to used frame list
    add_used_frame(main_memory_object,frame_number);
    
    return frame_number;
}


page_table* initialize_page_table(int frame_number_occupied)
{
    int i;
    page_table* page_table_object = (page_table*)malloc(sizeof(page_table));
    page_table_object->frame_occupied=frame_number_occupied;
    page_table_object->page_table_entries = (page_table_entry*)malloc(sizeof(page_table_entry)*256); //there are 2^8 entries in each page table

    for(i=0;i<256;i++)
    {
        page_table_object->page_table_entries[i].cache_disabled=1;
        page_table_object->page_table_entries[i].frame_base_address=-1;
        page_table_object->page_table_entries[i].referenced=0;
        page_table_object->page_table_entries[i].modified=0;
        page_table_object->page_table_entries[i].valid=0;
    }

    return page_table_object;

}

int get_pid_of_frame(main_memory* main_memory_object,int frame_number)
{
    return main_memory_object->frame_table[frame_number].pid;
}

int get_page_number_of_frame(main_memory* main_memory_object,int frame_number)
{
    return main_memory_object->frame_table[frame_number].page_number;
}

page_table* get_page_table_pointer_of_frame(main_memory* main_memory_object, int frame_number)
{
    return main_memory_object->frame_table[frame_number].pointer_to_stored_page_table;
}

void update_frame_table_entry(main_memory* main_memory_object,int frame_number,int pid,int page_number,page_table* page_table_object)
{
    main_memory_object->frame_table[frame_number].pid=pid;
    main_memory_object->frame_table[frame_number].page_number=page_number;
    main_memory_object->frame_table[frame_number].pointer_to_stored_page_table=page_table_object;
}

int check_frame_ownership(main_memory* main_memory_object,int pid,int frame_number)
{
    if(main_memory_object->frame_table[frame_number].pid==pid)
    {
        return 1;
    }
    
    return -1;
}

int page_table_walk(kernel* kernel_object, main_memory* main_memory_object, int pid, int logical_address)
{
    //32 bit Virtual Address split as:  6 | 8 | 8 | 10 . Hence 3 level paging is required

    //get frame of outermost page table
    int outer_page_table_frame_number = kernel_object->pcb_array[pid].outer_page_base_address;

    //check if you own frame, otherwise load new frame for the outermost page table

    int own_outer_page_table = check_frame_ownership(main_memory_object,pid,outer_page_table_frame_number);

    if(own_outer_page_table==-1)
    {
        outer_page_table_frame_number = get_frame(main_memory_object);

        //make this frame the outermost page table
        page_table* outermost_page_table = initialize_page_table(outer_page_table_frame_number);

        //update the frame table for the frame we got
        update_frame_table_entry(main_memory_object,outer_page_table_frame_number,pid,-1,outermost_page_table); //-1 for logical page as it is a page table. send the pointer to outermost page table as this frame is a page table
        
        //add it to the pcb of this process
        kernel_object->pcb_array[pid].outer_page_base_address=outer_page_table_frame_number;
    }
    set_reference_bit(main_memory_object,outer_page_table_frame_number);

    page_table* outer_page_table = get_page_table_pointer_of_frame(main_memory_object,outer_page_table_frame_number);

    int outer_page_table_offset = get_outer_page_table_offset(logical_address); //first 6 bits are the offset in the outermost page table

    //go to the middle level of paging
    
    int middle_page_table_frame_number = outer_page_table->page_table_entries[outer_page_table_offset].frame_base_address;

    int own_middle_page_table = check_frame_ownership(main_memory_object,pid,middle_page_table_frame_number);

    if(own_middle_page_table==-1 || outer_page_table->page_table_entries[outer_page_table_offset].valid==0)
    {
        middle_page_table_frame_number = get_frame(main_memory_object);

        //make this frame the middle page table
        page_table* middle_page_table = initialize_page_table(middle_page_table_frame_number);

        //update frame table entry
        //update the frame table for the frame we got
        update_frame_table_entry(main_memory_object,middle_page_table_frame_number,pid,-1,middle_page_table); //-1 for logical page as it is a page table. send the pointer to middle page table as this frame is a page table

    }

    set_reference_bit(main_memory_object,middle_page_table_frame_number);

    page_table* middle_page_table = get_page_table_pointer_of_frame(main_memory_object,middle_page_table_frame_number);

    int middle_page_table_offset = get_middle_page_table_offset(logical_address);

    //go to inner page table

    int inner_page_table_frame_number = middle_page_table->page_table_entries[middle_page_table_offset].frame_base_address;

    int own_inner_page_table = check_frame_ownership(main_memory_object,pid,inner_page_table_frame_number);

    if(own_inner_page_table==-1 || middle_page_table->page_table_entries[middle_page_table_offset].valid==0)
    {
        inner_page_table_frame_number=get_frame(main_memory_object);

        //make this frame the inner page table
        page_table* inner_page_table = initialize_page_table(inner_page_table_frame_number);

        //update frame table entry
        //update the frame table for the frame we got
        update_frame_table_entry(main_memory_object,inner_page_table_frame_number,pid,-1,inner_page_table); //-1 for logical page as it is a page table. send the pointer to inner page table as this frame is a page table
    }

    set_reference_bit(main_memory_object,inner_page_table_frame_number);

    page_table* inner_page_table = get_page_table_pointer_of_frame(main_memory_object,inner_page_table_frame_number);

    int inner_page_table_offset = get_inner_page_table_offset(logical_address);

    //get the frame we needed to store this logical address
    int needed_frame_number = inner_page_table->page_table_entries[inner_page_table_offset].frame_base_address;

    int own_needed_frame = check_frame_ownership(main_memory_object,pid,needed_frame_number);

    //get the logical page number
    int logical_page_number = get_logical_page_number(logical_address);

    if(own_needed_frame==-1 || inner_page_table->page_table_entries[inner_page_table_offset].valid==0)
    {
        //insert this entry into the frame table and the page table of this process
        needed_frame_number = get_frame(main_memory_object);

        //insert entry into page table
        inner_page_table->page_table_entries[inner_page_table_offset].frame_base_address=needed_frame_number;
        inner_page_table->page_table_entries[inner_page_table_offset].referenced=1;

        //update frame table
        update_frame_table_entry(main_memory_object,needed_frame_number,pid,logical_page_number,NULL); //pointer to page table is null as this page wont contain page table
    }

    set_reference_bit(main_memory_object,needed_frame_number);

    
    return 1;
}

void invalidate_page_table_entry(kernel* kernel_object, main_memory* main_memory_object, int pid, int logical_address)
{
    //32 bit Virtual Address split as:  6 | 8 | 8 | 10 . Hence 3 level paging is required

    //get frame of outermost page table
    int outer_page_table_frame_number = kernel_object->pcb_array[pid].outer_page_base_address;

    //check if you own frame, otherwise load new frame for the outermost page table

    int own_outer_page_table = check_frame_ownership(main_memory_object,pid,outer_page_table_frame_number);

    if(own_outer_page_table==-1)
    {
        return;
    }
    //set_reference_bit(main_memory_object,outer_page_table_frame_number);

    page_table* outer_page_table = get_page_table_pointer_of_frame(main_memory_object,outer_page_table_frame_number);

    int outer_page_table_offset = get_outer_page_table_offset(logical_address); //first 6 bits are the offset in the outermost page table

    //go to the middle level of paging
    
    int middle_page_table_frame_number = outer_page_table->page_table_entries[outer_page_table_offset].frame_base_address;

    int own_middle_page_table = check_frame_ownership(main_memory_object,pid,middle_page_table_frame_number);

    //we cant reach the next level of paging, hence return
    if(own_middle_page_table==-1 || outer_page_table->page_table_entries[outer_page_table_offset].valid==0)
    {
        return;

    }

    //set_reference_bit(main_memory_object,middle_page_table_frame_number);

    page_table* middle_page_table = get_page_table_pointer_of_frame(main_memory_object,middle_page_table_frame_number);

    int middle_page_table_offset = get_middle_page_table_offset(logical_address);

    //go to inner page table

    int inner_page_table_frame_number = middle_page_table->page_table_entries[middle_page_table_offset].frame_base_address;

    int own_inner_page_table = check_frame_ownership(main_memory_object,pid,inner_page_table_frame_number);

    //we cant reach the next level of paging , hence return
    if(own_inner_page_table==-1 || middle_page_table->page_table_entries[middle_page_table_offset].valid==0)
    {
        return;
    }

    //set_reference_bit(main_memory_object,inner_page_table_frame_number);

    page_table* inner_page_table = get_page_table_pointer_of_frame(main_memory_object,inner_page_table_frame_number);

    int inner_page_table_offset = get_inner_page_table_offset(logical_address);

    //get the frame we needed to store this logical address
    int needed_frame_number = inner_page_table->page_table_entries[inner_page_table_offset].frame_base_address;

    int own_needed_frame = check_frame_ownership(main_memory_object,pid,needed_frame_number);

    //get the logical page number
    int logical_page_number = get_logical_page_number(logical_address);

    //anyway the entry that we wanted to invalidate, is invalid or the frame it points to is not ours
    if(own_needed_frame==-1 || inner_page_table->page_table_entries[inner_page_table_offset].valid==0)
    {
        return;
    }

    //invalidate the entry
    inner_page_table->page_table_entries[inner_page_table_offset].valid=0;

    //set_reference_bit(main_memory_object,needed_frame_number);

}


/*
PreConditions
Inputs: {main_memory_size in MB, frame_size in KB}

PostConditions
Output: {pointer to intialized main memory}
*/
main_memory* initialize_main_memory(int main_memory_size, int frame_size)
{
    //find number of frames
    
    int number_of_frames=(main_memory_size/frame_size)*1024;

    main_memory* main_memory_object = (main_memory *)malloc(sizeof(main_memory));
    main_memory_object->number_of_frames=number_of_frames;

    //initialize the frames
    //main_memory_object->frame_array=(frame *)malloc(number_of_frames*sizeof(frame));

    //initialize frame table
    main_memory_object->frame_table=(frame_table_entry *)malloc(number_of_frames*sizeof(frame_table_entry));

    //all pages are invalid upon initialization
    for(int frame_number=0;frame_number<number_of_frames;frame_number++)
    {
        main_memory_object->frame_table[frame_number].pid=-1;
        main_memory_object->frame_table[frame_number].page_number=-1;
        main_memory_object->frame_table[frame_number].pointer_to_stored_page_table=NULL;
    }

    //initialize free frame list
    main_memory_object->ffl_dummy_head = (free_frame_list_dummy_head *)malloc(sizeof(free_frame_list_dummy_head));
    main_memory_object->ffl_dummy_head->next=NULL;
    main_memory_object->ffl_dummy_head->number_free_frames=0;
    main_memory_object->ffl_tail=NULL;

    //initialize used frame list
    main_memory_object->ufl_dummy_head=(used_frame_list_dummy_head *)malloc(sizeof(used_frame_list_dummy_head));
    main_memory_object->ufl_dummy_head->next=NULL;
    main_memory_object->ufl_dummy_head->number_used_frames=0;
    main_memory_object->recently_used_frame=NULL;
    

    main_memory_object->ffl_dummy_head->number_free_frames=number_of_frames; //all frames are initially free;

    //add all frames to the free frame list
    for(int frame_number=0;frame_number<number_of_frames;frame_number++)
    {
        if(main_memory_object->frame_table[frame_number].pid==-1)
        {
            
            add_free_frame(main_memory_object,frame_number);
            fprintf(stderr,"Tail: %d\n",main_memory_object->ffl_tail->frame_number);
        }
        
    }
    return main_memory_object;
}
