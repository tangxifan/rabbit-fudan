#include <stdio.h>
#include <stdlib.h>
#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <rbt_parser.h>

int
main()
{
	rbt_parse_netlist("simple_netlist.xml",
		"../../parts/parts.lst",
		"../../parts/parts/"
	);	
	return 0;
}
