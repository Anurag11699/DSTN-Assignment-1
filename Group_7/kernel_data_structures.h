#ifndef KERNEL_DATA_STRUCTURES_H
#define KERNEL_DATA_STRUCTURES_H


/*
    ADT for a Process Control Block

    unsigned int pid -> stores the pid of the process

    int outer_page_base_address:15 -> stores the frame number that the outermost page table of the process is in

    page_table* outer_page_table -> stores pointer to the outermost page table

    int outer_page_base_address_initialized_before:1 -> tells if the page table for outermost page table has been initialzed before or not. This is used to determine whether the page table needs to be initialized or merely swapped in

    FILE* fd -> file descriptor for the input file of the process.

    int state -> state of the process

    unsigned int number_of_frames_used -> number of frames used by this process. This information is used to keep number of pages used in limit and prevent thrashing
*/
typedef struct pcb
{   
    unsigned int pid;
    int outer_page_base_address:15;
    page_table* outer_page_table;
    int outer_page_base_address_initialized_before:1;
    FILE* fd;
    int state;
    unsigned int number_of_frames_used;
}pcb;


/*
    ADT for Kernel

    array of pcbs is stored in this ADT
*/
typedef struct kernel
{
    int currently_running_process_pid;
    //int CR3_reg;
    //int current_instruction;
    unsigned int number_of_processes;

    pcb* pcb_array;
}kernel;

#endif /*KERNEL_DATA_STRUCTURES_H*/