#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbt_parser.h"

int main()
{
	rbt_parse_netlist("simple_netlist.xml",
		"../../../../parts/parts.lst",
		"../../../../parts/parts/"
	);	
	return 0;
}
