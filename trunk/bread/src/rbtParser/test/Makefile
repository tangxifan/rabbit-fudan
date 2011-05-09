test_parser:	test_parser.c rbtParser.o
	gcc -o test_parser -g -Wall -lxml2 -I.. -I../../../core test_parser.c rbtParser.o

rbtParser.o:	../rbtParser.c ../rbtParser.h
	gcc -c -g -Wall  `xml2-config --cflags` `xml2-config --libs` -I../../../src/core ../rbtParser.c

clean:
	rm -f rbtParser.o
	rm -f test_parser
