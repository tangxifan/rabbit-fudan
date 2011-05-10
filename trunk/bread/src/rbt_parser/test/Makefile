test_parser:	test_parser.c rbt_parser.o
	gcc -o test_parser -g -Wall -lxml2 -I.. -I../../../src/core test_parser.c rbt_parser.o

rbt_parser.o:	../rbt_parser.c ../rbt_parser.h
	gcc -c -g -Wall  `xml2-config --cflags` `xml2-config --libs` -I../../../src/core ../rbt_parser.c

clean:
	rm -f rbt_parser.o
	rm -f test_parser
