CC = gcc
CFLAGS = -g -Wall -fstrict-aliasing -std=c99 

test:main
	@echo
	@echo "Test 1: Partial subgraph isomorphism" 
	./main -p pattern.txt -t target.txt  
	@echo
	@echo "Test 2: Induced subgraph isomorphism"
	./main -p pattern.txt -t target.txt -i 

main:main.c allDiff.c graph.c lad.c domainsPath.c allDiff_large.c graph_large.c lad_large.c domainsPath_large.c solve_large.c
	$(CC) $(CFLAGS) main.c solve_large.c -o $@



