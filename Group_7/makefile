driver: funcitons.o driver.c
	gcc -o driver functions.o driver.c
	@echo executable code generated
	
funcitons.o: functions.c
	gcc -c functions.c
	@echo object code generated
	
clean:
	rm priority_inversion functions.o
	@echo clean complete
