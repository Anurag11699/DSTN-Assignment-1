#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<assert.h>
#include "functions.h"

/*auxillary function used to get x^y*/
int power(int x, unsigned int y) 
{ 
    int temp; 
    if( y == 0) 
    {
        return 1;
    }
         
    temp = power(x, y/2); 
    if (y%2 == 0) 
    {
        return temp*temp; 
    }
        
    else
    {
        return x*temp*temp; 
    }
        
} 

/*
PreConditions
Inputs:{pointer to main memory object, frame number which we need to add to free frame list}
0<=frame number<number of frames in main memory{32,768 for 32MB main memory and 1KB frame size}
main_memory_object!=NULL

Purpose of the Function: Add the frame number to the tail of the linked list of free frames and increment number of free frames

PostConditions
Updated free frame list with new entry
*/
void add_free_frame(main_memory* main_memory_object, int frame_number)
{
    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");


    main_memory_object->frame_table[frame_number].pid=-1;
    main_memory_object->frame_table[frame_number].page_number=-1;

    free_frame *new_frame = (free_frame *)malloc(sizeof(free_frame));
    new_frame->frame_number=frame_number;
    new_frame->next=NULL;
    main_memory_object->ffl_dummy_head->number_free_frames++;

    if(main_memory_object->ffl_dummy_head->next==NULL)
    {
        main_memory_object->ffl_dummy_head->next=new_frame;
        main_memory_object->ffl_tail=new_frame;
        return;
    }

    main_memory_object->ffl_tail->next=new_frame;
    main_memory_object->ffl_tail=new_frame;
}

/*
PreConditions
Inputs:{pointer to main memory object}
main_memory_object!=NULL

Purpose of the Function: Remove a free frame entry from the head of the free frame list and return the frame number. Decrements the number of free frames.

PostConditions
Updated free frame list upon removing entry
Return Value: 
frame number of the removed frame if the free frame list was not empty. {0<=frame number<number of frames in main memory}
-1, if free frame list was empty
*/
int remove_free_frame(main_memory* main_memory_object)
{
    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    if( main_memory_object->ffl_dummy_head->next==NULL)
    {
        return -1; //no free frame to return;
    }

    int frame_number =  main_memory_object->ffl_dummy_head->next->frame_number;
    
    free_frame * temp =  main_memory_object->ffl_dummy_head->next;
    main_memory_object->ffl_dummy_head->next= main_memory_object->ffl_dummy_head->next->next;
    free(temp);
    main_memory_object->ffl_dummy_head->number_free_frames--;

    //check PostCondition
    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");

    return frame_number;
}

/*
PreConditions
Inputs:{pointer to main memory object, frame number which we need to add to the used frame list}
0<=frame number<number of frames in main memory{32,768 for 32MB main memory and 1KB frame size}
main_memory_object!=NULL

Purpose of the Function: Add the {frame number, reference bit} to linked list of used frames(fifo queue) and increment number of used frames. Make the currently added frame as the recently used frame

PostConditions
Updated used frame list with new entry
*/
void add_used_frame(main_memory* main_memory_object, int frame_number)
{

    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");
    
    used_frame *new_frame = (used_frame *)malloc(sizeof(free_frame));
    new_frame->frame_number=frame_number;
    new_frame->reference_bit=0;
    new_frame->next=NULL;
    main_memory_object->ufl_dummy_head->number_used_frames++;
    

    if(main_memory_object->ufl_dummy_head->next==NULL)
    {
        main_memory_object->ufl_dummy_head->next=new_frame;
        new_frame->next=new_frame;

        main_memory_object->recently_used_frame=new_frame;
        
        main_memory_object->ufl_dummy_head->next=main_memory_object->recently_used_frame;
        return;
    }

    new_frame->next=main_memory_object->recently_used_frame->next;
    main_memory_object->recently_used_frame->next=new_frame;
    main_memory_object->recently_used_frame=new_frame;

    main_memory_object->ufl_dummy_head->next=main_memory_object->recently_used_frame;

}

/*
PreConditions
Inputs:{pointer to main memory object}
kernel_object!=NULL
main_memory_object!=NULL

Purpose of the Function: 
The functions traverses the used frame list starting from the element after the recently used frame. This element is the frame which was used first (as it is a fifo circular queue). If the reference bit is set to 1, make it 0 and continue until you find an entry whose reference bit is 0. (giving second chance to a frame). This frame is removed from the list and returned. The recently used frame is updated appropriately.
We also need to take care of thrasing. We cannot remove a frame from a process who has less frames than the lower bound of frames per process.

PostConditions
Updated used frame list upon removing entry
Return Value: 
frame number of the removed frame if the used frame list was not empty. {0<=frame number<number of frames in main memory}
-1, if the used frame list was empty
*/
int remove_used_frame(kernel* kernel_object ,main_memory* main_memory_object)
{
    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    
    if(main_memory_object->ufl_dummy_head->next==NULL || main_memory_object->ufl_dummy_head->number_used_frames==0)
    {
        return -1;
    }

   //fprintf(stderr,"IN REMOVE USED FRAME\n");

   
    //first condition takes care of the refernce bit condition. If reference bit of the frame is 1, then set it to 0 and continue
    //second condition takes care of trashing. If the frame belongs to a process who holds less number of frames than the lower bound then we must not take from that process
    while(main_memory_object->recently_used_frame->next->reference_bit!=0 || kernel_object->pcb_array[get_pid_of_frame(main_memory_object,main_memory_object->recently_used_frame->frame_number)].number_of_frames_used<number_of_frames_per_process_lower_bound)
    {
        main_memory_object->recently_used_frame->next->reference_bit=0;
        main_memory_object->recently_used_frame=main_memory_object->recently_used_frame->next;
    }

    int frame_number = main_memory_object->recently_used_frame->next->frame_number;


    used_frame *temp = main_memory_object->recently_used_frame->next;
    main_memory_object->recently_used_frame->next=main_memory_object->recently_used_frame->next->next;
    free(temp);

    main_memory_object->ufl_dummy_head->number_used_frames--;

    main_memory_object->ufl_dummy_head->next=main_memory_object->recently_used_frame;

    //check PostCondition
    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");
    
    return frame_number;

}


/*
PreConditions
Inputs:{pointer to kernel object, pointer to main memory object, pid of process whose frames we want to free, number of frames we need to free}
kernel_object!=NULL
main_memory_object!=NULL
0<=pid<maximum number of processes

Purpose of the Function: 
This function is used to prevent thrashing. It is called upon when a process exceeds the upper bound of frames.The functions traverses the used frame list starting from the element after the recently used frame and removes the required number of frames from the process and adds them to the free frame list. 
This function is also used to remove all process frames upon termination.

PostConditions
Updated used frame list
Updated free frame list
Updated frame table
*/
void transfer_to_free_frame_list(kernel* kernel_object,main_memory* main_memory_object, int pid, int number_of_frames_to_transfer)
{

    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    used_frame *walker = main_memory_object->ufl_dummy_head->next;
    //used_frame *start = walker;

    while(number_of_frames_to_transfer>0)
    {
        if(get_pid_of_frame(main_memory_object,walker->next->frame_number)==pid)
        {
            //remove this frame from the used frame list
            used_frame* temp = walker->next;
            walker->next=walker->next->next;
            main_memory_object->ufl_dummy_head->number_used_frames--;

            if(temp==main_memory_object->recently_used_frame)
            {
                main_memory_object->recently_used_frame=walker;
            }

            // fprintf(output_fd,"Process: %d | Frame Removed: %d\n",pid,temp->frame_number);
            // fflush(output_fd);

            //add this frame to free frame list
            update_frame_table_entry(kernel_object,main_memory_object,temp->frame_number,-1,-1);
            add_free_frame(main_memory_object,temp->frame_number);

            kernel_object->pcb_array[pid].number_of_frames_used--;
            number_of_frames_to_transfer--;

            free(temp);
        }
        else
        {
            walker=walker->next;
        }
        
       
    }

    main_memory_object->ufl_dummy_head->next = main_memory_object->recently_used_frame;

}


/*
PreConditions
Inputs:{pointer to main memory object, frame number whose reference bit must be set in the used frame list}
0<=frame number<number of frames in main memory{32,768 for 32MB main memory and 1KB frame size}
frame number entry is present in used frame list

Purpose of the Function: Traverse the used frame list and set the reference bit of that entry to 1.

PostConditions
Updated used frame list.
*/
void set_reference_bit(main_memory* main_memory_object,int frame_number)
{

    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");


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


/*
PreConditions
Inputs:{pointer to the kernel object,pointer to main memory object, int pid of requesting process, if the requested frame is for page table, has the frame been brought in before}
main_memory_object!=NULL
kernel_object!=NULL
0<=pid<maximum number of processes

Purpose of the Function: This function is called by a process to get a frame for itself when needed. The function first tries to get a frame from the free frame list(using get_free_frame method)(as placement is preferred over replacement). If we correctly received the frame from the free frame list, the function returns the frame number. If the free frame list is empty, the last used frame is extracted from the used frame list (using get_used_frame method). Upon receiving the used frame we must find out the pid of the proccess currently using the frame and the logical page of that process which was using it. Using this information we can invalidate the entry for this frame in the page table of that process (using invalidate_page_table_entry method)

PostConditions
Return Value: 
frame number of the removed frame. {0<=frame number<number of frames in main memory}
*/
int get_frame(kernel* kernel_object,main_memory *main_memory_object,int pid,int is_page_table, int brought_in_before)
{

    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(pid<kernel_object->max_number_of_processes && "pid exceeded bounds");
    assert(pid>=0 && "pid less than 0 not allowed");

    //add page fault times
    total_time_taken=total_time_taken+page_fault_overhead_time+restart_overhead_time;

    //we are getting frame for a page table
    if(is_page_table==1)
    {
        //as it is an in memory DS 
        if(brought_in_before==0)
        {
            total_time_taken=total_time_taken; //we initialize the page table, dont swap it
        }
        else if(brought_in_before==1)
        {
            total_time_taken=total_time_taken+swap_space_to_main_memory_transfer_time; //swap in the page
        }
    }
    else if(is_page_table==0)
    {
        
        total_time_taken=total_time_taken+swap_space_to_main_memory_transfer_time; //swap in the page
        
    }

    //to avoid thrasing, if this process has crossed its number of pages upper limit, remove frames from it until it reaches the average number of frames per process
    if(kernel_object->pcb_array[pid].number_of_frames_used > number_of_frames_per_process_upper_bound)
    {
        int number_of_frames_to_remove_from_process = kernel_object->pcb_array[pid].number_of_frames_used - number_of_frames_per_process_average;

        // fprintf(output_fd,"Upper Bound: %d | Number of PID %d Frames: %d| Average Frames: %d\n",number_of_frames_per_process_upper_bound,pid,kernel_object->pcb_array[pid].number_of_frames_used, number_of_frames_per_process_average);
        // fflush(output_fd);

        //print_ffl(main_memory_object);
        //print_ufl(main_memory_object);

        transfer_to_free_frame_list(kernel_object,main_memory_object,pid,number_of_frames_to_remove_from_process);

        // fprintf(output_fd,"Upper Bound: %d | Number of PID %d Frames: %d| Average Frames: %d\n",number_of_frames_per_process_upper_bound,pid,kernel_object->pcb_array[pid].number_of_frames_used, number_of_frames_per_process_average);
        // fflush(output_fd);

        //print_ffl(main_memory_object);
        //print_ufl(main_memory_object);

        //fprintf(stderr,"HI\n");
        //sleep(1);
    }

    //prefer placement over replacement
    int frame_number = remove_free_frame(main_memory_object);

    if(frame_number!=-1)
    {
        add_used_frame(main_memory_object,frame_number);

        //increment the number number of frames used by the process
        kernel_object->pcb_array[pid].number_of_frames_used++;

        //check PostConditions
        assert(frame_number>=0 && "Frame Number out of bounds");
        assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");
        return frame_number;
    }

    //fprintf(output_fd,"REMOVING USED FRAME\n");
    //fflush(output_fd);

    //print_ufl(main_memory_object);
    frame_number = remove_used_frame(kernel_object,main_memory_object);
    //print_ufl(main_memory_object);


    //need to update the page table of the process from which this was taken from and frame table of the OS
    int current_pid_of_frame = get_pid_of_frame(main_memory_object,frame_number);
    int current_logical_page_of_frame = get_page_number_of_frame(main_memory_object,frame_number);

    //invalidate the frame table entry for this frame
    main_memory_object->frame_table[frame_number].pid=-1;
    
    //decrement the number number of frames used by the process
    kernel_object->pcb_array[current_pid_of_frame].number_of_frames_used--;

    //invalidate the page table entry for the pid and logical page we just got

    invalidate_page_table_entry(kernel_object,main_memory_object,current_pid_of_frame,current_logical_page_of_frame);

    //if the frame that we got was modified, it must be swapped out
    if(main_memory_object->frame_table[frame_number].modified==1)
    {
        total_time_taken=total_time_taken+main_memory_to_swap_space_transer_time;

        main_memory_object->frame_table[frame_number].modified=0;
    }
    
   
    //add to used frame list
    add_used_frame(main_memory_object,frame_number);

    //increment the number number of frames used by the process
    kernel_object->pcb_array[pid].number_of_frames_used++;
    

    //check PostConditions
    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");
    
    return frame_number;
}


/*
PreConditions
Inputs: {frame number that this page table will occupy}
0<=frame number<number of frames in main memory

Purpose of the Function: This function intializes a page table

PostConditions
Output: {pointer to intialized page table}
*/
page_table* initialize_page_table(int frame_number_occupied)
{

    int i;
    page_table* page_table_object = (page_table*)malloc(sizeof(page_table));
    //page_table_object->frame_occupied=frame_number_occupied;
    page_table_object->page_table_entries = (page_table_entry*)malloc(sizeof(page_table_entry)*256); //there are 2^8 entries in each page table

    for(i=0;i<256;i++)
    {
        //page_table_object->page_table_entries[i].cache_disabled=1;
        page_table_object->page_table_entries[i].frame_base_address=-1;
        page_table_object->page_table_entries[i].initialized_before=0;
        //page_table_object->page_table_entries[i].referenced=0;
        //page_table_object->page_table_entries[i].modified=0;
        page_table_object->page_table_entries[i].valid=0;
        page_table_object->page_table_entries[i].pointer_to_page_table=NULL;
        page_table_object->page_table_entries[i].swapped_out_before=0;
    }

    return page_table_object;

}


/*
PreConditions
Inputs: {pointer to main memory object, frame number}
0<=frame number<number of frames in main memory{32,768 for 32MB main memory and 1KB frame size}
main_memory_object!=NULL

Purpose of the Function: This function is used to just access the OS frame table using the frame number as its index and return the pid of the process using that frame

PostConditions
Output:{pid of process which has acquired that frame}
*/
int get_pid_of_frame(main_memory* main_memory_object,int frame_number)
{
    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");

    return main_memory_object->frame_table[frame_number].pid;
}



/*
PreConditions
Inputs: {pointer to main memory object, frame number}
0<=frame number<number of frames in main memory{32,768 for 32MB main memory and 1KB frame size}
main_memory_object!=NULL

Purpose of the Function: This function is used to just access the OS frame table using the frame number as its index and return the logical page of the process using that frame holds

PostConditions
Output:{logical page of the process using that frame holds}
if the frame is a page table, returns -1
*/
int get_page_number_of_frame(main_memory* main_memory_object,int frame_number)
{
    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");

    return main_memory_object->frame_table[frame_number].page_number;
}



/*
PreConditions
Inputs: {pointer to main memory object, frame number, pid, page_number, pointer to page table object}
kernel_object!=NULL
main_memory_object!=NULL
0<=frame number<number of frames in main memory{32,768 for 32MB main memory and 1KB frame size}
0<=pid<maximum number of processes | -1 if doesnt belong to any process
0<=page_number<=0x3fffff | -1 if frame doesnt contain a logical page

Purpose of the Function: This function is used to set the frame table entry indexed by the frame number

PostConditions
Updated OS frame table
*/
void update_frame_table_entry(kernel* kernel_object,main_memory* main_memory_object,int frame_number,int pid,int page_number)
{

    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");


    main_memory_object->frame_table[frame_number].pid=pid;
    main_memory_object->frame_table[frame_number].page_number=page_number;
    //main_memory_object->frame_table[frame_number].pointer_to_stored_page_table=page_table_object;

    //add time taken for this
    total_time_taken = total_time_taken + update_frame_table_entry_time;
}


/*
PreConditions
Inputs: {pointer to main memory object, pid, frame number}
kernel_object!=NULL
main_memory_object!=NULL
0<=pid<maximum number of processes


Purpose of the Function: Checks if the pid stored in the frame table entry matches the pid in the argument of the function. If it matches, the frame is owned by that process

PostConditions
Return Value:
1 if frame is owned by the process whose pid is given in the function argument
-1, otherwise
*/
int check_frame_ownership(kernel* kernel_object,main_memory* main_memory_object,int pid,int frame_number)
{
    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");


    assert(pid<kernel_object->max_number_of_processes && "pid exceeded bounds");
    assert(pid>=0 && "pid less than 0 not allowed");


    if(frame_number<0)
    {
        return -1;
    }
    if(main_memory_object->frame_table[frame_number].pid==pid)
    {
        return 1;
    }
    
    return -1;
}


/*
PreConditions
Inputs: {pointer to kernel object, pointer to main memory object, pid, logical address process is requesting for }
kernel_object!=NULL
main_memory_object!=NULL
0<=logical address< 2^32
0<=pid<maximum number of processes

Purpose of the Function: Starts from the outermost page table stored in the pcb array entry of the process in the kernel object. Goes through 3 levels of paging. If any page table is missing in the page walk, the function gets a frame for that page table an initializes the page table in that frame. In the end the function gets a frame to store the logical page of the function and stores its enty in the innermost page table.

PostConditions
Return Value:
Frame number assigned to the logical page requested by process
0<=frame number<number of frames in main memory{32,768 for 32MB main memory and 1KB frame size}
*/
int page_table_walk(kernel* kernel_object, main_memory* main_memory_object, int pid, unsigned long int logical_address)
{

    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(logical_address >= 0x0 && "Virtual Address must be >= 0x0");
    assert(logical_address <= 0xffffffff && "Virtual Address must be <= 0xffffffff");

    assert(pid<kernel_object->max_number_of_processes && "pid exceeded bounds");
    assert(pid>=0 && "pid less than 0 not allowed");


    fprintf(output_fd,"Starting page walk of PID: %d | Request: %lx (hexadecimal) | Request: %ld (decimal)\n",pid,logical_address,logical_address);
    fflush(output_fd);
    //32 bit Virtual Address split as:  6 | 8 | 8 | 10 . Hence 3 level paging is required

    //get the logical page number
    int logical_page_number = get_logical_page_number(logical_address,frame_size);

    //get frame of outermost page table
    int outer_page_table_frame_number = kernel_object->pcb_array[pid].outer_page_base_address;

    //add time to get the entry from main memory
    total_time_taken = total_time_taken + pcb_data_processor_to_from_main_memory_transfer_time;

    //increment page accesses
    number_of_page_accesses++;

    //check if you own frame, otherwise load new frame for the outermost page table

    int own_outer_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,outer_page_table_frame_number);

    fprintf(output_fd,"Outer Page Table Frame Number: %d | Own it: %d\n",outer_page_table_frame_number,own_outer_page_table);
    fflush(output_fd);

    if(own_outer_page_table==-1)
    {
        outer_page_table_frame_number = get_frame(kernel_object,main_memory_object,pid,1,kernel_object->pcb_array[pid].outer_page_base_address_initialized_before);

        //make this frame the outermost page table
        
        if(kernel_object->pcb_array[pid].outer_page_base_address_initialized_before==0)
        {
            page_table *outermost_page_table = initialize_page_table(outer_page_table_frame_number);
            kernel_object->pcb_array[pid].outer_page_table=outermost_page_table;
            kernel_object->pcb_array[pid].outer_page_base_address_initialized_before=1;
        }
        

        //update the frame table for the frame we got
        update_frame_table_entry(kernel_object,main_memory_object,outer_page_table_frame_number,pid,logical_page_number); 
        
        //add it to the pcb of this process
        kernel_object->pcb_array[pid].outer_page_base_address=outer_page_table_frame_number;

        //add time to transfer this entry back to main memory
        total_time_taken = total_time_taken + pcb_data_processor_to_from_main_memory_transfer_time;

        //increment page misses
        number_of_page_misses++;
        
    }

    own_outer_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,outer_page_table_frame_number);
    fprintf(output_fd,"Outer Page Table Frame Number: %d | Own it: %d\n",outer_page_table_frame_number,own_outer_page_table);
    fflush(output_fd);

    set_reference_bit(main_memory_object,outer_page_table_frame_number);

    page_table* outer_page_table = kernel_object->pcb_array[pid].outer_page_table;

    //page_table* outer_page_table = get_page_table_pointer_of_frame(main_memory_object,outer_page_table_frame_number);

    int outer_page_table_offset = get_outer_page_table_offset(logical_address); //first 6 bits are the offset in the outermost page table

    fprintf(output_fd,"Outer Page Table Offset: %d\n",outer_page_table_offset);
    fflush(output_fd);

    //go to the middle level of paging
    
    int middle_page_table_frame_number = outer_page_table->page_table_entries[outer_page_table_offset].frame_base_address;

    //add time to get this page table entry from main memory to processor
    total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

    //increment page accesses 
    number_of_page_accesses++;

    int own_middle_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,middle_page_table_frame_number);

    fprintf(output_fd,"Middle Page Table Frame Number: %d | Own it: %d\n",middle_page_table_frame_number, own_middle_page_table);
    fflush(output_fd);

    if(own_middle_page_table==-1 || outer_page_table->page_table_entries[outer_page_table_offset].valid==0)
    {
        middle_page_table_frame_number = get_frame(kernel_object,main_memory_object,pid,1,outer_page_table->page_table_entries[outer_page_table_offset].initialized_before);

        //make this frame the middle page table
        if(outer_page_table->page_table_entries[outer_page_table_offset].initialized_before==0)
        {
            page_table* middle_page_table = initialize_page_table(middle_page_table_frame_number);
            outer_page_table->page_table_entries[outer_page_table_offset].pointer_to_page_table=middle_page_table;
            outer_page_table->page_table_entries[outer_page_table_offset].initialized_before=1;
        }
      

        //update frame table entry
        //update the frame table for the frame we got
        update_frame_table_entry(kernel_object,main_memory_object,middle_page_table_frame_number,pid,logical_page_number); 

        //link it from outermost page table
        outer_page_table->page_table_entries[outer_page_table_offset].frame_base_address=middle_page_table_frame_number;
        outer_page_table->page_table_entries[outer_page_table_offset].valid=1;
        //outer_page_table->page_table_entries[outer_page_table_offset].modified=0;

        //add time to trnasfer the updated page table entry from processor to main memory
        total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

        //increment page misses
        number_of_page_misses++;

    }

    own_middle_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,middle_page_table_frame_number);

    fprintf(output_fd,"Middle Page Table Frame Number: %d | Own it: %d\n",middle_page_table_frame_number, own_middle_page_table);
    fflush(output_fd);

    set_reference_bit(main_memory_object,middle_page_table_frame_number);

    page_table* middle_page_table = outer_page_table->page_table_entries[outer_page_table_offset].pointer_to_page_table;
    //page_table* middle_page_table = get_page_table_pointer_of_frame(main_memory_object,middle_page_table_frame_number);

    int middle_page_table_offset = get_middle_page_table_offset(logical_address);

    fprintf(output_fd,"Middle Page Table Offset: %d\n",middle_page_table_offset);
    fflush(output_fd);

    //go to inner page table

    int inner_page_table_frame_number = middle_page_table->page_table_entries[middle_page_table_offset].frame_base_address;

    //add time to get this page table entry from main memory to processor
    total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

    //increment number of page accesses
    number_of_page_accesses++;

    int own_inner_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,inner_page_table_frame_number);

    fprintf(output_fd,"Inner Page Table Frame Number: %d | Own it: %d\n",inner_page_table_frame_number, own_inner_page_table);
    fflush(output_fd);

    if(own_inner_page_table==-1 || middle_page_table->page_table_entries[middle_page_table_offset].valid==0)
    {
        inner_page_table_frame_number=get_frame(kernel_object,main_memory_object,pid,1,middle_page_table->page_table_entries[middle_page_table_offset].initialized_before);

        //make this frame the inner page table
        if(middle_page_table->page_table_entries[middle_page_table_offset].initialized_before==0)
        {   
            page_table* inner_page_table = initialize_page_table(inner_page_table_frame_number);
            middle_page_table->page_table_entries[middle_page_table_offset].pointer_to_page_table=inner_page_table;
            middle_page_table->page_table_entries[middle_page_table_offset].initialized_before=1;
        }
       

        //update frame table entry
        //update the frame table for the frame we got
        update_frame_table_entry(kernel_object,main_memory_object,inner_page_table_frame_number,pid,logical_page_number); 

        //link it from middle page table
        middle_page_table->page_table_entries[middle_page_table_offset].frame_base_address=inner_page_table_frame_number;
        middle_page_table->page_table_entries[middle_page_table_offset].valid=1;
        //middle_page_table->page_table_entries[middle_page_table_offset].modified=0;

        //add time to trnasfer the updated page table entry from processor to main memory
        total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

        //increment number of page misses
        number_of_page_misses++;
        
    }

    own_inner_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,inner_page_table_frame_number);

    fprintf(output_fd,"Inner Page Table Frame Number: %d | Own it: %d\n",inner_page_table_frame_number, own_inner_page_table);
    fflush(output_fd);

    set_reference_bit(main_memory_object,inner_page_table_frame_number);

    page_table* inner_page_table = middle_page_table->page_table_entries[middle_page_table_offset].pointer_to_page_table;

    //page_table* inner_page_table = get_page_table_pointer_of_frame(main_memory_object,inner_page_table_frame_number);

    int inner_page_table_offset = get_inner_page_table_offset(logical_address);

    fprintf(output_fd,"Inner Page Table Offset: %d\n",inner_page_table_offset);
    fflush(output_fd);

    //get the frame we needed to store this logical address
    int needed_frame_number = inner_page_table->page_table_entries[inner_page_table_offset].frame_base_address;


    //add time to get this page table entry from main memory to processor
    total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

    //increment number of page accesses
    number_of_page_accesses++;

    int own_needed_frame = check_frame_ownership(kernel_object,main_memory_object,pid,needed_frame_number);

  
    fprintf(output_fd,"Logical Page Number: %d\n",logical_page_number);
    fflush(output_fd);

    fprintf(output_fd,"Needed Frame Number: %d | Own it: %d\n",needed_frame_number, own_needed_frame);
    fflush(output_fd);
    
    if(own_needed_frame==-1 || inner_page_table->page_table_entries[inner_page_table_offset].valid==0)
    {
        //insert this entry into the frame table and the page table of this process
        needed_frame_number = get_frame(kernel_object,main_memory_object,pid,0,inner_page_table->page_table_entries[inner_page_table_offset].initialized_before);
        

        //insert entry into page table
        inner_page_table->page_table_entries[inner_page_table_offset].frame_base_address=needed_frame_number;
        //inner_page_table->page_table_entries[inner_page_table_offset].referenced=1;

        //update frame table
        update_frame_table_entry(kernel_object,main_memory_object,needed_frame_number,pid,logical_page_number); 

        //link it from inner page table
        inner_page_table->page_table_entries[inner_page_table_offset].frame_base_address=needed_frame_number;
        inner_page_table->page_table_entries[inner_page_table_offset].pointer_to_page_table=NULL;
        inner_page_table->page_table_entries[inner_page_table_offset].valid=1;
        //inner_page_table->page_table_entries[inner_page_table_offset].modified=0;
        inner_page_table->page_table_entries[inner_page_table_offset].initialized_before=1;

        //add time to trnasfer the updated page table entry from processor to main memory
        total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

        //increment number of page misses
        number_of_page_misses++;
    }

    own_needed_frame = check_frame_ownership(kernel_object,main_memory_object,pid,needed_frame_number);
    fprintf(output_fd,"Needed Frame Number: %d | Own it: %d\n",needed_frame_number, own_needed_frame);
    fflush(output_fd);

    set_reference_bit(main_memory_object,needed_frame_number);

    fprintf(output_fd,"\n\n");
    fflush(output_fd);
    // fprintf(output_fd,"Print Outer Page Table\n");
    //fflush(output_fd);
    // print_page_table(outer_page_table);

    // fprintf(output_fd,"Print Middle Page Table\n");
    //fflush(output_fd);
    // print_page_table(middle_page_table);

    // fprintf(output_fd,"Print Inner Page Table\n");
    //fflush(output_fd);
    // print_page_table(inner_page_table);

    //print_frame_table(main_memory_object);
    //fflush(output_fd);

    assert(needed_frame_number>=0 && "Frame Number out of bounds");
    assert(needed_frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");

    return needed_frame_number;
}

/*
PreConditions
Inputs: {pointer to kernel object, pointer to main memory object, pid of processes whose page we need to invalidate , the logical page we need to invalidate }
kernel_object!=NULL
main_memory_object!=NULL
0<=logical page<=2^22
0<=pid<maximum number of processes

Purpose of the Function: Invalidate the page table entry of the given process and logcial page number

PostConditions
In the page table of the process whose pid we got, we have invalidated the entry for the given logical page.
*/
void invalidate_page_table_entry(kernel* kernel_object, main_memory* main_memory_object, int pid,unsigned long int logical_page)
{

    //check PreConditions
    assert(kernel_object!=NULL && "pointer to kernel cannot be NULL");
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(logical_page >= 0x0 && "Logical page number must be >= 0x0");
    assert(logical_page <= 0x3fffff && "Logical page number must be <= 0x3fffff");

    assert(pid<kernel_object->max_number_of_processes && "pid exceeded bounds");
    assert(pid>=0 && "pid less than 0 not allowed");

    if(pid<0 || logical_page<0)
    {
        return;
    }


    int logical_address = logical_page<<10;
    //32 bit Virtual Address split as:  6 | 8 | 8 | 10 . Hence 3 level paging is required

    //get frame of outermost page table
    int outer_page_table_frame_number = kernel_object->pcb_array[pid].outer_page_base_address;

    //add time to get this entry from pcb in main memory to processor
    total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

    //we dont own the outer page itself, hence return
    if (outer_page_table_frame_number==-1)
    {
        return;
    }
    

    //check if you own frame, otherwise load new frame for the outermost page table

    int own_outer_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,outer_page_table_frame_number);

    if(own_outer_page_table==-1)
    {
        kernel_object->pcb_array[pid].outer_page_base_address=-1;
        return;
    }
    //set_reference_bit(main_memory_object,outer_page_table_frame_number);

    page_table* outer_page_table = kernel_object->pcb_array[pid].outer_page_table;
    //page_table* outer_page_table = get_page_table_pointer_of_frame(main_memory_object,outer_page_table_frame_number);

    int outer_page_table_offset = get_outer_page_table_offset(logical_address); //first 6 bits are the offset in the outermost page table

    //go to the middle level of paging
    
    int middle_page_table_frame_number = outer_page_table->page_table_entries[outer_page_table_offset].frame_base_address;

    //add time to get this page table entry from main memory to processor
    total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

    int own_middle_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,middle_page_table_frame_number);

    //we cant reach the next level of paging, hence return
    if(own_middle_page_table==-1 || outer_page_table->page_table_entries[outer_page_table_offset].valid==0)
    {
        //invalidate the entry in case we dont own the frame
        outer_page_table->page_table_entries[outer_page_table_offset].valid=0;
        return;

    }

    //set_reference_bit(main_memory_object,middle_page_table_frame_number);

    page_table* middle_page_table = outer_page_table->page_table_entries[outer_page_table_offset].pointer_to_page_table;
    //page_table* middle_page_table = get_page_table_pointer_of_frame(main_memory_object,middle_page_table_frame_number);

    int middle_page_table_offset = get_middle_page_table_offset(logical_address);

    //go to inner page table

    int inner_page_table_frame_number = middle_page_table->page_table_entries[middle_page_table_offset].frame_base_address;

    //add time to get this page table entry from main memory to processor
    total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

    int own_inner_page_table = check_frame_ownership(kernel_object,main_memory_object,pid,inner_page_table_frame_number);

    //we cant reach the next level of paging , hence return
    if(own_inner_page_table==-1 || middle_page_table->page_table_entries[middle_page_table_offset].valid==0)
    {
        //invalidate the entry in case we dont own the frame
        middle_page_table->page_table_entries[middle_page_table_offset].valid=0;
        return;
    }

    //set_reference_bit(main_memory_object,inner_page_table_frame_number);

    page_table* inner_page_table = middle_page_table->page_table_entries[middle_page_table_offset].pointer_to_page_table;

    //page_table* inner_page_table = get_page_table_pointer_of_frame(main_memory_object,inner_page_table_frame_number);

    int inner_page_table_offset = get_inner_page_table_offset(logical_address);

    //get the frame we needed to store this logical address
    int needed_frame_number = inner_page_table->page_table_entries[inner_page_table_offset].frame_base_address;

    //add time to get this page table entry from main memory to processor
    total_time_taken = total_time_taken + page_table_entry_processor_to_from_main_memory_transfer_time;

    int own_needed_frame = check_frame_ownership(kernel_object,main_memory_object,pid,needed_frame_number);


    //anyway the entry that we wanted to invalidate, is invalid or the frame it points to is not ours
    if(own_needed_frame==-1 || inner_page_table->page_table_entries[inner_page_table_offset].valid==0)
    {
        //invalidate the entry in case we dont own the frame
        inner_page_table->page_table_entries[inner_page_table_offset].valid=0;
        return;
    }

    //invalidate the entry
    inner_page_table->page_table_entries[inner_page_table_offset].valid=0;

    //set_reference_bit(main_memory_object,needed_frame_number);

}


/*
PreConditions
None

Purpose of the Function: Initialze Main Memory Data Structure and all the components in the Main Memory like the frame table, free frame list, used frame list (with second chance)

PostConditions
Output: {pointer to intialized main memory}
main_memory_object!=NULL
*/
main_memory* initialize_main_memory()
{
    //find number of frames
    
    int number_of_frames=power(2,(main_memory_size - frame_size));

    fprintf(output_fd,"Number of Frames: %d\n",number_of_frames);
    fflush(output_fd);

    main_memory* main_memory_object = (main_memory *)malloc(sizeof(main_memory));
    main_memory_object->number_of_frames=number_of_frames;

    total_number_of_frames=number_of_frames;
    //initialize the frames
    //main_memory_object->frame_array=(frame *)malloc(number_of_frames*sizeof(frame));

    //initialize frame table
    main_memory_object->frame_table=(frame_table_entry *)malloc(number_of_frames*sizeof(frame_table_entry));

    //all pages are invalid upon initialization
    for(int frame_number=0;frame_number<number_of_frames;frame_number++)
    {
        main_memory_object->frame_table[frame_number].pid=-1;
        main_memory_object->frame_table[frame_number].page_number=-1;
        main_memory_object->frame_table[frame_number].modified=0;
        //main_memory_object->frame_table[frame_number].pointer_to_stored_page_table=NULL;
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
            //fprintf(stderr,"Tail: %d\n",main_memory_object->ffl_tail->frame_number);
        }
        
    }

    //check PostConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    return main_memory_object;
}


/*
PreConditions
Inputs: {pointer to main memory object, frame number which we need to mark as modified}
main_memory_object!=NULL
0<=frame number<number of frames in main memory{32,768 for 32MB main memory and 1KB frame size}

Purpose of the Function: Initialze Main Memory Data Structure and all the components in the Main Memory like the frame table, free frame list, used frame list (with second chance)

PostConditions
Output: {pointer to intialized main memory}
main_memory_object!=NULL
*/
void mark_frame_modified(main_memory* main_memory_object, int frame_number)
{
    //check PreConditions
    assert(main_memory_object!=NULL && "pointer to main memory cannot be NULL");

    assert(frame_number>=0 && "Frame Number out of bounds");
    assert(frame_number<main_memory_object->number_of_frames && "Frame Number out of bounds");

    main_memory_object->frame_table[frame_number].modified=1;

    //add time taken for this
    total_time_taken = total_time_taken + update_bit_in_main_memory_time;
}



/*auxillary function to print a page table, useful for debugging*/
void print_page_table(page_table* page_table_object)
{
    fprintf(output_fd,"Printing Page Table\n");
    fflush(output_fd);

    int i;
    for(i=0;i<256;i++)
    {
        fprintf(output_fd,"Index: %d | Frame Number: %d | Valid: %d\n",i,page_table_object->page_table_entries[i].frame_base_address,page_table_object->page_table_entries[i].valid);
        fflush(output_fd);
    }

    fprintf(output_fd,"\n\n");
    fflush(output_fd);
}


/*auxillary function to print the frame table, useful for debugging*/
void print_frame_table(main_memory* main_memory_object)
{
    fprintf(output_fd,"Printing OS Frame Table\n");
    fflush(output_fd);

    int number_of_frames = main_memory_object->number_of_frames;

    for(int i=0;i<number_of_frames;i++)
    {
        fprintf(output_fd,"Frame Number: %d | Page Number: %d | PID: %d | Modified: %d |\n",i,main_memory_object->frame_table[i].page_number,main_memory_object->frame_table[i].pid, main_memory_object->frame_table[i].modified);
        fflush(output_fd);
    }

    fprintf(output_fd,"\n\n");
    fflush(output_fd);
}



/*auxillary function to print the free frame list, useful for debugging*/
void print_ffl(main_memory *main_memory_object)
{
    fprintf(output_fd,"Printing Free Frame List\nNumber of Free Frames are: %d\n",main_memory_object->ffl_dummy_head->number_free_frames);
    fflush(output_fd);

    free_frame *walker;
    walker = main_memory_object->ffl_dummy_head->next;
    if(walker==NULL)
    {
        fprintf(output_fd,"Free Frame List is empty\n");
        fflush(output_fd);

        return;
    }
    while(walker->next!=NULL)
    {
        fprintf(output_fd,"{Frame Number: %d}-->",walker->frame_number);
        fflush(output_fd);

        walker=walker->next;
    }

    fprintf(output_fd,"\n");
    fflush(output_fd);

}



/*auxillary function to print the used frame list, useful for debugging*/
void print_ufl(main_memory *main_memory_object)
{
    fprintf(output_fd,"Printing Used Frame List\nNumber of Used Frames are: %d\n",main_memory_object->ufl_dummy_head->number_used_frames);
    fflush(output_fd);

    used_frame *walker;
    
    walker = main_memory_object->ufl_dummy_head->next;
    used_frame *temp=walker;
    if(walker!=NULL)
    {

        fprintf(output_fd,"Recently Used Frame: {Frame Number: %d, Reference Bit: %d}\n",main_memory_object->recently_used_frame->frame_number,main_memory_object->recently_used_frame->reference_bit);
        fflush(output_fd);
    }

    while(walker!=NULL && walker->next!=temp)
    {
        fprintf(output_fd,"{Frame Number: %d, Reference Bit: %d, PID: %d}-->",walker->frame_number, walker->reference_bit,get_pid_of_frame(main_memory_object,walker->frame_number));
        fflush(output_fd);

        walker=walker->next;
    }
    if(walker!=NULL)
    {
        fprintf(output_fd,"{Frame Number: %d, Reference Bit: %d, PID: %d}-->",walker->frame_number, walker->reference_bit,get_pid_of_frame(main_memory_object,walker->frame_number));
        fflush(output_fd);

    }

    fprintf(output_fd,"\n");
    fflush(output_fd);
}