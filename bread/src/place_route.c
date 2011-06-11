#include <stdio.h>
#include <stdlib.h>

#include <rabbit_type.h>
#include <place_route.h>
#include <place.h>

int 
place_route(IN int nvnet,
            IN t_vnet* vnets,
            IN int nmarco,
            INOUT t_pr_marco* pr_marco,
            INOUT t_bb_array* bb_array
           )
{
  try_place(nvnet,vnets,nmarco,pr_marco,bb_array);
  //detail_place(nvnet,vnets,nmarco,pr_marco,bb_array);
  //try_route(nvnet,vnets,nmarco,pr_marco,bb_array);
  return 1;
}
