#include <stdio.h>
#include <stdlib.h>
#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <rbt_parser.h>

int
main(int argc, char** argv)
{
	if (NULL == argv[1]){
		printf ("USAGE: rabbit <input_netlist_file>\n");
		return 1;
	}

	rbt_parse_netlist(argv[1],
		"../../parts/parts.lst",
		"../../parts/parts/"
	);	

	return 0;
}
