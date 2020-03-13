driver: general_functions.o tlb_functions.o cache_functions.o main_memory_functions.o driver.c
	gcc -o driver general_functions.o tlb_functions.o cache_functions.o main_memory_functions.o driver.c
	@echo executable code generated
	
general_funcitons.o: general_functions.c
	gcc -c general_functions.c
	@echo object code for general_functions generated

tlb_funcitons.o: tlb_functions.c
	gcc -c tlb_functions.c
	@echo object code for tlb_functions generated

cache_funcitons.o: cache_functions.c
	gcc -c cache_functions.c
	@echo object code for cache_functions generated

main_memory_funcitons.o: main_memory_functions.c
	gcc -c main_memory_functions.c
	@echo object code for main_memory_functions generated
	
	
clean:
	rm driver general_functions.o tlb_functions.o cache_functions.o main_memory_functions.o
	@echo clean complete
