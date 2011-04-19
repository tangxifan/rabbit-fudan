test_parser:	test_parser.c RBT_parser.o
	gcc -o test_parser -g -Wall -lxml2 -I.. -I../../../core test_parser.c RBT_parser.o

RBT_parser.o:	../RBT_parser.c ../RBT_parser.h
	gcc -c -g -Wall -I/usr/include/libxml2 -I../../../core ../RBT_parser.c

clean:
	rm -f RBT_parser.o
	rm -f test_parser
