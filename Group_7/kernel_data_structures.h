
typedef struct pcb
{
    int outermost_page_base_address:14;
    FILE* fd;
    int state;
}pcb;

typedef struct kernel
{
    unsigned int currently_running_process_pid;
    unsigned int CR3_reg;
    unsigned int current_instruction;

    pcb* pcb_array;
}kernel;