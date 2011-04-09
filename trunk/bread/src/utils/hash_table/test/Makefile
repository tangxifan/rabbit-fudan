test_hash: test_hash.c hash_table.o
	gcc -o test_hash -g -Wall -I.. test_hash.c hash_table.o

hash_table.o:	../hash_table.c ../hash_table.h
	gcc -c -g -Wall ../hash_table.c

clean:
	rm -f hash_table.o
	rm -f test_hash
