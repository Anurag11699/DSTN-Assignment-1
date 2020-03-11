#include "data_structures.h"

extern void add_free_frame(free_frame_list_dummy_head *, free_frame **, int);
extern int remove_free_frame(free_frame_list_dummy_head *);

extern void add_used_frame(used_frame_list_dummy_head *, used_frame **, int);
extern int remove_used_frame(used_frame_list_dummy_head *, used_frame **);

extern main_memory* initialize_main_memory(int, int);
extern tlb* initialize_tlb(int);