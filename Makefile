all: fork.c threads.c matrix_generator.c
	gcc -o fork fork.c -lpthread
	gcc -o threads threads.c -lpthread
	gcc -o gen matrix_generator.c
fork: fork.c
	gcc -o fork fork.c
threads: threads.c
	gcc -o threads threads.c -lpthread
gen: matrix_generator.c
	gcc -o gen matrix_generator.c
clean:
	rm -rf *.o
distclean:
	rm -rf threads gen fork
txtclean:
	rm -rf in1.txt in2.txt out.txt
