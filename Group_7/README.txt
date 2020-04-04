Group 7

Members:
Anurag Madnawat      2017A7PS1923G
Chinmay Gupta        2017A7PS0130G
Suryateja Ratakumtla 2017A7PS0113G
Madhav Sasikumar     2016B5A70479G

Interacted With:
Aboli Vijayanad Pai  2017A7PS0147G
Amol Vijayanad Pai   2017A7PS0038G


Contribution by Each Member:

Anurag Madnawat(40%): Main Memory, Kernel, driver, debugging and integrating code, EMAT calculation, contributing with ideas for code structure, planning the code    
Chinmay Gupta(20%): L2 cache, debugging, contributing with ideas for code structure, planning the code
Suryateja Ratakumtla(20%): L1 cache, debugging, contributing with ideas for code structure, planning the code
Madhav Sasikumar(20%): TLB,debugging, contributing with ideas for code structure, planning the code

Problem Description:

The memory subsystem [with TLBs, L1 Cache, L2 Cache and Main Memory] has following
configuration:
TLBs: L1 TLB with 12 entries and L2 TLB with 24 entries. Invalidation / Flush takes place at each
preemption point.
L1 Cache: 8KB (4KB Instruction and 4KB Data), 32B, 4 Way set associative cache. L1 and L2 caches are Exclusive in nature. The
L1 Cache follows Write through and Look Aside. It follows LRU Square Matrix as replacement
policy.
L2 Cache: 32KB, 32B, 16 Way set associative cache. The cache follows Write buffer [with 8
buffers] and look through. It follows LFU [with aging] as replacement policy.
Main Memory with Memory Management: 32MB Main memory with Second chance as
replacement policy. The memory management scheme used is Pure Paging

Technical Details:

Virtual Address - 32 bits, Physical Address - 25 bits. One page size = 1 KB. 
L1 TLB - Number of entries = 12. Page no = 22 bits, frame no = 15 bits.
L2 TLB - Number of entries = 12. Page no = 22 bits, frame no = 15 bits.
L1 cache - Block Size = 32 bytes. Number of entries = 32. Number of ways = 4. Tag = 15, Index = 5, Offset = 5.
L2 cache - Block Size = 32 bytes. Number of entries = 64. Number of ways = 16. Tag = 13, Index = 6, Offset = 5.
Main memory - Number of frames = 32768. Hierarchical paging with three level page tables. Number of entries in page table = 256. Size of one page table entry = 4B.


List of Files:

.h files:
macros.h: Contains all the macros used in the code.
global_variables.h: Contains all the global variables used in the code.
functions.h: Contains the declarations of all the functions used in the entire code.
tlb_data_structures.h: Contains all ADTs associated with L1 and L2 TLB.
cache_data_structures.h: Contains all ADTs associated with L1 and L2 cache.
main_memory_data_structures: Contains all ADTs associated with main memory.
kernel_data_structures.h: Contains all ADTs associated with kernel.

.c files:
driver.c: File which contains main. Takes all inputs and starts the simulation.
tlb_functions.c: Contains all the functions related to the working of the L1 and L2 tlb.
cache_functions.c: Contais all the functions related to the working of the L1 and L2 cache.
main_memory_functions.c: Contains all the functions related to the working of the main memory.
kernel_functions.c: Contains all the functions related to the working of the kernel. This binds all the memory level together and is used to call the needed functions based on the situation.

subdirectory: input_files.txt
input.txt: Contains number of processes and the input file name corresponding to the requests of each process.
time_values.txt: Contains the time values for each aspect of the effective memory access time calculation.
(This subdirectory also contains the process requests file for each process.)

subdirectory: output_files.txt
OUTPUT.txt: Upon simulation, will contain the output of the program.

makefile: used to compile the code


How to Compile:
Go to the path where the folder Group_7 is stored
cd Group_7 (to go into the folder)
either type make or make all to Compile

How to Execute:
Compile the code as show above
go to Group_7 folder
Type ./test in the terminal to execute the program



Structure of the program:
1. driver takes all inputs pertaining to memory times and file names of each process
2. All processes are loaded into memory using the "load_new_process" function. First two pages are prepaged for each process
3. Every process is executed for a fixed number of instructions until being context switched out. This happens in a cycle for all processes until all its requests are processes. It is terminated after that.
4. To process a request, we call "execute_process_request" function which does the following
    a. Checks if the logical frame mapping to physical frame exists in any of the tlb using "complete tlb search" function. If it does not, it does a page walk using the "page_table_walk" function to get the frame number for the corresponding logical page. It then inserts the mapping into the tlbs and restarts the process request.
    b. If there is a tlb hit, we get the physical frame number. Using this we search in both the L1 and L2 caches.
    c. We search both caches in parallel as caches are look aside.
    d. If L1 cache hit we get the requested data or write to the data based on read or write request. The request to the L2 cache is cancelled. As this policy is write through, if it is as write request the data is immediately written back to main memory and the frame is marked as dirty.
    e. If L2 cache hit we get the data if read. If write, the block is written to the write buffer to be written back to main memory once write buffer is full. The block is then transfered to L1 cache. The L1 cache also transfers a block to L2 as the caches are exclusive.
    f. If both the caches miss, then we get the data from memory and insert it into the L1 cache. The L1 cache also transfers a block to L2 as the caches are exclusive.
5. We are taking care of thrasing also. We set the lower and upper bounds of number of frames a process can have. If a process exceeds its upper bound, we release its frames until it has frames equal to the avg frames per process. Also, we do not take frames from a process which has number of frames less than the lower bound.
6. There is a separate L1 data and L1 instruction cache maintained.


Output of the Code:

Output is stored in OUTPUT.txt in the subdirectory output_files

for each process request the output displays:
PID of process
Logical Address
TLB hit/miss
If TLB miss, then page walk and Instruction restarts
If TLB hit, Physical frame number and Offset
IF L1 or L2 cache hit
IF both cache miss then getting data from MM and putting into L1 cache
L1 cache hit after searching in L1 cache

At the end of the file (and also the console output) the following memory statistics are displayed:
Total Time Taken
Number of Requests Processed
Effective Memory Access Time
TLB Hit Rate
L1 Cache Hit Rate
L2 Cache Hit Rate
Overall Cache Hit Rate
Page Fault Rate


Completion of Assignment: Have completed all aspects of the question.

Bugs: Tried our level best to hunt for and remove any bugs in the code. The program does not have any bugs that we know of .