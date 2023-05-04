CC = gcc
CFLAGS = -O3 -Wall -fstrict-aliasing -std=c99

test:PathLAD+
	@echo
	@echo "Test 1: Partial subgraph isomorphism"
	./PathLAD+ -p pattern.txt -t target.txt -f -s 3600 
	@echo
	@echo "Test 2: Induced subgraph isomorphism"
	./PathLAD+ -p pattern.txt -t target.txt -i -f -s 3600 

PathLAD+:main.c allDiff.c graph.c lad.c domainsPath.c
	$(CC) $(CFLAGS) main.c -o $@

