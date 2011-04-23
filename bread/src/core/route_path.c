#include <stdio.h>
#include <stdlib.h>

#include <rabbit_types.h>
#include <util.h>
#include <bb_type.h>
#include <place_route.h>
#include <device.h>
#include <setup_rabbit.h>


/*
 *Caculate the mahattan distance with given
 *source x,y and destination x,y.
 */
int 
find_manhattan_distance(IN t_bb_xyloc src,
                        IN t_bb_xyloc des
                        )
{
  return (abs(src.x-des.x)+abs(src.y-des.y));
}

float 
cal_route_cost(IN t_pr_marco* marco)
{
  
}
