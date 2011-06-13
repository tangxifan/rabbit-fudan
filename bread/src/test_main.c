#include <stdio.h>
#include <stdlib.h>
#include <rabbit_type.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <rbt_parser.h>
#include <setup_rabbit.h>

extern t_vnet *vnets;
extern int vnets_length;

extern t_icdev *devices;
extern int devices_length;

extern t_pr_marco *marcos;
extern int marcos_length;

extern t_bb_array bb_array;

int 
place_route(IN int nvnet,
            IN t_vnet* vnets,
            IN int nmarco,
            INOUT t_pr_marco* pr_marco,
            INOUT t_bb_array* bb_array
           );

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

//	printf ("%d\n", vnets->pins[1]->parent->name);

	setup_rabbit (vnets_length, vnets, marcos_length, marcos, &bb_array);

    place_route(vnets_length, vnets, marcos_length, marcos, &bb_array);
    return 0;
}
