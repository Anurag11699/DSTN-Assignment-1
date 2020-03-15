typedef struct kernel
{
    unsigned int currently_running_process_pid;
    unsigned int CR3_reg;
    unsigned int current_instruction;
}kernel;