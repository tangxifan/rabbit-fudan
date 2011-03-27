#include <stdio.h>
#include <stdlib.h>
#include <rabbit_type.h>
#include <place.h>
#include <route.h>

/*****************Subroutines descriptiong in this file*****************/

int
initial_place(IN  t_place_node *node_list,
			  INOUT  t_bb_array bb_placed,
			  );

int 
detail_place(IN  t_place_node *node_list,
			  INOUT  t_bb_array bb_placed,
			  );

/****************Subroutines****************/
