test_main:test_main.c bb_place.o place_crit.o place_route.o place_util.o route_bias.o route.o route_common.o route_crit.o route_path.o rbt_parser.o
	gcc -o test_main -Wall -g -I. `xml2-config --cflags` `xml2-config --libs` test_main.c bb_place.o place_crit.o place_route.o place_util.o route_bias.o route.o route_common.o route_crit.o route_path.o rbt_parser.o

rbt_parser.o:	rbt_parser.c rbt_parser.h
	gcc -c -g -Wall  `xml2-config --cflags` `xml2-config --libs` -I. rbt_parser.c

.c.o:
	gcc -c -I. $<


clean:
	rm -f test_main
	rm -f *.o
