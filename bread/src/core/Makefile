test_main:test_main.c bb_place.o place_crit.o place_route.o place_util.o route_bias.o route.o route_common.o route_crit.o route_path.o
	gcc -o test_main -Wall -g test_main.c 

.c.o:
	gcc -c -I. $<

clean:
	rm -f test_main
	rm -f ../*.o
